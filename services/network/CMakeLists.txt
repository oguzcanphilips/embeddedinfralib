ccola_component(network static_library)

	ccola_sources(
		Address.cpp
		Address.hpp
		Connection.cpp
		Connection.hpp
		ConnectionMbedTls.cpp
		ConnectionMbedTls.hpp
	)

	ccola_dependencies(
		infra.event
		infra.stream
		infra.timer
		crypto.mbed_tls
		hal.synchronous_interfaces
	)

ccola_end_component()

ccola_subdirectories(
	test_doubles
	test
)
