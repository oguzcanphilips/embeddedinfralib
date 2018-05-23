ccola_component(util static_library)

    ccola_sources(
        ConfigurationStore.cpp
        ConfigurationStore.hpp
        CyclicStore.cpp
        DebouncedButton.cpp
        DebugLed.cpp
        FlashDelayedInit.cpp
        FlashMultipleAccess.cpp
        FlashQuadSpiMicronN25q.cpp
        FlashQuadSpiCypressFll.cpp
        FlashSpi.cpp
        FlashRegion.cpp
        GpioPinInverted.cpp
        I2cMultipleAccess.cpp
        RepeatingButton.cpp
        SignalLed.cpp
        SpiMasterWithChipSelect.cpp
        SpiMultipleAccess.cpp
        Terminal.cpp
        CyclicStore.hpp
        DebouncedButton.hpp
        DebugLed.hpp
        FlashDelayedInit.hpp
        FlashMultipleAccess.hpp
        FlashQuadSpiMicronN25q.hpp
        FlashQuadSpiCypressFll.hpp
        FlashSpi.hpp
        FlashRegion.hpp
        GpioPinInverted.hpp
        I2cMultipleAccess.hpp
        RepeatingButton.hpp
        SignalLed.hpp
        SpiMasterWithChipSelect.hpp
        SpiMultipleAccess.hpp
        Terminal.hpp
    )

    ccola_dependencies(
        hal.interfaces
        infra.util
        infra.stream
		infra.syntax
        crypto.mbed_tls
    )

ccola_end_component()

ccola_subdirectories(
    test
)
