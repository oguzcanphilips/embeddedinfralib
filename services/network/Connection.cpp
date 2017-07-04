#include "services/network/Connection.hpp"
#include "infra/event/EventDispatcher.hpp"

namespace services
{
    ZeroCopyConnectionObserver::ZeroCopyConnectionObserver(services::ZeroCopyConnection& connection)
        : infra::SingleObserver<ZeroCopyConnectionObserver, ZeroCopyConnection>(connection)
    {}

    void ZeroCopyConnection::SwitchObserver(const infra::SharedPtr<ZeroCopyConnectionObserver>& newObserver)
    {
        this->observer = newObserver;
    }

    void ZeroCopyConnection::SetOwnership(const infra::SharedPtr<void>& owner, const infra::SharedPtr<ZeroCopyConnectionObserver>& observer)
    {
        this->owner = owner;
        this->observer = observer;
    }

    void ZeroCopyConnection::ResetOwnership()
    {
        if (observer != nullptr)
            observer->Detach();     // Someone may be keeping the observer alive, so detach it first so that the owner is not observed anymore
        observer = nullptr;
        owner = nullptr;
    }
}
