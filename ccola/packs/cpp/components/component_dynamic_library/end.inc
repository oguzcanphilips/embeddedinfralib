include("${CCOLA_PACK_DIR_cpp}/components/generic_cpp/end_before.inc")

add_library(${CCOLA_COMPONENT_NAME} SHARED ${CCOLA_COMPONENT_SOURCEFILES} ${CCOLA_COMPONENT_SOURCE_DEPENDENCIES})
ccola_set_component_properties_for_all_configurations(${CCOLA_COMPONENT_NAME} RUNTIME_OUTPUT_DIRECTORY ${CCOLA_DLL_INSTALL_DIR})
ccola_set_component_properties_for_all_configurations(${CCOLA_COMPONENT_NAME} ARCHIVE_OUTPUT_DIRECTORY ${CCOLA_IMPORT_LIBRARY_INSTALL_DIR})
ccola_set_component_properties_for_all_configurations(${CCOLA_COMPONENT_NAME} LIBRARY_OUTPUT_DIRECTORY ${CCOLA_IMPORT_LIBRARY_INSTALL_DIR})
ccola_set_component_properties_for_all_configurations(${CCOLA_COMPONENT_NAME} PDB_OUTPUT_DIRECTORY ${CCOLA_PDB_INSTALL_DIR})
ccola_set_component_properties_with_generator_expression(${CCOLA_COMPONENT_NAME} LINK_LIBRARIES ${CCOLA_COMPONENT_LINK_LIBRARIES})

ccola_runtime_directories(${CCOLA_DLL_INSTALL_DIR})
ccola_inheritable_link_libraries(${CCOLA_IMPORT_LIBRARY_INSTALL_DIR}/${CMAKE_IMPORT_LIBRARY_PREFIX}${CCOLA_COMPONENT_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX})

include("${CCOLA_PACK_DIR_cpp}/components/generic_cpp/end_after.inc")
