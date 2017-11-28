#include "services/network/test_doubles/ConnectionMock.hpp"

namespace services
{
    infra::SharedPtr<void> ConnectionFactoryMock::Listen(uint16_t port, ServerConnectionObserverFactory& factory)
    {
        this->serverConnectionObserverFactory = &factory;
        return ListenMock(port);
    }

    void ConnectionFactoryMock::NewConnection(Connection& connection, services::IPv4Address ipv4Address)
    {
        serverConnectionObserverFactory->ConnectionAccepted([&connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
        {
            connectionObserver->Attach(connection);
            connection.SetOwnership(nullptr, connectionObserver);
            connectionObserver->Connected();
        }, ipv4Address);
    }

    ConnectionObserverMock::ConnectionObserverMock(services::Connection& connection)
        : services::ConnectionObserver(connection)
    {}
}
