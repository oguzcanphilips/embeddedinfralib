macro(generate_fsm)

	ccola_get_executable_path(fsm_path fsm.fsm)

	foreach(file ${ARGV})
		ccola_get_filename_component_we(name "${file}")
		set(generated_dir ${CCOLA_COMPONENT_NAME}_generated)
		string(REPLACE "." "_" generated_dir "${generated_dir}")
		set(destination "${CCOLA_COMPONENT_INSTALL_DIR}/${generated_dir}")
		ccola_translate_component_name(destination)

		ccola_create_directory_on_demand("${destination}")

		add_custom_command(
			OUTPUT "${destination}/${name}.cpp" "${destination}/${name}.hpp"
			COMMAND "${fsm_path}" "${CMAKE_CURRENT_SOURCE_DIR}/${file}" "${destination}" cpp
			MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
			DEPENDS "${destination}"
			DEPENDS "${fsm_path}"
			WORKING_DIRECTORY "${destination}"
		)

		ccola_inheritable_include_directories(
			"${destination}/.."
		)

		ccola_sources(
		generated=true
			"${destination}/${name}.cpp"
			"${destination}/${name}.hpp"
		)
	endforeach()

endmacro()
