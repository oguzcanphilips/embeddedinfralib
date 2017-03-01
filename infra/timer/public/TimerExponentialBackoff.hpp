#ifndef INFRA_TIMER_EXPONENTIAL_BACKOFF_HPP
#define INFRA_TIMER_EXPONENTIAL_BACKOFF_HPP

#include "infra/timer/public/Timer.hpp"

namespace infra
{
    class TimerExponentialBackoff
        : public Timer
    {
    public:
        explicit TimerExponentialBackoff(uint32_t timerServiceId = systemTimerServiceId);
        TimerExponentialBackoff(Duration duration, Duration maximumBackoff, const infra::Function<bool()>& condition, const infra::Function<void()>& action, uint32_t timerServiceId = systemTimerServiceId);

        void Start(Duration duration, Duration maximumBackoff, const infra::Function<bool()>& aCondition, const infra::Function<void()>& aAction);

    protected:
        virtual const infra::Function<void()>& Action() const override;
        virtual void ComputeNextTriggerTime() override;

    private:
        TimePoint triggerStart;
        Duration triggerPeriod = Duration();
        Duration initialTriggerPeriod = Duration();
        Duration maximumBackoffPeriod = Duration();
        mutable bool conditionResult = true;
        infra::Function<bool()> condition;
        infra::Function<void()> action;
    };
}

#endif
