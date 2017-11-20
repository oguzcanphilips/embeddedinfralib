#include "lwip/lwip_cpp/LightweightIp.hpp"
#include "lwip/init.h"
#include "lwip/timeouts.h"

namespace services
{
    LightweightIp::LightweightIp(AllocatorListenerLwIp& listenerAllocator, AllocatorConnectorLwIp& connectorAllocator, AllocatorConnectionLwIp& connectionAllocator)
        : ConnectionFactoryLwIp(listenerAllocator, connectorAllocator, connectionAllocator)
    {
        lwip_init();
        sysCheckTimer.Start(std::chrono::milliseconds(50), [this]() { sys_check_timeouts(); }, infra::triggerImmediately);
    }
}
