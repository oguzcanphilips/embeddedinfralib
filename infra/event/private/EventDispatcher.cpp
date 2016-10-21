#include "infra/event/public/EventDispatcher.hpp"
#include <cassert>

namespace infra
{
    EventDispatcherWorker::EventDispatcherWorker(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage)
        : scheduledActions(scheduledActionsStorage)
        , scheduledActionsPushIndex(0)
        , scheduledActionsPopIndex(0)
        , minCapacity(scheduledActions.size())
    {
        for (auto& action : scheduledActions)
            action.second = false;
    }

    void EventDispatcherWorker::Schedule(const infra::Function<void()>& action)
    {
        uint32_t pushIndex = scheduledActionsPushIndex;
        uint32_t newPushIndex;

        do
        {
            newPushIndex = (pushIndex + 1) % scheduledActions.size();
        } while (!scheduledActionsPushIndex.compare_exchange_weak(pushIndex, newPushIndex));

        scheduledActions[pushIndex].first = action;
        assert(!scheduledActions[pushIndex].second);
        scheduledActions[pushIndex].second = true;

        minCapacity = std::min(minCapacity, (scheduledActions.size() + newPushIndex - scheduledActionsPopIndex) % scheduledActions.size());
        assert(minCapacity >= 1);

        RequestExecution();
    }

    void EventDispatcherWorker::Run()
    {
        while (true)                                                                                            //TICS !CPP4127
        {
            ExecuteAllActions();
            Idle();
        }
    }

    void EventDispatcherWorker::ExecuteAllActions()
    {
        while (TryExecuteAction())
        {
        }
    }

    bool EventDispatcherWorker::IsIdle() const
    {
        return !scheduledActions[scheduledActionsPopIndex].second;
    }

    std::size_t EventDispatcherWorker::MinCapacity() const
    {
        return minCapacity;
    }

    void EventDispatcherWorker::RequestExecution()
    {}

    void EventDispatcherWorker::Idle()
    {}

    bool EventDispatcherWorker::TryExecuteAction()
    {
        if (scheduledActions[scheduledActionsPopIndex].second)
        {
            scheduledActions[scheduledActionsPopIndex].first();
            scheduledActions[scheduledActionsPopIndex].second = false;
            scheduledActionsPopIndex = (scheduledActionsPopIndex + 1) % scheduledActions.size();
            return true;
        }
        else
            return false;
    }
}
