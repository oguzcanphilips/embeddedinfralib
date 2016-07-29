ccola_component(util_test gmock_test)

	ccola_sources(
		TestCyclicStore.cpp
		TestDebugLed.cpp
		TestFlashMultipleAccess.cpp
		TestFlashQuadSpi.cpp
		TestFlashRegion.cpp
		TestFlashSpi.cpp
		TestI2cMultipleAccess.cpp
		TestInverseLogicPin.cpp
		TestJson.cpp
		TestJsonFormatter.cpp
		TestRepeatingButton.cpp
		TestSignalLed.cpp
		TestSpiMasterWithChipSelect.cpp
		TestSpiMultipleAccess.cpp
	)

	ccola_dependencies(
		util
		hal.test_doubles
		infra.timer_test_helper
		infra.util_test_helper
	)

ccola_end_component()
