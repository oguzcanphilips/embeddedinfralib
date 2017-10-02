#ifndef SERVICES_CONNECTION_MBED_TLS_HPP
#define SERVICES_CONNECTION_MBED_TLS_HPP

#include "hal/synchronous_interfaces/SynchronousRandomDataGenerator.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "infra/util/SharedObjectAllocatorFixedSize.hpp"
#include "infra/util/SharedOptional.hpp"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_cache.h"
#include "services/network/Connection.hpp"

namespace services
{
    class MbedTlsCertificates
    {
    public:
        MbedTlsCertificates();
        MbedTlsCertificates(const MbedTlsCertificates& other) = delete;
        MbedTlsCertificates& operator=(const MbedTlsCertificates& other) = delete;
        ~MbedTlsCertificates();

        void AddCertificateAuthority(const infra::BoundedConstString& certificate);
        void AddOwnCertificate(const infra::BoundedConstString& certificate, const infra::BoundedConstString& privateKey);

        void Config(mbedtls2_ssl_config& sslConfig);

    private:
        mbedtls2_x509_crt caCertificates;
        mbedtls2_x509_crt ownCertificate;
        mbedtls2_pk_context privateKey;
    };

    class ConnectionMbedTls
        : public Connection
        , public ConnectionObserver
        , public infra::EnableSharedFromThis<ConnectionMbedTls>
    {
    public:
        ConnectionMbedTls(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator,
            bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession);
        ~ConnectionMbedTls();

        // ConnectionObserver
        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) override;
        virtual void DataReceived() override;
        virtual void ClosingConnection() override;

        // Connection
        virtual void RequestSendStream(std::size_t sendSize) override;
        virtual std::size_t MaxSendStreamSize() const override;
        virtual infra::SharedPtr<infra::DataInputStream> ReceiveStream() override;
        virtual void AckReceived() override;

        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;

        virtual IPv4Address Ipv4Address() const override;

        virtual void TlsInitFailure(int reason);
        virtual void TlsReadFailure(int reason);
        virtual void TlsWriteFailure(int reason);
        virtual void TlsLog(int level, const char* file, int line, const char* message);

    private:
        void TryAllocateSendStream();
        void TryAllocateEncryptedSendStream();
        static int StaticSslSend(void* context, const unsigned char* buffer, std::size_t size);
        static int StaticSslReceive(void *context, unsigned char* buffer, size_t size);
        int SslSend(infra::ConstByteRange buffer);
        int SslReceive(infra::ByteRange buffer);
        void TrySend();
        static int StaticGenerateRandomData(void* data, unsigned char* output, std::size_t size);
        void GenerateRandomData(infra::ByteRange data);
        static void StaticDebugWrapper(void* context, int level, const char* file, int line, const char* message);

    private:
        class StreamWriterMbedTls
            : public infra::StreamWriter
        {
        public:
            explicit StreamWriterMbedTls(ConnectionMbedTls& connection);
            ~StreamWriterMbedTls();

        private:
            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;

        private:
            ConnectionMbedTls& connection;
            std::size_t sent = 0;
        };

        class StreamReaderMbedTls
            : public infra::StreamReader
        {
        public:
            explicit StreamReaderMbedTls(ConnectionMbedTls& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool Empty() const override;
            virtual std::size_t Available() const override;

        private:
            ConnectionMbedTls& connection;
            std::size_t sizeRead = 0;
        };

    private:
        hal::SynchronousRandomDataGenerator& randomDataGenerator;
        bool server;
        mbedtls2_ssl_session* clientSession;
        mbedtls2_ssl_context sslContext;
        mbedtls2_ssl_config sslConfig;
        mbedtls2_ctr_drbg_context ctr_drbg;

        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> receiveBuffer;
        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> sendBuffer;

        infra::SharedOptional<infra::DataOutputStream::WithWriter<StreamWriterMbedTls>> sendStream;
        std::size_t requestedSendSize = 0;
        bool flushScheduled = false;
        infra::SharedOptional<infra::DataInputStream::WithReader<StreamReaderMbedTls>> receiveStream;

        infra::SharedPtr<infra::DataOutputStream> encryptedSendStream;
        std::size_t encryptedSendStreamSize = 0;

        bool initialHandshake = true;
    };

