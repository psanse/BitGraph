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
