#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "infra/timer/TimerServiceManager.hpp"
#include "services/network/CertificatesMbedTls.hpp"
#include "services/network/ConnectionMbedTls.hpp"

namespace services
{
    ConnectionMbedTls::ConnectionMbedTls(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver, CertificatesMbedTls& certificates,
        hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession)
        : createdObserver(std::move(createdObserver))
        , randomDataGenerator(randomDataGenerator)
        , server(server)
        , clientSession(clientSession)
    {
        mbedtls2_ssl_init(&sslContext);
        mbedtls2_ssl_config_init(&sslConfig);
        mbedtls2_ctr_drbg_init(&ctr_drbg);
        mbedtls2_ssl_conf_dbg(&sslConfig, StaticDebugWrapper, this);

        int result;

        result = mbedtls2_ctr_drbg_seed(&ctr_drbg, &ConnectionMbedTls::StaticGenerateRandomData, this, nullptr, 0);
        assert(result == 0);

        result = mbedtls2_ssl_config_defaults(&sslConfig, server ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
        assert(result == 0);
        mbedtls2_ssl_conf_rng(&sslConfig, mbedtls2_ctr_drbg_random, &ctr_drbg);
        mbedtls2_ssl_conf_authmode(&sslConfig, server ? MBEDTLS_SSL_VERIFY_NONE : MBEDTLS_SSL_VERIFY_REQUIRED);

        certificates.Config(sslConfig);

        if (server)
            mbedtls2_ssl_conf_session_cache(&sslConfig, serverCache, mbedtls2_ssl_cache_get, mbedtls2_ssl_cache_set);
    }

    void ConnectionMbedTls::InitTls()
    {
        int result = mbedtls2_ssl_setup(&sslContext, &sslConfig);
        if (result != 0)
            TlsInitFailure(result);
        really_assert(result == 0);

        if (!server)
        {
            result = mbedtls2_ssl_set_session(&sslContext, clientSession);
            assert(result == 0);
        }

        mbedtls2_ssl_set_bio(&sslContext, this, &ConnectionMbedTls::StaticSslSend, &ConnectionMbedTls::StaticSslReceive, nullptr);

        TryAllocateEncryptedSendStream();
    }

    ConnectionMbedTls::~ConnectionMbedTls()
    {
        encryptedSendStream = nullptr;

        mbedtls2_ctr_drbg_free(&ctr_drbg);
        mbedtls2_ssl_free(&sslContext);
        mbedtls2_ssl_config_free(&sslConfig);

        ResetOwnership();
    }

    void ConnectionMbedTls::CreatedObserver(infra::SharedPtr<services::ConnectionObserver> connectionObserver)
    {
        if (connectionObserver != nullptr)
        {
            connectionObserver->Attach(*this);
            SetOwnership(nullptr, connectionObserver);
            createdObserver(SharedFromThis());
        }
        else
            createdObserver(nullptr);
    }

    void ConnectionMbedTls::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        encryptedSendStream = stream;
        TrySend();
    }

