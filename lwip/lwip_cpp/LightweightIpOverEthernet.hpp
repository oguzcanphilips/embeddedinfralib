#ifndef ETHERNET_LW_IP_HPP
#define ETHERNET_LW_IP_HPP

#include "hal/interfaces/Ethernet.hpp"
#include "infra/timer/Timer.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/Optional.hpp"
#include "lwip/netif.h"
#include <array>

namespace services
{
    using IPv4Address = std::array<uint8_t, 4>;

    struct IPv4Addresses
    {
        IPv4Address address;
        IPv4Address netmask;
        IPv4Address gateway;

        bool operator==(const IPv4Addresses& other) const;
        bool operator!=(const IPv4Addresses& other) const;
    };

    struct IpConfig
    {
        bool useDhcp;
        IPv4Addresses staticAddresses;

        bool operator==(const IpConfig& other) const;
        bool operator!=(const IpConfig& other) const;
    };

    class LightweightIpOverEthernet
        : hal::EthernetMacObserver
    {
    public:
        LightweightIpOverEthernet(hal::EthernetMac& ethernet, netif& netInterface);
        ~LightweightIpOverEthernet();

        virtual infra::ByteRange RequestReceiveBuffer() override;
        virtual void ReceivedFrame(uint32_t usedBuffers, uint32_t frameSize) override;
        virtual void ReceivedErrorFrame(uint32_t usedBuffers, uint32_t frameSize) override;
        virtual void SentFrame() override;

        err_t Output(pbuf* p);

    private:
        void SendOneFrame();

    private:
        netif& netInterface;
        infra::BoundedDeque<pbuf*>::WithMaxSize<32> currentSendFrames;
        bool sending = false;
        pbuf* currentReceiveBufferFirst = nullptr;
        pbuf* currentReceiveBufferLast = nullptr;
        infra::TimerSingleShot retryAllocationTimer;
    };

    class LightweightIpOverEthernetFactory
    {
    public:
        struct Config
        {
            Config() {}

            std::array<char, 2> ifName = { { 'r', 'p' } };
            IpConfig ipConfig = { true };
        };

        LightweightIpOverEthernetFactory(hal::MacAddress macAddress, const Config& config = Config());
        ~LightweightIpOverEthernetFactory();

        void Create(hal::EthernetMac& ethernet);
        void Destroy();
        hal::MacAddress MacAddress() const;

    private:
        err_t Init();
        err_t Output(pbuf* buffer);

        static err_t StaticOutput(netif* netif, pbuf* buffer);
        static err_t StaticInit(netif* netif);

    private:
        hal::MacAddress macAddress;
        Config config;
        netif netInterface;
        infra::Optional<LightweightIpOverEthernet> ethernetStack;
    };
}

#endif
