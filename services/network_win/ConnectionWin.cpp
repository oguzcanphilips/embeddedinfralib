#include "services/network_win/ConnectionWin.hpp"
#include <thread>

namespace services
{
    ConnectionWin::ConnectionWin(EventDispatcherWithNetwork& network, SOCKET socket)
        : network(network)
        , socket(socket)
    {
        UpdateEventFlags();

        infra::EventDispatcher::Instance().Schedule([this]() { Receive(); });
    }

    ConnectionWin::~ConnectionWin()
    {
        if (socket != 0)
        {
            BOOL result = WSACloseEvent(event);
            assert(result == TRUE);
            result = closesocket(socket);
            if (result == SOCKET_ERROR)
            {
                DWORD error = GetLastError();
                std::abort();
            }
        }
    }

    void ConnectionWin::RequestSendStream(std::size_t sendSize)
    {
        assert(requestedSendSize == 0);
        assert(sendSize != 0 && sendSize <= MaxSendStreamSize());
        requestedSendSize = sendSize;
        TryAllocateSendStream();
    }

    std::size_t ConnectionWin::MaxSendStreamSize() const
    {
        return sendBuffer.max_size();
    }

    infra::SharedPtr<infra::DataInputStream> ConnectionWin::ReceiveStream()
    {
        return receiveStream.Emplace(*this, infra::softFail);
    }

    void ConnectionWin::AckReceived()
    {
        receiveStream->Reader().ConsumeRead();
    }

    void ConnectionWin::CloseAndDestroy()
    {
        AbortAndDestroy();
    }

    void ConnectionWin::AbortAndDestroy()
    {
        int result = closesocket(socket);
        assert(result != SOCKET_ERROR);
        socket = 0;
        ResetOwnership();
    }

    IPv4Address ConnectionWin::Ipv4Address() const
    {
        sockaddr_in address{};
        int addressLength = sizeof(address);
        getpeername(socket, reinterpret_cast<SOCKADDR*>(&address), &addressLength);

        return IPv4Address{
            static_cast<uint8_t>(address.sin_addr.s_addr >> 24),
            static_cast<uint8_t>(address.sin_addr.s_addr >> 16),
            static_cast<uint8_t>(address.sin_addr.s_addr >> 8),
            static_cast<uint8_t>(address.sin_addr.s_addr)
        };
    }

    void ConnectionWin::SetObserver(infra::SharedPtr<services::ConnectionObserver> connectionObserver)
    {
        connectionObserver->Attach(*this);
        SetOwnership(SharedFromThis(), connectionObserver);
        network.RegisterConnection(SharedFromThis());
        connectionObserver->Connected();
    }

