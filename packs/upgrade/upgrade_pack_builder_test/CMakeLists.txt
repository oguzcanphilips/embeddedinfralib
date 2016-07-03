ccola_component(pack_builder_library_test gmock_test)

	ccola_sources(
		ImageSecurityNone.cpp
		ImageSecurityNone.hpp
		TestImageAuthenticatorHmac.cpp
		TestImageEncryptorAes.cpp
		TestImageSecurityXteaHmac.cpp
		TestImageSignerEcDsa.cpp
		TestImageSignerRsa.cpp
		TestInputBinary.cpp
		TestInputHex.cpp
		TestUpgradePackBuilder.cpp
	)

	ccola_dependencies(
		pack_builder_library
		hal.test_doubles
	)

ccola_end_component()
