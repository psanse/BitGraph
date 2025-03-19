#########################
#headers for GRAPH lib
########################
set(useful_headers_for_IDE
${CMAKE_CURRENT_SOURCE_DIR}/graph.h
${CMAKE_CURRENT_SOURCE_DIR}/simple_graph.h
${CMAKE_CURRENT_SOURCE_DIR}/simple_ugraph.h
${CMAKE_CURRENT_SOURCE_DIR}/simple_graph_ew.h
${CMAKE_CURRENT_SOURCE_DIR}/simple_graph_w.h
${CMAKE_CURRENT_SOURCE_DIR}/algorithms/kcore.h
${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_gen.h
${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_map.h
${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_func.h
${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_fast_sort.h
${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_fast_sort_weighted.h
#algorithms/graph_sort.h
)

###################
# GRAPH lib
###################
add_library  (graph STATIC
simple_graph_w.cpp
simple_graph_ew.cpp
formats/mmio.cpp
algorithms/decode.cpp
${useful_headers_for_IDE}
)

set_target_properties(graph 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	#PUBLIC_HEADER   "${useful_headers_for_IDE}"        #does not preserve internal structure 
)

# Make sure the compiler can find include files for GRAPH
# when other libraries or executables link to it
target_include_directories(graph PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/graph>    # <prefix>/include/graph
)


###################
# UNIT TESTS for GRAPH lib
###################
#option(graph_build_tests "Build graph tests." ON)
if (BUILD_TESTS)

    add_subdirectory(tests)

endif()

###################
# GRAPH lib - Example targets
###################
option(graph_build_examples "Build graph examples." ON)
if (graph_build_examples)

	add_subdirectory(examples)

endif()

#############
# Install GRAPH lib
# GRAPH lib header files are deployed manually
# (to preserve internal structure)
#############

 # Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "lib/cmake/graph")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

 # Configuration
set(version_config "${generated_dir}/graphConfigVersion.cmake")
set(project_config "${generated_dir}/graphConfig.cmake")
set(TARGETS_EXPORT_NAME "graphTargets")
set(namespace "${PROJECT_NAME}::")


#Install GRAPH library
install(
   TARGETS graph
   EXPORT  "${TARGETS_EXPORT_NAME}"
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib
   RUNTIME DESTINATION bin
   INCLUDES DESTINATION include
)

#install header files
file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/graph/*.h")
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    file(RELATIVE_PATH REL_PATH ${PROJECT_SOURCE_DIR} ${ITEM})
    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
    install(FILES ${ITEM} DESTINATION include/${DIR_PATH})	 
endforeach()
 
 
 # Config
#   * <prefix>/lib/cmake/Foo/FooConfig.cmake
#   * <prefix>/lib/cmake/Foo/FooConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/Foo/FooTargets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)

# Include module with function 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/graphConfig.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)
 






