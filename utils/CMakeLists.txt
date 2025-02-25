#########################
#headers for UTILS lib
#########################
set(useful_headers_for_IDE 
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/prec_timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/info/info_base.h
)

#creates utils library
add_library (utils
    benchmark.cpp 
    result.cpp
    test_analyser.cpp
    info_analyser.cpp
    common.cpp
    common_types.cpp
    info/info_base.cpp
    info/info_clq.cpp                                   #move to clq algorithms and remove from here
    #logger.cpp 
    ${useful_headers_for_IDE}
)

# Make sure the compiler can find include files for our utils library
# when other libraries or executables link to bitscan

#target_include_directories (utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(utils PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/utils>    # <prefix>/include/mylib
)

set_target_properties(utils 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    #PUBLIC_HEADER   "${useful_headers_for_IDE}"        #does not preserve internal structure       
)

option(utils_build_tests "Build utils tests." OFF)
#set(utils_build_tests ON)
if (utils_build_tests)

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (test_utils ./tests/gtest_main.cc

    # TESTS CHECKED  (26/01/2025)
     tests/test_common.cpp
     tests/test_file.cpp    
     tests/test_info.cpp
     tests/test_info_clq.cpp
     tests/test_precise_timer.cpp
     tests/test_thread.cpp
     tests/test_logger.cpp
     tests/test_benchmark.cpp
     tests/test_infoAnalyser.cpp
     tests/test_result.cpp     
     tests/test_batch.cpp 

     #test working but deprecated / or expect to be deprecated
     #tests/test_testAnalyser.cpp
   
)

target_include_directories (test_utils PUBLIC ../googletest/include/)
target_link_libraries (test_utils LINK_PUBLIC utils gtest)

endif()

