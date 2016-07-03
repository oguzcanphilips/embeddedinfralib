macro(upgrade_pack output upgrader)

	ccola_component(${output} noncompile)
		if(NOT ${CCOLA_ARCH} STREQUAL "win")

			ccola_get_executable_path(upgrade_pack_builder_path ${upgrader})

			set(arguments ${ARGV})
			list(REMOVE_AT arguments 0 1)
			set(upgrade_arguments)
			set(upgrade_dependencies)

			foreach(argument ${arguments})
				# The argument is formed as "function=component", which is split into firmware_function and firmware_component
				string(FIND ${argument} "=" position)
				string(SUBSTRING ${argument} 0 ${position} firmware_function)

				string(LENGTH ${argument} size)
				if(${position} STREQUAL "-1")
					list(APPEND upgrade_arguments "${argument}")
				else()
					math(EXPR startposition "${position} + 1")
					math(EXPR resultsize "${size} - ${position} - 1")
					string(SUBSTRING ${argument} ${startposition} ${resultsize} firmware_component)

					set(exe_dir ${CCOLA_EXE_INSTALL_DIR})
					string(REPLACE "<CCOLA_COMPONENT_NAME>" "${firmware_component}" exe_dir "${exe_dir}")
					string(REPLACE "<CCOLA_CONFIGURATION>" "${CCOLA_SYSTEM_CONFIGURATION}" exe_dir "${exe_dir}")
					get_filename_component(firmware_path "${firmware_component}" ABSOLUTE)
					get_source_file_property(generated "${firmware_path}" GENERATED)

					if(EXISTS "${CCOLA_PUBLIC_COMPONENT_INFO_INSTALL_DIR}/${firmware_component}.generic_cpp.inc")
						set(firmware_path "${exe_dir}/${firmware_component}.hex")
						list(APPEND upgrade_arguments -${firmware_function} "${firmware_path}")
						list(APPEND upgrade_dependencies ${firmware_component})
					elseif(EXISTS "${firmware_path}" OR ${generated})
						list(APPEND upgrade_arguments -${firmware_function} "${firmware_path}")
						list(APPEND upgrade_dependencies ${firmware_component} "${firmware_path}")
					else()
						message(FATAL_ERROR "Could not find ${firmware_path}")
					endif()
				endif()
			endforeach()

			set(intermediate_destination ${CMAKE_CURRENT_BINARY_DIR})

			set(install_destination ${CCOLA_EXE_INSTALL_DIR})
			string(REPLACE "<CCOLA_COMPONENT_NAME>" ${CCOLA_PACKAGE_NAME}.${output} install_destination "${install_destination}")
			string(REPLACE "<CCOLA_CONFIGURATION>" "${CCOLA_SYSTEM_CONFIGURATION}" install_destination "${install_destination}")

			ccola_create_directory_on_demand("${install_destination}")

			add_custom_command(
				OUTPUT "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.upg"
				COMMAND "${upgrade_pack_builder_path}" "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.upg" ${upgrade_arguments}
				DEPENDS ${upgrade_dependencies}
				DEPENDS "${intermediate_destination}"
				DEPENDS "${upgrade_pack_builder_path}"
				WORKING_DIRECTORY "${intermediate_destination}"
			)

			add_custom_command(
				OUTPUT "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.o"
				COMMAND ${CCOLA_OBJ_COPY_TOOL} -I binary -O elf32-littlearm -B arm --rename-section .data=.upgrade_pack "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.upg" "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.o"
				DEPENDS "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.upg"
			)

			ccola_public_property(object "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.o")

			add_custom_command(
				OUTPUT "${install_destination}/${CCOLA_PACKAGE_NAME}.${output}.elf"
				COMMAND ${CCOLA_LD_TOOL} -m armelf "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.o" -T "${CCOLA_PACK_DIR_pack_builder_pack}/upgrade_pack.ld" -o "${install_destination}/${CCOLA_PACKAGE_NAME}.${output}.elf"
				DEPENDS "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.o"
				DEPENDS "${install_destination}"
			)

			add_custom_target(
				upgrade_pack_${CCOLA_PACKAGE_NAME}_${output}
				DEPENDS "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.upg"
				DEPENDS "${intermediate_destination}/${CCOLA_PACKAGE_NAME}.${output}.o"
				DEPENDS "${install_destination}/${CCOLA_PACKAGE_NAME}.${output}.elf"
			)

			ccola_configure_eclipse_launch("${install_destination}/${CCOLA_PACKAGE_NAME}.${output}.elf" ${CCOLA_PACKAGE_NAME}.${output})
		
		endif()
	ccola_end_component()

endmacro()
