#ifndef HAL_TIMER_SERVICE_WIN_HPP
#define HAL_TIMER_SERVICE_WIN_HPP

#include "infra/timer/TimerService.hpp"
#include <condition_variable>
#include <mutex>
#include <thread>

namespace hal
{
    class TimerServiceWin
        : public infra::TimerService
    {
    public:
        TimerServiceWin(uint32_t id);

        virtual void NextTriggerChanged() override;
        virtual infra::TimePoint Now() const override;
        virtual infra::Duration Resolution() const override;

    private:
        void WaitForTrigger();

    private:
        std::condition_variable condition;
        std::mutex mutex;
        infra::TimePoint nextTrigger;
        std::thread triggerThread;
    };
}

#endif
