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
        receiveStream->ConsumeRead();

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

    IPv4Address ConnectionLoopBackPeer::Ipv4Address() const
    {
        return IPv4Address{ 127, 0, 0, 1 };
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

    ConnectionLoopBackPeer::SendStreamLoopBackPeer::SendStreamLoopBackPeer(ConnectionLoopBackPeer& connection)
        : infra::DataOutputStream(static_cast<infra::StreamWriter&>(*this))
        , connection(connection)
    {}

    ConnectionLoopBackPeer::SendStreamLoopBackPeer::~SendStreamLoopBackPeer()
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

    void ConnectionLoopBackPeer::SendStreamLoopBackPeer::Insert(infra::ConstByteRange range)
    {
        connection.sendBuffer.insert(connection.sendBuffer.end(), range.begin(), range.end());
        sent += range.size();
    }

    void ConnectionLoopBackPeer::SendStreamLoopBackPeer::Insert(uint8_t element)
    {
        connection.sendBuffer.push_back(element);
        ++sent;
    }

    ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::ReceiveStreamLoopBackPeer(ConnectionLoopBackPeer& connection)
        : infra::DataInputStream(static_cast<infra::StreamReader&>(*this))
        , connection(connection)
    {}

    void ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::ConsumeRead()
    {
        connection.sendBuffer.erase(connection.sendBuffer.begin(), connection.sendBuffer.begin() + sizeRead);
        sizeRead = 0;
    }

    void ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::Extract(infra::ByteRange range)
    {
        std::copy(connection.sendBuffer.begin() + sizeRead, connection.sendBuffer.begin() + sizeRead + range.size(), range.begin());
        sizeRead += range.size();
    }

    uint8_t ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::ExtractOne()
    {
        return connection.sendBuffer[sizeRead++];
    }

    uint8_t ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::Peek()
    {
        return connection.sendBuffer[sizeRead];
    }

    infra::ConstByteRange ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange result = infra::Head(connection.sendBuffer.contiguous_range(connection.sendBuffer.begin() + sizeRead), max);
        sizeRead += result.size();
        return result;
    }

    bool ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::IsEmpty() const
    {
        return connection.sendBuffer.size() == sizeRead;
    }

    std::size_t ConnectionLoopBackPeer::ReceiveStreamLoopBackPeer::SizeAvailable() const
    {
        return connection.sendBuffer.size() - sizeRead;
    }

    ConnectionLoopBack::ConnectionLoopBack()
        : server(client, *this)
        , client(server, *this)
    {}

    ZeroCopyConnection& ConnectionLoopBack::Server()
    {
        return server;
    }

    ZeroCopyConnection& ConnectionLoopBack::Client()
    {
        return client;
    }

    ConnectionLoopBackListener::ConnectionLoopBackListener(uint16_t port, ConnectionLoopBackFactory& loopBackFactory, ZeroCopyServerConnectionObserverFactory& connectionObserverFactory)
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

    ConnectionLoopBackConnector::ConnectionLoopBackConnector(uint16_t port, ConnectionLoopBackFactory& loopBackFactory, ZeroCopyClientConnectionObserverFactory& connectionObserverFactory)
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
                object->connectionObserverFactory.ConnectionFailed(services::ZeroCopyClientConnectionObserverFactory::ConnectFailReason::refused);
        }, SharedFromThis());
    }

    void ConnectionLoopBackListener::Accept(ZeroCopyClientConnectionObserverFactory& clientObserverFactory)
    {
        infra::SharedPtr<ConnectionLoopBack> connection = infra::MakeSharedOnHeap<ConnectionLoopBack>();
        infra::SharedPtr<ZeroCopyConnectionObserver> serverObserver = connectionObserverFactory.ConnectionAccepted(connection->Server());
        if (serverObserver)
        {
            infra::SharedPtr<ZeroCopyConnectionObserver> clientObserver = clientObserverFactory.ConnectionEstablished(connection->Client());
            if (clientObserver)
            {
                connection->Server().SetOwnership(connection, serverObserver);
                connection->Client().SetOwnership(connection, clientObserver);
            }
        }
        else
            clientObserverFactory.ConnectionFailed(services::ZeroCopyClientConnectionObserverFactory::ConnectFailReason::connectionAllocationFailed);
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

    infra::SharedPtr<void> ConnectionLoopBackFactory::Listen(uint16_t port, ZeroCopyServerConnectionObserverFactory& factory)
    {
        return infra::MakeSharedOnHeap<ConnectionLoopBackListener>(port, *this, factory);
    }

    infra::SharedPtr<void> ConnectionLoopBackFactory::Connect(IPv4Address address, uint16_t port, ZeroCopyClientConnectionObserverFactory& factory)
    {
        infra::SharedPtr<ConnectionLoopBackConnector> connector = infra::MakeSharedOnHeap<ConnectionLoopBackConnector>(port, *this, factory);
        connector->Connect();
        return connector;
    }
}
