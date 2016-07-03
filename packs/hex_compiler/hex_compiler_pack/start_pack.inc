macro(combine_hex output)

	ccola_verify_nested(${output} CCOLA_PACKAGE_NAME "combine_hex" "Package")

	if(NOT ${CCOLA_ARCH} STREQUAL "win")

		ccola_get_executable_path(hex_compiler_path hex_compiler.main)

		set(targets ${ARGV})
		list(REMOVE_AT targets 0)
		set(hex_files)
	
		set(target_paths)
		foreach(target ${targets})
			set(exe_dir ${CCOLA_EXE_INSTALL_DIR})
			string(REPLACE "<CCOLA_COMPONENT_NAME>" ${target} exe_dir "${exe_dir}")
			string(REPLACE "<CCOLA_CONFIGURATION>" "${CCOLA_SYSTEM_CONFIGURATION}" exe_dir "${exe_dir}")
			set(hex_path "${exe_dir}/${target}.hex")
			list(APPEND target_paths ${hex_path})
		endforeach()

		set(destination ${CCOLA_EXE_INSTALL_DIR})
		string(REPLACE "<CCOLA_COMPONENT_NAME>" ${CCOLA_PACKAGE_NAME}.${output} destination "${destination}")
		string(REPLACE "<CCOLA_CONFIGURATION>" "${CCOLA_SYSTEM_CONFIGURATION}" destination "${destination}")

		ccola_create_directory_on_demand("${destination}")

		add_custom_command(
			OUTPUT "${destination}/${CCOLA_PACKAGE_NAME}.${output}.hex"
			COMMAND "${hex_compiler_path}" ${target_paths} -output "${destination}/${CCOLA_PACKAGE_NAME}.${output}.hex"
			DEPENDS ${targets}
			DEPENDS "${destination}"
			DEPENDS "${hex_compiler_path}"
			WORKING_DIRECTORY "${destination}"
		)

		add_custom_target(
			combine_${output}
			ALL
			DEPENDS "${destination}/${CCOLA_PACKAGE_NAME}.${output}.hex"
		)

		ccola_configure_eclipse_launch("${destination}/${CCOLA_PACKAGE_NAME}.${output}.hex" ${CCOLA_PACKAGE_NAME}.${output})

	endif()

endmacro()


macro(bin_to_hex binary_input hex_output offset)

	ccola_verify_nested(${hex_output} CCOLA_PACKAGE_NAME "combine_hex" "Package")

	if(NOT ${CCOLA_ARCH} STREQUAL "win")

		ccola_get_executable_path(hex_compiler_path hex_compiler.main)

		set(destination ${CCOLA_EXE_INSTALL_DIR})
		string(REPLACE "<CCOLA_COMPONENT_NAME>" ${CCOLA_PACKAGE_NAME}.${hex_output} destination "${destination}")
		string(REPLACE "<CCOLA_CONFIGURATION>" "${CCOLA_SYSTEM_CONFIGURATION}" destination "${destination}")

		ccola_create_directory_on_demand("${destination}")

		add_custom_command(
			OUTPUT "${destination}/${CCOLA_PACKAGE_NAME}.${hex_output}.hex"
			COMMAND "${hex_compiler_path}" -offset ${offset} -binary ${binary_input} -output "${destination}/${CCOLA_PACKAGE_NAME}.${hex_output}.hex"
			DEPENDS ${binary_input}
			DEPENDS "${destination}"
			DEPENDS "${hex_compiler_path}"
			WORKING_DIRECTORY "${destination}"
		)

		add_custom_target(
			bin_to_hex_${hex_output}
			ALL
			DEPENDS "${destination}/${CCOLA_PACKAGE_NAME}.${hex_output}.hex"
		)

		ccola_configure_eclipse_launch("${destination}/${CCOLA_PACKAGE_NAME}.${hex_output}.hex" ${CCOLA_PACKAGE_NAME}.${hex_output})

	endif()

endmacro()
