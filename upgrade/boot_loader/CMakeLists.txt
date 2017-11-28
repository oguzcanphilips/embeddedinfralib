ccola_component(boot_loader static_library)

	ccola_sources(
		DecryptorAesMbedTls.cpp
		DecryptorAesTiny.cpp
		DecryptorNone.cpp
		DecryptorXtea.cpp
		DiCommUart.cpp
		ImageUpgrader.cpp
		ImageUpgraderDiComm.cpp
		ImageUpgraderInternalFlash.cpp
		ImageUpgraderSkip.cpp
		PackUpgrader.cpp
		SecondStageToRamLoader.cpp
		VerifierEcDsa.cpp
		VerifierNone.cpp
		VerifierRsa.cpp
		Decryptor.hpp
		DecryptorAesMbedTls.hpp
		DecryptorAesTiny.hpp
		DecryptorNone.hpp
		DecryptorXtea.hpp
		DiComm.hpp
		DiCommUart.hpp
		ImageUpgrader.hpp
		ImageUpgraderDiComm.hpp
		ImageUpgraderInternalFlash.hpp
		ImageUpgraderSkip.hpp
		PackUpgrader.hpp
		SecondStageToRamLoader.hpp
		Verifier.hpp
		VerifierEcDsa.hpp
		VerifierNone.hpp
		VerifierRsa.hpp
	)

	ccola_dependencies(
		pack
		infra.util
		infra.stream
		infra.syntax
		crypto.mbed_tls
		crypto.micro_ecc
		crypto.tiny_aes128
		hal.synchronous_interfaces
		services.synchronous_util
	)

ccola_end_component()

ccola_subdirectories(
	test
)
