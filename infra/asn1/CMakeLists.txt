ccola_component(asn1 static_library)

	ccola_sources(
		private/Asn1Formatter.cpp
		public/Asn1Formatter.hpp
	)

	ccola_dependencies(
		stream
	)

ccola_end_component()

ccola_subdirectories(
	test
)
