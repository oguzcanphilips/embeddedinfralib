macro(protocol_buffer_echo input)

	if(NOT CCOLA_COMPONENT_SKIP)
		ccola_make_absolute_from_source(absolute_input ${input})
		ccola_sources("${absolute_input}")
		ccola_set_source_group("Source Files" "${absolute_input}")
		ccola_get_filename_component_we(source_base ${input})

		get_filename_component(generated_dir_echo "${CMAKE_CURRENT_BINARY_DIR}/generated/echo" ABSOLUTE)
		ccola_create_directory_on_demand("${generated_dir_echo}")

		ccola_get_executable_path(protoc_echo_plugin_path protobuf.protoc_echo_plugin)
		ccola_get_executable_path(protobuf_libprotobuf_path protobuf.libprotobuf)
		get_filename_component(protobuf_libprotobuf_path "${protobuf_libprotobuf_path}" DIRECTORY)
		ccola_translate_generator_expression(extra_path "${protobuf_libprotobuf_path}")

		string(REPLACE "\"" "" old_path "$ENV{PATH}")

		add_custom_command(
			OUTPUT "${generated_dir_echo}/${source_base}.pb.cpp" "${generated_dir_echo}/${source_base}.pb.hpp"
			COMMAND ${CMAKE_COMMAND} -E env "\"PATH=${extra_path};${old_path};\"" ${CCOLA_PACK_DIR_protoc}/../bin/protoc.exe --proto_path "${CMAKE_CURRENT_SOURCE_DIR}" --proto_path "${CCOLA_PACK_DIR_protoc}/../../protobuf-3.3.0/src/google/protobuf" --proto_path "${CCOLA_PACK_DIR_protoc}/../../../protobuf/echo_attributes" --plugin=protoc-gen-cpp-infra=${protoc_echo_plugin_path} --cpp-infra_out="${generated_dir_echo}" ${absolute_input}
			DEPENDS "${absolute_input}"
			DEPENDS "${generated_dir_echo}"
			DEPENDS "${protoc_echo_plugin_path}"
		)

		ccola_sources("${generated_dir_echo}/${source_base}.pb.cpp" "${generated_dir_echo}/${source_base}.pb.hpp")
		ccola_set_source_group("Generated Files" "${generated_dir_echo}/${source_base}.pb.cpp" "${generated_dir_echo}/${source_base}.pb.hpp")
		ccola_inheritable_include_directories("${CMAKE_CURRENT_BINARY_DIR}")

		ccola_dependencies(
			protobuf.echo
		)
	endif()

endmacro()
