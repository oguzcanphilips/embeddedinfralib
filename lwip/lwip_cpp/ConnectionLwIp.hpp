#ifndef TCP_CONNECTION_LW_IP_HPP
#define TCP_CONNECTION_LW_IP_HPP

#include "infra/timer/Timer.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "infra/util/ByteRange.hpp"
#include "infra/util/SharedObjectAllocatorFixedSize.hpp"
#include "infra/util/SharedOptional.hpp"
#include "infra/util/WithStorage.hpp"
#include "lwip/tcp.h"
#include "services/network/Address.hpp"
#include "services/network/Connection.hpp"

namespace services
{
    class ConnectionLwIp
        : public services::Connection
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
        virtual IPv4Address Ipv4Address() const override;

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
        class StreamWriterLwIp
            : public infra::StreamWriter
        {
        public:
            StreamWriterLwIp(ConnectionLwIp& connection, infra::ByteRange sendBuffer);
            ~StreamWriterLwIp();

        private:
            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;

        private:
            ConnectionLwIp& connection;
            infra::ByteRange sendBuffer;
            uint16_t sent = 0;
        };

        class StreamReaderLwIp
            : public infra::StreamReader
        {
        public:
            StreamReaderLwIp(ConnectionLwIp& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool Empty() const override;
            virtual std::size_t Available() const override;

        private:
            ConnectionLwIp& connection;
            uint16_t sizeRead = 0;
        };

    private:
        tcp_pcb* control;
        std::size_t requestedSendSize = 0;

        infra::SharedOptional<infra::DataOutputStream::WithWriter<StreamWriterLwIp>> sendStream;
        infra::SharedOptional<infra::DataInputStream::WithReader<StreamReaderLwIp>> receiveStream;

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

        ListenerLwIp(AllocatorConnectionLwIp& allocator, uint16_t port, ServerConnectionObserverFactory& factory);
        ~ListenerLwIp();

    private:
        static err_t Accept(void* arg, struct tcp_pcb* newPcb, err_t err);

        err_t Accept(tcp_pcb* newPcb, err_t err);

    private:
        AllocatorConnectionLwIp& allocator;
        tcp_pcb* listenPort;
        ServerConnectionObserverFactory& factory;
    };

    using AllocatorListenerLwIp = infra::SharedObjectAllocator<ListenerLwIp, void(AllocatorConnectionLwIp&, uint16_t, ServerConnectionObserverFactory&)>;

    class ConnectorLwIp
    {
    public:
        ConnectorLwIp(AllocatorConnectionLwIp& allocator, IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory);
        ~ConnectorLwIp();

    private:
        static err_t StaticConnected(void* arg, tcp_pcb* tpcb, err_t err);
        static void StaticError(void* arg, err_t err);
        err_t Connected();
        void Error(err_t err);

    private:
        AllocatorConnectionLwIp& allocator;
        ClientConnectionObserverFactory& factory;
        tcp_pcb* control;
    };

    using AllocatorConnectorLwIp = infra::SharedObjectAllocator<ConnectorLwIp, void(AllocatorConnectionLwIp& allocator, IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory)>;

    class LightweightIp
        : public ConnectionFactory
    {
    public:
        template<std::size_t MaxListeners, std::size_t MaxConnectors, std::size_t MaxConnections>
            using WithFixedAllocator = infra::WithStorage<infra::WithStorage<infra::WithStorage<LightweightIp,
                AllocatorListenerLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxListeners>>,
                AllocatorConnectorLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnectors>>,
                AllocatorConnectionLwIp::UsingAllocator<infra::SharedObjectAllocatorFixedSize>::WithStorage<MaxConnections>>;

    public:
        LightweightIp(AllocatorListenerLwIp& listenerAllocator, AllocatorConnectorLwIp& connectorAllocator, AllocatorConnectionLwIp& connectionAllocator);

        virtual infra::SharedPtr<void> Listen(uint16_t port, ServerConnectionObserverFactory& factory) override;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory) override;

    private:
        AllocatorListenerLwIp& listenerAllocator;
        AllocatorConnectorLwIp& connectorAllocator;
        AllocatorConnectionLwIp& connectionAllocator;
        infra::TimerRepeating sysCheckTimer;
    };
}

#endif
