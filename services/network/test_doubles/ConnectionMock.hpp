#ifndef NETWORK_CONNECTION_MOCK_HPP
#define NETWORK_CONNECTION_MOCK_HPP

#include "gmock/gmock.h"
#include "services/network/Connection.hpp"
#include <vector>

namespace services
{
    //TICS -INT#002: A mock or stub may have public data
    //TICS -INT#027: MOCK_METHOD can't add 'virtual' to its signature
    class ZeroCopyConnectionMock
        : public services::ZeroCopyConnection
    {
    public:
        MOCK_METHOD1(RequestSendStream, void(std::size_t sendSize));
        MOCK_CONST_METHOD0(MaxSendStreamSize, std::size_t());
        MOCK_METHOD0(ReceiveStream, infra::SharedPtr<infra::DataInputStream>());
        MOCK_METHOD0(AckReceived, void());
        MOCK_METHOD0(CloseAndDestroy, void());
        MOCK_METHOD0(AbortAndDestroy, void());
    };

    class ZeroCopyConnectionObserverMock
        : public services::ZeroCopyConnectionObserver
    {
    public:
        explicit ZeroCopyConnectionObserverMock(services::ZeroCopyConnection& connection);

        using services::ZeroCopyConnectionObserver::Subject;

        MOCK_METHOD1(SendStreamAvailable, void(infra::SharedPtr<infra::DataOutputStream>& stream));
        MOCK_METHOD0(DataReceived, void());
    };

    class ZeroCopyConnectionFactoryMock
        : public services::ZeroCopyConnectionFactory
    {
    public:
        MOCK_METHOD2(Listen, infra::SharedPtr<void>(uint16_t port, services::ZeroCopyServerConnectionObserverFactory& factory));
        MOCK_METHOD3(Connect, infra::SharedPtr<void>(services::IPv4Address address, uint16_t port, services::ZeroCopyClientConnectionObserverFactory& factory));
    };

    class ZeroCopyServerConnectionObserverFactoryMock
        : public services::ZeroCopyServerConnectionObserverFactory
    {
    public:
        MOCK_METHOD1(ConnectionAccepted, infra::SharedPtr<services::ZeroCopyConnectionObserver>(services::ZeroCopyConnection& newConnection));
    };

    class ZeroCopyClientConnectionObserverFactoryMock
        : public services::ZeroCopyClientConnectionObserverFactory
    {
    public:
        MOCK_METHOD1(ConnectionEstablished, infra::SharedPtr<ZeroCopyConnectionObserver>(ZeroCopyConnection& newConnection));
        MOCK_METHOD1(ConnectionFailed, void(ConnectFailReason reason));
    };

    class ConnectionMock
        : public Connection
    {
    public:
        virtual void Send(infra::ConstByteRange data) override;
        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;

        MOCK_METHOD1(SendMock, void(std::vector<uint8_t> dataSent));
        MOCK_METHOD0(CloseAndDestroyMock, void());
        MOCK_METHOD0(AbortAndDestroyMock, void());

        void DataSent();
        void DataReceived(infra::ConstByteRange data);
    };

    class ListenerMock
        : public services::ListenerFactory
    {
    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, ConnectionObserverFactory& connectionObserverFactory) override;

        MOCK_METHOD1(ListenMock, void(uint16_t));

        bool NewConnection(Connection& connection);

    private:
        ConnectionObserverFactory* connectionObserverFactory = nullptr;
    };
}

#endif
