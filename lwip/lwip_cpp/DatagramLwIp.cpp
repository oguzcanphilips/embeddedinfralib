#include "lwip/lwip_cpp/DatagramLwIp.hpp"

namespace services
{
    DatagramSenderPeerLwIp::DatagramSenderPeerLwIp(DatagramProviderLwIp& subject, infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize)
        : infra::Observer<DatagramSenderPeerLwIp, DatagramProviderLwIp>(subject)
        , state(infra::InPlaceType<WaitingSender>(), sender, address, port, sendSize)
    {}

    void DatagramSenderPeerLwIp::SetOwner(infra::SharedPtr<DatagramSenderPeerLwIp> owner)
    {
        this->owner = owner;
    }

    void DatagramSenderPeerLwIp::CleanupIfExpired()
    {
        if (state.Is<WaitingSender>() && state.Get<WaitingSender>().sender == nullptr)
            owner = nullptr;
    }

    void DatagramSenderPeerLwIp::TryAllocateBuffer()
    {
        if (state.Is<WaitingSender>())
        {
            udp_pcb* control = udp_new();
            if (control != nullptr)
            {
                pbuf* buffer = pbuf_alloc(PBUF_TRANSPORT, static_cast<uint16_t>(state.Get<WaitingSender>().sendSize), PBUF_POOL);
                if (buffer != nullptr)
                    ServeSender(control, buffer);
                else
                    udp_remove(control);
            }
        }
    }

    void DatagramSenderPeerLwIp::ServeSender(udp_pcb* control, pbuf* buffer)
    {
        WaitingSender waitingSender = state.Get<WaitingSender>();
        infra::SharedPtr<DatagramSender> sender = waitingSender.sender.lock();

        state.Emplace<DatagramSendStreamLwIp>(control, buffer, waitingSender.address, waitingSender.port);

        sender->SendStreamAvailable(infra::MakeContainedSharedObject(state.Get<DatagramSendStreamLwIp>(), std::move(owner)));
    }

    DatagramSenderPeerLwIp::WaitingSender::WaitingSender(infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize)
        : sender(sender)
        , address(address)
        , port(port)
        , sendSize(sendSize)
    {}

    DatagramSenderPeerLwIp::UdpWriter::UdpWriter(udp_pcb* control, pbuf* buffer, IPv4Address address, uint16_t port)
        : control(control)
        , buffer(buffer)
    {
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress, address[0], address[1], address[2], address[3]);
        err_t result = udp_connect(control, &ipAddress, port);
        assert(result == ERR_OK);
    }

    DatagramSenderPeerLwIp::UdpWriter::~UdpWriter()
    {
        pbuf_realloc(buffer, bufferOffset);
        err_t result = udp_send(control, buffer);
        pbuf_free(buffer);
        udp_remove(control);
    }

    void DatagramSenderPeerLwIp::UdpWriter::Insert(infra::ConstByteRange range)
    {
        ReportResult(range.size() <= Available());
        range.shrink_from_back_to(Available());
        err_t result = pbuf_take_at(buffer, range.begin(), static_cast<uint16_t>(range.size()), bufferOffset);
        assert(result == ERR_OK);
        bufferOffset += static_cast<uint16_t>(range.size());
    }

    void DatagramSenderPeerLwIp::UdpWriter::Insert(uint8_t element)
    {
        ReportResult(Available() >= 1);
        pbuf_put_at(buffer, bufferOffset, element);
        ++bufferOffset;
    }

    std::size_t DatagramSenderPeerLwIp::UdpWriter::Available() const
    {
        return buffer->tot_len - bufferOffset;
    }

    DatagramReceiverPeerLwIp::DatagramReceiverPeerLwIp(infra::SharedPtr<DatagramReceiver> receiver, uint16_t port, bool broadcastAllowed)
        : receiver(receiver)
    {
        control = udp_new();
        assert(control != nullptr);
        if (broadcastAllowed)
            ip_set_option(control, SOF_BROADCAST);
        err_t result = udp_bind(control, IP4_ADDR_ANY, port);
        assert(result == ERR_OK);
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
        receiver.lock()->DataReceived(stream, IPv4Address{ ip4_addr1(address), ip4_addr2(address), ip4_addr3(address), ip4_addr4(address) });
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

    void DatagramProviderLwIp::RequestSendStream(infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize)
    {
        CleanupExpiredSenders();
        infra::SharedPtr<DatagramSenderPeerLwIp> senderPeer = allocatorSenderPeer.Allocate(*this, sender, address, port, sendSize);
        senderPeer->SetOwner(senderPeer);
        senderPeer = nullptr;
        TryServeSender();
    }

    infra::SharedPtr<void> DatagramProviderLwIp::Listen(infra::SharedPtr<DatagramReceiver> receiver, uint16_t port, bool broadcastAllowed)
    {
        return allocatorReceiverPeer.Allocate(receiver, port, broadcastAllowed);
    }

    void DatagramProviderLwIp::TryServeSender()
    {
        NotifyObservers([](DatagramSenderPeerLwIp& peer) { peer.TryAllocateBuffer(); });
    }

    void DatagramProviderLwIp::CleanupExpiredSenders()
    {
        NotifyObservers([](DatagramSenderPeerLwIp& peer) { peer.CleanupIfExpired(); });
    }
}
