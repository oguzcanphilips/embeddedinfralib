macro(protocol_buffer_cpp_infra input)

	ccola_make_absolute_from_source(absolute_input ${input})
	ccola_sources("${absolute_input}")
	ccola_set_source_group("Source Files" "${absolute_input}")
	ccola_get_filename_component_we(source_base ${input})

	get_filename_component(generated_dir_cpp_infra "${CMAKE_CURRENT_BINARY_DIR}/generated/proto_cpp_infra" ABSOLUTE)
	ccola_create_directory_on_demand("${generated_dir_cpp_infra}")

	ccola_get_executable_path(protoc_cpp_infra_plugin_path protobuf.protoc_cpp_infra_plugin)
	include("${CCOLA_BUILD_DIR}/component_info/protobuf.protoc_cpp_infra_plugin.generic_cpp.inc")
	set(extra_path "${CCOLA_DEPENDED_COMPONENT_RUNTIME_DIRECTORIES}")
	ccola_translate_generator_expression(extra_path "${extra_path}")

	message("Bla: ${CCOLA_PACK_DIR_protoc}")

	add_custom_command(
		OUTPUT "${generated_dir_cpp_infra}/${source_base}.pb.cpp" "${generated_dir_cpp_infra}/${source_base}.pb.hpp"
		COMMAND ${CMAKE_COMMAND} -E env "PATH=${extra_path};$ENV{PATH}" ${CCOLA_PACK_DIR_protoc}/../bin/protoc.exe --proto_path "${CMAKE_CURRENT_SOURCE_DIR}" --proto_path "${CCOLA_PACK_DIR_protoc}/../../../protobuf/field_size" --plugin=protoc-gen-cpp-infra=${protoc_cpp_infra_plugin_path} --cpp-infra_out="${generated_dir_cpp_infra}" ${absolute_input}
		DEPENDS "${absolute_input}"
		DEPENDS "${generated_dir_cpp_infra}"
		DEPENDS protobuf.protoc_cpp_infra_plugin
	)

	ccola_sources("${generated_dir_cpp_infra}/${source_base}.pb.cpp" "${generated_dir_cpp_infra}/${source_base}.pb.hpp")
	ccola_set_source_group("Generated Files" "${generated_dir_cpp_infra}/${source_base}.pb.cpp" "${generated_dir_cpp_infra}/${source_base}.pb.hpp")
	ccola_include_directories("${CMAKE_CURRENT_BINARY_DIR}")

	ccola_dependencies(
		protobuf.libprotobuf
		protobuf.protobuf_cpp_infra
	)

endmacro()
