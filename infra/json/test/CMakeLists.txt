ccola_component(json_test gmock_test)

	ccola_sources(
		TestJson.cpp
		TestJsonFormatter.cpp
	)

	ccola_dependencies(
		json
	)

ccola_end_component()
