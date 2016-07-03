ccola_component(util static_library)

	ccola_sources(
		private/CyclicStore.cpp
		private/DebugLed.cpp
		private/FlashDelayedInit.cpp
		private/FlashMultipleAccess.cpp
		private/FlashQuadSpi.cpp
		private/FlashSpi.cpp
		private/FlashRegion.cpp
		private/GpioPinInverted.cpp
		private/I2cMultipleAccess.cpp
		private/RepeatingButton.cpp
		private/SignalLed.cpp
		private/SpiMasterWithChipSelect.cpp
		private/SpiMultipleAccess.cpp
		public/CyclicStore.hpp
		public/DebugLed.hpp
		public/FlashDelayedInit.hpp
		public/FlashMultipleAccess.hpp
		public/FlashQuadSpi.hpp
		public/FlashSpi.hpp
		public/FlashRegion.hpp
		public/GpioPinInverted.hpp
		public/I2cMultipleAccess.hpp
		public/RepeatingButton.hpp
		public/SignalLed.hpp
		public/SpiMasterWithChipSelect.hpp
		public/SpiMultipleAccess.hpp
	)

	ccola_dependencies(
		hal.interfaces
		infra.util
	)

ccola_end_component()

ccola_subdirectories(
	test
)
