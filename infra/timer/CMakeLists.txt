ccola_component(timer static_library)

    ccola_sources(
        BaseTimerService.cpp
        BaseTimerService.hpp
        ScalableDerivedTimerService.cpp
        ScalableDerivedTimerService.hpp
        Timer.cpp
        Timer.hpp
        TimerAlternating.cpp
        TimerAlternating.hpp
        TimerEvolving.cpp
        TimerEvolving.hpp
        TimerLimitedRepeating.cpp
        TimerLimitedRepeating.hpp
        TimerLimitedRepeatingWithClosingAction.cpp
        TimerLimitedRepeatingWithClosingAction.hpp
        TimerService.cpp
        TimerService.hpp
        TimerServiceManager.cpp
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