    void ConnectionWin::Receive()
    {
        while (!receiveBuffer.full())
        {
            std::array<uint8_t, 2048> buffer;
            int received = recv(socket, reinterpret_cast<char*>(buffer.data()), receiveBuffer.max_size() - receiveBuffer.size(), 0);
            if (received == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                    ResetOwnership();
                return;
            }
            else if (received != 0)
            {
                receiveBuffer.insert(receiveBuffer.end(), buffer.data(), buffer.data() + received);

                infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionWin>& object)
                {
                    object->GetObserver().DataReceived();
                }, SharedFromThis());
            }
            else
            {
                ResetOwnership();
                return;
            }
        }
    }

    void ConnectionWin::Send()
    {
        int sent = 0;

        do
        {
            UpdateEventFlags();     // If there is something to send, update the flags before calling send, because FD_SEND is an edge-triggered event.
            sent = send(socket, reinterpret_cast<char*>(sendBuffer.contiguous_range(sendBuffer.begin()).begin())
                , sendBuffer.contiguous_range(sendBuffer.begin()).size(), 0);
            UpdateEventFlags();

            if (sent == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                    ResetOwnership();
                return;
            }

            sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + sent);
        } while (sent != 0 && !sendBuffer.empty());

        if (requestedSendSize != 0)
            TryAllocateSendStream();
    }

    void ConnectionWin::TrySend()
    {
        if (trySend)
        {
            trySend = false;
            Send();
        }
    }

    void ConnectionWin::UpdateEventFlags()
    {
        int result = WSAEventSelect(socket, event, (!receiveBuffer.full() ? FD_READ : 0) | (!sendBuffer.empty() ? FD_WRITE : 0) | FD_CLOSE);
        assert(result == 0);
    }

    void ConnectionWin::TryAllocateSendStream()
    {
        assert(sendStream.Allocatable());
        if (sendBuffer.max_size() - sendBuffer.size() >= requestedSendSize)
        {
            auto size = requestedSendSize;
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([size](const infra::SharedPtr<ConnectionWin>& object)
            {
                infra::SharedPtr<infra::DataOutputStream> stream = object->sendStream.Emplace(*object, size);
                object->GetObserver().SendStreamAvailable(std::move(stream));
            }, SharedFromThis());

            requestedSendSize = 0;
        }
    }

    ConnectionWin::StreamWriterWin::StreamWriterWin(ConnectionWin& connection, std::size_t size)
        : std::vector<uint8_t>(size, 0)
        , infra::ByteOutputStreamWriter(infra::MakeRange(*this))
        , connection(connection)
    {}

    ConnectionWin::StreamWriterWin::~StreamWriterWin()
    {
        connection.sendBuffer.insert(connection.sendBuffer.end(), Processed().begin(), Processed().end());
        connection.trySend = true;
    }

    ConnectionWin::StreamReaderWin::StreamReaderWin(ConnectionWin& connection)
        : connection(connection)
    {}

    void ConnectionWin::StreamReaderWin::ConsumeRead()
    {
        connection.receiveBuffer.erase(connection.receiveBuffer.begin(), connection.receiveBuffer.begin() + sizeRead);
        sizeRead = 0;
    }

    void ConnectionWin::StreamReaderWin::Extract(infra::ByteRange range, infra::StreamErrorPolicy& errorPolicy)
    {
        std::copy(connection.receiveBuffer.begin() + sizeRead, connection.receiveBuffer.begin() + sizeRead + range.size(), range.begin());
        sizeRead += range.size();
    }

    uint8_t ConnectionWin::StreamReaderWin::Peek(infra::StreamErrorPolicy& errorPolicy)
    {
        return connection.receiveBuffer[sizeRead];
    }

    infra::ConstByteRange ConnectionWin::StreamReaderWin::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange result = infra::Head(connection.receiveBuffer.contiguous_range(connection.receiveBuffer.begin() + sizeRead), max);
        sizeRead += result.size();
        return result;
    }

    bool ConnectionWin::StreamReaderWin::Empty() const
    {
        return connection.receiveBuffer.size() == sizeRead;
    }

    std::size_t ConnectionWin::StreamReaderWin::Available() const
    {
        return connection.receiveBuffer.size() - sizeRead;
    }

    ListenerWin::ListenerWin(EventDispatcherWithNetwork& network, uint16_t port, services::ServerConnectionObserverFactory& factory)
        : network(network)
        , factory(factory)
    {
        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        assert(listenSocket != INVALID_SOCKET);
        int result = WSAEventSelect(listenSocket, event, FD_ACCEPT);
        assert(result == 0);

        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(port);
        if (bind(listenSocket, reinterpret_cast<SOCKADDR*>(&address), sizeof(SOCKADDR)) == SOCKET_ERROR)
            std::abort();

        if (listen(listenSocket, 1) == SOCKET_ERROR)
            std::abort();

        ULONG nonBlock = 1;
        if (ioctlsocket(listenSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
            std::abort();

        network.RegisterListener(*this);
    }

    ListenerWin::~ListenerWin()
    {
        BOOL result = WSACloseEvent(event);
        assert(result == TRUE);
        network.DeregisterListener(*this);
    }

    void ListenerWin::Accept()
    {
        SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);
        assert(acceptedSocket != INVALID_SOCKET);

        infra::SharedPtr<ConnectionWin> connection = infra::MakeSharedOnHeap<ConnectionWin>(network, acceptedSocket);
        factory.ConnectionAccepted([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
        {
            if (connectionObserver)
                connection->SetObserver(connectionObserver);
        }, connection->Ipv4Address());
    }

    ConnectorWin::ConnectorWin(EventDispatcherWithNetwork& network, IPv4Address address, uint16_t port, services::ClientConnectionObserverFactory& factory)
        : network(network)
        , factory(factory)
    {
        connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        assert(connectSocket != INVALID_SOCKET);
        int result = WSAEventSelect(connectSocket, event, FD_CONNECT);
        assert(result == 0);

        sockaddr_in saddress = {};
        saddress.sin_family = AF_INET;
        saddress.sin_addr.s_net = address[0];
        saddress.sin_addr.s_host = address[1];
        saddress.sin_addr.s_lh = address[2];
        saddress.sin_addr.s_impno = address[3];
        saddress.sin_port = htons(port);

        ULONG nonBlock = 1;
        if (ioctlsocket(connectSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
            std::abort();

        if (connect(connectSocket, reinterpret_cast<sockaddr*>(&saddress), sizeof(saddress)) == SOCKET_ERROR)
        {
            if (GetLastError() != WSAEWOULDBLOCK)
                std::abort();
        }

        network.RegisterConnector(*this);
    }

    ConnectorWin::~ConnectorWin()
    {
        BOOL result = WSACloseEvent(event);
        assert(result == TRUE);
        network.DeregisterConnector(*this);
    }

    void ConnectorWin::Connected()
    {
        infra::SharedPtr<ConnectionWin> connection = infra::MakeSharedOnHeap<ConnectionWin>(network, connectSocket);
        factory.ConnectionEstablished([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
        {
            if (connectionObserver)
                connection->SetObserver(connectionObserver);
        });

        infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectorWin>& connector) { connector->network.DeregisterConnector(*connector); }, SharedFromThis());
    }

    void ConnectorWin::Failed()
    {
        infra::WeakPtr<ConnectorWin> self = SharedFromThis();
        factory.ConnectionFailed(services::ClientConnectionObserverFactory::ConnectFailReason::refused);

        infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectorWin>& connector) { connector->network.DeregisterConnector(*connector); }, self);
    }

    EventDispatcherWithNetwork::EventDispatcherWithNetwork()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            std::abort();
    }

    EventDispatcherWithNetwork::~EventDispatcherWithNetwork()
    {
        BOOL result = WSACloseEvent(wakeUpEvent);
        assert(result == TRUE);
        WSACleanup();
    }

    void EventDispatcherWithNetwork::RegisterConnection(const infra::SharedPtr<ConnectionWin>& connection)
    {
        connections.push_back(connection);
    }

    void EventDispatcherWithNetwork::RegisterListener(ListenerWin& listener)
    {
        listeners.push_back(listener);
    }

    void EventDispatcherWithNetwork::DeregisterListener(ListenerWin& listener)
    {
        listeners.erase(listener);
    }

    void EventDispatcherWithNetwork::RegisterConnector(ConnectorWin& connector)
    {
        connectors.push_back(connector);
    }

    void EventDispatcherWithNetwork::DeregisterConnector(ConnectorWin& connector)
    {
        connectors.erase(connector);
    }

    infra::SharedPtr<void> EventDispatcherWithNetwork::Listen(uint16_t port, services::ServerConnectionObserverFactory& factory)
    {
        return infra::MakeSharedOnHeap<ListenerWin>(*this, port, factory);
    }

    infra::SharedPtr<void> EventDispatcherWithNetwork::Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory)
    {
        return infra::MakeSharedOnHeap<ConnectorWin>(*this, address, port, factory);
    }
    
    void EventDispatcherWithNetwork::RequestExecution()
    {
        BOOL result = WSASetEvent(wakeUpEvent);
        assert(result == TRUE);
    }

    void EventDispatcherWithNetwork::Idle()
    {
        std::vector<WSAEVENT> events;
        std::vector<std::function<void()>> functions;

        events.push_back(wakeUpEvent);
        functions.push_back([this]()
        {
            BOOL result = WSAResetEvent(wakeUpEvent);
            assert(result == TRUE);
        });

        for (auto& listener : listeners)
        {
            WSANETWORKEVENTS networkEvents;
            WSAEnumNetworkEvents(listener.listenSocket, listener.event, &networkEvents);
            assert((networkEvents.lNetworkEvents & FD_ACCEPT) != 0);

            events.push_back(listener.event);
            functions.push_back([&listener]() { listener.Accept(); });
        }

        for (auto& connector : connectors)
        {
            events.push_back(connector.event);
            functions.push_back([&connector]()
            {
                WSANETWORKEVENTS networkEvents;
                WSAEnumNetworkEvents(connector.connectSocket, connector.event, &networkEvents);
                assert((networkEvents.lNetworkEvents & FD_CONNECT) != 0);

                if (networkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
                    connector.Failed();
                else
                    connector.Connected();
            });
        }

        for (auto& weakConnection : connections)
            if (infra::SharedPtr<ConnectionWin> connection = weakConnection)
                connection->TrySend();

        for (auto& weakConnection : connections)
        {
            if (infra::SharedPtr<ConnectionWin> connection = weakConnection)
            {
                connection->UpdateEventFlags();
                events.push_back(connection->event);
                functions.push_back([weakConnection]()
                {
                    if (infra::SharedPtr<ConnectionWin> connection = weakConnection)
                    {
                        WSANETWORKEVENTS networkEvents;
                        WSAEnumNetworkEvents(connection->socket, connection->event, &networkEvents);

                        if ((networkEvents.lNetworkEvents & FD_READ_BIT) != 0)
                            connection->Receive();
                        if ((networkEvents.lNetworkEvents & FD_WRITE_BIT) != 0)
                            connection->Send();
                        if ((networkEvents.lNetworkEvents & FD_CLOSE_BIT) != 0)
                            connection->Receive();
                    }
                });
            }
        }

        DWORD index = WSAWaitForMultipleEvents(events.size(), events.data(), FALSE, WSA_INFINITE, FALSE);
        if (index >= WSA_WAIT_EVENT_0 && index < WSA_WAIT_EVENT_0 + events.size())
            functions[index - WSA_WAIT_EVENT_0]();
        else
            std::abort();

        connections.remove_if([](const infra::WeakPtr<ConnectionWin>& connection) { return connection.lock() == nullptr; });
    }
}
