#ifndef NETWORK_CONNECTION_MOCK_HPP
#define NETWORK_CONNECTION_MOCK_HPP

#include "gmock/gmock.h"
#include "services/network/Connection.hpp"
#include <vector>

namespace services
{
    //TICS -INT#002: A mock or stub may have public data
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
