###################
# UTILS library - Example targets
###################

add_executable(utils_collections_simple
    utils_collections_simple.cpp
)

target_link_libraries(utils_collections_simple
    PRIVATE utils
)

add_executable(utils_random_simple
    utils_random_simple.cpp
)

target_link_libraries(utils_random_simple
    PRIVATE utils
)

add_executable(utils_time_simple
    utils_time_simple.cpp
)

target_link_libraries(utils_time_simple
    PRIVATE utils
)

add_executable(utils_file_path_simple
    utils_file_path_simple.cpp
)

target_link_libraries(utils_file_path_simple
    PRIVATE utils
)

add_executable(utils_task_simple
    utils_task_simple.cpp
)

target_link_libraries(utils_task_simple
    PRIVATE utils
)

# Keep the UTILS introductory examples together with the other library
# examples while leaving the existing library output directories unchanged.
set_target_properties(
    utils_collections_simple
    utils_random_simple
    utils_time_simple
    utils_file_path_simple
    utils_task_simple
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples/UTILS"
)
