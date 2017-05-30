ccola_component(network_test_doubles win: static_library other: noncompile)

	ccola_sources(
		ConnectionMock.cpp
		ConnectionMock.hpp
	)

	ccola_dependencies(
		network
		gmock.gmock
	)

ccola_end_component()
