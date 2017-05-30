ccola_component(synchronous_test_doubles win: static_library other: noncompile)

	ccola_sources(
		SynchronousFlashStub.cpp
		SynchronousSerialCommunicationMock.cpp
		SynchronousFlashStub.hpp
		SynchronousSerialCommunicationMock.hpp
	)

	ccola_dependencies(
		synchronous_interfaces
		gmock.gmock
	)

ccola_end_component()
