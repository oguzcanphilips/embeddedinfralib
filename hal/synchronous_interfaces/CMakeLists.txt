ccola_component(synchronous_interfaces static_library)

	ccola_sources(
		private/SynchronousFlash.cpp
		private/SynchronousFlashHomogeneous.cpp
		private/SynchronousSpi.cpp
		public/SynchronousFlash.hpp
		public/SynchronousFlashHomogeneous.hpp
		public/SynchronousGpio.hpp
		public/SynchronousI2c.hpp
		public/SynchronousQuadSpi.hpp
		public/SynchronousSerialCommunication.hpp
		public/SynchronousSpi.hpp
		public/TimeKeeper.hpp
	)

	ccola_dependencies(
		infra.util
	)

ccola_end_component()

ccola_subdirectories(
	test_doubles
)