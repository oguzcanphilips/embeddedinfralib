ccola_component(test_doubles win: static_library other: noncompile)

	ccola_sources(
		private/FileSystemStub.cpp
		private/FlashMock.cpp
		private/FlashStub.cpp
		private/GpioStub.cpp
		private/I2cMock.cpp
		private/I2cRegisterAccessMock.cpp
		private/QuadSpiStub.cpp
		private/SerialCommunicationMock.cpp
		private/SerialCommunicationStub.cpp
		private/SpiMock.cpp
		public/AnalogToDigitalPinImplMock.hpp
		public/CommunicationConfiguratorMock.hpp
		public/DigitalToAnalogPinImplMock.hpp
		public/FileSystemStub.hpp
		public/FlashMock.hpp
		public/FlashStub.hpp
		public/GpioMock.hpp
		public/GpioStub.hpp
		public/I2cMock.hpp
		public/I2cRegisterAccessMock.hpp
		public/QuadSpiStub.hpp
		public/SerialCommunicationMock.hpp
		public/SerialCommunicationStub.hpp
		public/SpiMock.hpp
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