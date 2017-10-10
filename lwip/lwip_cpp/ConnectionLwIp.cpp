#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "lwip/lwip_cpp/ConnectionLwIp.hpp"
#include "lwip/init.h"
#include "lwip/timeouts.h"

namespace services
{
    ConnectionLwIp::ConnectionLwIp(tcp_pcb* control)
        : control(control)
    {
        assert(control != nullptr);
        tcp_arg(control, this);
        tcp_recv(control, &ConnectionLwIp::Recv);
        tcp_err(control, &ConnectionLwIp::Err);
        tcp_sent(control, &ConnectionLwIp::Sent);
        tcp_nagle_disable(control);
    }

    ConnectionLwIp::~ConnectionLwIp()
    {
        if (control)
        {
            tcp_pcb* c = control;
            ResetControl();
            tcp_abort(c);
        }
    }

    void ConnectionLwIp::RequestSendStream(std::size_t sendSize)
    {
        assert(requestedSendSize == 0);
        assert(sendSize != 0 && sendSize <= MaxSendStreamSize());
        requestedSendSize = sendSize;
        TryAllocateSendStream();
    }

    std::size_t ConnectionLwIp::MaxSendStreamSize() const
    {
        return tcp_mss(control);
    }

    infra::SharedPtr<infra::DataInputStream> ConnectionLwIp::ReceiveStream()
    {
        return receiveStream.Emplace(*this);
    }

    void ConnectionLwIp::AckReceived()
    {
        receiveStream->Reader().ConsumeRead();
    }

    void ConnectionLwIp::CloseAndDestroy()
    {
        err_t result = tcp_close(control);
        if  (result != ERR_OK)
            tcp_abort(control);

        ResetControl();
        ResetOwnership();
    }

    void ConnectionLwIp::AbortAndDestroy()
    {
        tcp_abort(control);

        ResetControl();
        ResetOwnership();
    }

    IPv4Address ConnectionLwIp::Ipv4Address() const
    {
        return IPv4Address{
            static_cast<uint8_t>(ip_addr_get_ip4_u32(&control->remote_ip) >> 24),
            static_cast<uint8_t>(ip_addr_get_ip4_u32(&control->remote_ip) >> 16),
            static_cast<uint8_t>(ip_addr_get_ip4_u32(&control->remote_ip) >> 8),
            static_cast<uint8_t>(ip_addr_get_ip4_u32(&control->remote_ip))
        };
    }

    void ConnectionLwIp::SendBuffer(infra::ConstByteRange buffer)
    {
        err_t result = tcp_write(control, buffer.begin(), static_cast<uint16_t>(buffer.size()), 0);
        if (result == ERR_OK)
        {
            tcp_output(control);
            sendBuffers.push_back(buffer);
            sendBuffer.clear();
        }
        else
        {
            sendBuffer = buffer;
            retrySendTimer.Start(std::chrono::milliseconds(50), [this]() { SendBuffer(sendBuffer); });
        }
    }

    void ConnectionLwIp::TryAllocateSendStream()
    {
        assert(sendStream.Allocatable());
        if (!sendBuffers.full() && !sendMemoryPool.full() && sendBuffer.empty())
        {
            sendMemoryPool.emplace_back();
            infra::ByteRange sendBuffer = infra::Head(infra::ByteRange(sendMemoryPool.back()), requestedSendSize);
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([sendBuffer](const infra::SharedPtr<ConnectionLwIp>& object)
            {
                infra::SharedPtr<infra::DataOutputStream> stream = object->sendStream.Emplace(*object, sendBuffer);
                object->GetObserver().SendStreamAvailable(std::move(stream));
            }, SharedFromThis());

            requestedSendSize = 0;
        }
    }

    void ConnectionLwIp::ResetControl()
    {
        tcp_arg(control, nullptr);
        tcp_recv(control, nullptr);
        tcp_err(control, nullptr);
        tcp_sent(control, nullptr);
        control = nullptr;
    }

    err_t ConnectionLwIp::Recv(void* arg, tcp_pcb* tpcb, pbuf* p, err_t err)
    {
        return static_cast<ConnectionLwIp*>(arg)->Recv(p, err);
    }

    void ConnectionLwIp::Err(void* arg, err_t err)
    {
        static_cast<ConnectionLwIp*>(arg)->Err(err);
    }

    err_t ConnectionLwIp::Sent(void* arg, struct tcp_pcb* tpcb, std::uint16_t len)
    {
        return static_cast<ConnectionLwIp*>(arg)->Sent(len);
    }

