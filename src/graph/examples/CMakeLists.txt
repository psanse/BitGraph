###################
# GRAPH lib - Example targets
###################

# Small introductory example for the dense/sparse and directed/undirected
# unweighted graph representations.  Existing examples above are kept
# unchanged because they serve different, more specialized purposes.
add_executable ( graph_types_simple graph_types_simple.cpp)
target_link_libraries (graph_types_simple LINK_PUBLIC graph bitscan utils)

# Small introductory example for named graph factories and RandomGen.
add_executable ( graph_generation_simple graph_generation_simple.cpp)
target_link_libraries (graph_generation_simple LINK_PUBLIC graph bitscan utils)

# Advanced example of G(n,p) random graph generation and isomorphism.
add_executable ( graph_generation_advanced graph_generation_advanced.cpp)
target_link_libraries (graph_generation_advanced LINK_PUBLIC graph bitscan utils)

# Small introductory example for degree and degeneracy vertex orderings.
add_executable ( graph_sorting_simple graph_sorting_simple.cpp)
target_link_libraries (graph_sorting_simple LINK_PUBLIC graph bitscan utils)

# Small introductory example for vertex-weighted and edge-weighted graphs.
add_executable ( graph_weighted_simple graph_weighted_simple.cpp)
target_link_libraries (graph_weighted_simple LINK_PUBLIC graph bitscan utils)

# Small introductory example for reading DIMACS, Matrix Market, and edge-list files.
add_executable ( graph_reading_simple graph_reading_simple.cpp)
target_link_libraries (graph_reading_simple LINK_PUBLIC graph bitscan utils)

add_executable ( graph_formats graph_formats.cpp)
target_link_libraries (graph_formats LINK_PUBLIC graph bitscan utils)

# Keep the new introductory examples together in their own output directory.
set_target_properties(
	graph_types_simple
	graph_generation_simple
	graph_generation_advanced
	graph_sorting_simple
	graph_weighted_simple
	graph_reading_simple
	graph_formats
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples/GRAPH"
)




