###################
# BITSCAN lib - Unit test targets
###################

add_executable (test_bitscan 

	#gtest_main.cc 
  
    #TESTS TO CHECK (26/02/2025)   
    test_algorithms.cpp   
    #test_example.cpp   
    #test_bbsentinel.cpp 

    #TESTS CHECKED (26/02/2025)
    test_bitblock.cpp
    test_masks.cpp 
    test_bbscan_nested.cpp 
     test_bbscan.cpp 
    test_bitset.cpp 
    test_bbscan_sparse.cpp 
    test_bitset_sparse.cpp
    test_bbscan_sparse_nested.cpp 

    test_bbexpr.cpp
)

set_target_properties(test_bitscan 
    PROPERTIES
    #ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    #LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

#target_include_directories (test_bitscan PRIVATE ../googletest/include/)

target_link_libraries (test_bitscan LINK_PRIVATE bitscan gtest_main utils)





