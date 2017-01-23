#include "services/network/public/Connection.hpp"
#include "infra/event/public/EventDispatcher.hpp"

namespace services
{
    ConnectionObserver::ConnectionObserver(services::Connection& connection)
        : infra::SingleObserver<ConnectionObserver, Connection>(connection)
    {}

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
