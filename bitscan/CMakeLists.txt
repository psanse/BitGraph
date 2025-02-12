#set useful headers for IDE
set(useful_headers_for_IDE
bbconfig.h
bbtypes.h
bitblock.h
bbset.h
bitboards.h
bbscan.h
bbintrinsic_sparse.h
bbsentinel.h
bbobject.h
bbscan_external.h
bitscan.h
)

###################
#creates BITSCAN lib
###################
add_library (bitscan 
bbsentinel.cpp 
bitblock.cpp 
bbset.cpp  
bitboards.cpp 
tables.cpp
bbscan_external.cpp
${useful_headers_for_IDE}
)

set_target_properties(bitscan 
    PROPERTIES
     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
target_include_directories (bitscan 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

###################
# UNIT TESTS - BITSCAN lib  
###################
option(bitscan_build_tests "Build bitscan tests." OFF)
if (bitscan_build_tests)

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (test_bitscan tests/gtest_main.cc 

    tests/test_bitstrings.cpp 
    tests/test_bitstring_N.cpp 
    tests/test_algorithms.cpp          #CHECK (05/02/2025)
    
    tests/test_sparse.cpp
    tests/test_example.cpp
    tests/test_sentinels.cpp

    tests/test_masking.cpp 

    tests/test_bitscanning.cpp 

    #TESTS CHECKED (31/01/2025)
    tests/test_bitblock.cpp

)

target_include_directories (test_bitscan PUBLIC ../googletest/include/)
# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
target_link_libraries (test_bitscan LINK_PUBLIC bitscan gtest utils)


endif()


