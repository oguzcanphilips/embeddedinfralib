ccola_component(timer_test gmock_test)

    ccola_sources(
        TestBaseTimerService.cpp
        TestScalableDerivedTimerService.cpp
        TestTimer.cpp
        TestTimerAlternating.cpp
        TestTimerLimitedRepeating.cpp
        TestTimerLimitedRepeatingWithClosingAction.cpp
    )

    ccola_dependencies(
        timer
        timer_test_helper
    )

ccola_end_component()
