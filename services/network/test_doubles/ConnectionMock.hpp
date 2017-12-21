#ifndef NETWORK_CONNECTION_MOCK_HPP
#define NETWORK_CONNECTION_MOCK_HPP

#include "gmock/gmock.h"
#include "services/network/Connection.hpp"
#include <vector>

namespace services
{
    //TICS -INT#002: A mock or stub may have public data
    //TICS -INT#027: MOCK_METHOD can't add 'virtual' to its signature
    class ConnectionMock
        : public services::Connection
    {
    public:
        MOCK_METHOD1(RequestSendStream, void(std::size_t sendSize));
        MOCK_CONST_METHOD0(MaxSendStreamSize, std::size_t());
        MOCK_METHOD0(ReceiveStream, infra::SharedPtr<infra::DataInputStream>());
        MOCK_METHOD0(AckReceived, void());
        MOCK_METHOD0(CloseAndDestroy, void());
        MOCK_METHOD0(AbortAndDestroy, void());
        MOCK_CONST_METHOD0(Ipv4Address, IPv4Address());
    };

    class ConnectionObserverMock
        : public services::ConnectionObserver
    {
    public:
        ConnectionObserverMock() = default;
        explicit ConnectionObserverMock(services::Connection& connection);

        using services::ConnectionObserver::Subject;

        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) override { SendStreamAvailableMock(stream); }
        MOCK_METHOD1(SendStreamAvailableMock, void(infra::SharedPtr<infra::DataOutputStream> stream));
        MOCK_METHOD0(DataReceived, void());
    };

    class ConnectionFactoryMock
        : public services::ConnectionFactory
    {
    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, ServerConnectionObserverFactory& factory) override;
        MOCK_METHOD3(Connect, infra::SharedPtr<void>(services::IPv4Address address, uint16_t port, services::ClientConnectionObserverFactory& factory));

        MOCK_METHOD1(ListenMock, infra::SharedPtr<void>(uint16_t));

        void NewConnection(Connection& connection, services::IPv4Address ipv4Address);

    private:
        ServerConnectionObserverFactory* serverConnectionObserverFactory = nullptr;
    };

    class ServerConnectionObserverFactoryMock
        : public services::ServerConnectionObserverFactory
    {
    public:
        void ConnectionAccepted(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver, services::IPv4Address ipv4Address) { ConnectionAcceptedMock(createdObserver.Clone(), ipv4Address); }
        MOCK_METHOD2(ConnectionAcceptedMock, void(infra::Function<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)> createdObserver, services::IPv4Address ipv4Address));
    };

    class ClientConnectionObserverFactoryMock
        : public services::ClientConnectionObserverFactory
    {
    public:
        void ConnectionEstablished(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver) { ConnectionEstablishedMock(createdObserver.Clone()); }
        MOCK_METHOD1(ConnectionEstablishedMock, void(infra::Function<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)> createdObserver));
        MOCK_METHOD1(ConnectionFailed, void(ConnectFailReason reason));
    };
}

#endif
