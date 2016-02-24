set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)		# Allows endif() without repeating the predicate of the if-statement
cmake_policy(SET CMP0005 NEW)					# Automatic escaping of preprocessor definitions
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
enable_testing()

project("Unnamed")

include(${CCOLA_DIR}/util.inc)
include(${CCOLA_DIR}/file_util.inc)

ccola_register_file_parameter(CCOLA_DIR)
ccola_register_file_parameter(CCOLA_INSTALL_DIR)
set(CCOLA_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CCOLA_TOP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
	set(CCOLA_SYSTEM_CONFIGURATION ${CMAKE_SYSTEM_NAME})
else()
	set(CCOLA_SYSTEM_CONFIGURATION ${CMAKE_SYSTEM_PROCESSOR})
	set(CCOLA_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
endif()

if(DEFINED CCOLA_EXTRA_CONFIGURATION)
	set(extra_configuration "-${CCOLA_EXTRA_CONFIGURATION}")
else()
	set(extra_configuration "")
endif()

include(${CCOLA_DIR}/project.inc)
include(${CCOLA_DIR}/platforms/platform-${CCOLA_SYSTEM_CONFIGURATION}.inc)

set(CCOLA_COMPONENT_INFO_INSTALL_DIR ${CCOLA_BUILD_DIR}/component_info)
set(CCOLA_PUBLIC_COMPONENT_INFO_INSTALL_DIR ${CCOLA_INSTALL_DIR}/component_info)
set(CCOLA_COMPONENT_INSTALL_DIR ${CCOLA_INSTALL_DIR}/components/<CCOLA_COMPONENT_NAME>)
set(CCOLA_COMPONENT_BINARY_INSTALL_DIR ${CCOLA_COMPONENT_INSTALL_DIR}/<CCOLA_CONFIGURATION>${extra_configuration})
set(CCOLA_EXE_INSTALL_DIR ${CCOLA_COMPONENT_BINARY_INSTALL_DIR}/bin)
set(CCOLA_DLL_INSTALL_DIR ${CCOLA_COMPONENT_BINARY_INSTALL_DIR}/bin)
set(CCOLA_IMPORT_LIBRARY_INSTALL_DIR ${CCOLA_BUILD_DIR}/<CCOLA_CONFIGURATION>-lib)
set(CCOLA_STATIC_LIBRARY_INSTALL_DIR ${CCOLA_BUILD_DIR}/<CCOLA_CONFIGURATION>-lib)
set(CCOLA_PDB_INSTALL_DIR ${CCOLA_BUILD_DIR}/<CCOLA_CONFIGURATION>-lib)
set(CCOLA_HEADER_INSTALL_DIR ${CCOLA_COMPONENT_INSTALL_DIR}/include)
set(CCOLA_TEST_RESULTS_DIR ${CCOLA_COMPONENT_BINARY_INSTALL_DIR}/test_results)

set(workaround_for_warning ${CMAKE_TOOLCHAIN_FILE})

set(CCOLA_PACK_SOURCE_DIRS ${CCOLA_DIR}/packs)

ccola_include_pack(cpp)
ccola_include_pack(csharp)
ccola_repository(${CCOLA_BUILD_DIR})

ccola_programmer(
lpc17xx:
	lpc-link
stm32f0xx:
	st-link
stm32f2xx:
	st-link
stm32f4xx:
	st-link
stm32f7xx:
	st-link
)

if(${CCOLA_ARCH_FAMILY} STREQUAL cortex)
	enable_language(ASM)
	set(can_use_assembler TRUE)
endif()
