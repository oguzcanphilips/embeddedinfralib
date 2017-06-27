#ifndef SERVICES_CONNECTION_HPP
#define SERVICES_CONNECTION_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/stream/OutputStream.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/SharedPtr.hpp"
#include "services/network/Address.hpp"

namespace services
{
    class ZeroCopyConnection;

    class ZeroCopyConnectionObserver
        : public infra::SingleObserver<ZeroCopyConnectionObserver, ZeroCopyConnection>
    {
    protected:
        explicit ZeroCopyConnectionObserver(ZeroCopyConnection& connection);
        ZeroCopyConnectionObserver(const ZeroCopyConnectionObserver& other) = delete;
        ZeroCopyConnectionObserver& operator=(const ZeroCopyConnectionObserver& other) = delete;
        ~ZeroCopyConnectionObserver() = default;

    public:
        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) = 0;
        virtual void DataReceived() = 0;

    private:
        friend class ZeroCopyConnection;
    };

    class ZeroCopyConnection
        : public infra::Subject<ZeroCopyConnectionObserver>
    {
    protected:
        ZeroCopyConnection() = default;
        ZeroCopyConnection(const ZeroCopyConnection& other) = delete;
        ZeroCopyConnection& operator=(const ZeroCopyConnection& other) = delete;
        ~ZeroCopyConnection() = default;

    public:
        // A new send stream may only be requested when any previous send stream has been destroyed
        virtual void RequestSendStream(std::size_t sendSize) = 0;
        virtual std::size_t MaxSendStreamSize() const = 0;
        // A new receive stream may only be requested when any previous receive streams has been destroyed
        virtual infra::SharedPtr<infra::DataInputStream> ReceiveStream() = 0;
        // When data from the receive stream has been processed, call AckReceived to free the TCP window
        // If AckReceived is not called, a next call to ReceiveStream will return a stream consisting of the same data
        virtual void AckReceived() = 0;

        virtual void CloseAndDestroy() = 0;
        virtual void AbortAndDestroy() = 0;

        void SwitchObserver(const infra::SharedPtr<ZeroCopyConnectionObserver>& newObserver);
        void SetOwnership(const infra::SharedPtr<void>& owner, const infra::SharedPtr<ZeroCopyConnectionObserver>& observer);
        void ResetOwnership();

    private:
        infra::SharedPtr<void> owner;
        infra::SharedPtr<ZeroCopyConnectionObserver> observer;
    };

    class ZeroCopyServerConnectionObserverFactory
    {
    protected:
        ZeroCopyServerConnectionObserverFactory() = default;
        ZeroCopyServerConnectionObserverFactory(const ZeroCopyServerConnectionObserverFactory& other) = delete;
        ZeroCopyServerConnectionObserverFactory& operator=(const ZeroCopyServerConnectionObserverFactory& other) = delete;
        ~ZeroCopyServerConnectionObserverFactory() = default;

    public:
        virtual infra::SharedPtr<ZeroCopyConnectionObserver> ConnectionAccepted(ZeroCopyConnection& newConnection) = 0;
    };

    class ZeroCopyClientConnectionObserverFactory
    {
    protected:
        ZeroCopyClientConnectionObserverFactory() = default;
        ZeroCopyClientConnectionObserverFactory(const ZeroCopyClientConnectionObserverFactory& other) = delete;
        ZeroCopyClientConnectionObserverFactory& operator=(const ZeroCopyClientConnectionObserverFactory& other) = delete;
        ~ZeroCopyClientConnectionObserverFactory() = default;

    public:
        enum ConnectFailReason
        {
            refused,
            connectionAllocationFailed
        };

        virtual infra::SharedPtr<ZeroCopyConnectionObserver> ConnectionEstablished(ZeroCopyConnection& newConnection) = 0;
        virtual void ConnectionFailed(ConnectFailReason reason) = 0;
    };

    class ZeroCopyConnectionFactory
    {
    protected:
        ZeroCopyConnectionFactory() = default;
        ZeroCopyConnectionFactory(const ZeroCopyConnectionFactory& other) = delete;
        ZeroCopyConnectionFactory& operator=(const ZeroCopyConnectionFactory& other) = delete;
        ~ZeroCopyConnectionFactory() = default;

    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, ZeroCopyServerConnectionObserverFactory& factory) = 0;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ZeroCopyClientConnectionObserverFactory& factory) = 0;
    };

    class Connection;

    class ConnectionObserver
        : public infra::SingleObserver<ConnectionObserver, Connection>
    {
    protected:
        explicit ConnectionObserver(services::Connection& connection);
        ~ConnectionObserver() = default;

    public:
        virtual void DataSent() = 0;
        virtual void DataReceived(infra::ConstByteRange data) = 0;
    };

    class Connection
        : public infra::Subject<ConnectionObserver>
    {
    public:
        virtual void Send(infra::ConstByteRange data) = 0;
        virtual void CloseAndDestroy() = 0;
        virtual void AbortAndDestroy() = 0;
        virtual IPv4Address GetIpv4Address() = 0;

        void SwitchObserver(const infra::SharedPtr<ConnectionObserver>& newObserver);
        void SetOwnership(const infra::SharedPtr<Connection>& connection, const infra::SharedPtr<ConnectionObserver>& observer);
        void ResetOwnership();        

    protected:
        ~Connection() = default;

    protected:
        infra::SharedPtr<Connection> Self();

    private:
        infra::SharedPtr<Connection> connection;
        infra::SharedPtr<ConnectionObserver> observer;
    };

    class ConnectionObserverFactory
    {
    public:
        ConnectionObserverFactory() = default;
        ConnectionObserverFactory(const ConnectionObserverFactory& other) = delete;
        ConnectionObserverFactory& operator=(const ConnectionObserverFactory& other) = delete;

        virtual infra::SharedPtr<ConnectionObserver> ConnectionAccepted(Connection& newConnection) = 0;

    protected:
        ~ConnectionObserverFactory() = default;
    };

    class ListenerFactory
    {
    public:
        ListenerFactory() = default;
        ListenerFactory(const ListenerFactory& other) = delete;
        ListenerFactory& operator=(const ListenerFactory& other) = delete;

        virtual infra::SharedPtr<void> Listen(uint16_t port, ConnectionObserverFactory& connectionObserverFactory) = 0;

    protected:
        ~ListenerFactory() = default;
    };
}

#endif
