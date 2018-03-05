#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "lwip/lwip_cpp/DatagramLwIp.hpp"

namespace services
{
    DatagramSenderPeerLwIp::DatagramSenderPeerLwIp(DatagramSenderObserver& sender, IPAddress address, uint16_t port)
        : sender(sender)
        , state(infra::InPlaceType<StateIdle>(), *this)
    {
        if (address.Is<IPv4Address>())
        {
            control = udp_new_ip_type(IPADDR_TYPE_V4);
            assert(control != nullptr);
            IPv4Address ipv4Address = address.Get<IPv4Address>();
            ip_addr_t ipAddress IPADDR4_INIT(0);
            IP4_ADDR(&ipAddress.u_addr.ip4, ipv4Address[0], ipv4Address[1], ipv4Address[2], ipv4Address[3]);
            err_t result = udp_connect(control, &ipAddress, port);
            assert(result == ERR_OK);
        }
        else
        {
            control = udp_new_ip_type(IPADDR_TYPE_V6);
            assert(control != nullptr);
            IPv6Address ipv6Address = address.Get<IPv6Address>();
            ip_addr_t ipAddress IPADDR6_INIT(0, 0, 0, 0);
            IP6_ADDR(&ipAddress.u_addr.ip6, PP_HTONL(ipv6Address[1] + (static_cast<uint32_t>(ipv6Address[0]) << 16)), PP_HTONL(ipv6Address[3] + (static_cast<uint32_t>(ipv6Address[2]) << 16)), PP_HTONL(ipv6Address[5] + (static_cast<uint32_t>(ipv6Address[4]) << 16)), PP_HTONL(ipv6Address[7] + (static_cast<uint32_t>(ipv6Address[6]) << 16)));
            err_t result = udp_connect(control, &ipAddress, port);
            assert(result == ERR_OK);
        }
    }

    DatagramSenderPeerLwIp::~DatagramSenderPeerLwIp()
    {
        udp_remove(control);
    }

    void DatagramSenderPeerLwIp::RequestSendStream(std::size_t sendSize)
    {
        state->RequestSendStream(sendSize);
    }

    DatagramSenderPeerLwIp::UdpWriter::UdpWriter(udp_pcb* control, pbuf* buffer)
        : control(control)
        , buffer(buffer)
    {}

    DatagramSenderPeerLwIp::UdpWriter::~UdpWriter()
    {
        pbuf_realloc(buffer, bufferOffset);
        err_t result = udp_send(control, buffer);
        pbuf_free(buffer);
    }

    void DatagramSenderPeerLwIp::UdpWriter::Insert(infra::ConstByteRange range, infra::StreamErrorPolicy& errorPolicy)
    {
        errorPolicy.ReportResult(range.size() <= Available());
        range.shrink_from_back_to(Available());
        err_t result = pbuf_take_at(buffer, range.begin(), static_cast<uint16_t>(range.size()), bufferOffset);
        assert(result == ERR_OK);
        bufferOffset += static_cast<uint16_t>(range.size());
    }

    std::size_t DatagramSenderPeerLwIp::UdpWriter::Available() const
    {
        return buffer->tot_len - bufferOffset;
    }

    void DatagramSenderPeerLwIp::StateBase::RequestSendStream(std::size_t sendSize)
    {
        std::abort();
    }

    DatagramSenderPeerLwIp::StateIdle::StateIdle(DatagramSenderPeerLwIp& datagramSender)
        : datagramSender(datagramSender)
    {}

    void DatagramSenderPeerLwIp::StateIdle::RequestSendStream(std::size_t sendSize)
    {
        StateWaitingForBuffer& state = datagramSender.state.Emplace<StateWaitingForBuffer>(datagramSender, sendSize);
        state.TryAllocateBuffer();
    }

    DatagramSenderPeerLwIp::StateWaitingForBuffer::StateWaitingForBuffer(DatagramSenderPeerLwIp& datagramSender, std::size_t sendSize)
        : datagramSender(datagramSender)
        , sendSize(sendSize)
        , allocateTimer(std::chrono::milliseconds(50), [this]() { TryAllocateBuffer(); })
    {}

    void DatagramSenderPeerLwIp::StateWaitingForBuffer::TryAllocateBuffer()
    {
        pbuf* buffer = pbuf_alloc(PBUF_TRANSPORT, static_cast<uint16_t>(sendSize), PBUF_POOL);
        if (buffer != nullptr)
            datagramSender.state.Emplace<StateBufferAllocated>(datagramSender, buffer);
    }

    DatagramSenderPeerLwIp::StateBufferAllocated::StateBufferAllocated(DatagramSenderPeerLwIp& datagramSender, pbuf* buffer)
        : datagramSender(datagramSender)
        , stream([this]() { this->datagramSender.state.Emplace<StateIdle>(this->datagramSender); })
        , streamPtr(stream.Emplace(datagramSender.control, buffer))
    {
        infra::EventDispatcherWithWeakPtr::Instance().Schedule([this](const infra::SharedPtr<DatagramSenderPeerLwIp>& datagramSender)
        {
            datagramSender->sender.SendStreamAvailable(std::move(streamPtr));
        }, datagramSender.SharedFromThis());
    }

