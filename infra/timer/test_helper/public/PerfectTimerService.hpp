#pragma once

#include "infra/timer/public/TimerService.hpp"

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

		uint32_t nextNotification;
		uint32_t ticksProgressed;
		bool notificationScheduled;
		infra::TimePoint previousTrigger;
	};
}
