#ifndef INFRA_EVENT_DISPATCHER_FIXTURE_HPP
#define INFRA_EVENT_DISPATCHER_FIXTURE_HPP

#include "infra/event/public/EventDispatcher.hpp"

namespace infra
{
    class EventDispatcherFixture
        : public EventDispatcher::WithSize<50>
    {
    public:
        // ExecuteAllActions() is used only in automatic tests, where a test wants to Wait
        // until all scheduled actions are executed.
        using EventDispatcher::WithSize<50>::ExecuteAllActions;
    }; 
}

#endif
