#ifndef TCP_CONNECTION_LW_IP_HPP
#define TCP_CONNECTION_LW_IP_HPP

#include "infra/timer/Timer.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "infra/util/ByteRange.hpp"
#include "infra/util/SharedObjectAllocatorFixedSize.hpp"
#include "infra/util/SharedOptional.hpp"
#include "infra/util/WithStorage.hpp"
#include "lwip/tcp.h"
#include "services/network/Connection.hpp"

namespace services
{
    class ConnectionLwIp
        : public services::ZeroCopyConnection
        , public infra::EnableSharedFromThis<ConnectionLwIp>
    {
    public:
        ConnectionLwIp(tcp_pcb* control);
        ~ConnectionLwIp();

        virtual void RequestSendStream(std::size_t sendSize) override;
        virtual std::size_t MaxSendStreamSize() const override;
        virtual infra::SharedPtr<infra::DataInputStream> ReceiveStream() override;
        virtual void AckReceived() override;
        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;

    private:
        void SendBuffer(infra::ConstByteRange buffer);
        void TryAllocateSendStream();
        void ResetControl();

        static err_t Recv(void* arg, tcp_pcb* tpcb, pbuf* p, err_t err);
        static void Err(void* arg, err_t err);
        static err_t Sent(void* arg, struct tcp_pcb* tpcb, uint16_t len);

        err_t Recv(pbuf* p, err_t err);
        void Err(err_t err);
        err_t Sent(uint16_t len);

    private:
        class SendStreamLwIp
            : private infra::StreamWriter
            , public infra::DataOutputStream
        {
        public:
            SendStreamLwIp(ConnectionLwIp& connection, infra::ByteRange sendBuffer);
            ~SendStreamLwIp();

        private:
            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;

        private:
            ConnectionLwIp& connection;
            infra::ByteRange sendBuffer;
            uint16_t sent = 0;
        };

        class ReceiveStreamLwIp
            : private infra::StreamReader
            , public infra::DataInputStream
        {
        public:
            ReceiveStreamLwIp(ConnectionLwIp& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool IsEmpty() const override;
            virtual std::size_t SizeAvailable() const override;

        private:
            ConnectionLwIp& connection;
            uint16_t sizeRead = 0;
        };

    private:
        tcp_pcb* control;
        std::size_t requestedSendSize = 0;

        infra::SharedOptional<SendStreamLwIp> sendStream;
        infra::SharedOptional<ReceiveStreamLwIp> receiveStream;

        infra::ConstByteRange sendBuffer;
        infra::TimerSingleShot retrySendTimer;
        infra::BoundedDeque<infra::ConstByteRange>::WithMaxSize<TCP_SND_QUEUELEN> sendBuffers;
        infra::BoundedDeque<std::array<uint8_t, TCP_MSS>>::WithMaxSize<TCP_SND_QUEUELEN> sendMemoryPool;

        infra::BoundedDeque<uint8_t>::WithMaxSize<TCP_WND> receiveBuffer;
        bool dataReceivedScheduled = false;
    };

    using AllocatorConnectionLwIp = infra::SharedObjectAllocator<ConnectionLwIp, void(tcp_pcb*)>;

    class ListenerLwIp
    {
    public:
        template<std::size_t Size>
            using WithFixedAllocator = infra::WithStorage<ListenerLwIp, AllocatorConnectionLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<Size>>;

        ListenerLwIp(AllocatorConnectionLwIp& allocator, uint16_t port, ZeroCopyConnectionObserverFactory& factory);
        ~ListenerLwIp();

    private:
        static err_t Accept(void* arg, struct tcp_pcb* newPcb, err_t err);

        err_t Accept(tcp_pcb* newPcb, err_t err);

    private:
        AllocatorConnectionLwIp& allocator;
        tcp_pcb* listenPort;
        ZeroCopyConnectionObserverFactory& factory;
    };

    using AllocatorListenerLwIp = infra::SharedObjectAllocator<ListenerLwIp, void(AllocatorConnectionLwIp&, uint16_t, ZeroCopyConnectionObserverFactory&)>;

    class LightweightIp
        : public ZeroCopyListenerFactory
    {
    public:
        template<std::size_t NumListeners, std::size_t NumConnections>
            using WithFixedAllocator = infra::WithStorage<
                infra::WithStorage<LightweightIp, AllocatorListenerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<NumListeners>>,
                AllocatorConnectionLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<NumConnections>>;

    public:
        LightweightIp(AllocatorListenerLwIp& listenerAllocator, AllocatorConnectionLwIp& connectionAllocator);

        virtual infra::SharedPtr<void> Listen(uint16_t port, ZeroCopyConnectionObserverFactory& factory) override;

    private:
        AllocatorListenerLwIp& listenerAllocator;
        AllocatorConnectionLwIp& connectionAllocator;
        infra::TimerRepeating sysCheckTimer;
    };
}

#endif
