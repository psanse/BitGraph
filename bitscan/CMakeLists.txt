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
add_library (bitscan 
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
    #PUBLIC_HEADER   "${useful_headers_for_IDE}"        #does not preserve internal structure  
)

# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
#target_include_directories (bitscan 
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#)

target_include_directories(bitscan PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/bitscan>    # <prefix>/include/bitscan
)

###################
# UNIT TESTS for BITSCAN lib  
###################
option(bitscan_build_tests "Build bitscan tests." OFF)
if (bitscan_build_tests)

add_executable (test_bitscan tests/gtest_main.cc 
  
    #TESTS TO CHECK (14/02/2025)   
    #tests/test_algorithms.cpp   
    #tests/test_example.cpp   
    #tests/test_bbsentinel.cpp 

    #TESTS CHECKED (31/01/2025)
    #tests/test_bitblock.cpp
    #tests/test_masks.cpp 
    #tests/test_bbscan_nested.cpp 
    #tests/test_bbscan.cpp 
    #tests/test_bitset.cpp 
    tests/test_bbscan_sparse.cpp 
    #tests/test_bitset_sparse.cpp
    tests/test_bbscan_sparse_nested.cpp 

)

target_include_directories (test_bitscan PUBLIC ../googletest/include/)

target_link_libraries (test_bitscan LINK_PUBLIC bitscan gtest utils)


endif()


