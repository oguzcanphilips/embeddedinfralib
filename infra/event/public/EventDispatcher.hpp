#ifndef INFRA_EVENT_DISPATCHER_HPP
#define INFRA_EVENT_DISPATCHER_HPP

#include "infra/util/public/Function.hpp"
#include "infra/util/public/InterfaceConnector.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <atomic>

namespace infra
{
    class EventDispatcherWorker
    {
    public:
        template<std::size_t StorageSize, class T = EventDispatcherWorker>
            using WithSize = infra::WithStorage<T, std::array<std::pair<infra::Function<void()>, std::atomic<bool>>, StorageSize>>;

        explicit EventDispatcherWorker(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage);

        void Schedule(const infra::Function<void()>& action);

        void Run();
        void ExecuteAllActions();
        bool IsIdle() const;

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

    template<class T>
    class EventDispatcherConnector
        : public infra::InterfaceConnector<EventDispatcherWorker>
        , public T
    {
    public:
        template<std::size_t StorageSize>
            using WithSize = typename T::template WithSize<StorageSize, EventDispatcherConnector<T>>;

        explicit EventDispatcherConnector(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage);
    };

    using EventDispatcher = EventDispatcherConnector<EventDispatcherWorker>;

    ////    Implementation    ////

    template<class T>
    EventDispatcherConnector<T>::EventDispatcherConnector(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage)
        : infra::InterfaceConnector<EventDispatcherWorker>(this)
        , T(scheduledActionsStorage)
    {}
}

#endif
