ccola_component(test_doubles win: static_library other: noncompile)

	ccola_sources(
		FileSystemStub.cpp
		FlashMock.cpp
		FlashStub.cpp
		GpioStub.cpp
		I2cMock.cpp
		I2cRegisterAccessMock.cpp
		QuadSpiStub.cpp
		SerialCommunicationMock.cpp
		SerialCommunicationStub.cpp
		SpiMock.cpp
		AnalogToDigitalPinImplMock.hpp
		CommunicationConfiguratorMock.hpp
		DigitalToAnalogPinImplMock.hpp
		FileSystemStub.hpp
		FlashMock.hpp
		FlashStub.hpp
		GpioMock.hpp
		GpioStub.hpp
		I2cMock.hpp
		I2cRegisterAccessMock.hpp
		QuadSpiStub.hpp
		SerialCommunicationMock.hpp
		SerialCommunicationStub.hpp
		SpiMock.hpp
	)

	ccola_dependencies(
		interfaces
		gmock.gmock
		infra.stream
	)

ccola_end_component()

ccola_subdirectories(
	test
)