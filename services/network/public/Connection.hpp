#ifndef NETWORK_CONNECTION_HPP
#define NETWORK_CONNECTION_HPP

#include "infra/stream/public/InputStream.hpp"
#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Observer.hpp"
#include "infra/util/public/SharedPtr.hpp"

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
        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream> stream) = 0;
        virtual void DataReceived() = 0;
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

        void SetOwnership(const infra::SharedPtr<ZeroCopyConnection>& connection, const infra::SharedPtr<ZeroCopyConnectionObserver>& observer);
        void ResetOwnership();

    protected:
        infra::SharedPtr<ZeroCopyConnection> Self();

    private:
        infra::SharedPtr<ZeroCopyConnection> connection;
        infra::SharedPtr<ZeroCopyConnectionObserver> observer;
    };

    class ZeroCopyConnectionObserverFactory
    {
    protected:
        ZeroCopyConnectionObserverFactory() = default;
        ZeroCopyConnectionObserverFactory(const ZeroCopyConnectionObserverFactory& other) = delete;
        ZeroCopyConnectionObserverFactory& operator=(const ZeroCopyConnectionObserverFactory& other) = delete;
        ~ZeroCopyConnectionObserverFactory() = default;

    public:
        virtual infra::SharedPtr<ZeroCopyConnectionObserver> ConnectionAccepted(ZeroCopyConnection& newConnection) = 0;
    };

    class ZeroCopyListenerFactory
    {
    protected:
        ZeroCopyListenerFactory() = default;
        ZeroCopyListenerFactory(const ZeroCopyListenerFactory& other) = delete;
        ZeroCopyListenerFactory& operator=(const ZeroCopyListenerFactory& other) = delete;
        ~ZeroCopyListenerFactory() = default;

    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, ZeroCopyConnectionObserverFactory& factory) = 0;
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
