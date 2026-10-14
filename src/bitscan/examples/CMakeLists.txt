###################
# BITSCAN lib - Example targets
###################

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

add_executable(bitset_core
    bitset_core_api.cpp
)

target_link_libraries(bitset_core
    PRIVATE bitscan
)

# Keep the new introductory examples together in their own output directory.
set_target_properties(
	bitscan_basic
	bitscan_destructive
	bitscan_views
	bitset_core
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples/BITSCAN"
)

