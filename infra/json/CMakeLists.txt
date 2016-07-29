ccola_component(json static_library)

	ccola_sources(
		private/Json.cpp
		private/JsonFormatter.cpp
		public/Json.hpp
		public/JsonFormatter.hpp
	)

	ccola_dependencies(
		stream
	)

ccola_end_component()

ccola_subdirectories(
	test
)
