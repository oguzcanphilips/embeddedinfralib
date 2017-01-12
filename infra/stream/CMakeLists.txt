ccola_component(stream static_library)

	ccola_sources(
		private/ByteInputStream.cpp
		private/ByteOutputStream.cpp
		private/InputStream.cpp
		private/OutputStream.cpp
		private/SavedMarkerStream.cpp
		private/StdStringInputStream.cpp
		private/StdStringOutputStream.cpp
		private/StreamManipulators.cpp
		private/StringInputStream.cpp
		private/StringOutputStream.cpp
		public/ByteInputStream.hpp
		public/ByteOutputStream.hpp
		public/InputStream.hpp
		public/OutputStream.hpp
		public/SavedMarkerStream.hpp
		public/StdStringInputStream.hpp
		public/StdStringOutputStream.hpp
		public/StreamManipulators.hpp
		public/StringInputStream.hpp
		public/StringOutputStream.hpp
	)

	ccola_dependencies(
		util
	)

ccola_end_component()

ccola_subdirectories(
	test
)
