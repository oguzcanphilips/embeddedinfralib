#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "services/network/test_doubles/ConnectionLoopBack.hpp"

namespace services
{
    ConnectionLoopBackPeer::ConnectionLoopBackPeer(ConnectionLoopBackPeer& peer, ConnectionLoopBack& loopBack)
        : peer(peer)
        , loopBack(loopBack)
    {}

    void ConnectionLoopBackPeer::RequestSendStream(std::size_t sendSize)
    {
        assert(requestedSendSize == 0);
        assert(sendStream.Allocatable());
        requestedSendSize = sendSize;
        TryAllocateSendStream();
    }

    std::size_t ConnectionLoopBackPeer::MaxSendStreamSize() const
    {
        return sendBuffer.max_size();
    }

    infra::SharedPtr<infra::DataInputStream> ConnectionLoopBackPeer::ReceiveStream()
    {
        return receiveStream.Emplace(peer);
    }

    void ConnectionLoopBackPeer::AckReceived()
    {
        receiveStream->Reader().ConsumeRead();

        if (peer.requestedSendSize != 0)
            peer.TryAllocateSendStream();
    }

    void ConnectionLoopBackPeer::CloseAndDestroy()
    {
        ResetOwnership();
        peer.ResetOwnership();
    }

    void ConnectionLoopBackPeer::AbortAndDestroy()
    {
        ResetOwnership();
        peer.ResetOwnership();
    }

    void ConnectionLoopBackPeer::TryAllocateSendStream()
    {
        assert(sendStream.Allocatable());
        if (sendBuffer.max_size() - sendBuffer.size() >= requestedSendSize)
        {
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([this](const infra::SharedPtr<ConnectionLoopBack>& loopBack)
            {
                infra::SharedPtr<infra::DataOutputStream> stream = sendStream.Emplace(*this);
                GetObserver().SendStreamAvailable(std::move(stream));
            }, loopBack.SharedFromThis());

            requestedSendSize = 0;
        }
    }

    ConnectionLoopBackPeer::StreamWriterLoopBack::StreamWriterLoopBack(ConnectionLoopBackPeer& connection)
        : connection(connection)
    {}

    ConnectionLoopBackPeer::StreamWriterLoopBack::~StreamWriterLoopBack()
    {
        if (sent != 0)
        {
            ConnectionLoopBackPeer& connection = this->connection;
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([&connection](const infra::SharedPtr<ConnectionLoopBack>& loopBack)
            {
                if (connection.peer.HasObserver())
                    connection.peer.GetObserver().DataReceived();
            }, connection.loopBack.SharedFromThis());
        }
    }

    void ConnectionLoopBackPeer::StreamWriterLoopBack::Insert(infra::ConstByteRange range, infra::StreamErrorPolicy& errorPolicy)
    {
        connection.sendBuffer.insert(connection.sendBuffer.end(), range.begin(), range.end());
        sent += range.size();
    }

    std::size_t ConnectionLoopBackPeer::StreamWriterLoopBack::Available() const
    {
        return connection.sendBuffer.size() - sent;
    }

    ConnectionLoopBackPeer::StreamReaderLoopBack::StreamReaderLoopBack(ConnectionLoopBackPeer& connection)
        : connection(connection)
    {}

    void ConnectionLoopBackPeer::StreamReaderLoopBack::ConsumeRead()
    {
        connection.sendBuffer.erase(connection.sendBuffer.begin(), connection.sendBuffer.begin() + sizeRead);
        sizeRead = 0;
    }

    void ConnectionLoopBackPeer::StreamReaderLoopBack::Extract(infra::ByteRange range)
    {
        std::copy(connection.sendBuffer.begin() + sizeRead, connection.sendBuffer.begin() + sizeRead + range.size(), range.begin());
        sizeRead += range.size();
    }

    uint8_t ConnectionLoopBackPeer::StreamReaderLoopBack::ExtractOne()
    {
        return connection.sendBuffer[sizeRead++];
    }

    uint8_t ConnectionLoopBackPeer::StreamReaderLoopBack::Peek()
    {
        return connection.sendBuffer[sizeRead];
    }

