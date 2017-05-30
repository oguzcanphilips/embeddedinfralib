#include "infra/util/CompareMembers.hpp"
#include "services/network/Address.hpp"

namespace services
{
    bool IPv4InterfaceAddresses::operator==(const IPv4InterfaceAddresses& other) const
    {
        return infra::Equals()
            (address, other.address)
            (netmask, other.netmask)
            (gateway, other.gateway);
    }

    bool IPv4InterfaceAddresses::operator!=(const IPv4InterfaceAddresses& other) const
    {
        return !(*this == other);
    }

    bool Ipv4Config::operator==(const Ipv4Config& other) const
    {
        return infra::Equals()
            (useDhcp, other.useDhcp)
            (staticAddresses, other.staticAddresses);
    }

    bool Ipv4Config::operator!=(const Ipv4Config& other) const
    {
        return !(*this == other);
    }
}
