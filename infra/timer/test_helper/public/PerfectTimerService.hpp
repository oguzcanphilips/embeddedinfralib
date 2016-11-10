#ifndef INFRA_PERFECT_TIMER_SERVICE_HPP
#define INFRA_PREFECT_TIMER_SERVICE_HPP

#include "infra/timer/public/TimerService.hpp"
#include <atomic>

namespace infra
{
	class PerfectTimerService
		: public TimerService
	{
	public:
		PerfectTimerService(uint32_t id);

		virtual void NextTriggerChanged() override;
		virtual TimePoint Now() const override;
		virtual Duration Resolution() const override;

		void SetResolution(Duration resolution);

		void TimeProgressed(Duration amount);
		void SystemTickInterrupt();

		// Test functions	
		void SetTestSystemTime(TimePoint time);

	private:
		void ProcessTicks();

	private:		
		TimePoint systemTime = TimePoint();
		Duration resolution;

		std::atomic<uint32_t> nextNotification;
		std::atomic<uint32_t> ticksProgressed;
		std::atomic_bool notificationScheduled;
		infra::TimePoint previousTrigger;
	};
}

#endif
