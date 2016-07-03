ccola_component(windows win: static_library other: noncompile)

	ccola_sources(
		private/FileSystemWin.cpp
		public/FileSystemWin.hpp
	)

	ccola_dependencies(
		interfaces
	)

ccola_end_component()
