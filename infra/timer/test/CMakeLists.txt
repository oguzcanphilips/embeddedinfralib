ccola_component(timer_test gmock_test)

    ccola_sources(
        TestDerivedTimerService.cpp
        TestScalableDerivedTimerService.cpp
        TestTickOnInterruptTimerService.cpp
        TestTimer.cpp
        TestTimerAlternating.cpp
        TestTimerEvolving.cpp
        TestTimerLimitedRepeating.cpp
        TestTimerLimitedRepeatingWithClosingAction.cpp
    )

    ccola_dependencies(
        timer
        timer_test_helper
    )

ccola_end_component()
