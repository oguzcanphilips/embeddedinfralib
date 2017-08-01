#include "services/network/TracingConnectionMbedTls.hpp"

namespace services
{
    TracingConnectionMbedTls::TracingConnectionMbedTls(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession, Tracer& tracer)
        : ConnectionMbedTls(connection, certificates, randomDataGenerator, server, serverCache, clientSession)
        , tracer(tracer)
    {}

    void TracingConnectionMbedTls::TlsReadFailure(int reason)
    {
        tracer.Trace() << "TracingConnectionMbedTls::TlsReadFailure: 0x" << infra::hex << infra::Width(4, '0') << reason;
    }

    void TracingConnectionMbedTls::TlsWriteFailure(int reason)
    {
        tracer.Trace() << "TracingConnectionMbedTls::TlsWTriteFailure: 0x" << infra::hex << infra::Width(4, '0') << reason;
    }

    AllocatorTracingConnectionMbedTlsAdapter::AllocatorTracingConnectionMbedTlsAdapter(AllocatorTracingConnectionMbedTls& allocator, Tracer& tracer)
        : allocator(allocator)
        , tracer(tracer)
    {}

    infra::SharedPtr<ConnectionMbedTls> AllocatorTracingConnectionMbedTlsAdapter::Allocate(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator,
        bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession)
    {
        return allocator.Allocate(connection, certificates, randomDataGenerator, server, serverCache, clientSession, tracer);
    }

    TracingConnectionFactoryMbedTls::TracingConnectionFactoryMbedTls(AllocatorTracingConnectionMbedTls& connectionAllocator, AllocatorConnectionMbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator,
        ConnectionFactory& factory, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, Tracer& tracer)
        : ConnectionFactoryMbedTls(allocatorAdapter, listenerAllocator, connectorAllocator, factory, certificates, randomDataGenerator)
        , allocatorAdapter(connectionAllocator, tracer)
    {}
}
