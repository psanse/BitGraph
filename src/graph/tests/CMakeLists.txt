###################
# GRAPH lib - Example targets
###################

add_executable (test_graph 

 # gtest_main.cpp
  

#  TESTS CHECKED  (26/01/2025)
  test_kcore.cpp
  test_func.cpp
  test_graph.cpp
  test_ugraph.cpp
  test_wgraph.cpp
  test_wegraph.cpp
  test_sparse_graph.cpp 
  test_wegraph.cpp
  test_graph_fast_sort.cpp
  test_graph_fast_sort_weighted.cpp
  test_decode.cpp
  test_random_graph.cpp
  test_graph_conversions.cpp
  test_sparse_graph.cpp
  test_format.cpp 
  test_graph_map.cpp 
  test_clq_func.cpp

#  TESTS TO BE CHECKED
    
#  tests/test_graph_sort.cpp  

)

set_target_properties(test_graph 
    PROPERTIES
    #ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    #LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
   
)

#target_include_directories (test_graph PUBLIC ../googletest/include/)

target_link_libraries (test_graph LINK_PUBLIC graph bitscan utils gtest_main)





