#include "services/network/public/Connection.hpp"
#include "infra/event/public/EventDispatcher.hpp"

namespace services
{
    ZeroCopyConnectionObserver::ZeroCopyConnectionObserver(services::ZeroCopyConnection& connection)
        : infra::SingleObserver<ZeroCopyConnectionObserver, ZeroCopyConnection>(connection)
    {}

    void ZeroCopyConnection::SetOwnership(const infra::SharedPtr<ZeroCopyConnection>& connection, const infra::SharedPtr<ZeroCopyConnectionObserver>& observer)
    {
        this->connection = connection;
        this->observer = observer;
    }

    void ZeroCopyConnection::ResetOwnership()
    {
        observer = nullptr;
        connection = nullptr;
    }

    infra::SharedPtr<ZeroCopyConnection> ZeroCopyConnection::Self()
    {
        return connection;
    }

    ConnectionObserver::ConnectionObserver(services::Connection& connection)
        : infra::SingleObserver<ConnectionObserver, Connection>(connection)
    {}

    void Connection::SwitchObserver(const infra::SharedPtr<ConnectionObserver>& newObserver)
    {
        this->observer = nullptr;
        this->observer = newObserver;
    }

    void Connection::SetOwnership(const infra::SharedPtr<Connection>& connection, const infra::SharedPtr<ConnectionObserver>& observer)
    {
        this->connection = connection;
        this->observer = observer;
    }

    void Connection::ResetOwnership()
    {
        observer = nullptr;
        connection = nullptr;
    }

    infra::SharedPtr<Connection> Connection::Self()
    {
        return connection;
    }
}
