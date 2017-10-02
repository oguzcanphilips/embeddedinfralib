#include "services/network/TracingConnectionMbedTls.hpp"
#include "mbedtls/debug.h"
#include "mbedtls/error.h"

namespace services
{
    TracingConnectionMbedTls::TracingConnectionMbedTls(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession, Tracer& tracer)
        : ConnectionMbedTls(connection, certificates, randomDataGenerator, server, serverCache, clientSession)
        , tracer(tracer)
    {}

    void TracingConnectionMbedTls::TlsInitFailure(int reason)
    {
        LogFailure("TracingConnectionMbedTls::TlsInitFailure", reason);
    }

    void TracingConnectionMbedTls::TlsReadFailure(int reason)
    {
        LogFailure("TracingConnectionMbedTls::TlsReadFailure", reason);
    }

    void TracingConnectionMbedTls::TlsWriteFailure(int reason)
    {
        LogFailure("TracingConnectionMbedTls::TlsWriteFailure", reason);
    }

    void TracingConnectionMbedTls::TlsLog(int level, const char* file, int line, const char* message)
    {
    	infra::BoundedConstString fileCopy(file);

    	tracer.Trace() << "[" << fileCopy.substr(fileCopy.find_last_of('\\') + 1) << ":" << line
    			       << "] (" << level << "): " << message;
    }

    void TracingConnectionMbedTls::LogFailure(const char* what, int reason)
    {
        infra::BoundedString::WithStorage<128> description;
        mbedtls2_strerror(reason, description.data(), description.max_size());

        tracer.Trace() << what << ": " << description
            << " (-0x" << infra::hex << infra::Width(4, '0') << std::abs(reason) << ")";
    }

    AllocatorTracingConnectionMbedTlsAdapter::AllocatorTracingConnectionMbedTlsAdapter(AllocatorTracingConnectionMbedTls& allocator, Tracer& tracer)
        : allocator(allocator)
        , tracer(tracer)
    {
        tracer.Trace() << "AllocatorTracingConnectionMbedTlsAdapter::AllocatorTracingConnectionMbedTlsAdapter()";
    }

    infra::SharedPtr<ConnectionMbedTls> AllocatorTracingConnectionMbedTlsAdapter::Allocate(Connection& connection, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator,
        bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession)
    {
        tracer.Trace() << "AllocatorTracingConnectionMbedTlsAdapter::Allocate()";
        return allocator.Allocate(connection, certificates, randomDataGenerator, server, serverCache, clientSession, tracer);
    }

    TracingConnectionFactoryMbedTls::TracingConnectionFactoryMbedTls(AllocatorTracingConnectionMbedTls& connectionAllocator, AllocatorConnectionMbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator,
        ConnectionFactory& factory, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, Tracer& tracer, DebugLevel level)
        : ConnectionFactoryMbedTls(allocatorAdapter, listenerAllocator, connectorAllocator, factory, certificates, randomDataGenerator)
        , allocatorAdapter(connectionAllocator, tracer)
    {
        tracer.Trace() << "TracingConnectionFactoryMbedTls::TracingConnectionFactoryMbedTls()";
        mbedtls2_debug_set_threshold(level);
    }
}
