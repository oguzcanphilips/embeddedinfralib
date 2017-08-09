ccola_component(protoc_cpp_infra_plugin_test gmock_test)

	protocol_buffer_cpp_infra(TestMessages.proto)

	ccola_sources(
		TestProtoCCppInfraPlugin.cpp
	)

	ccola_dependencies(
		protoc_cpp_infra_plugin
	)

ccola_end_component()
