ccola_find_first(info_file component_info/${item}.csharp_executable.inc ${CCOLA_REPOSITORIES})
if(NOT info_file)
	message(FATAL_ERROR "Dependency ${item} not found")
endif()
include(${info_file})

ccola_inheritable_dependencies(${CCOLA_DEPENDED_COMPONENT_DEPENDENCIES})
ccola_inheritable_include_directories(${CCOLA_DEPENDED_COMPONENT_INCLUDE_DIRECTORIES})
ccola_inheritable_definitions(${CCOLA_DEPENDED_COMPONENT_INHERITABLE_DEFINITIONS})
