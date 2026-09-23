###################
# UTILS lib - Unit test targets
###################

add_executable(
    test_utils

    # Tests checked on 26/01/2025.
    test_common.cpp
    test_file.cpp
    test_info_base.cpp
    test_precise_timer.cpp
    test_task.cpp
    test_logger.cpp
    #test_benchmark.cpp
    test_batch.cpp
)

set_target_properties(
    test_utils
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

#target_include_directories (test_utils PUBLIC ../googletest/include/)

target_link_libraries (
    test_utils
    PRIVATE 
        utils
        GTest::gtest_main
)

# Import GoogleTest's CMake integration utilities and register each discovered
# GoogleTest case individually with CTest and Visual Studio Test Explorer.
include(GoogleTest)
gtest_discover_tests(test_utils)




