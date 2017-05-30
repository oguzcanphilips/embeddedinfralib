ccola_component(lwip_cpp static_library)

	ccola_sources(
		ConnectionLwIP.cpp
		LightweightIpOverEthernet.cpp
		ConnectionLwIP.hpp
		LightweightIpOverEthernet.hpp
	)

	ccola_dependencies(
		services.network
		lwip
		infra.event
	)

ccola_end_component()
