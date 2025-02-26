cmake_minimum_required (VERSION 3.4)
project (bitset_graphs)

add_executable(mygraph main.cpp)

target_include_directories(mygraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (mygraph LINK_PUBLIC graph bitscan utils)
target_link_directories (mygraph PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

