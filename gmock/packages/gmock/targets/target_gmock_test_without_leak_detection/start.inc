if(${CCOLA_ARCH} STREQUAL win)
	include("${CCOLA_PACKAGE_DIR_cpp}/targets/target_executable/start.inc")
	ccola_dependencies(
		gmock.gmock
		gmock.gtest_runner
	)
else()
	include("${CCOLA_PACKAGE_DIR_cpp}/targets/target_skip/start.inc")
endif()
