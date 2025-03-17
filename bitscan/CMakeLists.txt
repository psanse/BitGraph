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
    #RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
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

#message("CMAKE_BINARY_DIR: '${CMAKE_BINARY_DIR}'")
#message("CMAKE_CURRENT_SOURCE_DIR: '${CMAKE_CURRENT_SOURCE_DIR}'")
#message("CMAKE_SOURCE_DIR: '${CMAKE_SOURCE_DIR}'")

#############
# Install BITSCAN lib
# BITSCAN lib header files are deployed manually
# (to preserve internal structure)
#############

#Install BITSCAN library
install(TARGETS bitscan
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib
   #RUNTIME DESTINATION bin
   #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/bitscan        //header files deployed without internal structure
)

file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/bitscan/*.h")
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    file(RELATIVE_PATH REL_PATH ${PROJECT_SOURCE_DIR} ${ITEM})
    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
    install(FILES ${ITEM} DESTINATION include/${DIR_PATH})	 
 endforeach()

###################
# UNIT TESTS for BITSCAN lib  
###################
option(bitscan_build_tests "Build bitscan tests." ON)
if (bitscan_build_tests)

	add_subdirectory(tests)

endif()


