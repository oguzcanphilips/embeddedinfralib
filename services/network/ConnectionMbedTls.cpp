#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "services/network/ConnectionMbedTls.hpp"
#include "mbedtls/certs.h"

#include <iostream>

void debug(void*, int, const char* file, int line, const char* message)
{
    std::cout << file << "(" << line << "): " << message << std::flush;
}

namespace services
{
    ConnectionMbedTls::ConnectionMbedTls(ZeroCopyConnection& connection, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server)
        : ZeroCopyConnectionObserver(connection)
        , randomDataGenerator(randomDataGenerator)
    {
        mbedtls_ssl_init(&sslContext);
        mbedtls_ssl_config_init(&sslConfig);
        mbedtls_x509_crt_init(&serverCertificate);
        mbedtls_pk_init(&publicKey);
        mbedtls_ctr_drbg_init(&ctr_drbg);

        int result;

        if (server)
        {
            result = mbedtls_x509_crt_parse(&serverCertificate, reinterpret_cast<const unsigned char*>(mbedtls_test_srv_crt), mbedtls_test_srv_crt_len);
            assert(result == 0);
            result = mbedtls_x509_crt_parse(&serverCertificate, reinterpret_cast<const unsigned char*>(mbedtls_test_cas_pem), mbedtls_test_cas_pem_len);
            assert(result == 0);
            result = mbedtls_pk_parse_key(&publicKey, reinterpret_cast<const unsigned char*>(mbedtls_test_srv_key), mbedtls_test_srv_key_len, NULL, 0);
            assert(result == 0);
        }
        else
        {
            result = mbedtls_x509_crt_parse(&serverCertificate, reinterpret_cast<const unsigned char*>(mbedtls_test_cli_crt), mbedtls_test_cli_crt_len);
            assert(result == 0);
            result = mbedtls_x509_crt_parse(&serverCertificate, reinterpret_cast<const unsigned char*>(mbedtls_test_cas_pem), mbedtls_test_cas_pem_len);
            assert(result == 0);
            result = mbedtls_pk_parse_key(&publicKey, reinterpret_cast<const unsigned char*>(mbedtls_test_cli_key), mbedtls_test_cli_key_len, NULL, 0);
            assert(result == 0);
        }

        result = mbedtls_ctr_drbg_seed(&ctr_drbg, &ConnectionMbedTls::StaticGenerateRandomData, this, nullptr, 0);
        assert(result == 0);

        result = mbedtls_ssl_config_defaults(&sslConfig, server ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
        assert(result == 0);
        mbedtls_ssl_conf_rng(&sslConfig, mbedtls_ctr_drbg_random, &ctr_drbg);
        mbedtls_ssl_conf_authmode(&sslConfig, server ? MBEDTLS_SSL_VERIFY_NONE : MBEDTLS_SSL_VERIFY_REQUIRED);

        mbedtls_ssl_conf_ca_chain(&sslConfig, serverCertificate.next, nullptr);
        result = mbedtls_ssl_conf_own_cert(&sslConfig, &serverCertificate, &publicKey);
        assert(result == 0);
        result = mbedtls_ssl_setup(&sslContext, &sslConfig);
        assert(result == 0);

        mbedtls_ssl_set_bio(&sslContext, this, &ConnectionMbedTls::StaticSslSend, &ConnectionMbedTls::StaticSslReceive, nullptr);
        mbedtls_ssl_conf_dbg(&sslConfig, debug, nullptr);

        TryAllocateEncryptedSendStream();
    }

    ConnectionMbedTls::~ConnectionMbedTls()
    {
        encryptedSendStream = nullptr;

        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_pk_free(&publicKey);
        mbedtls_x509_crt_free(&serverCertificate);
        mbedtls_ssl_free(&sslContext);
        mbedtls_ssl_config_free(&sslConfig);

        ResetOwnership();
    }

    void ConnectionMbedTls::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>& stream)
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

