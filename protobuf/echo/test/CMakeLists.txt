ccola_component(echo_test gmock_test)

	ccola_sources(
		TestProtoFormatter.cpp
		TestProtoParser.cpp
	)

	ccola_dependencies(
		echo
	)

ccola_end_component()
