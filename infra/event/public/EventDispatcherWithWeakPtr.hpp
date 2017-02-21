#ifndef INFRA_EVENT_DISPATCHER_WITH_WEAK_PTR_HPP
#define INFRA_EVENT_DISPATCHER_WITH_WEAK_PTR_HPP

#include "infra/event/public/EventDispatcher.hpp"
#include "infra/util/public/SharedPtr.hpp"

namespace infra
{
    class EventDispatcherWithWeakPtrWorker
        : public EventDispatcherWorker
    {
    private:
        class Action
        {
        public:
            virtual ~Action() = default;

            virtual void Execute() = 0;
        };

        class ActionFunction
            : public Action
        {
        public:
            explicit ActionFunction(const Function<void()>& function);

            virtual void Execute() override;

        private:
            Function<void()> function;
        };

        template<class T>
        class ActionWithWeakPtr
            : public Action
        {
        public:
            ActionWithWeakPtr(const infra::Function<void(const infra::SharedPtr<T>& object)>& function, const infra::SharedPtr<T>& object);

            virtual void Execute() override;

        private:
            infra::Function<void(const infra::SharedPtr<T>& object)> function;
            infra::WeakPtr<T> object;
        };

    public:
        using ActionStorage = StaticStorageForPolymorphicObjects<Action, INFRA_DEFAULT_FUNCTION_EXTRA_SIZE + 12>;
        template<std::size_t StorageSize, class T = EventDispatcherWithWeakPtrWorker>
            using WithSize = infra::WithStorage<T, std::array<std::pair<ActionStorage, std::atomic<bool>>, StorageSize>>;

        explicit EventDispatcherWithWeakPtrWorker(MemoryRange<std::pair<ActionStorage, std::atomic<bool>>> scheduledActionsStorage);

        virtual void Schedule(const infra::Function<void()>& action) override;

        template<class T>
            void Schedule(const typename std::decay<infra::Function<void(const infra::SharedPtr<T>& object)>>::type& action, const infra::SharedPtr<T>& object);

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
        infra::MemoryRange<std::pair<ActionStorage, std::atomic<bool>>> scheduledActions;
        std::atomic<uint32_t> scheduledActionsPushIndex;
        uint32_t scheduledActionsPopIndex;
        uint32_t minCapacity;
    };

    template<class T>
    class EventDispatcherWithWeakPtrConnector
        : public infra::InterfaceConnector<EventDispatcherWorker>
        , public infra::InterfaceConnector<EventDispatcherWithWeakPtrWorker>
        , public T
    {
    public:
        using infra::InterfaceConnector<EventDispatcherWithWeakPtrWorker>::Instance;

        template<std::size_t StorageSize>
            using WithSize = typename T::template WithSize<StorageSize, EventDispatcherWithWeakPtrConnector<T>>;

        explicit EventDispatcherWithWeakPtrConnector(MemoryRange<std::pair<EventDispatcherWithWeakPtrWorker::ActionStorage, std::atomic<bool>>> scheduledActionsStorage);
    };

    using EventDispatcherWithWeakPtr = EventDispatcherWithWeakPtrConnector<EventDispatcherWithWeakPtrWorker>;

    ////    Implementation    ////

    template<class T>
    void EventDispatcherWithWeakPtrWorker::Schedule(const typename std::decay<infra::Function<void(const infra::SharedPtr<T>& object)>>::type& action, const infra::SharedPtr<T>& object)
    {
        uint32_t pushIndex = scheduledActionsPushIndex;
        uint32_t newPushIndex;

        do
        {
            newPushIndex = (pushIndex + 1) % scheduledActions.size();
        } while (!scheduledActionsPushIndex.compare_exchange_weak(pushIndex, newPushIndex));

        scheduledActions[pushIndex].first.Construct<ActionWithWeakPtr<T>>(action, object);
        assert(!scheduledActions[pushIndex].second);
        scheduledActions[pushIndex].second = true;

        minCapacity = std::min(minCapacity, (scheduledActions.size() + newPushIndex - scheduledActionsPopIndex) % scheduledActions.size());
        assert(minCapacity >= 1);

        RequestExecution();
    }

    template<class T>
    EventDispatcherWithWeakPtrWorker::ActionWithWeakPtr<T>::ActionWithWeakPtr(const infra::Function<void(const infra::SharedPtr<T>& object)>& function, const infra::SharedPtr<T>& object)
        : function(function)
        , object(object)
    {}

    template<class T>
    void EventDispatcherWithWeakPtrWorker::ActionWithWeakPtr<T>::Execute()
    {
        infra::SharedPtr<T> sharedObject = object;
        if (sharedObject)
            function(sharedObject);
    }

    template<class T>
    EventDispatcherWithWeakPtrConnector<T>::EventDispatcherWithWeakPtrConnector(MemoryRange<std::pair<EventDispatcherWithWeakPtrWorker::ActionStorage, std::atomic<bool>>> scheduledActionsStorage)
        : infra::InterfaceConnector<EventDispatcherWorker>(this)
        , infra::InterfaceConnector<EventDispatcherWithWeakPtrWorker>(this)
        , T(scheduledActionsStorage)
    {}
}

#endif
