ccola_component(synchronous_interfaces static_library)

	ccola_sources(
		SynchronousFlash.cpp
		SynchronousFlashHomogeneous.cpp
		SynchronousSpi.cpp
		SynchronousFlash.hpp
		SynchronousFlashHomogeneous.hpp
		SynchronousGpio.hpp
		SynchronousI2c.hpp
		SynchronousQuadSpi.hpp
		SynchronousSerialCommunication.hpp
		SynchronousSpi.hpp
		TimeKeeper.hpp
	)

	ccola_dependencies(
		infra.util
	)

ccola_end_component()

ccola_subdirectories(
	test_doubles
)