    using AllocatorConnectionMbedTls = infra::SharedObjectAllocator<ConnectionMbedTls,
        void(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator,
            bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession)>;

    class ConnectionMbedTlsListener
        : public ServerConnectionObserverFactory
    {
    public:
        ConnectionMbedTlsListener(AllocatorConnectionMbedTls& allocator, ServerConnectionObserverFactory& factory,
            MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_cache_context& serverCache);

        virtual infra::SharedPtr<ConnectionObserver> ConnectionAccepted(Connection& newConnection) override;

        void SetListener(infra::SharedPtr<void> listener);

    private:
        AllocatorConnectionMbedTls& allocator;
        ServerConnectionObserverFactory& factory;
        MbedTlsCertificates& certificates;
        hal::SynchronousRandomDataGenerator& randomDataGenerator;
        mbedtls2_ssl_cache_context& serverCache;
        infra::SharedPtr<void> listener;
    };

    using AllocatorConnectionMbedTlsListener = infra::SharedObjectAllocator<ConnectionMbedTlsListener,
        void(AllocatorConnectionMbedTls& allocator, ServerConnectionObserverFactory& factory, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_cache_context& serverCache)>;

    class ConnectionMbedTlsConnector
        : public ClientConnectionObserverFactory
    {
    public:
        ConnectionMbedTlsConnector(AllocatorConnectionMbedTls& allocator, ClientConnectionObserverFactory& factory,
            MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_session& clientSession);

        virtual infra::SharedPtr<ConnectionObserver> ConnectionEstablished(Connection& newConnection) override;
        virtual void ConnectionFailed(ConnectFailReason reason) override;

        void SetConnector(infra::SharedPtr<void> connector);

    private:
        AllocatorConnectionMbedTls& allocator;
        ClientConnectionObserverFactory& factory;
        MbedTlsCertificates& certificates;
        hal::SynchronousRandomDataGenerator& randomDataGenerator;
        mbedtls2_ssl_session& clientSession;
        infra::SharedPtr<void> connector;
    };

    using AllocatorConnectionMbedTlsConnector = infra::SharedObjectAllocator<ConnectionMbedTlsConnector,
        void(AllocatorConnectionMbedTls& allocator, ClientConnectionObserverFactory& factory, MbedTlsCertificates& certificates,
            hal::SynchronousRandomDataGenerator& randomDataGenerator, mbedtls2_ssl_session& clientSession)>;

#ifdef _MSC_VER                                                                                                         //TICS !POR#021
#pragma warning(disable:4503)                                                                                           //TICS !POR#018
#endif

    class ConnectionFactoryMbedTls
        : public ConnectionFactory
    {
    public:
        template<std::size_t MaxConnections, std::size_t MaxListeners, std::size_t MaxConnectors>
            using WithMaxConnectionsListenersAndConnectors = infra::WithStorage<infra::WithStorage<infra::WithStorage<ConnectionFactoryMbedTls
                , AllocatorConnectionMbedTls::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnections>>
                , AllocatorConnectionMbedTlsListener::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxListeners>>
                , AllocatorConnectionMbedTlsConnector::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnectors>>;

        ConnectionFactoryMbedTls(AllocatorConnectionMbedTls& connectionAllocator, AllocatorConnectionMbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator,
            ConnectionFactory& factory, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator);
        ~ConnectionFactoryMbedTls();

        virtual infra::SharedPtr<void> Listen(uint16_t port, ServerConnectionObserverFactory& connectionObserverFactory) override;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& connectionObserverFactory) override;

    private:
        AllocatorConnectionMbedTls& connectionAllocator;
        AllocatorConnectionMbedTlsListener& listenerAllocator;
        AllocatorConnectionMbedTlsConnector& connectorAllocator;
        ConnectionFactory& factory;
        MbedTlsCertificates& certificates;
        hal::SynchronousRandomDataGenerator& randomDataGenerator;
        mbedtls2_ssl_cache_context serverCache;
        mbedtls2_ssl_session clientSession = {};
    };
}

#endif
