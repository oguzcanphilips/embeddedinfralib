#ifndef SERVICES_TRACING_CONNECTION_MBED_TLS_HPP
#define SERVICES_TRACING_CONNECTION_MBED_TLS_HPP

#include "services/network/ConnectionMbedTls.hpp"
#include "services/tracer/Tracer.hpp"

namespace services
{
    class TracingConnectionMbedTls
        : public ConnectionMbedTls
    {
    public:
        TracingConnectionMbedTls(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession, Tracer& tracer);

        virtual void TlsReadFailure(int reason) override;
        virtual void TlsWriteFailure(int reason) override;

    private:
        Tracer& tracer;
    };

    using AllocatorTracingConnectionMbedTls = infra::SharedObjectAllocator<TracingConnectionMbedTls,
        void(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession, Tracer& tracer)>;

    class AllocatorTracingConnectionMbedTlsAdapter
        : public AllocatorConnectionMbedTls
    {
    public:
        AllocatorTracingConnectionMbedTlsAdapter(AllocatorTracingConnectionMbedTls& allocator, Tracer& tracer);

        virtual infra::SharedPtr<ConnectionMbedTls> Allocate(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator,
            bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession) override;

    private:
        AllocatorTracingConnectionMbedTls& allocator;
        Tracer& tracer;
    };

    class TracingConnectionFactoryMbedTls
        : public ConnectionFactoryMbedTls
    {
    public:
        template<std::size_t MaxConnections, std::size_t MaxListeners, std::size_t MaxConnectors>
        using WithMaxConnectionsListenersAndConnectors = infra::WithStorage<infra::WithStorage<infra::WithStorage<TracingConnectionFactoryMbedTls
            , AllocatorTracingConnectionMbedTls::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnections>>
            , AllocatorConnectionMbedTlsListener::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxListeners>>
            , AllocatorConnectionMbedTlsConnector::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnectors>>;

        TracingConnectionFactoryMbedTls(AllocatorTracingConnectionMbedTls& connectionAllocator, AllocatorConnectionMbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator,
            ConnectionFactory& factory, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, Tracer& tracer);

    private:
        AllocatorTracingConnectionMbedTlsAdapter allocatorAdapter;
    };
}

#endif
