if(${CCOLA_ARCH} STREQUAL win)
	set(CCOLA_TARGET_TEST 1)
	set(CCOLA_TARGET_TEST_PARAMS "--gtest_output=xml:JUnitResults.xml")

	ccola_translate_generator_expression(test_results_dir ${CCOLA_TEST_RESULTS_DIR})
	file(TO_NATIVE_PATH "${test_results_dir}" test_results_dir_n)
	set(CCOLA_TARGET_TEST_WORKING_DIR ${test_results_dir_n})

	include("${CCOLA_PACKAGE_DIR_cpp}/targets/target_executable/end.inc")

	add_custom_command(
		TARGET ${CCOLA_TARGET_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${test_results_dir}"
	)
else()
	include("${CCOLA_PACKAGE_DIR_cpp}/targets/target_skip/end.inc")
endif()
