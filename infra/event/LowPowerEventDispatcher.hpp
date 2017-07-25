#ifndef LOW_POWER_EVENT_DISPATCHER
#define LOW_POWER_EVENT_DISPATCHER

#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "infra/util/BoundedList.hpp"

namespace infra
{
    class MainClockReference
        : public infra::InterfaceConnector<MainClockReference>
    {
    public:
        void Refere();
        void Release();
        bool IsReferenced() const;

    private:
        uint32_t numReferenced = 0;
    };

    class LowPowerStrategy
    {
    protected:
        LowPowerStrategy() = default;
        LowPowerStrategy(const LowPowerStrategy& other) = delete;
        LowPowerStrategy& operator=(const LowPowerStrategy& other) = delete;
        ~LowPowerStrategy() = default;

    public:
        virtual void Idle(const MainClockReference& mainClock) = 0;
    };

    class LowPowerEventDispatcherWorker
        : public infra::EventDispatcherWithWeakPtrWorker
    {
    public:
        template<std::size_t StorageSize, class T = LowPowerEventDispatcherWorker>
            using WithSize = infra::EventDispatcherWithWeakPtrWorker::WithSize<StorageSize, T>;

        LowPowerEventDispatcherWorker(infra::MemoryRange<std::pair<infra::EventDispatcherWithWeakPtrWorker::ActionStorage, std::atomic<bool>>> scheduledActionsStorage
            , LowPowerStrategy& lowPowerStrategy);
        
    protected:
        void Idle() override;

    private:
        LowPowerStrategy& lowPowerStrategy;
        MainClockReference mainClock;
    };

    using LowPowerEventDispatcher = infra::EventDispatcherWithWeakPtrConnector<LowPowerEventDispatcherWorker>;
}

#endif
