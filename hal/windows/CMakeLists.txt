ccola_component(windows win: static_library other: noncompile)

	ccola_sources(
		FileSystemWin.cpp
		FileSystemWin.hpp
	)

	ccola_dependencies(
		interfaces
	)

ccola_end_component()
