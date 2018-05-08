#ifndef SERVICES_CONNECTION_WIN_HPP
#define SERVICES_CONNECTION_WIN_HPP

#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "infra/stream/ByteOutputStream.hpp"
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
        : public services::Connection
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
        
        IPv4Address Ipv4Address() const;
        void SetObserver(infra::SharedPtr<services::ConnectionObserver> connectionObserver);

        void Receive();
        void Send();
        void TrySend();
        void UpdateEventFlags();

    private:
        void TryAllocateSendStream();

    private:
        class StreamWriterWin
            : private std::vector<uint8_t>
            , public infra::ByteOutputStreamWriter
        {
        public:
            StreamWriterWin(ConnectionWin& connection, std::size_t size);
            ~StreamWriterWin();

        private:
            ConnectionWin& connection;
        };

        class StreamReaderWin
            : public infra::StreamReader
        {
        public:
            StreamReaderWin(ConnectionWin& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range, infra::StreamErrorPolicy& errorPolicy) override;
            virtual uint8_t Peek(infra::StreamErrorPolicy& errorPolicy) override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool Empty() const override;
            virtual std::size_t Available() const override;

        private:
            ConnectionWin& connection;
            std::size_t sizeRead = 0;
        };

    private:
        friend class EventDispatcherWithNetwork;

        EventDispatcherWithNetwork& network;
        SOCKET socket;
        WSAEVENT event = WSACreateEvent();

        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> receiveBuffer;
        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> sendBuffer;

        infra::SharedOptional<infra::DataOutputStream::WithWriter<StreamWriterWin>> sendStream;
        std::size_t requestedSendSize = 0;
        infra::SharedOptional<infra::DataInputStream::WithReader<StreamReaderWin>> receiveStream;
        bool trySend = false;
    };

    using AllocatorConnectionWin = infra::SharedObjectAllocator<ConnectionWin, void(EventDispatcherWithNetwork&, SOCKET)>;

    class ListenerWin
        : public infra::IntrusiveList<ListenerWin>::NodeType
    {
    public:
        ListenerWin(EventDispatcherWithNetwork& network, uint16_t port, services::ServerConnectionObserverFactory& factory);
        ~ListenerWin();

        void Accept();

    private:
        friend class EventDispatcherWithNetwork;

        EventDispatcherWithNetwork& network;
        services::ServerConnectionObserverFactory& factory;
        SOCKET listenSocket;
        WSAEVENT event = WSACreateEvent();
    };

    class ConnectorWin
        : public infra::IntrusiveList<ConnectorWin>::NodeType
        , public infra::EnableSharedFromThis<ConnectorWin>
    {
    public:
        ConnectorWin(EventDispatcherWithNetwork& network, IPv4Address address, uint16_t port, services::ClientConnectionObserverFactory& factory);
        ~ConnectorWin();

        void Connected();
        void Failed();

    private:
        friend class EventDispatcherWithNetwork;

        EventDispatcherWithNetwork& network;
        services::ClientConnectionObserverFactory& factory;
        SOCKET connectSocket;
        WSAEVENT event = WSACreateEvent();
    };

    class EventDispatcherWithNetwork
        : public infra::EventDispatcherWithWeakPtr::WithSize<50>
        , public services::ConnectionFactory
    {
    public:
        EventDispatcherWithNetwork();
        ~EventDispatcherWithNetwork();

        void RegisterConnection(const infra::SharedPtr<ConnectionWin>& connection);
        void RegisterListener(ListenerWin& listener);
        void DeregisterListener(ListenerWin& listener);
        void RegisterConnector(ConnectorWin& connector);
        void DeregisterConnector(ConnectorWin& connector);

    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, services::ServerConnectionObserverFactory& factory) override;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory) override;

    protected:
        virtual void RequestExecution();
        virtual void Idle() override;

    private:
        std::list<infra::WeakPtr<ConnectionWin>> connections;
        infra::IntrusiveList<ListenerWin> listeners;
        infra::IntrusiveList<ConnectorWin> connectors;
        WSAEVENT wakeUpEvent = WSACreateEvent();
    };
}

#endif
