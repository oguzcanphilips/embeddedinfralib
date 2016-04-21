#ifndef INFRA_BASE_TIMER_SERVICE_HPP
#define INFRA_BASE_TIMER_SERVICE_HPP

#include "infra/timer/public/TimerService.hpp"

namespace infra
{
    class BaseTimerService
        : public TimerService
    {
    public:
        BaseTimerService(uint32_t id, Duration resolution);

        TimePoint Now() const override;
        Duration Resolution() const override;

        void SetResolution(Duration resolution);

        void TimeProgressed(Duration amount);

    public:
        // Test mode functions
        void EnterTestMode();
        void LeaveTestMode();
        void SetTestSystemTime(TimePoint time);

    private:
        bool testMode = false;
        TimePoint systemTime = TimePoint();
        Duration resolution;
    };
}

#endif
