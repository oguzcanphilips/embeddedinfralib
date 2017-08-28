ccola_component(protoc_cpp_infra_plugin_lib win: static_library other: skip)

	ccola_compile_options(/wd4018 /wd4251 /wd4273 /wd4800)

	ccola_inheritable_definitions(LIBPROTOC_EXPORTS)

	ccola_sources(
		CppFormatter.cpp
		CppFormatter.hpp
		ProtoCCppInfraPlugin.cpp
		ProtoCCppInfraPlugin.hpp
	)

	ccola_dependencies(
		libprotobuf
		libprotoc
		field_size
		infra.stream
	)

ccola_end_component()

ccola_component(protoc_cpp_infra_plugin win: executable other: skip)

	ccola_compile_options(/wd4018 /wd4251 /wd4273 /wd4800)

	ccola_sources(
		Main.cpp
	)

	ccola_dependencies(
		protoc_cpp_infra_plugin_lib
	)

ccola_end_component()
