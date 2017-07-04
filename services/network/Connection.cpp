#include "services/network/Connection.hpp"
#include "infra/event/EventDispatcher.hpp"

namespace services
{
    ConnectionObserver::ConnectionObserver(services::Connection& connection)
        : infra::SingleObserver<ConnectionObserver, Connection>(connection)
    {}

    void Connection::SwitchObserver(const infra::SharedPtr<ConnectionObserver>& newObserver)
    {
        this->observer = newObserver;
    }

    void Connection::SetOwnership(const infra::SharedPtr<void>& owner, const infra::SharedPtr<ConnectionObserver>& observer)
    {
        this->owner = owner;
        this->observer = observer;
    }

    void Connection::ResetOwnership()
    {
        if (observer != nullptr)
            observer->Detach();     // Someone may be keeping the observer alive, so detach it first so that the owner is not observed anymore
        observer = nullptr;
        owner = nullptr;
    }
}
