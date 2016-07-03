ccola_component(util_test_helper win: source other: skip)

	ccola_sources(
		public/BoundedStringMatcher.hpp
		public/MockCallback.hpp
		public/MoveConstructible.hpp
	)

	ccola_dependencies(
		gmock.gmock
		util
	)

ccola_end_component()
