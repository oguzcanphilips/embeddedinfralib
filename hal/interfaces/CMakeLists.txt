ccola_component(interfaces static_library)

	ccola_sources(
		private/Ethernet.cpp
		private/Flash.cpp
		private/FlashHomogeneous.cpp
		private/Gpio.cpp
		private/I2c.cpp
		private/I2cRegisterAccess.cpp
		private/QuadSpi.cpp
		private/Spi.cpp
		public/AnalogToDigitalPin.hpp
		public/CommunicationConfigurator.hpp
		public/DigitalToAnalogPin.hpp
		public/Ethernet.hpp
		public/FileSystem.hpp
		public/FileSystem.cpp
		public/Flash.hpp
		public/FlashHomogeneous.hpp
		public/Gpio.hpp
		public/I2c.hpp
		public/I2cRegisterAccess.hpp
		public/QuadSpi.hpp
		public/SerialCommunication.hpp
		public/Spi.hpp
		public/UsbCustomHid.hpp
		public/UsbLinkLayer.hpp
	)

	ccola_dependencies(
		infra.util
		infra.event
	)

ccola_end_component()

ccola_subdirectories(
	test_doubles
	test
)