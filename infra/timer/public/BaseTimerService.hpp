#ifndef INFRA_BASE_TIMER_SERVICE_HPP
#define INFRA_BASE_TIMER_SERVICE_HPP

#include "infra/timer/public/TimerService.hpp"
#include <atomic>

namespace infra
{
    class BaseTimerService
        : public TimerService
    {
    public:
        BaseTimerService(uint32_t id, Duration resolution);

        virtual void NextTriggerChanged() override;
        virtual TimePoint Now() const override;
        virtual Duration Resolution() const override;

        void SetResolution(Duration resolution);

        void TimeProgressed(Duration amount);
        void SystemTickInterrupt();

    private:
        void ProcessTicks();

    private:
        bool testMode = false;
        TimePoint systemTime = TimePoint();
        Duration resolution;

        std::atomic<uint32_t> nextNotification;
        std::atomic<uint32_t> ticksProgressed;
        std::atomic_bool notificationScheduled;
        infra::TimePoint previousTrigger;
    };
}

#endif
