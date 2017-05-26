#include <cstdlib>
#include <cstring>
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/CompareMembers.hpp"
#include "lwip/lwip_cpp/LightweightIpOverEthernet.hpp"
#include "netif/etharp.h"
#include "lwip/dhcp.h"

namespace services
{
    bool IPv4Addresses::operator==(const IPv4Addresses& other) const
    {
        return infra::Equals()
            (address, other.address)
            (netmask, other.netmask)
            (gateway, other.gateway);
    }

    bool IPv4Addresses::operator!=(const IPv4Addresses& other) const
    {
        return !(*this == other);
    }

    bool IpConfig::operator==(const IpConfig& other) const
    {
        return infra::Equals()
            (useDhcp, other.useDhcp)
            (staticAddresses, other.staticAddresses);
    }

    bool IpConfig::operator!=(const IpConfig& other) const
    {
        return !(*this == other);
    }

    LightweightIpOverEthernet::LightweightIpOverEthernet(hal::EthernetMac& ethernet, netif& netInterface)
        : hal::EthernetMacObserver(ethernet)
        , netInterface(netInterface)
    {
        netif_set_link_up(&netInterface);
    }

    LightweightIpOverEthernet::~LightweightIpOverEthernet()
    {
        if (currentReceiveBufferFirst)
            pbuf_free(currentReceiveBufferFirst);

        netif_set_link_down(&netInterface);
    }

    infra::ByteRange LightweightIpOverEthernet::RequestReceiveBuffer()
    {
        pbuf* buffer = pbuf_alloc(PBUF_RAW, LWIP_MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE), PBUF_POOL);
        if (buffer == nullptr)
        {
            retryAllocationTimer.Start(std::chrono::milliseconds(250), [this]() { Subject().RetryAllocation(); });
            return infra::ByteRange();
        }

        assert(buffer->next == nullptr);

        if (currentReceiveBufferFirst == nullptr)
            currentReceiveBufferFirst = buffer;
        else
            currentReceiveBufferLast->next = buffer;
        currentReceiveBufferLast = buffer;

