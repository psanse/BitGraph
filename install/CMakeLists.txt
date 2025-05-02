cmake_minimum_required (VERSION 2.8...3.15)
project (main LANGUAGES CXX C)

find_package(BitGraph CONFIG REQUIRED) 

#######################
# target graph_basics
#######################

add_executable(graph_basics src/graph_basic.cpp)

target_link_libraries (graph_basics LINK_PUBLIC 
	BitGraph::graph
	BitGraph::bitscan
	BitGraph::utils				)

set_target_properties(graph_basics
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" )

#######################
# target graph_sorting
#######################

add_executable(graph_sorting src/graph_sort.cpp)

target_link_libraries (graph_sorting LINK_PUBLIC 
	BitGraph::graph
	BitGraph::bitscan
	BitGraph::utils				)

set_target_properties(graph_sorting
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" )


#######################
# target uninform random graphs
#######################

add_executable(graph_random src/graph_rand.cpp)

target_link_libraries (graph_random LINK_PUBLIC 
	BitGraph::graph
	BitGraph::bitscan
	BitGraph::utils				)

set_target_properties(graph_random
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" )

####################
# target bitset_example
####################

add_executable(bitset_example src/bitstrings.cpp)

target_link_libraries (bitset_example LINK_PUBLIC 
	BitGraph::bitscan 
	BitGraph::utils				)

set_target_properties(bitset_example
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" )
