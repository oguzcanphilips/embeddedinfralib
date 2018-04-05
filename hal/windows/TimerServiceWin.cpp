#include "hal/windows/TimerServiceWin.hpp"
#include "infra/event/EventDispatcher.hpp"
#include <cassert>
#include <windows.h>

namespace hal
{
    TimerServiceWin::TimerServiceWin(uint32_t id)
        : TimerService(id)
        , triggerThread([this]() { WaitForTrigger(); })
    {
        nextTrigger = NextTrigger();
    }

    void TimerServiceWin::NextTriggerChanged()
    {
        std::unique_lock<std::mutex> lock(mutex);
        nextTrigger = NextTrigger();
        condition.notify_one();
    }

    infra::TimePoint TimerServiceWin::Now() const
    {
        LARGE_INTEGER counter;
        BOOL result = QueryPerformanceCounter(&counter);
        assert(result != 0);
        
        LARGE_INTEGER frequency;
        result = QueryPerformanceFrequency(&frequency);
        assert(result != 0);

        return infra::TimePoint() + std::chrono::microseconds(counter.QuadPart) / (frequency.QuadPart / 1000000);
    }

    infra::Duration TimerServiceWin::Resolution() const
    {
        LARGE_INTEGER frequency;
        BOOL result = QueryPerformanceFrequency(&frequency);
        assert(result != 0);
        return std::chrono::seconds(1) / frequency.QuadPart;
    }

    void TimerServiceWin::WaitForTrigger()
    {
        std::unique_lock<std::mutex> lock(mutex);

        while (true)
        {
            if (nextTrigger != infra::TimePoint::max())
                condition.wait_until(lock, nextTrigger);
            else
                condition.wait(lock);

            if (nextTrigger != infra::TimePoint::max() && nextTrigger <= Now())
            {
                nextTrigger = infra::TimePoint::max();

                infra::EventDispatcher::Instance().Schedule([this]()
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    Progressed(Now());
                    NextTriggerChanged();
                });
            }
        }
    }
}
