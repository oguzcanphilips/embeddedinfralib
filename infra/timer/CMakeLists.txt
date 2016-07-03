ccola_component(timer static_library)

	ccola_sources(
		private/BaseTimerService.cpp
		private/ScalableDerivedTimerService.cpp
		private/Timer.cpp
		private/TimerAlternating.cpp
		private/TimerLimitedRepeating.cpp
		private/TimerLimitedRepeatingWithClosingAction.cpp
		private/TimerService.cpp
		private/TimerServiceManager.cpp
		public/BaseTimerService.hpp
		public/ScalableDerivedTimerService.hpp
		public/Timer.hpp
		public/TimerAlternating.hpp
		public/TimerLimitedRepeating.hpp
		public/TimerLimitedRepeatingWithClosingAction.hpp
		public/TimerService.hpp
		public/TimerServiceManager.hpp
	)

	ccola_dependencies(
		util
		event
	)

ccola_end_component()

ccola_subdirectories(
	test_helper
	test
)
