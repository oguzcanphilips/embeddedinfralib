ccola_component(protobuf_infra static_library)

	ccola_sources(
		ProtoParser.cpp
		ProtoParser.hpp
		ProtoFormatter.cpp
		ProtoFormatter.hpp
	)

	ccola_dependencies(
		libprotobuf
		infra.stream
	)

ccola_end_component()

ccola_subdirectories(
	test
)
