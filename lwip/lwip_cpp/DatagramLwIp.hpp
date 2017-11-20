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
        : public infra::EnableSharedFromThis<DatagramSenderPeerLwIp>
        , public infra::Observer<DatagramSenderPeerLwIp, DatagramProviderLwIp>
    {
    public:
        DatagramSenderPeerLwIp(DatagramProviderLwIp& subject, infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize);

        void SetOwner(infra::SharedPtr<DatagramSenderPeerLwIp> owner);
        void CleanupIfExpired();
        void TryAllocateBuffer();
        void ServeSender(udp_pcb* control, pbuf* buffer);

    private:
        class DatagramSendStreamLwIpWriter
            : public infra::StreamWriter
        {
        public:
            DatagramSendStreamLwIpWriter(udp_pcb* control, pbuf* buffer, IPv4Address address, uint16_t port);
            ~DatagramSendStreamLwIpWriter();

            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;
            virtual std::size_t Available() const override;

        private:
            udp_pcb* control;
            pbuf* buffer;
            uint16_t bufferOffset = 0;
        };

        using DatagramSendStreamLwIp = infra::DataOutputStream::WithWriter<DatagramSendStreamLwIpWriter>;

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

    class DatagramProviderLwIp
        : public DatagramProvider
        , public infra::Subject<DatagramSenderPeerLwIp>
    {
    public:
        virtual void RequestSendStream(infra::SharedPtr<DatagramSender> sender, IPv4Address address, uint16_t port, std::size_t sendSize) override;
        virtual infra::SharedPtr<void> Listen(infra::SharedPtr<DatagramReceiver> receiver, uint16_t port, bool multicastAllowed) override;

    private:
        void TryServeSender();
        void CleanupExpiredSenders();

    private:
        AllocatorDatagramSenderPeerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MEMP_NUM_UDP_PCB> allocatorSenderPeer;
    };
}

#endif
