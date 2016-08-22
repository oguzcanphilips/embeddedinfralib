#ifndef INFRA_TIMER_HPP
#define INFRA_TIMER_HPP

#include "infra/util/public/Function.hpp"
#include "infra/util/public/IntrusiveForwardList.hpp"
#include <array>
#include <chrono>
#include <cstdlib>

// Classes in this header file:
//
//      - Timer                 : Base class for all timers
//      - TimerSingleShot       : Triggers once
//      - TimerRepeating        : Triggers continuously

namespace infra
{
    using TimePoint = std::chrono::system_clock::time_point;
    using Duration = std::chrono::system_clock::duration;

    class TimerService;

    struct TriggerImmediately {};
    const TriggerImmediately triggerImmediately;

    const uint32_t systemTimerServiceId = 0;

    class Timer
        : public infra::IntrusiveForwardList<Timer>::NodeType
    {
    protected:
        explicit Timer(uint32_t timerServiceId);
        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;
        ~Timer();

    public:
        void Cancel();

        bool Armed() const;
        TimePoint NextTrigger() const;

        virtual const infra::Function<void()>& Action() const;
        virtual void ComputeNextTriggerTime() = 0;

    protected:
        TimePoint Now() const;
        Duration Resolution() const;
        void SetNextTriggerTime(TimePoint time, const infra::Function<void()>& action);

    private:
        void RegisterSelf();
        void UnregisterSelf(TimePoint oldTriggerTime);
        void UpdateTriggerTime(TimePoint oldTriggerTime);

        uint32_t Convert(TimePoint point) const;
        TimePoint Convert(uint32_t point) const;

    private:
        TimerService& timerService;
        infra::Function<void()> action;
        uint32_t nextTriggerTime = 0;
    };

    class TimerSingleShot
        : public Timer
    {
    public:
        explicit TimerSingleShot(uint32_t timerServiceId = systemTimerServiceId);
        TimerSingleShot(Duration duration, const infra::Function<void()>& action, uint32_t timerServiceId = systemTimerServiceId);

        void Start(TimePoint time, const infra::Function<void()>& action);
        void Start(Duration duration, const infra::Function<void()>& action);

    protected:
        virtual void ComputeNextTriggerTime() override;
    };

    class TimerRepeating
        : public Timer
    {
    public:
        explicit TimerRepeating(uint32_t timerServiceId = systemTimerServiceId);
        TimerRepeating(Duration duration, const infra::Function<void()>& action, uint32_t timerServiceId = systemTimerServiceId);
        TimerRepeating(Duration duration, const infra::Function<void()>& action, TriggerImmediately, uint32_t timerServiceId = systemTimerServiceId);

        void Start(Duration duration, const infra::Function<void()>& action);
        void Start(Duration duration, const infra::Function<void()>& action, TriggerImmediately);

    protected:
        virtual void ComputeNextTriggerTime() override;

    private:
        void SetDuration(Duration duration);

    private:
        uint32_t triggerPeriod = 0;
    };
}

#endif
