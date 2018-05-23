ccola_component(util_test gmock_test)

    ccola_sources(
        TestConfigurationStore.cpp
        TestCyclicStore.cpp
        TestDebouncedButton.cpp
        TestDebugLed.cpp
        TestFlashMultipleAccess.cpp
        TestFlashQuadSpiMicronN25q.cpp
        TestFlashQuadSpiCypressFll.cpp
        TestFlashRegion.cpp
        TestFlashSpi.cpp
        TestI2cMultipleAccess.cpp
        TestInverseLogicPin.cpp
        TestRepeatingButton.cpp
        TestSignalLed.cpp
        TestSpiMasterWithChipSelect.cpp
        TestSpiMultipleAccess.cpp
        TestTerminal.cpp
    )

    ccola_dependencies(
        network
        util
        hal.test_doubles
        infra.timer_test_helper
        infra.util_test_helper
    )

ccola_end_component()
