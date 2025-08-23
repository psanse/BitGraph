#########################
#headers for BITSCAN lib
#########################

file(GLOB HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

###################
#creates BITSCAN lib
###################
add_library (bitscan STATIC
bbsentinel.cpp 
bitblock.cpp 
bbset.cpp  
bbset_sparse.cpp 
tables.cpp
${HEADER_FILES}
)

set_target_properties(bitscan 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    #PUBLIC_HEADER   "${HEADER_FILES}"        #does not preserve internal structure  
)

# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
target_include_directories(bitscan PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${include_install_dir}>    # <prefix>/include/<PROJECT_NAME>
)

#############
# Install BITSCAN lib
# BITSCAN lib header files are deployed manually
# to preserve internal structure
#############

if(INSTALL_BITGRAPH)

#Install BITSCAN library
install(
   TARGETS bitscan
   EXPORT  "${TARGETS_EXPORT_NAME}"
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib
   RUNTIME DESTINATION bin
   INCLUDES DESTINATION include
)

#install header files
file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${source_code_dir}/bitscan/*.h")
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    file(RELATIVE_PATH REL_PATH ${source_code_dir} ${ITEM})
    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
    install(FILES ${ITEM} DESTINATION ${include_install_dir}/${DIR_PATH})	 
 endforeach()

endif(INSTALL_BITGRAPH)

###################
# UNIT TESTS for BITSCAN lib  
###################
#option(bitscan_build_tests "Build bitscan tests." ON)
if (BUILD_TESTS)

	add_subdirectory(tests)

endif()


