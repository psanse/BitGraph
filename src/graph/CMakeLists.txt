#########################
# Headers for GRAPH
#########################

file(
    GLOB HEADER_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/algorithms/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/algorithms/clique/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/formats/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/formats/detail/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/detail/*.h"
)

#removed this file, currently deprecated
list(
    REMOVE_ITEM HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_sort.h"
)

###################
# GRAPH lib
###################

add_library  (
    graph STATIC
    simple_graph_w.cpp
    simple_graph_ew.cpp
    formats/detail/mmio.cpp
    algorithms/decode.cpp
    ${HEADER_FILES}
)

set_target_properties(
    graph
    PROPERTIES
        CXX_EXTENSIONS NO
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Public include directories used when building and installing GRAPH.
target_include_directories(
    graph
    PUBLIC
        "$<BUILD_INTERFACE:${BITGRAPH_ROOT_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${include_install_dir}>"
)

# GRAPH depends on BITSCAN and BITSCAN on UTILS (already defined). These dependencies are PUBLIC so their
# usage requirements propagate correctly to GRAPH consumers, although users
# normally link against the umbrella BitGraph target.

target_link_libraries(
    graph
    PUBLIC
        bitscan
)

target_compile_features(
    graph 
    PUBLIC cxx_std_14
)

# Enable compiler warnings.
if (MSVC)
    # not enabled because this is not a tested target
    target_compile_options(graph PRIVATE /W4)
else()
    target_compile_options(graph PRIVATE -Wall -Wextra -Wpedantic)
endif()


#############
# Install GRAPH lib
# (preserves internal structure)
#############

if(INSTALL_BITGRAPH)

    install(
        TARGETS graph
        EXPORT "${TARGETS_EXPORT_NAME}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${include_install_dir}"
    )

    # Installs header files as <prefix>/include/BitGraph/graph/...
    install(
        DIRECTORY "${source_code_dir}/graph"
        DESTINATION "${include_install_dir}"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "graph_sort.h"  EXCLUDE     # currently deprecated, also removed from the list of header files above
        PATTERN "tests"         EXCLUDE
        PATTERN "examples"      EXCLUDE
        PATTERN "data"          EXCLUDE
    )

   ###########
   #deprecated scripts 
   ###########

    ##Install GRAPH library
    #install(
    #   TARGETS graph
    #   EXPORT  "${TARGETS_EXPORT_NAME}"
    #   ARCHIVE DESTINATION lib
    #   LIBRARY DESTINATION lib
    #   RUNTIME DESTINATION bin
    #   INCLUDES DESTINATION "${include_install_dir}"
    #)

    ##include header files
    #file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${source_code_dir}/graph/*.h")
    #foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    #    file(RELATIVE_PATH REL_PATH ${source_code_dir} ${ITEM})
    #    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
    #    install(FILES ${ITEM} DESTINATION ${include_install_dir}/${DIR_PATH})	 
    # endforeach()

endif(INSTALL_BITGRAPH)

###################
# UNIT TESTS for GRAPH lib
###################

set(
    GRAPH_TEST_DATA_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/data"
    # CACHE PATH
    #"Path to GRAPH unit-test data"
)

# Per-target option to enable building unit tests for the graph lib
option(
    GRAPH_BUILD_TESTS 
    "Build bitscan tests."
    ON
)

if (BUILD_TESTING AND GRAPH_BUILD_TESTS )
    add_subdirectory(tests)
endif()

###################
# EXAMPLES for GRAPH lib
###################

# Per-target option to enable building examples for the graph lib target
option(
    GRAPH_BUILD_EXAMPLES
    "Build graph examples."
    ON
)

if (BUILD_EXAMPLES AND GRAPH_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()





