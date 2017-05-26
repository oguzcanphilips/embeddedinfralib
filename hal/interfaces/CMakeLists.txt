ccola_component(interfaces static_library)

	ccola_sources(
		Ethernet.cpp
		Flash.cpp
		FlashHomogeneous.cpp
		Gpio.cpp
		I2c.cpp
		I2cRegisterAccess.cpp
		QuadSpi.cpp
		Spi.cpp
		AnalogToDigitalPin.hpp
		CommunicationConfigurator.hpp
		DigitalToAnalogPin.hpp
		Ethernet.hpp
		FileSystem.hpp
		FileSystem.cpp
		Flash.hpp
		FlashHomogeneous.hpp
		Gpio.hpp
		I2c.hpp
		I2cRegisterAccess.hpp
		QuadSpi.hpp
		SerialCommunication.hpp
		Spi.hpp
		UsbCustomHid.hpp
		UsbLinkLayer.hpp
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