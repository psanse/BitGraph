cmake_minimum_required (VERSION 3.4)
project (examples_GRAPH_BITSCAN)

add_executable(graph_example graph.cpp)

target_include_directories(graph_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (graph_example LINK_PUBLIC graph bitscan utils)
target_link_directories (graph_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)


add_executable(bitset_example bitstrings.cpp)

target_include_directories(bitset_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (bitset_example LINK_PUBLIC bitscan utils)
target_link_directories (bitset_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)