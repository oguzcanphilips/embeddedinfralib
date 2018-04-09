#include "lwip/lwip_cpp/LightweightIp.hpp"
#include "lwip/init.h"
#include "lwip/timeouts.h"

extern "C" uint32_t StaticLwIpRand()
{
    return services::LightweightIp::Instance().Rand();
}

namespace services
{
    LightweightIp::LightweightIp(AllocatorListenerLwIp& listenerAllocator, AllocatorConnectorLwIp& connectorAllocator, AllocatorConnectionLwIp& connectionAllocator, hal::SynchronousRandomDataGenerator& randomDataGenerator)
        : ConnectionFactoryLwIp(listenerAllocator, connectorAllocator, connectionAllocator)
        , randomDataGenerator(randomDataGenerator)
    {
		tcpip_init();
    }

    uint32_t LightweightIp::Rand()
    {
        uint32_t result;
        randomDataGenerator.GenerateRandomData(infra::MakeByteRange(result));
        return result;
    }
}
