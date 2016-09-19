#ifndef INFRA_EVENT_DISPATCHER_HPP
#define INFRA_EVENT_DISPATCHER_HPP

#include "infra/util/public/Function.hpp"
#include "infra/util/public/InterfaceConnector.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <atomic>

namespace infra
{
    class EventDispatcher
        : public infra::InterfaceConnector<EventDispatcher>
    {
    public:
        template<std::size_t StorageSize>
            using WithSize = infra::WithStorage<EventDispatcher, std::array<std::pair<infra::Function<void()>, std::atomic<bool>>, StorageSize>>;

        explicit EventDispatcher(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage);

        void Schedule(const infra::Function<void()>& action);

        void Run();
        void ExecuteAllActions();

        std::size_t MinCapacity() const;

    protected:
        virtual void RequestExecution();
        virtual void Idle();

    private:
        bool TryExecuteAction();

    private:
        infra::MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActions;
        std::atomic<uint32_t> scheduledActionsPushIndex;
        uint32_t scheduledActionsPopIndex;
        uint32_t minCapacity;
    };
}

#endif
