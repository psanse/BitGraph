#########################
#headers for BITSCAN lib
#########################
set(useful_headers_for_IDE
 ${CMAKE_CURRENT_SOURCE_DIR}/bbconfig.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbtypes.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bitblock.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbset.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbset_sparse.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbscan.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbscan_sparse.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbsentinel.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbobject.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bbalgorithm.h
 ${CMAKE_CURRENT_SOURCE_DIR}/bitscan.h
)

###################
#creates BITSCAN lib
###################
add_library (bitscan STATIC
bbsentinel.cpp 
bitblock.cpp 
bbset.cpp  
bbset_sparse.cpp 
tables.cpp
${useful_headers_for_IDE}
)

set_target_properties(bitscan 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    #PUBLIC_HEADER   "${useful_headers_for_IDE}"        #does not preserve internal structure  
)


# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
#target_include_directories (bitscan 
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#)

# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
target_include_directories(bitscan PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/bitscan>    # <prefix>/include/bitscan
)

#############
# Install BITSCAN lib
# BITSCAN lib header files are deployed manually
# to preserve internal structure
#############

# Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "lib/cmake/bitscan")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

 # Configuration
set(version_config "${generated_dir}/bitscanConfigVersion.cmake")
set(project_config "${generated_dir}/bitscanConfig.cmake")
set(TARGETS_EXPORT_NAME "bitscanTargets")
set(namespace "${PROJECT_NAME}::")

#Install BITSCAN library
install(
   TARGETS bitscan
   EXPORT  "${TARGETS_EXPORT_NAME}"
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib
   RUNTIME DESTINATION bin
   INCLUDES DESTINATION include
   #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/bitscan        //header files deployed without internal structure
)

#install header files
file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/bitscan/*.h")
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
    "cmake/bitscanConfig.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)


###################
# UNIT TESTS for BITSCAN lib  
###################
#option(bitscan_build_tests "Build bitscan tests." ON)
if (BUILD_TESTS)

	add_subdirectory(tests)

endif()


