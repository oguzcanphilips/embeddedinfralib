ccola_component(network_test gmock_test)

    ccola_sources(
		TestCertificatesMbedTls.cpp
        TestConnectionMbedTls.cpp
    )

    ccola_dependencies(
        network
        network_test_doubles
        infra.timer_test_helper
        infra.util_test_helper
        hal.windows
    )

ccola_end_component()
