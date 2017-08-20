ccola_component(protoc_cpp_infra_plugin_test gmock_test)

	ccola_compile_options(/wd4018 /wd4251 /wd4273 /wd4800)
	
	protocol_buffer_cpp_infra(TestMessages.proto)

	ccola_sources(
		TestCppFormatter.cpp
		TestProtoCCppInfraPlugin.cpp
	)

	ccola_dependencies(
		protoc_cpp_infra_plugin_lib
	)

ccola_end_component()