    void ConnectionMbedTls::DataReceived()
    {
        std::size_t startSize = receiveBuffer.size();

        while (!receiveBuffer.full())
        {
            std::size_t newBufferStart = receiveBuffer.size();
            receiveBuffer.resize(receiveBuffer.max_size());
            infra::ByteRange buffer = receiveBuffer.contiguous_range(receiveBuffer.begin() + newBufferStart);

            int result = mbedtls2_ssl_read(&sslContext, reinterpret_cast<unsigned char*>(buffer.begin()), buffer.size());
            if (result == MBEDTLS_ERR_SSL_WANT_WRITE || result == MBEDTLS_ERR_SSL_WANT_READ)
            {
                receiveBuffer.resize(newBufferStart);
                break;
            }
            else if (result == MBEDTLS_ERR_SSL_BAD_INPUT_DATA)  // Precondition failure
            {
                TlsReadFailure(result);
                std::abort();
            }
            else if (result < 0)
            {
                encryptedSendStream = nullptr;
                TlsReadFailure(result);
                ConnectionObserver::Subject().AbortAndDestroy();
                return;
            }
            else
                receiveBuffer.resize(newBufferStart + static_cast<std::size_t>(result));
        }

        if (receiveBuffer.size() != startSize)
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionMbedTls>& object)
            {
                object->GetObserver().DataReceived();
            }, SharedFromThis());
    }

    void ConnectionMbedTls::Connected()
    {
        InitTls();
        GetObserver().Connected();
    }

    void ConnectionMbedTls::ClosingConnection()
    {
        encryptedSendStream = nullptr;
        ResetOwnership();
    }

    void ConnectionMbedTls::RequestSendStream(std::size_t sendSize)
    {
        assert(requestedSendSize == 0);
        assert(sendSize != 0 && sendSize <= MaxSendStreamSize());
        requestedSendSize = sendSize;
        TryAllocateSendStream();
    }

    std::size_t ConnectionMbedTls::MaxSendStreamSize() const
    {
        return sendBuffer.max_size();
    }

    infra::SharedPtr<infra::DataInputStream> ConnectionMbedTls::ReceiveStream()
    {
        return receiveStream.Emplace(*this);
    }

    void ConnectionMbedTls::AckReceived()
    {
        receiveStream->Reader().ConsumeRead();
        DataReceived();
    }

    void ConnectionMbedTls::CloseAndDestroy()
    {
        ConnectionObserver::Subject().CloseAndDestroy();
    }

    void ConnectionMbedTls::AbortAndDestroy()
    {
        encryptedSendStream = nullptr;
        ConnectionObserver::Subject().AbortAndDestroy();
    }

    void ConnectionMbedTls::TlsInitFailure(int reason)
    {}

    void ConnectionMbedTls::TlsReadFailure(int reason)
    {}

    void ConnectionMbedTls::TlsWriteFailure(int reason)
    {}

    void ConnectionMbedTls::TlsLog(int level, const char* file, int line, const char* message)
    {}

    void ConnectionMbedTls::TryAllocateSendStream()
    {
        assert(sendStream.Allocatable());
        if (sendBuffer.max_size() - sendBuffer.size() >= requestedSendSize)
        {
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionMbedTls>& object)
            {
                infra::SharedPtr<infra::DataOutputStream> stream = object->sendStream.Emplace(*object);
                object->GetObserver().SendStreamAvailable(std::move(stream));
            }, SharedFromThis());

            requestedSendSize = 0;
        }
    }

    void ConnectionMbedTls::TryAllocateEncryptedSendStream()
    {
        encryptedSendStreamSize = ConnectionObserver::Subject().MaxSendStreamSize();
        ConnectionObserver::Subject().RequestSendStream(encryptedSendStreamSize);
    }

    int ConnectionMbedTls::StaticSslSend(void* context, const unsigned char* buffer, std::size_t size)
    {
        return reinterpret_cast<ConnectionMbedTls*>(context)->SslSend(infra::ConstByteRange(reinterpret_cast<const uint8_t*>(buffer), reinterpret_cast<const uint8_t*>(buffer) + size));
    }

    int ConnectionMbedTls::StaticSslReceive(void *context, unsigned char* buffer, size_t size)
    {
        return reinterpret_cast<ConnectionMbedTls*>(context)->SslReceive(infra::ByteRange(reinterpret_cast<uint8_t*>(buffer), reinterpret_cast<uint8_t*>(buffer) + size));
    }

    int ConnectionMbedTls::SslSend(infra::ConstByteRange buffer)
    {
        if (encryptedSendStream && encryptedSendStreamSize != 0)
        {
            buffer = infra::Head(buffer, encryptedSendStreamSize);
            encryptedSendStreamSize -= buffer.size();
            *encryptedSendStream << buffer;

            if (!buffer.empty() && !flushScheduled)
            {
                flushScheduled = true;
                infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionMbedTls>& object)
                {
                    object->flushScheduled = false;
                    object->encryptedSendStream = nullptr;
                    object->TryAllocateEncryptedSendStream();
                }, SharedFromThis());
            }

            return buffer.size();
        }
        else
            return MBEDTLS_ERR_SSL_WANT_WRITE;
    }

    int ConnectionMbedTls::SslReceive(infra::ByteRange buffer)
    {
        infra::SharedPtr<infra::DataInputStream> stream = ConnectionObserver::Subject().ReceiveStream();
        infra::ConstByteRange streamBuffer = stream->ContiguousRange(buffer.size());
        std::copy(streamBuffer.begin(), streamBuffer.end(), buffer.begin());
        ConnectionObserver::Subject().AckReceived();

        if (!streamBuffer.empty())
        {
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionMbedTls>& object) { object->TrySend(); }, SharedFromThis());
            return streamBuffer.size();
        }
        else
            return MBEDTLS_ERR_SSL_WANT_READ;
    }

    void ConnectionMbedTls::TrySend()
    {
        while (initialHandshake || !sendBuffer.empty())
        {
            infra::ConstByteRange range = sendBuffer.contiguous_range(sendBuffer.begin());
            int result = initialHandshake
                ? mbedtls2_ssl_handshake(&sslContext)
                : mbedtls2_ssl_write(&sslContext, reinterpret_cast<const unsigned char*>(range.begin()), range.size());
            if (result == MBEDTLS_ERR_SSL_WANT_WRITE || result == MBEDTLS_ERR_SSL_WANT_READ)
                return;
            else if (result == MBEDTLS_ERR_SSL_BAD_INPUT_DATA)  // Precondition failure
            {
                TlsWriteFailure(result);
                std::abort();
            }
            else if (result < 0)
            {
                encryptedSendStream = nullptr;
                TlsWriteFailure(result);
                ConnectionObserver::Subject().AbortAndDestroy();
                return;
            }
            else if (initialHandshake)
            {
                initialHandshake = false;

                if (!server)
                {
                    result = mbedtls2_ssl_get_session(&sslContext, clientSession);
                    assert(result == 0);
                }
            }
            else
            {
                sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + result);
                if (static_cast<std::size_t>(result) < range.size())
                    break;
            }
        }

        if (requestedSendSize != 0)
            TryAllocateSendStream();
    }

    int ConnectionMbedTls::StaticGenerateRandomData(void* data, unsigned char* output, std::size_t size)
    {
        reinterpret_cast<ConnectionMbedTls*>(data)->GenerateRandomData(infra::ByteRange(reinterpret_cast<uint8_t*>(output), reinterpret_cast<uint8_t*>(output) + size));
        return 0;
    }

    void ConnectionMbedTls::GenerateRandomData(infra::ByteRange data)
    {
        randomDataGenerator.GenerateRandomData(data);
    }

    void ConnectionMbedTls::StaticDebugWrapper(void* context, int level, const char* file, int line, const char* message)
    {
        reinterpret_cast<ConnectionMbedTls*>(context)->TlsLog(level, file, line, message);
    }

    ConnectionMbedTls::StreamWriterMbedTls::StreamWriterMbedTls(ConnectionMbedTls& connection)
        : connection(connection)
    {}

    ConnectionMbedTls::StreamWriterMbedTls::~StreamWriterMbedTls()
    {
        if (sent != 0)
            connection.TrySend();
    }

    void ConnectionMbedTls::StreamWriterMbedTls::Insert(infra::ConstByteRange range, infra::StreamErrorPolicy& errorPolicy)
    {
        connection.sendBuffer.insert(connection.sendBuffer.end(), range.begin(), range.end());
        sent += range.size();
    }

    std::size_t ConnectionMbedTls::StreamWriterMbedTls::Available() const
    {
        return connection.sendBuffer.size() - sent;
    }

    ConnectionMbedTls::StreamReaderMbedTls::StreamReaderMbedTls(ConnectionMbedTls& connection)
        : infra::StreamReader(infra::softFail)
        , connection(connection)
    {}

    void ConnectionMbedTls::StreamReaderMbedTls::ConsumeRead()
    {
        connection.receiveBuffer.erase(connection.receiveBuffer.begin(), connection.receiveBuffer.begin() + sizeRead);
        sizeRead = 0;
    }

    void ConnectionMbedTls::StreamReaderMbedTls::Extract(infra::ByteRange range)
    {
        bool ok = sizeRead + range.size() <= connection.receiveBuffer.size();
        ReportResult(ok);

        if (ok)
        {
            std::copy(connection.receiveBuffer.begin() + sizeRead, connection.receiveBuffer.begin() + sizeRead + range.size(), range.begin());
            sizeRead += range.size();
        }
    }

    uint8_t ConnectionMbedTls::StreamReaderMbedTls::ExtractOne()
    {
        bool ok = sizeRead + 1 <= connection.receiveBuffer.size();
        ReportResult(ok);

        if (ok)
            return connection.receiveBuffer[sizeRead++];
        else
            return 0;
    }

    uint8_t ConnectionMbedTls::StreamReaderMbedTls::Peek()
    {
        bool ok = sizeRead + 1 <= connection.receiveBuffer.size();
        ReportResult(ok);

        if (ok)
            return connection.receiveBuffer[sizeRead];
        else
            return 0;
    }

    infra::ConstByteRange ConnectionMbedTls::StreamReaderMbedTls::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange result = infra::Head(connection.receiveBuffer.contiguous_range(connection.receiveBuffer.begin() + sizeRead), max);
        sizeRead += result.size();
        return result;
    }

    bool ConnectionMbedTls::StreamReaderMbedTls::Empty() const
    {
        return connection.receiveBuffer.size() == sizeRead;
    }

    std::size_t ConnectionMbedTls::StreamReaderMbedTls::Available() const
    {
        return connection.receiveBuffer.size() - sizeRead;
    }

    ConnectionMbedTlsListener::ConnectionMbedTlsListener(AllocatorConnectionMbedTls& allocator, ServerConnectionObserverFactory& factory, CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_cache_context& serverCache)
        : allocator(allocator)
        , factory(factory)
        , certificates(certificates)
        , randomDataGenerator(randomDataGenerator)
        , serverCache(serverCache)
    {}

    void ConnectionMbedTlsListener::ConnectionAccepted(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver, services::IPv4Address ipv4Address)
    {
        infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)> creationFailed = createdObserver.Clone();
        infra::SharedPtr<ConnectionMbedTls> connection = allocator.Allocate(std::move(createdObserver), certificates, randomDataGenerator, true, &serverCache, nullptr);
        if (connection)
        {
            factory.ConnectionAccepted([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
            {
                connection->CreatedObserver(connectionObserver);
            }, ipv4Address);
        }
        else
            creationFailed(nullptr);
    }

    void ConnectionMbedTlsListener::SetListener(infra::SharedPtr<void> listener)
    {
        this->listener = listener;
    }

    ConnectionMbedTlsConnector::ConnectionMbedTlsConnector(AllocatorConnectionMbedTls& allocator, ClientConnectionObserverFactory& factory,
        CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_session& clientSession)
        : allocator(allocator)
        , factory(factory)
        , certificates(certificates)
        , randomDataGenerator(randomDataGenerator)
        , clientSession(clientSession)
    {}

    void ConnectionMbedTlsConnector::ConnectionEstablished(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver)
    {
        infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)> creationFailed = createdObserver.Clone();
        infra::SharedPtr<ConnectionMbedTls> connection = allocator.Allocate(std::move(createdObserver), certificates, randomDataGenerator, false, nullptr, &clientSession);
        if (connection)
        {
            factory.ConnectionEstablished([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
            {
                connection->CreatedObserver(connectionObserver);
            });
        }
        else
            creationFailed(nullptr);
    }

    void ConnectionMbedTlsConnector::ConnectionFailed(ConnectFailReason reason)
    {
        factory.ConnectionFailed(reason);
    }

    void ConnectionMbedTlsConnector::SetConnector(infra::SharedPtr<void> connector)
    {
        this->connector = connector;
    }

    ConnectionFactoryMbedTls::ConnectionFactoryMbedTls(AllocatorConnectionMbedTls& connectionAllocator,
        AllocatorConnectionMbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator,
        ConnectionFactory& factory, CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator)
        : connectionAllocator(connectionAllocator)
        , listenerAllocator(listenerAllocator)
        , connectorAllocator(connectorAllocator)
        , factory(factory)
        , certificates(certificates)
        , randomDataGenerator(randomDataGenerator)
    {
        mbedtls2_ssl_cache_init(&serverCache);
        mbedtls2_ssl_session_init(&clientSession);
    }

    ConnectionFactoryMbedTls::~ConnectionFactoryMbedTls()
    {
        mbedtls2_ssl_cache_free(&serverCache);
        mbedtls2_ssl_session_free(&clientSession);
    }

    infra::SharedPtr<void> ConnectionFactoryMbedTls::Listen(uint16_t port, ServerConnectionObserverFactory& connectionObserverFactory)
    {
        infra::SharedPtr<ConnectionMbedTlsListener> listener = listenerAllocator.Allocate(connectionAllocator, connectionObserverFactory, certificates, randomDataGenerator, serverCache);

        if (listener)
        {
            infra::SharedPtr<void> networkListener = factory.Listen(port, *listener);
            if (networkListener)
            {
                listener->SetListener(networkListener);
                return listener;
            }
        }

        return nullptr;
    }

    infra::SharedPtr<void> ConnectionFactoryMbedTls::Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& connectionObserverFactory)
    {
        infra::SharedPtr<ConnectionMbedTlsConnector> connector = connectorAllocator.Allocate(connectionAllocator, connectionObserverFactory, certificates, randomDataGenerator, clientSession);

        if (connector)
        {
            infra::SharedPtr<void> networkConnector = factory.Connect(address, port, *connector);
            if (networkConnector)
            {
                connector->SetConnector(networkConnector);
                return connector;
            }
        }

        return nullptr;
    }

    ConnectionIPv6MbedTlsListener::ConnectionIPv6MbedTlsListener(AllocatorConnectionMbedTls& allocator, ServerConnectionIPv6ObserverFactory& factory, CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_cache_context& serverCache)
        : allocator(allocator)
        , factory(factory)
        , certificates(certificates)
        , randomDataGenerator(randomDataGenerator)
        , serverCache(serverCache)
    {}

    void ConnectionIPv6MbedTlsListener::ConnectionAccepted(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver, services::IPv6Address address)
    {
        infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)> creationFailed = createdObserver.Clone();
        infra::SharedPtr<ConnectionMbedTls> connection = allocator.Allocate(std::move(createdObserver), certificates, randomDataGenerator, true, &serverCache, nullptr);
        if (connection)
        {
            factory.ConnectionAccepted([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
            {
                connection->CreatedObserver(connectionObserver);
            }, address);
        }
        else
            creationFailed(nullptr);
    }

    void ConnectionIPv6MbedTlsListener::SetListener(infra::SharedPtr<void> listener)
    {
        this->listener = listener;
    }

    ConnectionIPv6MbedTlsConnector::ConnectionIPv6MbedTlsConnector(AllocatorConnectionMbedTls& allocator, ClientConnectionIPv6ObserverFactory& factory,
        CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_session& clientSession)
        : allocator(allocator)
        , factory(factory)
        , certificates(certificates)
        , randomDataGenerator(randomDataGenerator)
        , clientSession(clientSession)
    {}

    void ConnectionIPv6MbedTlsConnector::ConnectionEstablished(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver)
    {
        infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)> creationFailed = createdObserver.Clone();
        infra::SharedPtr<ConnectionMbedTls> connection = allocator.Allocate(std::move(createdObserver), certificates, randomDataGenerator, false, nullptr, &clientSession);
        if (connection)
        {
            factory.ConnectionEstablished([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
            {
                connection->CreatedObserver(connectionObserver);
            });
        }
        else
            creationFailed(nullptr);
    }

    void ConnectionIPv6MbedTlsConnector::ConnectionFailed(ConnectFailReason reason)
    {
        factory.ConnectionFailed(reason);
    }

    void ConnectionIPv6MbedTlsConnector::SetConnector(infra::SharedPtr<void> connector)
    {
        this->connector = connector;
    }

    ConnectionIPv6FactoryMbedTls::ConnectionIPv6FactoryMbedTls(AllocatorConnectionMbedTls& connectionAllocator,
        AllocatorConnectionIPv6MbedTlsListener& listenerAllocator, AllocatorConnectionIPv6MbedTlsConnector& connectorAllocator,
        ConnectionIPv6Factory& factory, CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator)
        : connectionAllocator(connectionAllocator)
        , listenerAllocator(listenerAllocator)
        , connectorAllocator(connectorAllocator)
        , factory(factory)
        , certificates(certificates)
        , randomDataGenerator(randomDataGenerator)
    {
        mbedtls2_ssl_cache_init(&serverCache);
        mbedtls2_ssl_session_init(&clientSession);
    }

    ConnectionIPv6FactoryMbedTls::~ConnectionIPv6FactoryMbedTls()
    {
        mbedtls2_ssl_cache_free(&serverCache);
        mbedtls2_ssl_session_free(&clientSession);
    }

    infra::SharedPtr<void> ConnectionIPv6FactoryMbedTls::Listen(uint16_t port, ServerConnectionIPv6ObserverFactory& connectionObserverFactory)
    {
        infra::SharedPtr<ConnectionIPv6MbedTlsListener> listener = listenerAllocator.Allocate(connectionAllocator, connectionObserverFactory, certificates, randomDataGenerator, serverCache);

        if (listener)
        {
            infra::SharedPtr<void> networkListener = factory.Listen(port, *listener);
            if (networkListener)
            {
                listener->SetListener(networkListener);
                return listener;
            }
        }

        return nullptr;
    }

    infra::SharedPtr<void> ConnectionIPv6FactoryMbedTls::Connect(IPv6Address address, uint16_t port, ClientConnectionIPv6ObserverFactory& connectionObserverFactory)
    {
        infra::SharedPtr<ConnectionIPv6MbedTlsConnector> connector = connectorAllocator.Allocate(connectionAllocator, connectionObserverFactory, certificates, randomDataGenerator, clientSession);

        if (connector)
        {
            infra::SharedPtr<void> networkConnector = factory.Connect(address, port, *connector);
            if (networkConnector)
            {
                connector->SetConnector(networkConnector);
                return connector;
            }
        }

        return nullptr;
    }
}
