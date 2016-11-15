#include "infra/event/public/EventDispatcher.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include "infra/timer/test_helper/public/ClockFixture.hpp"

namespace infra
{
    ClockFixture::ClockFixture(uint32_t timerSericeId)
        : systemTimerService(timerSericeId)
    {	
    }

    ClockFixture::~ClockFixture()
    {
    }

    void ClockFixture::ForwardTime(Duration amount)
    {
        ExecuteAllActions();
        TimePoint newSystemTime = systemTimerService.Now() + amount;

        do
        {
            TimePoint minimumTriggerTime = std::min(newSystemTime, systemTimerService.NextTrigger());

			systemTimerService.SetTestSystemTime(minimumTriggerTime);
            ExecuteAllActions();
        } while (systemTimerService.Now() != newSystemTime);
    }

	testing::Matcher<const std::tuple<>&> ClockFixture::After(Duration duration) const
	{
		return testing::Matcher<const std::tuple<>&>(new TimeMatcher(systemTimerService.Now() + duration));
	}

    std::string ClockFixture::TimeToString(TimePoint time)
    {
        std::ostringstream os;

        std::chrono::microseconds microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch());
        std::time_t systemTime = std::chrono::duration_cast<std::chrono::seconds>(microSeconds).count();
        std::size_t fractionalSecondsMilli = microSeconds.count() % 1000000 / 1000;
        std::size_t fractionalSecondsMicro = microSeconds.count() % 1000;

        const char format[] = "%Y-%m-%d %H:%M:%S";

        std::tm* gmTime = std::gmtime(&systemTime);
        std::string buffer(40, ' ');
        std::size_t size = strftime(&buffer.front(), buffer.size(), format, gmTime);
        buffer.resize(size);

        os << buffer << " "
            << std::setw(3) << std::setfill('0') << fractionalSecondsMilli << "."
            << std::setw(3) << std::setfill('0') << fractionalSecondsMicro;

        return os.str();
    }

    ClockFixture::TimeMatcher::TimeMatcher(infra::TimePoint expectedCallTime)
        : expectedCallTime(expectedCallTime)
    {}

    bool ClockFixture::TimeMatcher::MatchAndExplain(const std::tuple<>& x, testing::MatchResultListener* listener) const
    {
        return expectedCallTime == infra::TimerServiceManager::Instance().GetTimerService(infra::systemTimerServiceId).Now();
    }

    void ClockFixture::TimeMatcher::DescribeTo(std::ostream* os) const
    {
        if (os)
        {
            *os << "To be called at " << TimeToString(expectedCallTime) << std::endl
                << "                   It is now       " << TimeToString(infra::TimerServiceManager::Instance().GetTimerService(infra::systemTimerServiceId).Now()) << std::endl;
        }
    }
}
