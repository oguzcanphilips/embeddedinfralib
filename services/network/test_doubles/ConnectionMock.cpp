#include "services/network/test_doubles/ConnectionMock.hpp"

namespace services
{
    infra::SharedPtr<void> ConnectionFactoryMock::Listen(uint16_t port, ServerConnectionObserverFactory& factory)
    {
        this->serverConnectionObserverFactory = &factory;
        return ListenMock(port);
    }

    bool ConnectionFactoryMock::NewConnection(Connection& connection)
    {
        infra::SharedPtr<services::ConnectionObserver> connectionObserver = serverConnectionObserverFactory->ConnectionAccepted(connection);

        if (!connectionObserver)
            return false;

        connection.SetOwnership(nullptr, connectionObserver);
        return true;
    }

    ConnectionObserverMock::ConnectionObserverMock(services::Connection& connection)
        : services::ConnectionObserver(connection)
    {}
}
