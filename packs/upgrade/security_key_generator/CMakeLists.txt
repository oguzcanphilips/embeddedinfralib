ccola_component(security_key_generator win: executable other: noncompile)

	ccola_sources(
		private/Main.cpp
		private/MaterialGenerator.cpp
		public/MaterialGenerator.hpp
	)

	ccola_dependencies(
		crypto.mbed_tls
		pack_builder
	)

ccola_end_component()
