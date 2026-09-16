# Output directory for BITSCAN example executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin/examples/BITSCAN"
)

add_executable(bitscan_basic
    basic_scanning.cpp
)

target_link_libraries(bitscan_basic
    PRIVATE bitscan
)

add_executable(bitscan_destructive
    basic_destructive_scanning.cpp
)

target_link_libraries(bitscan_destructive
    PRIVATE bitscan
)

add_executable(bitscan_views
    independent_views.cpp
)

target_link_libraries(bitscan_views
    PRIVATE bitscan
)