            int result = mbedtls_ssl_read(&sslContext, reinterpret_cast<unsigned char*>(buffer.begin()), buffer.size());
            if (result == MBEDTLS_ERR_SSL_WANT_WRITE || result == MBEDTLS_ERR_SSL_WANT_READ)
            {
                receiveBuffer.resize(newBufferStart);
                break;
            }
            else if (result < 0)
            {
                encryptedSendStream = nullptr;
                ZeroCopyConnectionObserver::Subject().AbortAndDestroy();
                return;
            }
            else
                receiveBuffer.resize(newBufferStart + result);
        }

        if (receiveBuffer.size() != startSize)
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionMbedTls>& object)
            {
                object->GetObserver().DataReceived();
            }, SharedFromThis());
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
        receiveStream->ConsumeRead();
    }

    void ConnectionMbedTls::CloseAndDestroy()
    {
        ZeroCopyConnectionObserver::Subject().CloseAndDestroy();
    }

    void ConnectionMbedTls::AbortAndDestroy()
    {
        encryptedSendStream = nullptr;
        ZeroCopyConnectionObserver::Subject().AbortAndDestroy();
    }

    void ConnectionMbedTls::TryAllocateSendStream()
    {
        assert(sendStream.Allocatable());
        if (sendBuffer.max_size() - sendBuffer.size() >= requestedSendSize)
        {
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionMbedTls>& object)
            {
                infra::SharedPtr<infra::DataOutputStream> stream = object->sendStream.Emplace(*object);
                object->GetObserver().SendStreamAvailable(stream);
            }, SharedFromThis());

            requestedSendSize = 0;
        }
    }

    void ConnectionMbedTls::TryAllocateEncryptedSendStream()
    {
        encryptedSendStreamSize = ZeroCopyConnectionObserver::Subject().MaxSendStreamSize();
        ZeroCopyConnectionObserver::Subject().RequestSendStream(encryptedSendStreamSize);
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
        if (encryptedSendStream)
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
        infra::SharedPtr<infra::DataInputStream> stream = ZeroCopyConnectionObserver::Subject().ReceiveStream();
        infra::ConstByteRange streamBuffer = stream->ContiguousRange(buffer.size());
        std::copy(streamBuffer.begin(), streamBuffer.end(), buffer.begin());
        ZeroCopyConnectionObserver::Subject().AckReceived();

        infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionMbedTls>& object) { object->TrySend(); }, SharedFromThis());

        if (!streamBuffer.empty())
            return streamBuffer.size();
        else
            return MBEDTLS_ERR_SSL_WANT_READ;
    }

    void ConnectionMbedTls::TrySend()
    {
        do
        {
            infra::ConstByteRange range = sendBuffer.contiguous_range(sendBuffer.begin());
            int result = mbedtls_ssl_write(&sslContext, reinterpret_cast<const unsigned char*>(range.begin()), range.size());
            if (result == MBEDTLS_ERR_SSL_WANT_WRITE || result == MBEDTLS_ERR_SSL_WANT_READ)
                return;
            else if (result < 0)
            {
                encryptedSendStream = nullptr;
                ZeroCopyConnectionObserver::Subject().AbortAndDestroy();
                return;
            }
            else
            {
                sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + result);
                if (static_cast<std::size_t>(result) < range.size())
                    break;
            }
        } while (!sendBuffer.empty());

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

    ConnectionMbedTls::SendStreamMbedTls::SendStreamMbedTls(ConnectionMbedTls& connection)
        : infra::DataOutputStream(static_cast<infra::StreamWriter&>(*this))
        , connection(connection)
    {}

    ConnectionMbedTls::SendStreamMbedTls::~SendStreamMbedTls()
    {
        if (sent != 0)
            connection.TrySend();
    }

    void ConnectionMbedTls::SendStreamMbedTls::Insert(infra::ConstByteRange range)
    {
        connection.sendBuffer.insert(connection.sendBuffer.end(), range.begin(), range.end());
        sent += range.size();
    }

    void ConnectionMbedTls::SendStreamMbedTls::Insert(uint8_t element)
    {
        connection.sendBuffer.push_back(element);
        ++sent;
    }

    ConnectionMbedTls::ReceiveStreamMbedTls::ReceiveStreamMbedTls(ConnectionMbedTls& connection)
        : infra::DataInputStream(static_cast<infra::StreamReader&>(*this))
        , connection(connection)
    {}

    void ConnectionMbedTls::ReceiveStreamMbedTls::ConsumeRead()
    {
        connection.receiveBuffer.erase(connection.receiveBuffer.begin(), connection.receiveBuffer.begin() + sizeRead);
        sizeRead = 0;
    }

    void ConnectionMbedTls::ReceiveStreamMbedTls::Extract(infra::ByteRange range)
    {
        std::copy(connection.receiveBuffer.begin() + sizeRead, connection.receiveBuffer.begin() + sizeRead + range.size(), range.begin());
        sizeRead += range.size();
    }

    uint8_t ConnectionMbedTls::ReceiveStreamMbedTls::ExtractOne()
    {
        return connection.receiveBuffer[sizeRead++];
    }

    uint8_t ConnectionMbedTls::ReceiveStreamMbedTls::Peek()
    {
        return connection.receiveBuffer[sizeRead];
    }

    infra::ConstByteRange ConnectionMbedTls::ReceiveStreamMbedTls::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange result = infra::Head(connection.receiveBuffer.contiguous_range(connection.receiveBuffer.begin() + sizeRead), max);
        sizeRead += result.size();
        return result;
    }

    bool ConnectionMbedTls::ReceiveStreamMbedTls::IsEmpty() const
    {
        return connection.receiveBuffer.size() == sizeRead;
    }

    std::size_t ConnectionMbedTls::ReceiveStreamMbedTls::SizeAvailable() const
    {
        return connection.receiveBuffer.size() - sizeRead;
    }

    ConnectionMbedTlsListener::ConnectionMbedTlsListener(AllocatorConnectionMbedTls& allocator, ZeroCopyServerConnectionObserverFactory& factory, hal::SynchronousRandomDataGenerator& randomDataGenerator)
        : allocator(allocator)
        , factory(factory)
        , randomDataGenerator(randomDataGenerator)
    {}

    infra::SharedPtr<ZeroCopyConnectionObserver> ConnectionMbedTlsListener::ConnectionAccepted(ZeroCopyConnection& newConnection)
    {
        infra::SharedPtr<ConnectionMbedTls> connection = allocator.Allocate(newConnection, randomDataGenerator, true);
        if (connection)
        {
            infra::SharedPtr<ZeroCopyConnectionObserver> observer = factory.ConnectionAccepted(*connection);
            if (observer)
            {
                connection->SetOwnership(nullptr, observer);    // We are being held alive by another Connection object
                return connection;
            }
        }

        return nullptr;
    }

    void ConnectionMbedTlsListener::SetListener(infra::SharedPtr<void> listener)
    {
        this->listener = listener;
    }

    ConnectionMbedTlsConnector::ConnectionMbedTlsConnector(AllocatorConnectionMbedTls& allocator, ZeroCopyClientConnectionObserverFactory& factory, hal::SynchronousRandomDataGenerator& randomDataGenerator)
        : allocator(allocator)
        , factory(factory)
        , randomDataGenerator(randomDataGenerator)
    {}

    infra::SharedPtr<ZeroCopyConnectionObserver> ConnectionMbedTlsConnector::ConnectionEstablished(ZeroCopyConnection& newConnection)
    {
        infra::SharedPtr<ConnectionMbedTls> connection = allocator.Allocate(newConnection, randomDataGenerator, false);
        if (connection)
        {
            infra::SharedPtr<ZeroCopyConnectionObserver> observer = factory.ConnectionEstablished(*connection);
            if (observer)
            {
                connection->SetOwnership(nullptr, observer);    // We are being held alive by another Connection object
                return connection;
            }
        }

        return nullptr;
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
        ZeroCopyConnectionFactory& factory, hal::SynchronousRandomDataGenerator& randomDataGenerator)
        : connectionAllocator(connectionAllocator)
        , listenerAllocator(listenerAllocator)
        , connectorAllocator(connectorAllocator)
        , factory(factory)
        , randomDataGenerator(randomDataGenerator)
    {}

    infra::SharedPtr<void> ConnectionFactoryMbedTls::Listen(uint16_t port, ZeroCopyServerConnectionObserverFactory& connectionObserverFactory)
    {
        infra::SharedPtr<ConnectionMbedTlsListener> listener = listenerAllocator.Allocate(connectionAllocator, connectionObserverFactory, randomDataGenerator);

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

    infra::SharedPtr<void> ConnectionFactoryMbedTls::Connect(IPv4Address address, uint16_t port, ZeroCopyClientConnectionObserverFactory& connectionObserverFactory)
    {
        infra::SharedPtr<ConnectionMbedTlsConnector> connector = connectorAllocator.Allocate(connectionAllocator, connectionObserverFactory, randomDataGenerator);

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
