ccola_component(network static_library)

	ccola_sources(
		private/Connection.cpp
		public/Connection.hpp
	)

	ccola_dependencies(
		infra.event
		infra.timer
	)

ccola_end_component()

ccola_subdirectories(
	test_doubles
)
