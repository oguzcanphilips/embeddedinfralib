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
        infra::TimePoint nextTrigger = NextTrigger();
        if (nextTrigger != infra::TimePoint::max())
        {
            infra::Duration durationToNextNotification = std::max(NextTrigger() - systemTime, Duration());
            ticksNextNotification = static_cast<uint32_t>((durationToNextNotification + resolution - std::chrono::nanoseconds(1)) / resolution);
        }
        else
            ticksNextNotification = std::numeric_limits<uint32_t>::max();
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
        NextTriggerChanged();
    }

    void BaseTimerService::TimeProgressed(Duration amount)
    {
        systemTime += amount;

        Progressed(systemTime);
    }

    void BaseTimerService::SystemTickInterrupt()
    {
        ++ticksProgressed;
        if (ticksProgressed >= ticksNextNotification && !notificationScheduled.exchange(true))
            infra::EventDispatcher::Instance().Schedule([this]() { ProcessTicks(); });
    }

    void BaseTimerService::ProcessTicks()
    {
        TimeProgressed(ticksProgressed.exchange(0) * resolution);
        NextTriggerChanged();

        // If in the meantime ticksProgressed has been increased beyond ticksNextNotification,
        // the event has not been scheduled by the interrupt, so schedule the event here.
        // Use the result of the assign to notificationScheduled, in order to avoid notificationScheduled
        // being set to false, then receiving an interrupt setting it to true, and not handling the newly scheduled
        // event immediately.
        bool reschedule = notificationScheduled = ticksProgressed >= ticksNextNotification;
        if (reschedule)
            infra::EventDispatcher::Instance().Schedule([this]() { ProcessTicks(); });
    }
}
