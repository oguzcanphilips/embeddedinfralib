#ifndef LWIP_DATAGRAM_LW_IP_HPP
#define LWIP_DATAGRAM_LW_IP_HPP

#include "infra/util/Observer.hpp"
#include "infra/util/SharedObjectAllocatorFixedSize.hpp"
#include "infra/util/SharedOptional.hpp"
#include "infra/util/Variant.hpp"
#include "lwip/udp.h"
#include "services/network/Datagram.hpp"

namespace services
{
    class DatagramProviderLwIp;

    class DatagramSenderPeerLwIp
        : public infra::Observer<DatagramSenderPeerLwIp, DatagramProviderLwIp>
    {
    public:
        DatagramSenderPeerLwIp(DatagramProviderLwIp& subject, infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize);

        void SetOwner(infra::SharedPtr<DatagramSenderPeerLwIp> owner);
        void CleanupIfExpired();
        void TryAllocateBuffer();
        void ServeSender(udp_pcb* control, pbuf* buffer);

    private:
        class UdpWriter
            : public infra::StreamWriter
        {
        public:
            UdpWriter(udp_pcb* control, pbuf* buffer, IPv4Address address, uint16_t port);
            ~UdpWriter();

            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;
            virtual std::size_t Available() const override;

        private:
            udp_pcb* control;
            pbuf* buffer;
            uint16_t bufferOffset = 0;
        };

        using DatagramSendStreamLwIp = infra::DataOutputStream::WithWriter<UdpWriter>;

        struct WaitingSender
        {
            WaitingSender(infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize);

            infra::WeakPtr<DatagramSender> sender;
            IPv4Address address;
            uint16_t port;
            std::size_t sendSize;
        };

    private:
        infra::SharedPtr<DatagramSenderPeerLwIp> owner;
        infra::Variant<WaitingSender, DatagramSendStreamLwIp> state;
    };

    using AllocatorDatagramSenderPeerLwIp = infra::SharedObjectAllocator<DatagramSenderPeerLwIp, void(DatagramProviderLwIp& subject, infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize)>;

    class DatagramReceiverPeerLwIp
    {
    public:
        DatagramReceiverPeerLwIp(infra::SharedPtr<DatagramReceiver> receiver, uint16_t port, bool broadcastAllowed);
        ~DatagramReceiverPeerLwIp();

    private:
        static void StaticRecv(void* arg, udp_pcb* pcb, pbuf* buffer, const ip_addr_t* address, u16_t port);
        void Recv(pbuf* buffer, const ip_addr_t* address, u16_t port);

    private:
        class UdpReader
            : public infra::StreamReader
        {
        public:
            UdpReader(pbuf* buffer);
            ~UdpReader();

            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool Empty() const override;
            virtual std::size_t Available() const override;

        private:
            pbuf* buffer;
            uint16_t bufferOffset = 0;
        };

    private:
        infra::WeakPtr<DatagramReceiver> receiver;
        udp_pcb* control;
    };

    using AllocatorDatagramReceiverPeerLwIp = infra::SharedObjectAllocator<DatagramReceiverPeerLwIp, void(infra::SharedPtr<DatagramReceiver> receiver, uint16_t port, bool multicastAllowed)>;

    class DatagramProviderLwIp
        : public DatagramProvider
        , public infra::Subject<DatagramSenderPeerLwIp>
    {
    public:
        virtual void RequestSendStream(infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize) override;
        virtual infra::SharedPtr<void> Listen(infra::SharedPtr<DatagramReceiver> receiver, uint16_t port, bool broadcastAllowed) override;

    private:
        void TryServeSender();
        void CleanupExpiredSenders();

    private:
        AllocatorDatagramSenderPeerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MEMP_NUM_UDP_PCB> allocatorSenderPeer;
        AllocatorDatagramReceiverPeerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MEMP_NUM_UDP_PCB> allocatorReceiverPeer;
    };
}

#endif
