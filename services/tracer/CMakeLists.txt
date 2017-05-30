ccola_component(tracer static_library)

	ccola_sources(
		StreamWriterOnSerialCommunication.cpp
		StreamWriterOnSynchronousSerialCommunication.cpp
		Tracer.cpp
        TracerAdapterPrintf.cpp
		TracerWithDateTime.cpp
		StreamWriterOnSerialCommunication.hpp
		StreamWriterOnSynchronousSerialCommunication.hpp
		Tracer.hpp
        TracerAdapterPrintf.hpp
		TracerWithDateTime.hpp
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
