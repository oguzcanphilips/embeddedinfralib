#ifndef SERVICES_ADDRESS_HPP
#define SERVICES_ADDRESS_HPP

#include "infra/util/Variant.hpp"
#include <array>

namespace services
{
    using IPv4Address = std::array<uint8_t, 4>;
    using IPv6Address = std::array<uint16_t, 8>;
    using IPAddress = infra::Variant<IPv4Address, IPv6Address>;

    IPv4Address IPv4AddressLocalHost();
    IPv6Address IPv6AddressLocalHost();

    struct IPv4InterfaceAddresses
    {
        IPv4Address address;
        IPv4Address netmask;
        IPv4Address gateway;

        bool operator==(const IPv4InterfaceAddresses& other) const;
        bool operator!=(const IPv4InterfaceAddresses& other) const;
    };

    struct Ipv4Config
    {
        bool useDhcp;
        IPv4InterfaceAddresses staticAddresses;

        bool operator==(const Ipv4Config& other) const;
        bool operator!=(const Ipv4Config& other) const;
    };
}

#endif
