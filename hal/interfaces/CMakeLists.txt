ccola_component(interfaces static_library)

	ccola_sources(
		private/Flash.cpp
		private/FlashHomogeneous.cpp
		private/Gpio.cpp
		private/I2cRegisterAccess.cpp
		private/QuadSpi.cpp
		private/Spi.cpp
		public/FileSystem.hpp
		public/Flash.hpp
		public/FlashHomogeneous.hpp
		public/Gpio.hpp
		public/I2c.hpp
		public/I2cRegisterAccess.hpp
		public/QuadSpi.hpp
		public/SerialCommunication.hpp
		public/Spi.hpp
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