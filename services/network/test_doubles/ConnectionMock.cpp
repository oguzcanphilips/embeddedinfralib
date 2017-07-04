#include "services/network/test_doubles/ConnectionMock.hpp"

namespace services
{
    infra::SharedPtr<void> ZeroCopyConnectionFactoryMock::Listen(uint16_t port, ZeroCopyServerConnectionObserverFactory& factory)
    {
        this->serverConnectionObserverFactory = &factory;
        return ListenMock(port);
    }

    bool ZeroCopyConnectionFactoryMock::NewConnection(ZeroCopyConnection& connection)
    {
        infra::SharedPtr<services::ZeroCopyConnectionObserver> connectionObserver = serverConnectionObserverFactory->ConnectionAccepted(connection);

        if (!connectionObserver)
            return false;

        connection.SetOwnership(nullptr, connectionObserver);
        return true;
    }

    ZeroCopyConnectionObserverMock::ZeroCopyConnectionObserverMock(services::ZeroCopyConnection& connection)
        : services::ZeroCopyConnectionObserver(connection)
    {}
}
