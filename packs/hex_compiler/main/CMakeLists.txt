ccola_component(main win: executable other: noncompile)

	ccola_sources(
		Main.cpp
	)

	ccola_dependencies(
		application
		hal.windows
	)

ccola_end_component()
