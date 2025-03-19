cmake_minimum_required (VERSION 2.18...3.15)
project (examples_GRAPH_BITSCAN LANGUAGES CXX C)

##############################
# using find_package to locate the BitGraph libraries
find_package(bitscan REQUIRED) 
find_package(graph REQUIRED) 
find_package(utils REQUIRED) 

#common directories for all the targets
#(ideally use target_link_directories for each target, but not available in CMake versions prior 3.13)
#link_directories (PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

#######################
# target graph_example
#######################

add_executable(graph_example graph.cpp)

#target_include_directories(graph_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#target_link_libraries (graph_example LINK_PUBLIC graph bitscan utils)
target_link_libraries (graph_example LINK_PUBLIC BitGraph::graph BitGraph::bitscan BitGraph::utils)


####################
# target bitset_example
####################


add_executable(bitset_example bitstrings.cpp)

#target_include_directories(bitset_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (bitset_example LINK_PUBLIC BitGraph::bitscan BitGraph::utils)