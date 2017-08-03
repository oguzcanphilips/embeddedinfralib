ccola_component(protobuf_cpp_infra static_library)

	ccola_sources(
		ProtoParser.cpp
		ProtoParser.hpp
		ProtoFormatter.cpp
		ProtoFormatter.hpp
	)

	ccola_dependencies(
		infra.stream
	)

ccola_end_component()

ccola_subdirectories(
	test
)
