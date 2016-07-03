ccola_component(synchronous_test_doubles win: static_library other: noncompile)

	ccola_sources(
		private/SynchronousFlashStub.cpp
		private/SynchronousSerialCommunicationMock.cpp
		public/SynchronousFlashStub.hpp
		public/SynchronousSerialCommunicationMock.hpp
	)

	ccola_dependencies(
		synchronous_interfaces
		gmock.gmock
	)

ccola_end_component()
