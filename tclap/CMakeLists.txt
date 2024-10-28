# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
#add_library (bitscan bbsentinel.cpp  bitboard.cpp  bitboardn.cpp  bitboards.cpp  tables.cpp)

# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

