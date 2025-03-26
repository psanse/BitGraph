cmake_minimum_required (VERSION 2.8...3.15)
project (main LANGUAGES CXX C)

find_package(BitGraph CONFIG REQUIRED) 

#######################
# target graph_example
#######################

add_executable(graph_example src/graph.cpp)

target_link_libraries (graph_example LINK_PUBLIC 
	BitGraph::graph
	BitGraph::bitscan
	BitGraph::utils				)

set_target_properties(graph_example
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
