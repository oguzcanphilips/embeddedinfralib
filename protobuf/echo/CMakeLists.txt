ccola_component(echo static_library)

	ccola_sources(
		Echo.cpp
		Echo.hpp
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
