#include "services/network/test_doubles/ConnectionMock.hpp"

namespace services
{
    void ConnectionMock::Send(infra::ConstByteRange data)
    {
        SendMock(std::vector<uint8_t>(data.begin(), data.end()));
    }

    void ConnectionMock::CloseAndDestroy()
    {
        CloseAndDestroyMock();
        ResetOwnership();
    }

    void ConnectionMock::AbortAndDestroy()
    {
        AbortAndDestroyMock();
        ResetOwnership();
    }

    void ConnectionMock::DataSent()
    {
        GetObserver().DataSent();
    }

    void ConnectionMock::DataReceived(infra::ConstByteRange data)
    {
        GetObserver().DataReceived(data);
    }

    infra::SharedPtr<void> ListenerMock::Listen(uint16_t port, ConnectionObserverFactory& connectionObserverFactory)
    {
        this->connectionObserverFactory = &connectionObserverFactory;
        ListenMock(port);
        return nullptr;
    }

    bool ListenerMock::NewConnection(Connection& connection)
    {
        infra::SharedPtr<services::ConnectionObserver> connectionObserver = connectionObserverFactory->ConnectionAccepted(connection);

        if (!connectionObserver)
            return false;

        connection.SetOwnership(nullptr, connectionObserver);
        return true;
    }
}
