ccola_package(protobuf)

	ccola_subdirectories(
		protoc_cpp_infra_plugin
		protobuf_cpp_infra
		field_size
	)

	ccola_pack_directory(protoc_cpp_infra)

	ccola_subdirectories(
		protoc_cpp_infra_plugin/test
	)

ccola_end_package()
