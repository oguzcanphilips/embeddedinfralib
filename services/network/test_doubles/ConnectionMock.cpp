#include "services/network/test_doubles/ConnectionMock.hpp"

namespace services
{
    infra::SharedPtr<void> ConnectionFactoryMock::Listen(uint16_t port, ServerConnectionObserverFactory& factory)
    {
        this->serverConnectionObserverFactory = &factory;
        return ListenMock(port);
    }

    void ConnectionFactoryMock::NewConnection(Connection& connection, services::IPv4Address address)
    {
        serverConnectionObserverFactory->ConnectionAccepted([&connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
        {
            connectionObserver->Attach(connection);
            connection.SetOwnership(nullptr, connectionObserver);
            connectionObserver->Connected();
        }, address);
    }

    ConnectionObserverMock::ConnectionObserverMock(services::Connection& connection)
        : services::ConnectionObserver(connection)
    {}

    infra::SharedPtr<void> ConnectionIPv6FactoryMock::Listen(uint16_t port, ServerConnectionIPv6ObserverFactory& factory)
    {
        this->serverConnectionObserverFactory = &factory;
        return ListenMock(port);
    }

    void ConnectionIPv6FactoryMock::NewConnection(Connection& connection, services::IPv6Address address)
    {
        serverConnectionObserverFactory->ConnectionAccepted([&connection](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
        {
            connectionObserver->Attach(connection);
            connection.SetOwnership(nullptr, connectionObserver);
            connectionObserver->Connected();
        }, address);
    }
}
