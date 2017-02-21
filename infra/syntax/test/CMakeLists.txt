ccola_component(syntax_test gmock_test)

	ccola_sources(
		TestAsn1Formatter.cpp
		TestJson.cpp
		TestJsonFormatter.cpp
	)

	ccola_dependencies(
		syntax
	)

ccola_end_component()
