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

    DatagramSenderPeerLwIp::DatagramSendStreamLwIpWriter::DatagramSendStreamLwIpWriter(udp_pcb* control, pbuf* buffer, IPv4Address address, uint16_t port)
        : control(control)
        , buffer(buffer)
    {
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress, address[0], address[1], address[2], address[3]);
        err_t result = udp_connect(control, &ipAddress, port);
        assert(result == ERR_OK);
    }

    DatagramSenderPeerLwIp::DatagramSendStreamLwIpWriter::~DatagramSendStreamLwIpWriter()
    {
        pbuf_realloc(buffer, bufferOffset);
        err_t result = udp_send(control, buffer);
        pbuf_free(buffer);
        udp_remove(control);
    }

    void DatagramSenderPeerLwIp::DatagramSendStreamLwIpWriter::Insert(infra::ConstByteRange range)
    {
        ReportResult(range.size() <= Available());
        range.shrink_from_back_to(Available());
        err_t result = pbuf_take_at(buffer, range.begin(), static_cast<uint16_t>(range.size()), bufferOffset);
        assert(result == ERR_OK);
        bufferOffset += static_cast<uint16_t>(range.size());
    }

    void DatagramSenderPeerLwIp::DatagramSendStreamLwIpWriter::Insert(uint8_t element)
    {
        ReportResult(Available() >= 1);
        pbuf_put_at(buffer, bufferOffset, element);
        ++bufferOffset;
    }

    std::size_t DatagramSenderPeerLwIp::DatagramSendStreamLwIpWriter::Available() const
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

    infra::SharedPtr<void> DatagramProviderLwIp::Listen(infra::SharedPtr<DatagramReceiver> receiver, uint16_t port, bool multicastAllowed)
    {
        return nullptr;
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
