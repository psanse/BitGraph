###################
# UTILS lib - Unit test targets
###################

add_executable (test_utils gtest_main.cc

    # TESTS CHECKED  (26/01/2025)
     test_common.cpp
     test_file.cpp    
     test_info.cpp
     test_info_clq.cpp
     test_precise_timer.cpp
     test_thread.cpp
     test_logger.cpp
     test_benchmark.cpp
     test_infoAnalyser.cpp
     test_result.cpp     
     test_batch.cpp 

     #test working but deprecated / or expect to be deprecated
     #tests/test_testAnalyser.cpp
   
)

set_target_properties(test_utils 
    PROPERTIES
    #ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    #LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

target_include_directories (test_utils PUBLIC ../googletest/include/)

target_link_libraries (test_utils LINK_PUBLIC utils gtest)



