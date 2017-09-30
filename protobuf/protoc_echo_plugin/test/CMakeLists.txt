ccola_component(protoc_echo_plugin_test gmock_test)

	ccola_compile_options(/wd4018 /wd4251 /wd4273 /wd4800)
	
	protocol_buffer_echo(TestMessages.proto)

	ccola_sources(
		TestCppFormatter.cpp
		TestProtoCEchoPlugin.cpp
	)

	ccola_dependencies(
		protoc_echo_plugin_lib
	)

ccola_end_component()
