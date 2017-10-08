ccola_component(stream static_library)

	ccola_sources(
		ByteInputStream.cpp
		ByteInputStream.hpp
		ByteOutputStream.cpp
		ByteOutputStream.hpp
		InputStream.cpp
		InputStream.hpp
		IoOutputStream.cpp
		IoOutputStream.hpp
		LimitedInputStream.cpp
		LimitedInputStream.hpp
		OutputStream.cpp
		OutputStream.hpp
		SavedMarkerStream.cpp
		SavedMarkerStream.hpp
		StdStringInputStream.cpp
		StdStringInputStream.hpp
		StdStringOutputStream.cpp
		StdStringOutputStream.hpp
		StreamManipulators.cpp
		StreamManipulators.hpp
		StringInputStream.cpp
		StringInputStream.hpp
		StringOutputStream.cpp
		StringOutputStream.hpp
	)

	ccola_dependencies(
		util
	)

ccola_end_component()

ccola_subdirectories(
	test
)
