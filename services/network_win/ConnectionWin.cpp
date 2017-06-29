#include "services/network_win/ConnectionWin.hpp"
#include <thread>

namespace services
{
    ConnectionWin::ConnectionWin(EventDispatcherWithNetwork& network, SOCKET socket)
        : network(network)
        , socket(socket)
    {}

    ConnectionWin::~ConnectionWin()
    {
        if (socket != 0)
        {
            int result = closesocket(socket);
            assert(result != SOCKET_ERROR);
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
        return receiveStream.Emplace(*this);
    }

    void ConnectionWin::AckReceived()
    {
        receiveStream->ConsumeRead();
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
        sockaddr_in address {};
        int addressLength = sizeof(address);
        getpeername(socket, reinterpret_cast<SOCKADDR*>(&address), &addressLength);

        return IPv4Address{
            static_cast<uint8_t>(address.sin_addr.s_addr >> 24),
            static_cast<uint8_t>(address.sin_addr.s_addr >> 16),
            static_cast<uint8_t>(address.sin_addr.s_addr >> 8),
            static_cast<uint8_t>(address.sin_addr.s_addr)
        };
    }

    bool ConnectionWin::ReadyToReceive() const
    {
        return !receiveBuffer.full();
    }

    bool ConnectionWin::ReadyToSend() const
    {
        return !sendBuffer.empty();
    }

    void ConnectionWin::Receive()
    {
        std::array<uint8_t, 2048> buffer;
        int received = recv(socket, reinterpret_cast<char*>(buffer.data()), receiveBuffer.max_size() - receiveBuffer.size(), 0);
        if (received == SOCKET_ERROR)
            ResetOwnership();
        else if (received != 0)
        {
            receiveBuffer.insert(receiveBuffer.end(), buffer.data(), buffer.data() + received);

            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionWin>& object)
            {
                object->GetObserver().DataReceived();
            }, SharedFromThis());
        }
        else
            ResetOwnership();
    }

    void ConnectionWin::Send()
    {
        int sent = 0;

        do
        {
            sent = send(socket, reinterpret_cast<char*>(sendBuffer.contiguous_range(sendBuffer.begin()).begin())
                , sendBuffer.contiguous_range(sendBuffer.begin()).size(), 0);

            if (sent == SOCKET_ERROR)
            {
                ResetOwnership();
                return;
            }

            sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + sent);
        } while (sent != 0 && !sendBuffer.empty());

        if (requestedSendSize != 0)
            TryAllocateSendStream();
    }

    void ConnectionWin::TryAllocateSendStream()
    {
        assert(sendStream.Allocatable());
        if (sendBuffer.max_size() - sendBuffer.size() >= requestedSendSize)
        {
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionWin>& object)
            {
                infra::SharedPtr<infra::DataOutputStream> stream = object->sendStream.Emplace(*object);
                object->GetObserver().SendStreamAvailable(std::move(stream));
            }, SharedFromThis());

            requestedSendSize = 0;
        }
    }

    ConnectionWin::SendStream::SendStream(ConnectionWin& connection)
        : infra::DataOutputStream(static_cast<infra::StreamWriter&>(*this))
        , connection(connection)
    {}

    void ConnectionWin::SendStream::Insert(infra::ConstByteRange range)
    {
        connection.sendBuffer.insert(connection.sendBuffer.end(), range.begin(), range.end());
    }

    void ConnectionWin::SendStream::Insert(uint8_t element)
    {
        connection.sendBuffer.push_back(element);
    }

    ConnectionWin::ReceiveStreamWin::ReceiveStreamWin(ConnectionWin& connection)
        : infra::DataInputStream(static_cast<infra::StreamReader&>(*this))
        , connection(connection)
    {}

    void ConnectionWin::ReceiveStreamWin::ConsumeRead()
    {
        connection.receiveBuffer.erase(connection.receiveBuffer.begin(), connection.receiveBuffer.begin() + sizeRead);
        sizeRead = 0;
    }

    void ConnectionWin::ReceiveStreamWin::Extract(infra::ByteRange range)
    {
        std::copy(connection.receiveBuffer.begin() + sizeRead, connection.receiveBuffer.begin() + sizeRead + range.size(), range.begin());
        sizeRead += range.size();
    }

    uint8_t ConnectionWin::ReceiveStreamWin::ExtractOne()
    {
        return connection.receiveBuffer[sizeRead++];
    }

    uint8_t ConnectionWin::ReceiveStreamWin::Peek()
    {
        return connection.receiveBuffer[sizeRead];
    }

    infra::ConstByteRange ConnectionWin::ReceiveStreamWin::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange result = infra::Head(connection.receiveBuffer.contiguous_range(connection.receiveBuffer.begin() + sizeRead), max);
        sizeRead += result.size();
        return result;
    }

    bool ConnectionWin::ReceiveStreamWin::IsEmpty() const
    {
        return connection.receiveBuffer.size() == sizeRead;
    }

    std::size_t ConnectionWin::ReceiveStreamWin::SizeAvailable() const
    {
        return connection.receiveBuffer.size() - sizeRead;
    }

    ListenerWin::ListenerWin(EventDispatcherWithNetwork& network, uint16_t port, services::ZeroCopyServerConnectionObserverFactory& factory)
        : network(network)
        , factory(factory)
    {
        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        assert(listenSocket != INVALID_SOCKET);

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
        network.DeregisterListener(*this);
    }

    void ListenerWin::Accept()
    {
        SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);
        if (acceptedSocket == INVALID_SOCKET)
            std::abort();

        infra::SharedPtr<ConnectionWin> connection = infra::MakeSharedOnHeap<ConnectionWin>(network, acceptedSocket);
        infra::SharedPtr<services::ZeroCopyConnectionObserver> observer = factory.ConnectionAccepted(*connection);

        if (observer)
        {
            connection->SetOwnership(connection, observer);
            network.RegisterConnection(connection);
        }
    }

    EventDispatcherWithNetwork::EventDispatcherWithNetwork()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            std::abort();
    }

    EventDispatcherWithNetwork::~EventDispatcherWithNetwork()
    {
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

    infra::SharedPtr<void> EventDispatcherWithNetwork::Listen(uint16_t port, services::ZeroCopyServerConnectionObserverFactory& factory)
    {
        return infra::MakeSharedOnHeap<ListenerWin>(*this, port, factory);
    }

    infra::SharedPtr<void> EventDispatcherWithNetwork::Connect(IPv4Address address, uint16_t port, ZeroCopyClientConnectionObserverFactory& factory)
    {
        return nullptr;
    }

    void EventDispatcherWithNetwork::Idle()
    {
        FD_SET writeSet = {};
        FD_SET readSet = {};

        for (auto& listener : listeners)
            FD_SET(listener.listenSocket, &readSet);

        for (auto& weakConnection : connections)
        {
            if (infra::SharedPtr<ConnectionWin> connection = weakConnection)
            {
                if (connection->ReadyToReceive())
                    FD_SET(connection->socket, &readSet);
                if (connection->ReadyToSend())
                    FD_SET(connection->socket, &writeSet);
            }
        }

        if (readSet.fd_count != 0 || writeSet.fd_count != 0)
        {
            DWORD total = select(0, &readSet, &writeSet, nullptr, nullptr);
            if (total == SOCKET_ERROR)
                std::abort();
        }
        else
            std::this_thread::sleep_for(std::chrono::seconds(1));

        for (auto& listener : listeners)
        {
            if (FD_ISSET(listener.listenSocket, &readSet))
                listener.Accept();
        }

        for (auto& weakConnection : connections)
        {
            if (infra::SharedPtr<ConnectionWin> connection = weakConnection)
            {
                if (FD_ISSET(connection->socket, &readSet))
                    connection->Receive();
                if (FD_ISSET(connection->socket, &writeSet))
                    connection->Send();
            }
        }

        connections.remove_if([](const infra::WeakPtr<ConnectionWin>& connection) { return connection.lock() == nullptr; });
    }
}
