ccola_component(stream static_library)

	ccola_sources(
		ByteInputStream.cpp
		ByteOutputStream.cpp
		InputStream.cpp
		OutputStream.cpp
		SavedMarkerStream.cpp
		StdStringInputStream.cpp
		StdStringOutputStream.cpp
		StreamManipulators.cpp
		StringInputStream.cpp
		StringOutputStream.cpp
		ByteInputStream.hpp
		ByteOutputStream.hpp
		InputStream.hpp
		OutputStream.hpp
		SavedMarkerStream.hpp
		StdStringInputStream.hpp
		StdStringOutputStream.hpp
		StreamManipulators.hpp
		StringInputStream.hpp
		StringOutputStream.hpp
	)

	ccola_dependencies(
		util
	)

ccola_end_component()

ccola_subdirectories(
	test
)
