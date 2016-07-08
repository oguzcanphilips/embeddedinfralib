#include "infra/event/public/EventDispatcher.hpp"
#include "infra/timer/public/BaseTimerService.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include <cassert>

namespace infra
{
    BaseTimerService::BaseTimerService(uint32_t id, Duration resolution)
        : TimerService(id)
        , resolution(resolution)
    {
        ticksProgressed = 0;
        notificationScheduled = false;
    }

    void BaseTimerService::NextTriggerChanged()
    {
        nextNotification = static_cast<uint32_t>(std::max<std::chrono::milliseconds::rep>(
            std::chrono::duration_cast<std::chrono::milliseconds>(NextTrigger() - previousTrigger).count(), 0));
    }

    TimePoint BaseTimerService::Now() const
    {
        return systemTime + std::chrono::milliseconds(ticksProgressed);
    }

    Duration BaseTimerService::Resolution() const
    {
        return resolution;
    }

    void BaseTimerService::SetResolution(Duration resolution)
    {
        this->resolution = resolution;
    }

    void BaseTimerService::TimeProgressed(Duration amount)
    {
        systemTime += amount;
        
        Progressed(systemTime);
    }

    void BaseTimerService::SystemTickInterrupt()
    {
        ++ticksProgressed;
        if (ticksProgressed >= nextNotification && !notificationScheduled.exchange(true))
            infra::EventDispatcher::Instance().Schedule([this]() { ProcessTicks(); });
    }

    void BaseTimerService::ProcessTicks()
    {
        while (notificationScheduled)
        {
            TimeProgressed(std::chrono::milliseconds(ticksProgressed.exchange(0)));
            previousTrigger = Now();
            nextNotification = static_cast<uint32_t>(std::max<std::chrono::milliseconds::rep>(
                std::chrono::duration_cast<std::chrono::milliseconds>(NextTrigger() - previousTrigger).count(), 0));
            // If in the meantime ticksProgressed has been increased beyond nextNotification,
            // do not wait until the next timer tick, but immediately process this new amount
            notificationScheduled = ticksProgressed >= nextNotification;
        }
    }

    void BaseTimerService::EnterTestMode()
    {
        testMode = true;
        systemTime = TimePoint();  // epoch value
    }

    void BaseTimerService::LeaveTestMode()
    {
        testMode = false;
    }

    void BaseTimerService::SetTestSystemTime(TimePoint time)
    {
        assert(testMode);
        systemTime = time;

        Progressed(systemTime);
    }
}
