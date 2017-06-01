#ifndef SERVICES_CONNECTION_WIN_HPP
#define SERVICES_CONNECTION_WIN_HPP

#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "infra/util/IntrusiveList.hpp"
#include "infra/util/SharedObjectAllocator.hpp"
#include "infra/util/SharedOptional.hpp"
#include "services/network/Connection.hpp"
#include <list>
#include <winsock2.h>

namespace services
{
    class EventDispatcherWithNetwork;

    class ConnectionWin
        : public services::ZeroCopyConnection
        , public infra::EnableSharedFromThis<ConnectionWin>
    {
    public:
        ConnectionWin(EventDispatcherWithNetwork& network, SOCKET socket);
        ~ConnectionWin();

        virtual void RequestSendStream(std::size_t sendSize) override;
        virtual std::size_t MaxSendStreamSize() const override;
        virtual infra::SharedPtr<infra::DataInputStream> ReceiveStream() override;
        virtual void AckReceived() override;
        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;

        bool ReadyToReceive() const;
        bool ReadyToSend() const;
        void Receive();
        void Send();

    private:
        void TryAllocateSendStream();

    private:
        class SendStream
            : private infra::StreamWriter
            , public infra::DataOutputStream
        {
        public:
            SendStream(ConnectionWin& connection);

        private:
            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;

        private:
            ConnectionWin& connection;
            uint16_t sent = 0;
        };

        class ReceiveStreamWin
            : private infra::StreamReader
            , public infra::DataInputStream
        {
        public:
            ReceiveStreamWin(ConnectionWin& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool IsEmpty() const override;
            virtual std::size_t SizeAvailable() const override;

        private:
            ConnectionWin& connection;
            std::size_t sizeRead = 0;
        };

    private:
        friend class EventDispatcherWithNetwork;

        EventDispatcherWithNetwork& network;
        SOCKET socket;

        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> receiveBuffer;
        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> sendBuffer;

        infra::SharedOptional<SendStream> sendStream;
        std::size_t requestedSendSize = 0;
        infra::SharedOptional<ReceiveStreamWin> receiveStream;
    };

    using AllocatorConnectionWin = infra::SharedObjectAllocator<ConnectionWin, void(EventDispatcherWithNetwork&, SOCKET)>;

    class ListenerWin
        : public infra::IntrusiveList<ListenerWin>::NodeType
    {
    public:
        ListenerWin(EventDispatcherWithNetwork& network, uint16_t port, services::ZeroCopyServerConnectionObserverFactory& factory);
        ~ListenerWin();

        void Accept();

    private:
        friend class EventDispatcherWithNetwork;

        EventDispatcherWithNetwork& network;
        services::ZeroCopyServerConnectionObserverFactory& factory;
        SOCKET listenSocket;
    };

    using AllocatorListenerWin = infra::SharedObjectAllocator<ListenerWin, void(EventDispatcherWithNetwork&, uint16_t, services::ZeroCopyServerConnectionObserverFactory&)>;

    class EventDispatcherWithNetwork
        : public infra::EventDispatcherWithWeakPtr::WithSize<50>
        , public services::ZeroCopyConnectionFactory
    {
    public:
        EventDispatcherWithNetwork();
        ~EventDispatcherWithNetwork();

        void RegisterConnection(const infra::SharedPtr<ConnectionWin>& connection);
        void RegisterListener(ListenerWin& listener);
        void DeregisterListener(ListenerWin& listener);

    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, services::ZeroCopyServerConnectionObserverFactory& factory) override;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ZeroCopyClientConnectionObserverFactory& factory) override;

    protected:
        virtual void Idle() override;

    private:
        std::list<infra::WeakPtr<ConnectionWin>> connections;
        infra::IntrusiveList<ListenerWin> listeners;
    };
}

#endif
