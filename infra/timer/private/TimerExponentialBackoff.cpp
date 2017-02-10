#include "infra/timer/public/TimerExponentialBackoff.hpp"

namespace infra
{
    TimerExponentialBackoff::TimerExponentialBackoff(uint32_t timerServiceId)
        : Timer(timerServiceId)
    {}

    TimerExponentialBackoff::TimerExponentialBackoff(Duration duration, Duration maximumBackoff, const infra::Function<bool()>& condition, const infra::Function<void()>& action, uint32_t timerServiceId)
        : Timer(timerServiceId)
    {
        Start(duration, maximumBackoff, condition, action);
    }

    void TimerExponentialBackoff::Start(Duration duration, Duration maximumBackoff, const infra::Function<bool()>& aCondition, const infra::Function<void()>& aAction)
    {
        condition = aCondition;
        action = aAction;

        triggerStart = Now() + Resolution();
        initialTriggerPeriod = duration;
        maximumBackoffPeriod = maximumBackoff;
        
        ComputeNextTriggerTime();
    }

    const infra::Function<void()>& TimerExponentialBackoff::Action() const
    {
        conditionResult = condition();

        if (conditionResult)
            return action;
        else
            return infra::emptyFunction;
    }

    void TimerExponentialBackoff::ComputeNextTriggerTime()
    {
        if (!conditionResult)
            triggerPeriod = std::min(triggerPeriod * 2, maximumBackoffPeriod);
        else
            triggerPeriod = initialTriggerPeriod;

        TimePoint now = Now();
        Duration diff = now - triggerStart;
        if (diff < Duration())
            now += triggerPeriod;
        diff %= triggerPeriod;
        SetNextTriggerTime(now - diff + triggerPeriod, infra::emptyFunction);
    }
}
