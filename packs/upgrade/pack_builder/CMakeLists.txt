ccola_component(pack_builder win: static_library other: noncompile)

	ccola_sources(
		private/BuildReferenceUpgradePack.cpp
		private/BuildUpgradePack.cpp
		private/ImageAuthenticatorHmac.cpp
		private/ImageEncryptorAes.cpp
		private/ImageEncryptorNone.cpp
		private/ImageEncryptorXtea.cpp
		private/ImageSecurityXteaHmac.cpp
		private/ImageSignerEcDsa.cpp
		private/ImageSignerRsa.cpp
		private/Input.cpp
		private/InputBinary.cpp
		private/InputHex.cpp
		private/RandomNumberGenerator.cpp
		private/UpgradePackBuilder.cpp
		private/UpgradePackInputFactory.cpp
		public/BuildReferenceUpgradePack.hpp
		public/BuildUpgradePack.hpp
		public/ImageAUthenticatorHmac.hpp
		public/ImageEncryptorAes.hpp
		public/ImageEncryptorNone.hpp
		public/ImageEncryptorXtea.hpp
		public/ImageSecurity.hpp
		public/ImageSecurityXteaHmac.hpp
		public/ImageSigner.hpp
		public/ImageSignerEcDsa.hpp
		public/ImageSignerRsa.hpp
		public/Input.hpp
		public/InputBinary.hpp
		public/InputFactory.hpp
		public/InputHex.hpp
		public/RandomNumberGenerator.hpp
		public/UpgradePackBuilder.hpp
		public/UpgradePackInputFactory.hpp
	)

	ccola_dependencies(
		pack
		infra.util
		hal.interfaces
		crypto.mbed_tls
		crypto.micro_ecc
		crypto.tiny_aes128
		hex_compiler.application
	)

ccola_end_component()

ccola_subdirectories(
	test
)
