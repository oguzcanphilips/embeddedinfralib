#include "lwip/lwip_cpp/MulticastLwIp.hpp"
#include "lwip/igmp.h"
#include "lwip/netif.h"
#include <cassert>

namespace services
{
    void MulticastLwIp::JoinMulticastGroup(IPv4Address multicastAddress)
    {
        ip4_addr_t groupAddress;
        IP4_ADDR(&groupAddress, multicastAddress[0], multicastAddress[1], multicastAddress[2], multicastAddress[3]);
        err_t result = igmp_joingroup_netif(netif_default, &groupAddress);
        assert(result == ERR_OK);
    }

    void MulticastLwIp::LeaveMulticastGroup(IPv4Address multicastAddress)
    {
        ip4_addr_t groupAddress;
        IP4_ADDR(&groupAddress, multicastAddress[0], multicastAddress[1], multicastAddress[2], multicastAddress[3]);
        err_t result = igmp_leavegroup_netif(netif_default, &groupAddress);
        assert(result == ERR_OK);
    }
}
