ccola_component(network static_library)

	ccola_sources(
		Address.cpp
		Address.hpp
		Connection.cpp
		Connection.hpp
	)

	ccola_dependencies(
		infra.event
		infra.timer
	)

ccola_end_component()

ccola_subdirectories(
	test_doubles
)
