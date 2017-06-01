macro(protocol_buffer source)

	ccola_make_absolute_from_source(absolute_source ${source})
	ccola_set_source_group("Source Files" "${absolute_source}")
	ccola_get_filename_component_we(source_base ${source})

	get_filename_component(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/proto" ABSOLUTE)
	ccola_create_directory_on_demand("${generated_dir}")

	add_custom_command(
		OUTPUT "${generated_dir}/${source_base}.pb.cc" "${generated_dir}/${source_base}.pb.h"
		COMMAND ${CCOLA_PACK_DIR_protoc_pack}/../bin/protoc.exe --proto_path "${CMAKE_CURRENT_SOURCE_DIR}" --cpp_out "${generated_dir}" ${absolute_source}
		DEPENDS "${absolute_source}"
		DEPENDS "${generated_dir}"
	)

	ccola_sources("${generated_dir}/${source_base}.pb.cc" "${generated_dir}/${source_base}.pb.h")
	ccola_set_source_group("Generated Files" "${generated_dir}/${source_base}.pb.cc" "${generated_dir}/${source_base}.pb.h")
	ccola_include_directories("${CMAKE_CURRENT_BINARY_DIR}")

	ccola_dependencies(
		protobuf.libprotobuf
	)

endmacro()
