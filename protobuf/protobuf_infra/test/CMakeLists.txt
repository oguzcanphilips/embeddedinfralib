ccola_component(protobuf_infra_test gmock_test)

	ccola_sources(
		TestProtoFormatter.cpp
		TestProtoParser.cpp
	)

	ccola_dependencies(
		protobuf_infra
	)

ccola_end_component()