    DatagramReceiverPeerLwIp::DatagramReceiverPeerLwIp(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed, bool ipv6)
        : receiver(receiver)
    {
        if (!ipv6)
        {
            control = udp_new_ip_type(IPADDR_TYPE_V4);
            assert(control != nullptr);
            if (broadcastAllowed)
                ip_set_option(control, SOF_BROADCAST);
            err_t result = udp_bind(control, IP4_ADDR_ANY, port);
            assert(result == ERR_OK);
        }
        else
        {
            control = udp_new_ip_type(IPADDR_TYPE_V6);
            assert(control != nullptr);
            err_t result = udp_bind(control, IP6_ADDR_ANY, port);
            assert(result == ERR_OK);
        }
        udp_recv(control, &DatagramReceiverPeerLwIp::StaticRecv, this);
    }

    DatagramReceiverPeerLwIp::~DatagramReceiverPeerLwIp()
    {
        udp_remove(control);
    }

    void DatagramReceiverPeerLwIp::StaticRecv(void* arg, udp_pcb* pcb, pbuf* buffer, const ip_addr_t* address, u16_t port)
    {
        reinterpret_cast<DatagramReceiverPeerLwIp*>(arg)->Recv(buffer, address, port);
    }

    void DatagramReceiverPeerLwIp::Recv(pbuf* buffer, const ip_addr_t* address, u16_t port)
    {
        infra::DataInputStream::WithReader<UdpReader> stream(buffer);
        if (IP_GET_TYPE(address) == IPADDR_TYPE_V4)
            receiver.DataReceived(stream, IPv4Address{ ip4_addr1(ip_2_ip4(address)), ip4_addr2(ip_2_ip4(address)), ip4_addr3(ip_2_ip4(address)), ip4_addr4(ip_2_ip4(address)) }, port);
        else
            receiver.DataReceived(stream, IPv6Address{ IP6_ADDR_BLOCK1(ip_2_ip6(address)), IP6_ADDR_BLOCK2(ip_2_ip6(address)), IP6_ADDR_BLOCK3(ip_2_ip6(address)), IP6_ADDR_BLOCK4(ip_2_ip6(address)),
                IP6_ADDR_BLOCK5(ip_2_ip6(address)), IP6_ADDR_BLOCK6(ip_2_ip6(address)), IP6_ADDR_BLOCK7(ip_2_ip6(address)), IP6_ADDR_BLOCK8(ip_2_ip6(address))}, port );
    }

    DatagramReceiverPeerLwIp::UdpReader::UdpReader(pbuf* buffer)
        : infra::StreamReader(infra::softFail)
        , buffer(buffer)
    {}

    DatagramReceiverPeerLwIp::UdpReader::~UdpReader()
    {
        pbuf_free(buffer);
    }

    void DatagramReceiverPeerLwIp::UdpReader::Extract(infra::ByteRange range)
    {
        ReportResult(range.size() <= Available());
        range.shrink_from_back_to(Available());

        u16_t numCopied = pbuf_copy_partial(buffer, range.begin(), static_cast<uint16_t>(range.size()), bufferOffset);
        assert(numCopied == range.size());
        bufferOffset += static_cast<uint16_t>(range.size());
    }

    uint8_t DatagramReceiverPeerLwIp::UdpReader::ExtractOne()
    {
        uint8_t result;
        Extract(infra::MakeByteRange(result));
        return result;
    }

    uint8_t DatagramReceiverPeerLwIp::UdpReader::Peek()
    {
        ReportResult(!Empty());

        uint8_t result;
        pbuf_copy_partial(buffer, &result, 1, bufferOffset);
        return result;
    }

    infra::ConstByteRange DatagramReceiverPeerLwIp::UdpReader::ExtractContiguousRange(std::size_t max)
    {
        uint16_t offset = bufferOffset;
        pbuf* currentBuffer = buffer;
        while (offset >= currentBuffer->len)
        {
            offset -= currentBuffer->len;
            currentBuffer = currentBuffer->next;
        }

        infra::ConstByteRange result = infra::Head(infra::ConstByteRange(reinterpret_cast<const uint8_t*>(currentBuffer->payload),
            reinterpret_cast<const uint8_t*>(currentBuffer->payload) + currentBuffer->len), max);
        bufferOffset += static_cast<uint16_t>(result.size());
        return result;
    }

    bool DatagramReceiverPeerLwIp::UdpReader::Empty() const
    {
        return Available() != 0;
    }

    std::size_t DatagramReceiverPeerLwIp::UdpReader::Available() const
    {
        return buffer->tot_len - bufferOffset;
    }

    infra::SharedPtr<void> DatagramProviderLwIp::ListenIPv4(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed)
    {
        return allocatorReceiverPeer.Allocate(receiver, port, broadcastAllowed, false);
    }

    infra::SharedPtr<DatagramSender> DatagramProviderLwIp::ConnectIPv4(DatagramSenderObserver& sender, IPv4Address address, uint16_t port)
    {
        return allocatorSenderPeer.Allocate(sender, address, port);
    }

    infra::SharedPtr<void> DatagramProviderLwIp::ListenIPv6(DatagramReceiver& receiver, uint16_t port)
    {
        return allocatorReceiverPeer.Allocate(receiver, port, false, true);
    }

    infra::SharedPtr<DatagramSender> DatagramProviderLwIp::ConnectIPv6(DatagramSenderObserver& sender, IPv6Address address, uint16_t port)
    {
        return allocatorSenderPeer.Allocate(sender, address, port);
    }
}

