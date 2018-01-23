#include "infra/util/CompareMembers.hpp"
#include "services/network/Address.hpp"

namespace services
{
    IPv4Address IPv4AddressLocalHost()
    {
        return IPv4Address{ 127, 0, 0, 1 };
    }

    IPv6Address IPv6AddressLocalHost()
    {
        return IPv6Address{ 0, 0, 0, 0, 0, 0, 0, 1 };
    }

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

namespace infra
{
    infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const services::IPv4Address& address)
    {
        stream << address[0] << "." << address[1] << "." << address[2] << "." << address[3];

        return stream;
    }

    infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const services::IPv6Address& address)
    {
        stream << infra::hex << address[0] << ":" << address[1] << ":" << address[2] << ":" << address[3] << ":" << address[4] << ":" << address[5] << ":" << address[6] << ":" << address[7];

        return stream;
    }

    infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const services::IPAddress& address)
    {
        if (address.Is<services::IPv4Address>())
            stream << address.Get<services::IPv4Address>();
        else
            stream << address.Get<services::IPv6Address>();

        return stream;
    }
}
