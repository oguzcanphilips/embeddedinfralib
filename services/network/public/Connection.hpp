#ifndef NETWORK_CONNECTION_HPP
#define NETWORK_CONNECTION_HPP

#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Observer.hpp"
#include "infra/util/public/SharedPtr.hpp"

namespace services
{
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
