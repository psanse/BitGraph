# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
add_library (bitscan bbsentinel.cpp  bitboard.cpp  bitboardn.cpp  bitboards.cpp  tables.cpp)
set_target_properties(bitscan 
    PROPERTIES
     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
target_include_directories (bitscan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

###################
# BITSCAN lib - UNIT TESTS
###################
option(bitscan_build_tests "Build bitscan tests." OFF)
if (bitscan_build_tests)

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (test_bitscan tests/gtest_main.cc 

    #tests/test_bitstrings.cpp 
    #tests/test_algorithms.cpp 
    #tests/test_masking.cpp 
    #tests/test_sparse.cpp
    #tests/test_example.cpp
    #tests/test_sentinels.cpp


    #TESTS CHECKED (31/01/2025)
    tests/test_bitblock.cpp

)

target_include_directories (test_bitscan PUBLIC ../googletest/include/)
# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
target_link_libraries (test_bitscan LINK_PUBLIC bitscan gtest utils)


endif()


