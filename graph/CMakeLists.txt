# Create a library called "Hello" which includes the source file "hello.cxGRBx".
# The extension is already found. Any number of sources could be listed here.
set(useful_headers_for_IDE
graph.h
simple_graph.h
simple_sparse_graph.h
simple_sparse_ugraph.h
simple_ugraph.h
simple_graph_ew.h
simple_graph_w.h
algorithms/kcore.h
algorithms/graph_gen.h
#algorithms/graph_sort.h
algorithms/graph_map.h
algorithms/graph_func.h
algorithms/graph_fast_sort.h
algorithms/graph_fast_sort_weighted.h
)

###################
# GRAPH lib
###################
add_library  (graph
simple_graph.cpp
simple_ugraph.cpp
simple_graph_w.cpp
simple_graph_ew.cpp
formats/mmio.cpp
algorithms/decode.cpp
${useful_headers_for_IDE}
)

set_target_properties(graph 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)


# Make sure the compiler can find include files for our bitscan library
# when other libraries or executables link to bitscan
#target_include_directories (graph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(graph PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/graph>    # <prefix>/include/bitscan
)

###################
# GRAPH lib - UNIT TESTS
###################
option(graph_build_tests "Build graph tests." OFF)
if (graph_build_tests)

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable ( test_graph  tests/gtest_main.cpp

#  TESTS CHECKED  (26/01/2025)
  tests/test_kcore.cpp
  tests/test_func.cpp
  tests/test_graph.cpp
  tests/test_ugraph.cpp
  tests/test_wgraph.cpp
  tests/test_wegraph.cpp
  tests/test_sparse_graph.cpp 
  tests/test_wegraph.cpp
  tests/test_graph_fast_sort.cpp
  tests/test_graph_fast_sort_weighted.cpp
  tests/test_decode.cpp
  tests/test_random_graph.cpp
  tests/test_graph_conversions.cpp
  tests/test_sparse_graph.cpp
  tests/test_format.cpp 
  tests/test_graph_map.cpp 

#  TESTS TO BE CHECKED
#  tests/test_graph_sort.cpp  

)

target_include_directories (test_graph PUBLIC ../googletest/include/)
# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
target_link_libraries (test_graph LINK_PUBLIC graph bitscan utils gtest)

endif()

###################
# GRAPH lib - Example targets
###################
option(graph_build_examples "Build graph examples." OFF)
if (graph_build_examples)

add_executable ( gen_random_benchmark ./examples/gen_random_bench.cpp)
target_link_libraries (gen_random_benchmark LINK_PUBLIC graph bitscan utils)
add_executable ( graph_formats ./examples/graph_formats.cpp)
target_link_libraries (graph_formats LINK_PUBLIC graph bitscan utils)
add_executable ( lb_kcore ./examples/lb_kcore.cpp)
target_link_libraries (lb_kcore LINK_PUBLIC graph bitscan utils)


set_target_properties( gen_random_benchmark graph_formats lb_kcore
		PROPERTIES
	#    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	#    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


endif()


