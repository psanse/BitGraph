###################
# BitGraph API reference (Doxygen)
#
# Opt-in. Configure with -DBITGRAPH_BUILD_DOCS=ON, then build the "docs" target:
#     cmake -S . -B build -DBITGRAPH_BUILD_DOCS=ON
#     cmake --build build --target docs
# The HTML is written to <build>/docs/html/index.html.
###################

find_package(Doxygen REQUIRED OPTIONAL_COMPONENTS dot)

option(
    BITGRAPH_DOCS_XML
    "Also generate the Doxygen XML output (for tooling)."
    OFF
)

set(BITGRAPH_DOXYGEN_OUTPUT "${CMAKE_BINARY_DIR}/docs")

if(DOXYGEN_DOT_FOUND)
    set(BITGRAPH_HAVE_DOT YES)
else()
    set(BITGRAPH_HAVE_DOT NO)
endif()

if(BITGRAPH_DOCS_XML)
    set(BITGRAPH_DOXYGEN_XML YES)
else()
    set(BITGRAPH_DOXYGEN_XML NO)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
    "${BITGRAPH_DOXYGEN_OUTPUT}/Doxyfile"
    @ONLY
)

add_custom_target(
    docs
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BITGRAPH_DOXYGEN_OUTPUT}"
    COMMAND ${DOXYGEN_EXECUTABLE} "${BITGRAPH_DOXYGEN_OUTPUT}/Doxyfile"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Generating the BitGraph API reference with Doxygen"
    VERBATIM
)
