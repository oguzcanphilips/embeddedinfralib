macro(generate_xslt output)

	set(inputs ${ARGV})
	list(REMOVE_AT inputs 0)

	set(absolute_inputs)
	foreach(input ${inputs})
		ccola_make_absolute_from_source(absolute_input ${input})
		list(APPEND absolute_inputs ${absolute_input})
	endforeach()

	add_custom_command(
		OUTPUT ${output}
		COMMAND ${CCOLA_PACK_DIR_xsltproc}/xsltproc.exe --output ${output} ${absolute_inputs}
		DEPENDS ${absolute_inputs}
	)

endmacro()
