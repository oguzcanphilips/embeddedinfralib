ccola_component(tracer static_library)

	ccola_sources(
		private/StreamWriterOnSerialCommunication.cpp
		private/StreamWriterOnSynchronousSerialCommunication.cpp
		private/Tracer.cpp
        private/TracerAdapterPrintf.cpp
		private/TracerWithDateTime.cpp
		public/StreamWriterOnSerialCommunication.hpp
		public/StreamWriterOnSynchronousSerialCommunication.hpp
		public/Tracer.hpp
        public/TracerAdapterPrintf.hpp
		public/TracerWithDateTime.hpp
	)

	ccola_dependencies(
		hal.interfaces
		hal.synchronous_interfaces
		infra.stream
	)

ccola_end_component()

ccola_subdirectories(
	test
)
