ccola_package(upgrade)

	ccola_pack_directory(pack_builder_pack)

	ccola_subdirectories(
		upgrade_pack
		upgrade_pack_builder_library
		upgrade_pack_builder_test
		upgrade_security_key_generator
		upgrade_boot_loader
		upgrade_boot_loader_test
		upgrade_deploy_pack_to_external
		upgrade_deploy_pack_to_external_test
	)

ccola_end_package()
