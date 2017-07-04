#ifndef NETWORK_CONNECTION_LOOP_BACK_HPP
#define NETWORK_CONNECTION_LOOP_BACK_HPP

#include "infra/util/BoundedDeque.hpp"
#include "infra/util/SharedOptional.hpp"
#include "services/network/Connection.hpp"
#include <map>

namespace services
{
    class ConnectionLoopBack;
    class ConnectionLoopBackFactory;

    class ConnectionLoopBackPeer
        : public Connection
    {
    public:
        ConnectionLoopBackPeer(ConnectionLoopBackPeer& peer, ConnectionLoopBack& loopBack);

        virtual void RequestSendStream(std::size_t sendSize) override;
        virtual std::size_t MaxSendStreamSize() const override;
        virtual infra::SharedPtr<infra::DataInputStream> ReceiveStream() override;
        virtual void AckReceived() override;
        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;
        virtual IPv4Address Ipv4Address() const override;

    private:
        void TryAllocateSendStream();

    private:
        class SendStreamLoopBackPeer
            : private infra::StreamWriter
            , public infra::DataOutputStream
        {
        public:
            explicit SendStreamLoopBackPeer(ConnectionLoopBackPeer& connection);
            ~SendStreamLoopBackPeer();

        private:
            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;

        private:
            ConnectionLoopBackPeer& connection;
            std::size_t sent = 0;
        };

        class ReceiveStreamLoopBackPeer
            : private infra::StreamReader
            , public infra::DataInputStream
        {
        public:
            explicit ReceiveStreamLoopBackPeer(ConnectionLoopBackPeer& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool IsEmpty() const override;
            virtual std::size_t SizeAvailable() const override;

        private:
            ConnectionLoopBackPeer& connection;
            std::size_t sizeRead = 0;
        };

    private:
        ConnectionLoopBackPeer& peer;
        ConnectionLoopBack& loopBack;
        infra::BoundedDeque<uint8_t>::WithMaxSize<1024> sendBuffer;
        infra::SharedOptional<SendStreamLoopBackPeer> sendStream;
        std::size_t requestedSendSize = 0;
        infra::SharedOptional<ReceiveStreamLoopBackPeer> receiveStream;
    };

    class ConnectionLoopBack
        : public infra::EnableSharedFromThis<ConnectionLoopBack>
    {
    public:
        ConnectionLoopBack();

        Connection& Server();
        Connection& Client();

    private:
        ConnectionLoopBackPeer server;
        ConnectionLoopBackPeer client;
    };

    class ConnectionLoopBackListener
    {
    public:
        ConnectionLoopBackListener(uint16_t port, ConnectionLoopBackFactory& loopBackFactory, ServerConnectionObserverFactory& connectionObserverFactory);
        ~ConnectionLoopBackListener();

        void Accept(ClientConnectionObserverFactory& clientObserverFactory);

    private:
        uint16_t port;
        ConnectionLoopBackFactory& loopBackFactory;
        ServerConnectionObserverFactory& connectionObserverFactory;
    };

    class ConnectionLoopBackConnector
        : public infra::EnableSharedFromThis<ConnectionLoopBackConnector>
    {
    public:
        ConnectionLoopBackConnector(uint16_t port, ConnectionLoopBackFactory& loopBackFactory, ClientConnectionObserverFactory& connectionObserverFactory);

        void Connect();

    private:
        uint16_t port;
        ConnectionLoopBackFactory& loopBackFactory;
        ClientConnectionObserverFactory& connectionObserverFactory;
    };

    class ConnectionLoopBackFactory
        : public ConnectionFactory
    {
    public:
        ~ConnectionLoopBackFactory();

        void RegisterListener(uint16_t port, ConnectionLoopBackListener* listener);
        void UnregisterListener(uint16_t port);

        virtual infra::SharedPtr<void> Listen(uint16_t port, ServerConnectionObserverFactory& factory) override;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory) override;

    private:
        friend class ConnectionLoopBackConnector;
        std::map<uint16_t, ConnectionLoopBackListener*> listeners;
    };

}

#endif
