ccola_component(syntax static_library)

	ccola_sources(
		private/Asn1Formatter.cpp
		private/Json.cpp
		private/JsonFormatter.cpp
		public/Asn1Formatter.hpp
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
