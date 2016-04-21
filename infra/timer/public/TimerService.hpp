#ifndef INFRA_TIMER_SERVICE_HPP
#define INFRA_TIMER_SERVICE_HPP

#include "infra/timer/public/Timer.hpp"
#include "infra/util/public/IntrusiveForwardList.hpp"
#include "infra/util/public/IntrusivePriorityQueue.hpp"

namespace infra
{
    class TimerService
        : public infra::IntrusiveForwardList<TimerService>::NodeType
    {
    public:
        TimerService(uint32_t id);
        TimerService(const TimerService& other) = delete;
        ~TimerService();
        TimerService& operator=(const TimerService& other) = delete;

        uint32_t Id() const;
        
        void RegisterTimer(Timer& timer);
        void UnregisterTimer(Timer& timer, TimePoint oldTriggerTime);
        void UpdateTriggerTime(Timer& timer, TimePoint oldTriggerTime);

        void Progressed(TimePoint time);
        TimePoint NextTrigger() const;

        virtual void NextTriggerChanged();
        virtual TimePoint Now() const = 0;
        virtual Duration Resolution() const = 0;

    private:
        void ComputeNextTrigger();

    private:
        uint32_t id;
        infra::IntrusiveForwardList<Timer> scheduledTimers;
        infra::IntrusiveForwardList<Timer>::iterator timerIterator;

        TimePoint nextTrigger = TimePoint::max();
        bool holdUpdate = false;
        bool updateNeeded = false;
    };
}

#endif
