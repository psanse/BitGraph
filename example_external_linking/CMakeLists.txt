cmake_minimum_required (VERSION 3.4)
project (examples_GRAPH_BITSCAN)

############
# c++11 flag
############

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#######################
# target graph_example
#######################

add_executable(graph_example graph.cpp)

target_include_directories(graph_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (graph_example LINK_PUBLIC graph bitscan utils)
link_directories (graph_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

####################
# target bitset
####################

add_executable(bitset_example bitstrings.cpp)

target_include_directories(bitset_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (bitset_example LINK_PUBLIC bitscan utils)
link_directories (bitset_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)