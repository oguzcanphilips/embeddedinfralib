ccola_component(tracer static_library)

	ccola_sources(
		private/StreamWriterOnSerialCommunication.cpp
		private/Tracer.cpp
		private/TracerWithDateTime.cpp
		public/StreamWriterOnSerialCommunication.hpp
		public/Tracer.hpp
		public/TracerWithDateTime.hpp
	)

	ccola_dependencies(
		hal.interfaces
		infra.stream
	)

ccola_end_component()

ccola_subdirectories(
	test
)
