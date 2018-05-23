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

macro(protocol_buffer_csharp input)

	ccola_make_absolute_from_source(absolute_input ${input})
	ccola_sources("${absolute_input}")
	ccola_set_source_group("Source Files" "${absolute_input}")
	ccola_get_filename_component_we(source_base ${input})

	get_filename_component(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated" ABSOLUTE)
	ccola_create_directory_on_demand("${generated_dir_csharp}")

	add_custom_command(
		OUTPUT "${generated_dir}/${source_base}.cs"
		COMMAND ${CCOLA_PACK_DIR_protoc}/../bin/protoc.exe --proto_path "${CMAKE_CURRENT_SOURCE_DIR}" --proto_path "${CCOLA_PACK_DIR_protoc}/../../protobuf-3.3.0/src/google/protobuf" --proto_path "${CCOLA_PACK_DIR_protoc}/../../../protobuf/echo_attributes" --csharp_out="${generated_dir}" ${absolute_input}
		DEPENDS "${absolute_input}"
		DEPENDS "${generated_dir}"
	)

	ccola_sources("${generated_dir}/${source_base}.cs")
	ccola_set_source_group("Generated Files" "${generated_dir}/${source_base}.cs")

endmacro()

macro(protocol_buffer_dependencies)

	if(NOT CCOLA_COMPONENT_SKIP)
		ccola_assert(CCOLA_COMPONENT_NAME "This command may only be used in a component")

		set(dependencies "${ARGV}")
		ccola_parse_selection_list(dependencies)

		foreach(item ${dependencies})
			ccola_make_full_component_name(item)
			ccola_dependencies(${item})
			set(PROTOBUF_DEPENDENCIES ${PROTOBUF_DEPENDENCIES} ${item})
		endforeach()
	endif()

endmacro()
