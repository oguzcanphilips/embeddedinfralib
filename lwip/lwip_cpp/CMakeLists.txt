ccola_component(lwip_cpp static_library)

	ccola_sources(
		ConnectionLwIp.cpp
		ConnectionLwIp.hpp
		DatagramLwIp.cpp
		DatagramLwIp.hpp
		LightweightIp.cpp
		LightweightIp.hpp
		LightweightIpOverEthernet.cpp
		LightweightIpOverEthernet.hpp
		MulticastLwIp.cpp
		MulticastLwIp.hpp
	)

	ccola_dependencies(
		services.network
		lwip
		infra.event
		hal.interfaces
	)

ccola_end_component()
