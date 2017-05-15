ccola_component(lwip_cpp static_library)

	ccola_sources(
		private/ConnectionLwIP.cpp
		private/LightweightIpOverEthernet.cpp
		public/ConnectionLwIP.hpp
		public/LightweightIpOverEthernet.hpp
	)

	ccola_dependencies(
		services.network
		lwip
		infra.event
	)

ccola_end_component()
