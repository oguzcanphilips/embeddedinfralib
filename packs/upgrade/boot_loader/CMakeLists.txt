ccola_component(boot_loader static_library)

	ccola_sources(
		private/DecryptorAesMbedTls.cpp
		private/DecryptorAesTiny.cpp
		private/DecryptorNone.cpp
		private/DecryptorXtea.cpp
		private/DiCommUart.cpp
		private/ImageUpgrader.cpp
		private/ImageUpgraderDiComm.cpp
		private/ImageUpgraderInternalFlash.cpp
		private/ImageUpgraderSkip.cpp
		private/JsonObject.cpp
		private/PackUpgrader.cpp
		private/SecondStageToRamLoader.cpp
		private/VerifierEcDsa.cpp
		private/VerifierNone.cpp
		private/VerifierRsa.cpp
		public/Decryptor.hpp
		public/DecryptorAesMbedTls.hpp
		public/DecryptorAesTiny.hpp
		public/DecryptorNone.hpp
		public/DecryptorXtea.hpp
		public/DiComm.hpp
		public/DiCommUart.hpp
		public/ImageUpgrader.hpp
		public/ImageUpgraderDiComm.hpp
		public/ImageUpgraderInternalFlash.hpp
		public/ImageUpgraderSkip.hpp
		public/JsonObject.hpp
		public/PackUpgrader.hpp
		public/SecondStageToRamLoader.hpp
		public/Verifier.hpp
		public/VerifierEcDsa.hpp
		public/VerifierNone.hpp
		public/VerifierRsa.hpp
	)

	ccola_dependencies(
		pack
		infra.util
		infra.stream
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
