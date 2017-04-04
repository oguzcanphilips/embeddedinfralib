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
    protected:
        EventDispatcherWorker() = default;
        EventDispatcherWorker(const EventDispatcherWorker& other) = delete;
        EventDispatcherWorker& operator=(const EventDispatcherWorker& other) = delete;
        ~EventDispatcherWorker() = default;

    public:
        virtual void Schedule(const infra::Function<void()>& action) = 0;
    };

    class EventDispatcherWorkerImpl
        : public EventDispatcherWorker
    {
    public:
        template<std::size_t StorageSize, class T = EventDispatcherWorkerImpl>
            using WithSize = infra::WithStorage<T, std::array<std::pair<infra::Function<void()>, std::atomic<bool>>, StorageSize>>;

        explicit EventDispatcherWorkerImpl(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage);

        virtual void Schedule(const infra::Function<void()>& action) override;

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

        template<class... ConstructionArgs>
            explicit EventDispatcherConnector(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage, ConstructionArgs&&... args);
    };

    using EventDispatcher = EventDispatcherConnector<EventDispatcherWorkerImpl>;

    ////    Implementation    ////

    template<class T>
    template<class... ConstructionArgs>
    EventDispatcherConnector<T>::EventDispatcherConnector(MemoryRange<std::pair<infra::Function<void()>, std::atomic<bool>>> scheduledActionsStorage, ConstructionArgs&&... args)
        : infra::InterfaceConnector<EventDispatcherWorker>(this)
        , T(scheduledActionsStorage, std::forward(args)...)
    {}
}

#endif
