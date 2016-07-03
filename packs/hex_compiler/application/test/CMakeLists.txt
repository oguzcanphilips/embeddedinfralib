ccola_component(test gmock_test)

	ccola_sources(
		TestBinaryObject.cpp
		TestCompiler.cpp
		TestHexOutput.cpp
		TestSparseVector.cpp
	)

	ccola_dependencies(
		application
		hal.test_doubles
	)

ccola_end_component()
