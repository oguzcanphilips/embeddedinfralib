ccola_component(timer_test_helper win: static_library other: skip)

	ccola_sources(
		private/ClockFixture.cpp
		public/ClockFixture.hpp
	)

	ccola_dependencies(
		timer
		util_test_helper
	)

ccola_end_component()
