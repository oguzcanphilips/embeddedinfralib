#include "services/network/TracingConnectionMbedTls.hpp"
#include "mbedtls/debug.h"
#include "mbedtls/error.h"

namespace services
{
    TracingConnectionMbedTls::TracingConnectionMbedTls(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver,
        CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, const ConnectionMbedTls::ParametersWorkaround& parameters, Tracer& tracer)
        : ConnectionMbedTls(std::move(createdObserver), certificates, randomDataGenerator, parameters)
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

    void TracingConnectionMbedTls::ReceivedFirst(std::size_t size)
    {
        tracer.Trace() << "TracingConnectionMbedTls::ReceivedFirst: " << size;
    }

    void TracingConnectionMbedTls::SentFirst(std::size_t size)
    {
        tracer.Trace() << "TracingConnectionMbedTls::SentFirst: " << size;
    }

    void TracingConnectionMbedTls::LogFailure(const char* what, int reason)
    {
        tracer.Trace() << what << ": ";

#if defined(MBEDTLS_ERROR_C)
        infra::BoundedString::WithStorage<128> description;
        mbedtls2_strerror(reason, description.data(), description.max_size());

        tracer.Continue() << description;
#endif

        tracer.Continue() << " (-0x" << infra::hex << infra::Width(4, '0') << std::abs(reason) << ")";
    }

    AllocatorTracingConnectionMbedTlsAdapter::AllocatorTracingConnectionMbedTlsAdapter(AllocatorTracingConnectionMbedTls& allocator, Tracer& tracer)
        : allocator(allocator)
        , tracer(tracer)
    {
        tracer.Trace() << "AllocatorTracingConnectionMbedTlsAdapter::AllocatorTracingConnectionMbedTlsAdapter()";
    }

    infra::SharedPtr<ConnectionMbedTls> AllocatorTracingConnectionMbedTlsAdapter::Allocate(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver,
        CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, const ConnectionMbedTls::ParametersWorkaround& parameters)
    {
        tracer.Trace() << "AllocatorTracingConnectionMbedTlsAdapter::Allocate()";
        return allocator.Allocate(std::move(createdObserver), certificates, randomDataGenerator, parameters, tracer);
    }

    TracingConnectionFactoryMbedTls::TracingConnectionFactoryMbedTls(AllocatorTracingConnectionMbedTls& connectionAllocator, AllocatorConnectionMbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator,
        ConnectionFactory& factory, CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, Tracer& tracer, DebugLevel level, bool needsAuthenticationDefault)
        : ConnectionFactoryMbedTls(allocatorAdapter, listenerAllocator, connectorAllocator, factory, certificates, randomDataGenerator, needsAuthenticationDefault)
        , allocatorAdapter(connectionAllocator, tracer)
    {
        tracer.Trace() << "TracingConnectionFactoryMbedTls::TracingConnectionFactoryMbedTls()";
#if defined(MBEDTLS_DEBUG_C)
        mbedtls2_debug_set_threshold(level);
#endif
    }

    TracingConnectionIPv6FactoryMbedTls::TracingConnectionIPv6FactoryMbedTls(AllocatorTracingConnectionMbedTls& connectionAllocator, AllocatorConnectionIPv6MbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator,
        ConnectionIPv6Factory& factory, CertificatesMbedTls& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, Tracer& tracer, DebugLevel level, bool needsAuthenticationDefault)
        : ConnectionIPv6FactoryMbedTls(allocatorAdapter, listenerAllocator, connectorAllocator, factory, certificates, randomDataGenerator, needsAuthenticationDefault)
        , allocatorAdapter(connectionAllocator, tracer)
    {
        tracer.Trace() << "TracingConnectionIPv6FactoryMbedTls::TracingConnectionIPv6FactoryMbedTls()";
#if defined(MBEDTLS_DEBUG_C)
        mbedtls2_debug_set_threshold(level);
#endif
    }
}
