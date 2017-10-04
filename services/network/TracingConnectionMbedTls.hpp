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
        TracingConnectionMbedTls(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession, Tracer& tracer);

        virtual void TlsInitFailure(int reason) override;
        virtual void TlsReadFailure(int reason) override;
        virtual void TlsWriteFailure(int reason) override;
        virtual void TlsLog(int level, const char* file, int line, const char* message) override;

    private:
        void LogFailure(const char* what, int reason);

    private:
        Tracer& tracer;
    };

    using AllocatorTracingConnectionMbedTls = infra::SharedObjectAllocator<TracingConnectionMbedTls,
        void(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession, Tracer& tracer)>;

    class AllocatorTracingConnectionMbedTlsAdapter
        : public AllocatorConnectionMbedTls
    {
    public:
        AllocatorTracingConnectionMbedTlsAdapter(AllocatorTracingConnectionMbedTls& allocator, Tracer& tracer);

        virtual infra::SharedPtr<ConnectionMbedTls> Allocate(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver,
            MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, bool server, mbedtls2_ssl_cache_context* serverCache, mbedtls2_ssl_session* clientSession) override;

    private:
        AllocatorTracingConnectionMbedTls& allocator;
        Tracer& tracer;
    };

    class TracingConnectionFactoryMbedTls
        : public ConnectionFactoryMbedTls
    {
    public:
    	enum DebugLevel
    	{
    		NoDebug,
    		Error,
    		StateChange,
    		Informational,
    		Verbose
    	};

    public:
        template<std::size_t MaxConnections, std::size_t MaxListeners, std::size_t MaxConnectors>
        using WithMaxConnectionsListenersAndConnectors = infra::WithStorage<infra::WithStorage<infra::WithStorage<TracingConnectionFactoryMbedTls
            , AllocatorTracingConnectionMbedTls::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnections>>
            , AllocatorConnectionMbedTlsListener::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxListeners>>
            , AllocatorConnectionMbedTlsConnector::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnectors>>;

        TracingConnectionFactoryMbedTls(AllocatorTracingConnectionMbedTls& connectionAllocator, AllocatorConnectionMbedTlsListener& listenerAllocator, AllocatorConnectionMbedTlsConnector& connectorAllocator, //TICS !OLC#020
            ConnectionFactory& factory, MbedTlsCertificates& certificates, hal::SynchronousRandomDataGenerator& randomDataGenerator, Tracer& tracer, DebugLevel level);

    private:
        AllocatorTracingConnectionMbedTlsAdapter allocatorAdapter;
    };
}

#endif
