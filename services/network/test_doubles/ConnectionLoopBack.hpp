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

    private:
        void TryAllocateSendStream();

    private:
        class StreamWriterLoopBack
            : public infra::StreamWriter
        {
        public:
            explicit StreamWriterLoopBack(ConnectionLoopBackPeer& connection);
            ~StreamWriterLoopBack();

        private:
            virtual void Insert(infra::ConstByteRange range, infra::StreamErrorPolicy& errorPolicy) override;
            virtual std::size_t Available() const override;

        private:
            ConnectionLoopBackPeer& connection;
            std::size_t sent = 0;
        };

        class StreamReaderLoopBack
            : public infra::StreamReader
        {
        public:
            explicit StreamReaderLoopBack(ConnectionLoopBackPeer& connection);

            void ConsumeRead();

        private:
            virtual void Extract(infra::ByteRange range, infra::StreamErrorPolicy& errorPolicy) override;
            virtual uint8_t ExtractOne(infra::StreamErrorPolicy& errorPolicy) override;
            virtual uint8_t Peek(infra::StreamErrorPolicy& errorPolicy) override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool Empty() const override;
            virtual std::size_t Available() const override;

        private:
            ConnectionLoopBackPeer& connection;
            std::size_t sizeRead = 0;
        };

    private:
        ConnectionLoopBackPeer& peer;
        ConnectionLoopBack& loopBack;
        infra::BoundedDeque<uint8_t>::WithMaxSize<1024> sendBuffer;
        infra::SharedOptional<infra::DataOutputStream::WithWriter<StreamWriterLoopBack>> sendStream;
        std::size_t requestedSendSize = 0;
        infra::SharedOptional<infra::DataInputStream::WithReader<StreamReaderLoopBack>> receiveStream;
    };

    class ConnectionLoopBack
        : public infra::EnableSharedFromThis<ConnectionLoopBack>
    {
    public:
        ConnectionLoopBack(ClientConnectionObserverFactory& clientObserverFactory);

        ClientConnectionObserverFactory& ClientObserverFactory();
        Connection& Server();
        Connection& Client();

    private:
        ClientConnectionObserverFactory& clientObserverFactory;
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
