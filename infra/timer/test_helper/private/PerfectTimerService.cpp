#include "infra/event/public/EventDispatcher.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include "infra/timer/test_helper/public/PerfectTimerService.hpp"
#include <cassert>

namespace infra
{
    PerfectTimerService::PerfectTimerService(uint32_t id)
        : TimerService(id)
        , resolution(std::chrono::milliseconds(0))
        , systemTime(TimePoint())
    {}

    void PerfectTimerService::NextTriggerChanged()
    {               
        nextNotification = static_cast<uint32_t>(std::max<std::chrono::milliseconds::rep>(
                        std::chrono::duration_cast<std::chrono::milliseconds>(NextTrigger() - previousTrigger).count(), 0));    
    }

    TimePoint PerfectTimerService::Now() const
    {
        return systemTime;
    }

    Duration PerfectTimerService::Resolution() const
    {
        return resolution;
    }

    void PerfectTimerService::SetResolution(Duration resolution)
    {
        this->resolution = resolution;
    }

    void PerfectTimerService::TimeProgressed(Duration amount)
    {
        systemTime += amount;
        Progressed(systemTime);
    }
}
