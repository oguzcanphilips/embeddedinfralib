ccola_component(util static_library)

    ccola_sources(
        ConfigurationStore.cpp
        ConfigurationStore.hpp
        CyclicStore.cpp
        CyclicStore.hpp
        DebouncedButton.cpp
        DebouncedButton.hpp
        DebugLed.cpp
        DebugLed.hpp
        FlashDelayedInit.cpp
        FlashDelayedInit.hpp
        FlashMultipleAccess.cpp
        FlashMultipleAccess.hpp
        FlashQuadSpi.cpp
        FlashQuadSpi.hpp
        FlashQuadSpiMicronN25q.cpp
        FlashQuadSpiMicronN25q.hpp
        FlashQuadSpiCypressFll.cpp
        FlashQuadSpiCypressFll.hpp
        FlashSpi.cpp
        FlashSpi.hpp
        FlashRegion.cpp
        FlashRegion.hpp
        GpioPinInverted.cpp
        GpioPinInverted.hpp
        I2cMultipleAccess.cpp
        I2cMultipleAccess.hpp
        RepeatingButton.cpp
        RepeatingButton.hpp
        SignalLed.cpp
        SignalLed.hpp
        SpiMasterWithChipSelect.cpp
        SpiMasterWithChipSelect.hpp
        SpiMultipleAccess.cpp
        SpiMultipleAccess.hpp
        Terminal.cpp
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
