ccola_component(util_test_helper win: source other: skip)

	ccola_sources(
		InfraUtilMatchers.hpp
		MockCallback.hpp
		MockHelpers.hpp
		MonitoredConstructionObject.hpp
		MoveConstructible.hpp
	)

	ccola_dependencies(
		gmock.gmock
		util
	)

ccola_end_component()
