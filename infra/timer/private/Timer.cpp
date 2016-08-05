#include "infra/timer/public/Timer.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include <cassert>

namespace infra
{
    Timer::Timer(uint32_t timerServiceId)
        : timerService(static_cast<TimerService&>(TimerServiceManager::Instance().GetTimerService(timerServiceId)))
    {}

    Timer::~Timer()
    {
        Cancel();
    }

    void Timer::Cancel()
    {
        if (action)
            UnregisterSelf(Convert(nextTriggerTime));

        nextTriggerTime = Convert(TimePoint());
        action = nullptr;
    }

    bool Timer::Armed() const
    {
        return action != nullptr;
    }

    TimePoint Timer::NextTrigger() const
    {
        return Convert(nextTriggerTime);
    }

    const infra::Function<void()>& Timer::Action() const
    {
        return action;
    }

    void Timer::RegisterSelf()
    {
        timerService.RegisterTimer(*this);
    }

    void Timer::UnregisterSelf(TimePoint oldTriggerTime)
    {
        timerService.UnregisterTimer(*this, oldTriggerTime);
    }

    void Timer::UpdateTriggerTime(TimePoint oldTriggerTime)
    {
        timerService.UpdateTriggerTime(*this, oldTriggerTime);
    }

    uint32_t Timer::Convert(TimePoint point) const
    {
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(point - TimePoint()).count());
    }

    TimePoint Timer::Convert(uint32_t point) const
    {
        uint64_t nowInMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(Now() - TimePoint()).count();
        uint64_t milliSecondsSinceStart = ((nowInMilliSeconds >> 32) << 32);

        if (static_cast<uint32_t>(nowInMilliSeconds) > (static_cast<uint32_t>(1) << 31))
            milliSecondsSinceStart += static_cast<uint64_t>(1) << 32;

        return TimePoint() + std::chrono::milliseconds(milliSecondsSinceStart + point);
    }

    void Timer::SetNextTriggerTime(TimePoint time, const infra::Function<void()>& action)
    {
        TimePoint oldTriggerTime = Convert(nextTriggerTime);

        assert(std::chrono::duration_cast<std::chrono::milliseconds>(time - Now()).count() < (static_cast<uint32_t>(1) << 31));
        nextTriggerTime = Convert(time);

        if (!this->action)
            RegisterSelf();
        else
            UpdateTriggerTime(oldTriggerTime);

        this->action = action;
    }

    TimePoint Timer::Now() const
    {
        return timerService.Now();
    }

    Duration Timer::Resolution() const
    {
        return timerService.Resolution();
    }

    TimerSingleShot::TimerSingleShot(uint32_t timerServiceId)
        : Timer(timerServiceId)
    {}

    TimerSingleShot::TimerSingleShot(Duration duration, const infra::Function<void()>& aAction, uint32_t timerServiceId)
        : Timer(timerServiceId)
    {
        Start(duration, aAction);
    }

    void TimerSingleShot::Start(TimePoint time, const infra::Function<void()>& action)
    {
        SetNextTriggerTime(time + Resolution(), action);
    }

    void TimerSingleShot::Start(Duration duration, const infra::Function<void()>& action)
    {
        SetNextTriggerTime(Now() + duration + Resolution(), action);
    }

    void TimerSingleShot::ComputeNextTriggerTime()
    {
        Cancel();
    }

    TimerRepeating::TimerRepeating(uint32_t timerServiceId)
        : Timer(timerServiceId)
    {}

    TimerRepeating::TimerRepeating(Duration duration, const infra::Function<void()>& aAction, uint32_t timerServiceId)
        : Timer(timerServiceId)
    {
        Start(duration, aAction);
    }

    TimerRepeating::TimerRepeating(Duration duration, const infra::Function<void()>& aAction, TriggerImmediately, uint32_t timerServiceId)
        : Timer(timerServiceId)
    {
        Start(duration, aAction, triggerImmediately);
    }

    void TimerRepeating::Start(Duration duration, const infra::Function<void()>& action)
    {
        SetNextTriggerTime(Now() + Resolution(), action);  // Initialize NextTrigger() for ComputeNextTriggerTime
        SetDuration(duration);
    }

    void TimerRepeating::Start(Duration duration, const infra::Function<void()>& action, TriggerImmediately)
    {
        Start(duration, action);
        action();
    }

    void TimerRepeating::ComputeNextTriggerTime()
    {
        TimePoint now = std::max(Now(), NextTrigger());
        Duration diff = (now - NextTrigger()) % triggerPeriod;

        SetNextTriggerTime(now - diff + std::chrono::milliseconds(triggerPeriod), Action());
    }

    void TimerRepeating::SetDuration(Duration duration)
    {
        assert(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() < std::numeric_limits<uint32_t>::max());
        triggerPeriod = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

        ComputeNextTriggerTime();
    }
}
