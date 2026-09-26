###################
# GRAPH lib - Example targets
###################

add_executable (
  test_graph 

  # TESTS CHECKED  (last 29/11/2025)
  test_func.cpp
  test_graph.cpp
  test_ugraph.cpp
  test_wgraph.cpp
  test_wegraph.cpp
  test_graph_fast_sort.cpp
  test_graph_fast_sort_weighted.cpp
  test_random_graph.cpp
  test_graph_conversions.cpp
  test_sparse_graph.cpp
  test_format.cpp 
  test_decode.cpp
  test_clq_func.cpp
  test_graph_map.cpp
  test_graph_benchmark.cpp
  test_algorithm_info.cpp

  # Tests to be checked:
  # test_kcore.cpp             # to fix (26/09/2026)
  # test_graph_sort.cpp        # deprecated class 
)

target_compile_definitions(
    test_graph
    PRIVATE
        PATH_GRAPH_TESTS_CMAKE_SRC_CODE="${GRAPH_TEST_DATA_DIR}/"
        BITGRAPH_GRAPH_TEST_DATA_DIR="${GRAPH_TEST_DATA_DIR}/"
)

set_target_properties(
    test_graph
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

target_link_libraries(
    test_graph
    PRIVATE
        graph
        GTest::gtest_main
)

# Register each GoogleTest case individually with CTest and Visual Studio
# Test Explorer.
include(GoogleTest)
gtest_discover_tests(test_graph)






