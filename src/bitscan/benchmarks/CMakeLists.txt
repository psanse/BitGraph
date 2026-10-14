###################
# BITSCAN lib - Benchmark targets
# (not registered with CTest; run manually, preferably in Release)
###################

add_executable(
    bench_bitset
    bench_bitset.cpp
)

set_target_properties(
    bench_bitset
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks"
)

target_link_libraries(
    bench_bitset
    PRIVATE
        bitscan
)

add_executable(
    bench_stl
    bench_stl.cpp
)

set_target_properties(
    bench_stl
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks"
)

target_link_libraries(
    bench_stl
    PRIVATE
        bitscan
)

add_executable(
    bench_masking
    bench_masking.cpp
)

set_target_properties(
    bench_masking
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks"
)

target_link_libraries(
    bench_masking
    PRIVATE
        bitscan
)

add_executable(
    bench_hotpaths
    bench_hotpaths.cpp
)

set_target_properties(
    bench_hotpaths
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks"
)

target_link_libraries(
    bench_hotpaths
    PRIVATE
        bitscan
)
