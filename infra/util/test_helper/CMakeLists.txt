ccola_component(util_test_helper win: source other: skip)

	ccola_sources(
		BoundedStringMatcher.hpp
		MockCallback.hpp
		MoveConstructible.hpp
	)

	ccola_dependencies(
		gmock.gmock
		util
	)

ccola_end_component()
