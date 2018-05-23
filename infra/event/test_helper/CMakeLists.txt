ccola_component(event_test_helper win: source other: skip)

    ccola_sources(
        EventDispatcherFixture.hpp
        EventDispatcherWithWeakPtrFixture.hpp
    )

    ccola_dependencies(
        event
        util_test_helper
    )

ccola_end_component()
