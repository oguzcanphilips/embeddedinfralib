#ifndef SERVICES_CONNECTION_HPP
#define SERVICES_CONNECTION_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/stream/OutputStream.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/SharedPtr.hpp"
#include "services/network/Address.hpp"

namespace services
{
    class Connection;

    class ConnectionObserver
        : public infra::SingleObserver<ConnectionObserver, Connection>
    {
    protected:
        explicit ConnectionObserver(Connection& connection);
        ConnectionObserver(const ConnectionObserver& other) = delete;
        ConnectionObserver& operator=(const ConnectionObserver& other) = delete;
        ~ConnectionObserver() = default;

    public:
        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) = 0;
        virtual void DataReceived() = 0;
        virtual void ClosingConnection() {}

    private:
        friend class Connection;
    };

    class Connection
        : public infra::Subject<ConnectionObserver>
    {
    protected:
        Connection() = default;
        Connection(const Connection& other) = delete;
        Connection& operator=(const Connection& other) = delete;
        ~Connection() = default;

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

        virtual IPv4Address Ipv4Address() const = 0;

        void SwitchObserver(const infra::SharedPtr<ConnectionObserver>& newObserver);
        void SetOwnership(const infra::SharedPtr<void>& owner, const infra::SharedPtr<ConnectionObserver>& observer);
        void ResetOwnership();

    private:
        infra::SharedPtr<void> owner;
        infra::SharedPtr<ConnectionObserver> observer;
    };

    class ServerConnectionObserverFactory
    {
    protected:
        ServerConnectionObserverFactory() = default;
        ServerConnectionObserverFactory(const ServerConnectionObserverFactory& other) = delete;
        ServerConnectionObserverFactory& operator=(const ServerConnectionObserverFactory& other) = delete;
        ~ServerConnectionObserverFactory() = default;

    public:
        virtual infra::SharedPtr<ConnectionObserver> ConnectionAccepted(Connection& newConnection) = 0;
    };

    class ClientConnectionObserverFactory
    {
    protected:
        ClientConnectionObserverFactory() = default;
        ClientConnectionObserverFactory(const ClientConnectionObserverFactory& other) = delete;
        ClientConnectionObserverFactory& operator=(const ClientConnectionObserverFactory& other) = delete;
        ~ClientConnectionObserverFactory() = default;

    public:
        enum ConnectFailReason
        {
            refused,
            connectionAllocationFailed
        };

        virtual infra::SharedPtr<ConnectionObserver> ConnectionEstablished(Connection& newConnection) = 0;
        virtual void ConnectionFailed(ConnectFailReason reason) = 0;
    };

    class ConnectionFactory
    {
    protected:
        ConnectionFactory() = default;
        ConnectionFactory(const ConnectionFactory& other) = delete;
        ConnectionFactory& operator=(const ConnectionFactory& other) = delete;
        ~ConnectionFactory() = default;

    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, ServerConnectionObserverFactory& factory) = 0;
        virtual infra::SharedPtr<void> Connect(IPv4Address address, uint16_t port, ClientConnectionObserverFactory& factory) = 0;
    };
}

#endif