    err_t ConnectionLwIp::Recv(pbuf* p, err_t err)
    {
        if (p != nullptr)
        {
            for (pbuf* q = p; q != nullptr; q = q->next)
                receiveBuffer.insert(receiveBuffer.end(), reinterpret_cast<uint8_t*>(q->payload), reinterpret_cast<uint8_t*>(q->payload) + q->len);
            pbuf_free(p);

            if (!dataReceivedScheduled && HasObserver())
            {
                dataReceivedScheduled = true;
                infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionLwIp>& object)
                {
                    object->dataReceivedScheduled = false;
                    object->GetObserver().DataReceived();
                }, SharedFromThis());
            }
        }
        else
            ResetOwnership();

        return ERR_OK;
    }

    void ConnectionLwIp::Err(err_t err)
    {
        assert(err == ERR_RST || err == ERR_CLSD);
        ResetControl();
        ResetOwnership();
    }

    err_t ConnectionLwIp::Sent(std::uint16_t len)
    {
        while (len != 0)
        {
            assert(!sendBuffers.empty());
            if (sendBuffers.front().size() <= len)
            {
                len -= static_cast<uint16_t>(sendBuffers.front().size());
                sendBuffers.pop_front();
                sendMemoryPool.pop_front();
            }
            else
            {
                sendBuffers.front().pop_front(len);
                len = 0;
            }
        }

        if (requestedSendSize != 0)
            TryAllocateSendStream();

        return ERR_OK;
    }

    ConnectionLwIp::StreamWriterLwIp::StreamWriterLwIp(ConnectionLwIp& connection, infra::ByteRange sendBuffer)
        : infra::ByteOutputStreamWriter(sendBuffer)
        , connection(connection)
    {}

    ConnectionLwIp::StreamWriterLwIp::~StreamWriterLwIp()
    {
        if (!Processed().empty())
            connection.SendBuffer(Processed());
        else
            connection.sendMemoryPool.pop_back();
    }

    ConnectionLwIp::StreamReaderLwIp::StreamReaderLwIp(ConnectionLwIp& connection)
        : infra::StreamReader(infra::softFail)
        , connection(connection)
    {}

    void ConnectionLwIp::StreamReaderLwIp::ConsumeRead()
    {
        tcp_recved(connection.control, sizeRead);
        connection.receiveBuffer.erase(connection.receiveBuffer.begin(), connection.receiveBuffer.begin() + sizeRead);
        sizeRead = 0;
    }

    void ConnectionLwIp::StreamReaderLwIp::Extract(infra::ByteRange range)
    {
        while (!range.empty() && !Empty())
        {
            infra::ByteRange inputRange = connection.receiveBuffer.contiguous_range(connection.receiveBuffer.begin() + sizeRead);
            inputRange.shrink_from_back_to(range.size());
            infra::Copy(inputRange, infra::Head(range, inputRange.size()));
            range.pop_front(inputRange.size());
            sizeRead += static_cast<uint16_t>(inputRange.size());
        }

        ReportResult(range.empty());
    }

    uint8_t ConnectionLwIp::StreamReaderLwIp::ExtractOne()
    {
        bool available = !Empty();

        ReportResult(available);
        if (available)
            return connection.receiveBuffer[sizeRead++];
        else
            return 0;
    }

    uint8_t ConnectionLwIp::StreamReaderLwIp::Peek()
    {
        bool available = !Empty();

        ReportResult(available);
        if (available)
            return connection.receiveBuffer[sizeRead];
        else
            return 0;
    }

    infra::ConstByteRange ConnectionLwIp::StreamReaderLwIp::ExtractContiguousRange(std::size_t max)
    {
        if (Empty())
            return infra::ConstByteRange();
        else
        {
            infra::ConstByteRange result = infra::Head(connection.receiveBuffer.contiguous_range(connection.receiveBuffer.begin() + sizeRead), max);
            sizeRead += static_cast<uint16_t>(result.size());
            return result;
        }
    }

    bool ConnectionLwIp::StreamReaderLwIp::Empty() const
    {
        return sizeRead == connection.receiveBuffer.size();
    }

    std::size_t ConnectionLwIp::StreamReaderLwIp::Available() const
    {
        return connection.receiveBuffer.size() - sizeRead;
    }

    ListenerLwIp::ListenerLwIp(AllocatorConnectionLwIp& allocator, uint16_t port, ServerConnectionObserverFactory& factory)
        : allocator(allocator)
        , factory(factory)
    {
        tcp_pcb* pcb = tcp_new();
        assert(pcb != nullptr);
        err_t err = tcp_bind(pcb, IP_ADDR_ANY, port);
        listenPort = tcp_listen(pcb);
        assert(listenPort != nullptr);
        tcp_accept(listenPort, &ListenerLwIp::Accept);
        tcp_arg(listenPort, this);
    }

    ListenerLwIp::~ListenerLwIp()
    {
        tcp_close(listenPort);
    }

    err_t ListenerLwIp::Accept(void* arg, struct tcp_pcb* newPcb, err_t err)
    {
        return static_cast<ListenerLwIp*>(arg)->Accept(newPcb, err);
    }

    err_t ListenerLwIp::Accept(tcp_pcb* newPcb, err_t err)
    {
        tcp_accepted(listenPort);
        infra::SharedPtr<ConnectionLwIp> connection = allocator.Allocate(newPcb);
        if (connection)
        {
            factory.ConnectionAccepted([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
            {
                if (connectionObserver)
                {
                    connectionObserver->Attach(*connection);
                    connection->SetOwnership(connection, connectionObserver);
                    connectionObserver->Connected();
                }
            });

            infra::WeakPtr<ConnectionLwIp> weakConnection = connection;
            connection = nullptr;
            if (weakConnection.lock())
                return ERR_OK;
            else
                return ERR_ABRT;
        }
        else
        {
            tcp_abort(newPcb);
            return ERR_ABRT;
        }
    }

    ConnectorLwIp::ConnectorLwIp(AllocatorConnectionLwIp& allocator, IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory)
        : allocator(allocator)
        , factory(factory)
        , control(tcp_new())
    {
        tcp_arg(control, this);
        tcp_err(control, &ConnectorLwIp::StaticError);

        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress, address[0], address[1], address[2], address[3]);
        err_t result = tcp_connect(control, &ipAddress, port, &ConnectorLwIp::StaticConnected);
        assert(result == ERR_OK);
    }

    ConnectorLwIp::~ConnectorLwIp()
    {
        if (control != nullptr)
            tcp_abort(control);
    }

    err_t ConnectorLwIp::StaticConnected(void* arg, tcp_pcb* tpcb, err_t err)
    {
        ConnectorLwIp* connector = reinterpret_cast<ConnectorLwIp*>(arg);
        assert(tpcb == connector->control);
        assert(err == ERR_OK);
        return connector->Connected();
    }

    void ConnectorLwIp::StaticError(void* arg, err_t err)
    {
        reinterpret_cast<ConnectorLwIp*>(arg)->Error(err);
    }

    err_t ConnectorLwIp::Connected()
    {
        infra::SharedPtr<ConnectionLwIp> connection = allocator.Allocate(control);
        if (connection)
        {
            control = nullptr;
            factory.ConnectionEstablished([connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
            {
                if (connectionObserver)
                {
                    connectionObserver->Attach(*connection);
                    connection->SetOwnership(connection, connectionObserver);
                    connectionObserver->Connected();
                }
            });

            infra::WeakPtr<ConnectionLwIp> weakConnection = connection;
            connection = nullptr;
            if (weakConnection.lock())
                return ERR_OK;
            else
                return ERR_ABRT;
        }
        else
        {
            tcp_abort(control);
            control = nullptr;
            factory.ConnectionFailed(factory.ConnectFailReason::connectionAllocationFailed);
            return ERR_ABRT;
        }
    }

    void ConnectorLwIp::Error(err_t err)
    {
        control = nullptr;
        factory.ConnectionFailed(factory.ConnectFailReason::refused);
    }

    LightweightIp::LightweightIp(AllocatorListenerLwIp& listenerAllocator, AllocatorConnectorLwIp& connectorAllocator, AllocatorConnectionLwIp& connectionAllocator)
        : listenerAllocator(listenerAllocator)
        , connectorAllocator(connectorAllocator)
        , connectionAllocator(connectionAllocator)
    {
        lwip_init();
        sysCheckTimer.Start(std::chrono::milliseconds(50), [this]() { sys_check_timeouts(); }, infra::triggerImmediately);
    }

    infra::SharedPtr<void> LightweightIp::Listen(uint16_t port, ServerConnectionObserverFactory& factory)
    {
        return listenerAllocator.Allocate(connectionAllocator, port, factory);
    }

    infra::SharedPtr<void> LightweightIp::Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory)
    {
        return connectorAllocator.Allocate(connectionAllocator, address, port, factory);
    }
}
