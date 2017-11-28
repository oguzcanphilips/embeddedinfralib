#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "lwip/lwip_cpp/DatagramLwIp.hpp"

namespace services
{
    DatagramSenderPeerLwIp::DatagramSenderPeerLwIp(DatagramSenderObserver& sender, IPv4Address address, uint16_t port)
        : sender(sender)
        , state(infra::InPlaceType<StateIdle>(), *this)
    {
        control = udp_new();
        assert(control != nullptr);
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress, address[0], address[1], address[2], address[3]);
        err_t result = udp_connect(control, &ipAddress, port);
        assert(result == ERR_OK);
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
    {
        infra::SharedPtr<DatagramSendStreamLwIp> streamPtr = stream.Emplace(datagramSender.control, buffer);
        infra::EventDispatcherWithWeakPtr::Instance().Schedule([streamPtr](const infra::SharedPtr<DatagramSenderPeerLwIp>& datagramSender)
        {
            datagramSender->sender.SendStreamAvailable(streamPtr);
        }, datagramSender.SharedFromThis());
    }

    DatagramReceiverPeerLwIp::DatagramReceiverPeerLwIp(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed)
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
        receiver.DataReceived(stream, IPv4Address{ ip4_addr1(address), ip4_addr2(address), ip4_addr3(address), ip4_addr4(address) });
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

    infra::SharedPtr<DatagramSender> DatagramProviderLwIp::Connect(DatagramSenderObserver& sender, IPv4Address address, uint16_t port)
    {
        return allocatorSenderPeer.Allocate(sender, address, port);
    }

    infra::SharedPtr<void> DatagramProviderLwIp::Listen(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed)
    {
        return allocatorReceiverPeer.Allocate(receiver, port, broadcastAllowed);
    }
}

