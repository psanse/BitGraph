###################
# BITSCAN lib - Unit test targets
###################

add_executable(
    test_bitscan

    # Tests checked on 26/02/2025.
    test_bitblock.cpp
    test_masks.cpp
    test_bbscan_nested.cpp
    test_bbscan.cpp
    test_bitset.cpp
    test_bbscan_sparse.cpp
    test_bitset_sparse.cpp
    test_bbscan_sparse_nested.cpp
    test_bbscan_view.cpp

    # Tests to be checked:
    # test_algorithms.cpp
    # test_example.cpp
    # test_bbsentinel.cpp
)

set_target_properties(
    test_bitscan
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

target_link_libraries(
    test_bitscan
    PRIVATE
        bitscan
        GTest::gtest_main
)


# Register each GoogleTest case individually with CTest and Visual Studio
# Test Explorer.
include(GoogleTest)
gtest_discover_tests(test_bitscan)






