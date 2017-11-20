#ifndef LWIP_LIGHTWEIGHT_IP_HPP
#define LWIP_LIGHTWEIGHT_IP_HPP

#include "lwip/lwip_cpp/ConnectionLwIp.hpp"
#include "lwip/lwip_cpp/DatagramLwIp.hpp"

namespace services
{
    class LightweightIp
        : public ConnectionFactoryLwIp
        , public DatagramProviderLwIp
    {
    public:
        template<std::size_t MaxListeners, std::size_t MaxConnectors, std::size_t MaxConnections>
            using WithFixedAllocator = infra::WithStorage<infra::WithStorage<infra::WithStorage<LightweightIp,
                AllocatorListenerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxListeners>>,
                AllocatorConnectorLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnectors>>,
                AllocatorConnectionLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnections>>;

        LightweightIp(AllocatorListenerLwIp& listenerAllocator, AllocatorConnectorLwIp& connectorAllocator, AllocatorConnectionLwIp& connectionAllocator);

    private:
        infra::TimerRepeating sysCheckTimer;
    };
}

#endif
