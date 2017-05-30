#ifndef INFRA_CLOCK_FIXTURE_HPP
#define INFRA_CLOCK_FIXTURE_HPP

#include "gmock/gmock.h"
#include "infra/event/test_helper/EventDispatcherWithWeakPtrFixture.hpp"
#include "infra/timer/test_helper/PerfectTimerService.hpp"
#include "infra/timer/TimerService.hpp"

namespace infra
{
    class ClockFixture
        : public EventDispatcherWithWeakPtrFixture
    {
    public:
        ClockFixture(uint32_t timerSericeId = systemTimerServiceId);
        ClockFixture(const ClockFixture&) = delete;
        ClockFixture& operator=(const ClockFixture&) = delete;
        ~ClockFixture();

        // Forward the 'apparent' system time, thereby triggering any timers that were waiting
        void ForwardTime(Duration time);

        testing::Matcher<const std::tuple<>&> After(infra::Duration duration) const;

        static std::string TimeToString(TimePoint time);

        PerfectTimerService systemTimerService;

    private:
        class TimeMatcher
            : public testing::MatcherInterface<const std::tuple<>&>
        {
        public:
            TimeMatcher(infra::TimePoint expectedCallTime);

            virtual bool MatchAndExplain(const std::tuple<>& x, testing::MatchResultListener* listener) const override;
            virtual void DescribeTo(std::ostream* os) const override;

        private:
            void StreamTime(TimePoint time, std::ostream& os) const;

            infra::TimePoint expectedCallTime;
        };
    };
}

#endif
