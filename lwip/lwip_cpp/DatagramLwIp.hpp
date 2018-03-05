#ifndef LWIP_DATAGRAM_LW_IP_HPP
#define LWIP_DATAGRAM_LW_IP_HPP

#include "infra/timer/Timer.hpp"
#include "infra/util/SharedObjectAllocatorFixedSize.hpp"
#include "infra/util/SharedOptional.hpp"
#include "infra/util/PolymorphicVariant.hpp"
#include "lwip/udp.h"
#include "lwip/lwip_cpp/ConnectionLwIp.hpp"
#include "services/network/Datagram.hpp"

namespace services
{
    class DatagramProviderLwIp;

    class DatagramSenderPeerLwIp
        : public DatagramSender
        , public infra::EnableSharedFromThis<DatagramSenderPeerLwIp>
    {
    public:
        DatagramSenderPeerLwIp(DatagramSenderObserver& sender, IPAddress address, uint16_t port);
        ~DatagramSenderPeerLwIp();

        virtual void RequestSendStream(std::size_t sendSize) override;

    private:
        class UdpWriter
            : public infra::StreamWriter
        {
        public:
            UdpWriter(udp_pcb* control, pbuf* buffer);
            ~UdpWriter();

            virtual void Insert(infra::ConstByteRange range, infra::StreamErrorPolicy& errorPolicy) override;
            virtual std::size_t Available() const override;

        private:
            udp_pcb* control;
            pbuf* buffer;
            uint16_t bufferOffset = 0;
        };

        using DatagramSendStreamLwIp = infra::DataOutputStream::WithWriter<UdpWriter>;

        class StateBase
        {
        public:
            virtual ~StateBase() = default;

            virtual void RequestSendStream(std::size_t sendSize);
        };

        class StateIdle
            : public StateBase
        {
        public:
            StateIdle(DatagramSenderPeerLwIp& datagramSender);

            virtual void RequestSendStream(std::size_t sendSize) override;
            
        private:
            DatagramSenderPeerLwIp& datagramSender;
        };

        class StateWaitingForBuffer
            : public StateBase
        {
        public:
            StateWaitingForBuffer(DatagramSenderPeerLwIp& datagramSender, std::size_t sendSize);

            void TryAllocateBuffer();

        private:
            DatagramSenderPeerLwIp& datagramSender;
            std::size_t sendSize;
            infra::TimerRepeating allocateTimer;
        };
        
        class StateBufferAllocated
            : public StateBase
        {
        public:
            StateBufferAllocated(DatagramSenderPeerLwIp& datagramSender, pbuf* buffer);

        private:
            DatagramSenderPeerLwIp& datagramSender;
            infra::NotifyingSharedOptional<DatagramSendStreamLwIp> stream;
            infra::SharedPtr<DatagramSendStreamLwIp> streamPtr;
        };

    private:
        DatagramSenderObserver& sender;
        infra::PolymorphicVariant<StateBase, StateIdle, StateWaitingForBuffer, StateBufferAllocated> state;
        udp_pcb* control;
    };

    using AllocatorDatagramSenderPeerLwIp = infra::SharedObjectAllocator<DatagramSenderPeerLwIp, void(DatagramSenderObserver& sender, IPAddress address, uint16_t port)>;

    class DatagramReceiverPeerLwIp
    {
    public:
        DatagramReceiverPeerLwIp(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed, bool ipv6);
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
        DatagramReceiver& receiver;
        udp_pcb* control;
    };

    using AllocatorDatagramReceiverPeerLwIp = infra::SharedObjectAllocator<DatagramReceiverPeerLwIp, void(DatagramReceiver& receiver, uint16_t port, bool multicastAllowed, bool ipv6)>;

    class DatagramProviderLwIp
        : public DatagramProvider
    {
    public:
        virtual infra::SharedPtr<void> ListenIPv4(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed) override;
        virtual infra::SharedPtr<DatagramSender> ConnectIPv4(DatagramSenderObserver& sender, IPv4Address address, uint16_t port) override;
        virtual infra::SharedPtr<void> ListenIPv6(DatagramReceiver& receiver, uint16_t port) override;
        virtual infra::SharedPtr<DatagramSender> ConnectIPv6(DatagramSenderObserver& sender, IPv6Address address, uint16_t port) override;

    private:
        AllocatorDatagramSenderPeerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MEMP_NUM_UDP_PCB> allocatorSenderPeer;
        AllocatorDatagramReceiverPeerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MEMP_NUM_UDP_PCB> allocatorReceiverPeer;
    };
}

#endif
