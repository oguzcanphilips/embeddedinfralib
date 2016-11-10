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
        nextNotification = std::max(NextTrigger() - previousTrigger, Duration()) / resolution;
    }

    TimePoint BaseTimerService::Now() const
    {
        return systemTime + ticksProgressed.load() * resolution;
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
    	bool continueLoop = notificationScheduled;
        while (continueLoop)
        {
            TimeProgressed(ticksProgressed.exchange(0) * resolution);

            previousTrigger = Now();
            nextNotification = std::max(NextTrigger() - previousTrigger, Duration()) / resolution;

            // If in the meantime ticksProgressed has been increased beyond nextNotification,
            // do not wait until the next timer tick, but immediately process this new amount
            // Use the result of the assign to notificationScheduled, in order to avoid notificationScheduled
            // being set to false, then receiving an interrupt setting it to true, and not handling the newly scheduled
            // event immediately.
            continueLoop = notificationScheduled = ticksProgressed >= nextNotification;
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