        return infra::ByteRange(reinterpret_cast<uint8_t*>(buffer->payload), reinterpret_cast<uint8_t*>(buffer->payload) + buffer->len);
    }

    void LightweightIpOverEthernet::ReceivedFrame(uint32_t usedBuffers, uint32_t frameSize)
    {
        assert(currentReceiveBufferFirst != nullptr);

        pbuf* end = currentReceiveBufferFirst;
        end->tot_len = frameSize;
        while (--usedBuffers != 0)
        {
            frameSize -= end->len;
            end = end->next;
        }

        assert(end != nullptr);
        end->len = frameSize;

        pbuf* newBegin = end->next;
        end->next = nullptr;

        err_t err = netInterface.input(currentReceiveBufferFirst, &netInterface);

        if (err != ERR_OK)
            pbuf_free(currentReceiveBufferFirst);

        currentReceiveBufferFirst = newBegin;
        if (currentReceiveBufferFirst == nullptr)
            currentReceiveBufferLast = nullptr;
    }

    void LightweightIpOverEthernet::ReceivedErrorFrame(uint32_t usedBuffers, uint32_t frameSize)
    {
        pbuf* end = currentReceiveBufferFirst;
        while (--usedBuffers != 0)
            end = end->next;

        assert(end != nullptr);
        pbuf* newBegin = end->next;
        end->next = nullptr;

        pbuf_free(currentReceiveBufferFirst);

        currentReceiveBufferFirst = newBegin;
        if (currentReceiveBufferFirst == nullptr)
            currentReceiveBufferLast = nullptr;
    }

    void LightweightIpOverEthernet::SentFrame()
    {
        pbuf_free(currentSendFrames.front());
        currentSendFrames.pop_front();
        sending = false;

        if (!currentSendFrames.empty())
            SendOneFrame();
    }

    err_t LightweightIpOverEthernet::Output(pbuf* p)
    {
        if (!currentSendFrames.full())
        {
            pbuf_ref(p);
            currentSendFrames.push_back(p);
            if (!sending)
                SendOneFrame();

            return ERR_OK;
        }
        else
            return ERR_MEM;
    }

    void LightweightIpOverEthernet::SendOneFrame()
    {
        sending = true;
        pbuf* buffer = currentSendFrames.front();

        while (true)
        {
            bool lastOfFrame = buffer->tot_len == buffer->len;

            Subject().SendBuffer(infra::ConstByteRange(static_cast<const uint8_t*>(buffer->payload), static_cast<const uint8_t*>(buffer->payload) + buffer->len)
                , lastOfFrame);

            if (lastOfFrame)
                break;

            buffer = buffer->next;
        }
    }

    LightweightIpOverEthernetFactory::LightweightIpOverEthernetFactory(hal::MacAddress macAddress, const Config& config)
        : macAddress(macAddress)
        , config(config)
    {
        ip_addr_t ipaddr = IPADDR4_INIT(0);
        ip_addr_t netmask = IPADDR4_INIT(0);
        ip_addr_t gw = IPADDR4_INIT(0);

        if (!config.ipConfig.useDhcp)
        {
            IP4_ADDR(&ipaddr, config.ipConfig.staticAddresses.address[0], config.ipConfig.staticAddresses.address[1], config.ipConfig.staticAddresses.address[2], config.ipConfig.staticAddresses.address[3]);
            IP4_ADDR(&netmask, config.ipConfig.staticAddresses.netmask[0], config.ipConfig.staticAddresses.netmask[1], config.ipConfig.staticAddresses.netmask[2], config.ipConfig.staticAddresses.netmask[3]);
            IP4_ADDR(&gw, config.ipConfig.staticAddresses.gateway[0], config.ipConfig.staticAddresses.gateway[1], config.ipConfig.staticAddresses.gateway[2], config.ipConfig.staticAddresses.gateway[3]);
        }

        netif_add(&netInterface, &ipaddr, &netmask, &gw, this, &LightweightIpOverEthernetFactory::StaticInit, &ethernet_input);
        netif_set_default(&netInterface);

        if (config.ipConfig.useDhcp)
            dhcp_start(&netInterface);
    }

    LightweightIpOverEthernetFactory::~LightweightIpOverEthernetFactory()
    {
        if (config.ipConfig.useDhcp)
            dhcp_stop(&netInterface);
        netif_remove(&netInterface);
    }

    void LightweightIpOverEthernetFactory::Create(hal::EthernetMac& ethernet)
    {
        ethernetStack.Emplace(ethernet, netInterface);
    }

    void LightweightIpOverEthernetFactory::Destroy()
    {
        ethernetStack = infra::none;
    }

    hal::MacAddress LightweightIpOverEthernetFactory::MacAddress() const
    {
        return macAddress;
    }

    err_t LightweightIpOverEthernetFactory::Init()
    {
        netInterface.name[0] = config.ifName[0];
        netInterface.name[1] = config.ifName[1];

        netInterface.output = etharp_output;
        netInterface.linkoutput = &LightweightIpOverEthernetFactory::StaticOutput;

        netInterface.hwaddr_len = static_cast<uint8_t>(macAddress.size());
        netInterface.hwaddr[0] = macAddress[0];
        netInterface.hwaddr[1] = macAddress[1];
        netInterface.hwaddr[2] = macAddress[2];
        netInterface.hwaddr[3] = macAddress[3];
        netInterface.hwaddr[4] = macAddress[4];
        netInterface.hwaddr[5] = macAddress[5];

        netInterface.mtu = 1500;
        netInterface.flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_UP;

        return ERR_OK;
    }

    err_t LightweightIpOverEthernetFactory::Output(pbuf* p)
    {
        if (ethernetStack)
            return ethernetStack->Output(p);
        else
            return ERR_IF;
    }

    err_t LightweightIpOverEthernetFactory::StaticOutput(netif* netif, pbuf* buffer)
    {
        return static_cast<LightweightIpOverEthernetFactory*>(netif->state)->Output(buffer);
    }

    err_t LightweightIpOverEthernetFactory::StaticInit(netif* netif)
    {
        return static_cast<LightweightIpOverEthernetFactory*>(netif->state)->Init();
    }
}
