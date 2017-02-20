ccola_component(event_test_helper win: source other: skip)

	ccola_sources(
		public/EventDispatcherFixture.hpp
		public/EventDispatcherWithWeakPtrFixture.hpp
	)

	ccola_dependencies(
		event
		util_test_helper
	)

ccola_end_component()
