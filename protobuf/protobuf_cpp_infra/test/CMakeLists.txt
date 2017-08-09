ccola_component(protobuf_cpp_infra_test gmock_test)

	ccola_sources(
		TestProtoFormatter.cpp
		TestProtoParser.cpp
	)

	ccola_dependencies(
		protobuf_cpp_infra
	)

ccola_end_component()
