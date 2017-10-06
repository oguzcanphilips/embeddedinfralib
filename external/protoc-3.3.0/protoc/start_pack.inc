macro(protocol_buffer_cpp input)

	ccola_make_absolute_from_source(absolute_input ${input})
	ccola_sources("${absolute_input}")
	ccola_set_source_group("Source Files" "${absolute_input}")
	ccola_get_filename_component_we(source_base ${input})

	get_filename_component(generated_dir_cpp "${CMAKE_CURRENT_BINARY_DIR}/generated/proto_cpp" ABSOLUTE)
	ccola_create_directory_on_demand("${generated_dir_cpp}")

	add_custom_command(
		OUTPUT "${generated_dir_cpp}/${source_base}.pb.cc" "${generated_dir_cpp}/${source_base}.pb.h"
		COMMAND ${CCOLA_PACK_DIR_protoc}/../bin/protoc.exe --proto_path "${CMAKE_CURRENT_SOURCE_DIR}" --proto_path "${CCOLA_PACK_DIR_protoc}/../../protobuf-3.3.0/src/google/protobuf" --proto_path "${CCOLA_PACK_DIR_protoc}/../../../protobuf/echo_attributes" --cpp_out="${generated_dir_cpp}" ${absolute_input}
		DEPENDS "${absolute_input}"
		DEPENDS "${generated_dir_cpp}"
	)

	ccola_sources("${generated_dir_cpp}/${source_base}.pb.cc" "${generated_dir_cpp}/${source_base}.pb.h")
	ccola_set_source_group("Generated Files" "${generated_dir_cpp}/${source_base}.pb.cc" "${generated_dir_cpp}/${source_base}.pb.h")
	ccola_inheritable_include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/generated/proto_cpp")

	ccola_dependencies(
		protobuf.libprotobuf
	)

endmacro()
