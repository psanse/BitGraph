
###################
# GRAPH lib - Example targets
###################


add_executable ( gen_random_benchmark gen_random_bench.cpp)
target_link_libraries (gen_random_benchmark LINK_PUBLIC graph bitscan utils)

add_executable ( graph_formats graph_formats.cpp)
target_link_libraries (graph_formats LINK_PUBLIC graph bitscan utils)

add_executable ( kcore lb_kcore.cpp)
target_link_libraries ( kcore LINK_PUBLIC graph bitscan utils)

set_target_properties( gen_random_benchmark graph_formats kcore
		PROPERTIES
	#    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	#    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)




