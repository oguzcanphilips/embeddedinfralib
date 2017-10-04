ccola_component(timer static_library)

	ccola_sources(
		BaseTimerService.cpp
		ScalableDerivedTimerService.cpp
		Timer.cpp
		TimerAlternating.cpp
		TimerLimitedRepeating.cpp
		TimerLimitedRepeatingWithClosingAction.cpp
		TimerService.cpp
		TimerServiceManager.cpp
		BaseTimerService.hpp
		ScalableDerivedTimerService.hpp
		Timer.hpp
		TimerAlternating.hpp
		TimerLimitedRepeating.hpp
		TimerLimitedRepeatingWithClosingAction.hpp
		TimerService.hpp
		TimerServiceManager.hpp
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
