ccola_component(network_test_doubles win: static_library other: noncompile)

	ccola_sources(
		ConnectionLoopBack.cpp
		ConnectionLoopBack.hpp
		ConnectionMock.cpp
		ConnectionMock.hpp
		ConnectionStub.cpp
		ConnectionStub.hpp
		DatagramMock.hpp
		MulticastMock.hpp
	)

	ccola_dependencies(
		network
		gmock.gmock
	)

ccola_end_component()