    infra::ConstByteRange ConnectionLoopBackPeer::StreamReaderLoopBack::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange result = infra::Head(connection.sendBuffer.contiguous_range(connection.sendBuffer.begin() + sizeRead), max);
        sizeRead += result.size();
        return result;
    }

    bool ConnectionLoopBackPeer::StreamReaderLoopBack::Empty() const
    {
        return connection.sendBuffer.size() == sizeRead;
    }

    std::size_t ConnectionLoopBackPeer::StreamReaderLoopBack::Available() const
    {
        return connection.sendBuffer.size() - sizeRead;
    }

    ConnectionLoopBack::ConnectionLoopBack(ClientConnectionObserverFactory& clientObserverFactory)
        : clientObserverFactory(clientObserverFactory)
        , server(client, *this)
        , client(server, *this)
    {}

    ClientConnectionObserverFactory& ConnectionLoopBack::ClientObserverFactory()
    {
        return clientObserverFactory;
    }

    Connection& ConnectionLoopBack::Server()
    {
        return server;
    }

    Connection& ConnectionLoopBack::Client()
    {
        return client;
    }

    ConnectionLoopBackListener::ConnectionLoopBackListener(uint16_t port, ConnectionLoopBackFactory& loopBackFactory, ServerConnectionObserverFactory& connectionObserverFactory)
        : port(port)
        , loopBackFactory(loopBackFactory)
        , connectionObserverFactory(connectionObserverFactory)
    {
        loopBackFactory.RegisterListener(port, this);
    }

    ConnectionLoopBackListener::~ConnectionLoopBackListener()
    {
        loopBackFactory.UnregisterListener(port);
    }

    ConnectionLoopBackConnector::ConnectionLoopBackConnector(uint16_t port, ConnectionLoopBackFactory& loopBackFactory, ClientConnectionObserverFactory& connectionObserverFactory)
        : port(port)
        , loopBackFactory(loopBackFactory)
        , connectionObserverFactory(connectionObserverFactory)
    {}

    void ConnectionLoopBackConnector::Connect()
    {
        infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionLoopBackConnector>& object)
        {
            auto listener = object->loopBackFactory.listeners.find(object->port);
            if (listener != object->loopBackFactory.listeners.end())
                listener->second->Accept(object->connectionObserverFactory);
            else
                object->connectionObserverFactory.ConnectionFailed(services::ClientConnectionObserverFactory::ConnectFailReason::refused);
        }, SharedFromThis());
    }

    void ConnectionLoopBackListener::Accept(ClientConnectionObserverFactory& clientObserverFactory)
    {
        infra::SharedPtr<ConnectionLoopBack> connection = infra::MakeSharedOnHeap<ConnectionLoopBack>(clientObserverFactory);
        connectionObserverFactory.ConnectionAccepted([connection](infra::SharedPtr<services::ConnectionObserver> serverObserver)
        {
            if (serverObserver)
            {
                serverObserver->Attach(connection->Server());
                connection->Server().SetOwnership(connection, serverObserver);
                connection->ClientObserverFactory().ConnectionEstablished([connection](infra::SharedPtr<services::ConnectionObserver> clientObserver)
                {
                    if (clientObserver)
                    {
                        clientObserver->Attach(connection->Client());
                        connection->Client().SetOwnership(connection, clientObserver);
                        connection->Client().GetObserver().Connected();
                        connection->Server().GetObserver().Connected();
                    }
                });
            }
            else
                connection->ClientObserverFactory().ConnectionFailed(services::ClientConnectionObserverFactory::ConnectFailReason::connectionAllocationFailed);
        }, services::IPv4AddressLocalHost());
    }

    ConnectionLoopBackFactory::~ConnectionLoopBackFactory()
    {
        assert(listeners.empty());
    }

    void ConnectionLoopBackFactory::RegisterListener(uint16_t port, ConnectionLoopBackListener* listener)
    {
        assert(listeners.find(port) == listeners.end());
        listeners.insert(std::make_pair(port, listener));
    }

    void ConnectionLoopBackFactory::UnregisterListener(uint16_t port)
    {
        assert(listeners.find(port) != listeners.end());
        listeners.erase(port);
    }

    infra::SharedPtr<void> ConnectionLoopBackFactory::Listen(uint16_t port, ServerConnectionObserverFactory& factory)
    {
        return infra::MakeSharedOnHeap<ConnectionLoopBackListener>(port, *this, factory);
    }

    infra::SharedPtr<void> ConnectionLoopBackFactory::Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory)
    {
        infra::SharedPtr<ConnectionLoopBackConnector> connector = infra::MakeSharedOnHeap<ConnectionLoopBackConnector>(port, *this, factory);
        connector->Connect();
        return connector;
    }
}
