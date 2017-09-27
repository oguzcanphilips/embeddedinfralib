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
        virtual IPv4Address Ipv4Address() const override;

        bool ReadyToReceive() const;
        bool ReadyToSend() const;
        void Receive();
        void Send();

    private:
        void TryAllocateSendStream();

    private:
        class StreamWriterWin
            : public infra::StreamWriter
        {
        public:
            StreamWriterWin(ConnectionWin& connection);

        private:
            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;
            virtual std::size_t Available() const override;

        private:
            ConnectionWin& connection;
            uint16_t sent = 0;
        };

        class StreamReaderWin
            : public infra::StreamReader
        {
        public:
            StreamReaderWin(ConnectionWin& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
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

        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> receiveBuffer;
        infra::BoundedDeque<uint8_t>::WithMaxSize<2048> sendBuffer;

        infra::SharedOptional<infra::DataOutputStream::WithWriter<StreamWriterWin>> sendStream;
        std::size_t requestedSendSize = 0;
        infra::SharedOptional<infra::DataInputStream::WithReader<StreamReaderWin>> receiveStream;
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
    };

    using AllocatorListenerWin = infra::SharedObjectAllocator<ListenerWin, void(EventDispatcherWithNetwork&, uint16_t, services::ServerConnectionObserverFactory&)>;

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

    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, services::ServerConnectionObserverFactory& factory) override;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory) override;

    protected:
        virtual void Idle() override;

    private:
        std::list<infra::WeakPtr<ConnectionWin>> connections;
        infra::IntrusiveList<ListenerWin> listeners;
    };
}

#endif
