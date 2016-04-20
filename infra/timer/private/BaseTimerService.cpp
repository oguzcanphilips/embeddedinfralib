#include "infra/timer/public/BaseTimerService.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include <cassert>

namespace infra
{
    BaseTimerService::BaseTimerService(uint32_t id, Duration resolution)
        : TimerService(id)
        , resolution(resolution)
    {}

    TimePoint BaseTimerService::Now() const
    {
        return systemTime;
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
