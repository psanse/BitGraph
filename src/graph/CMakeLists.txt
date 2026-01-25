#########################
#headers for GRAPH lib
########################
file(GLOB HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.h 
                        ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/*.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/clique/*.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/formats/*.h
)

#removed this file, currently deprecated
list(REMOVE_ITEM HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_sort.h) 

###################
# GRAPH lib
###################
add_library  (graph STATIC
simple_graph_w.cpp
simple_graph_ew.cpp
formats/mmio.cpp
algorithms/decode.cpp
${HEADER_FILES}
)

set_target_properties(graph PROPERTIES
    CMAKE_CXX_STANDARD 14					
    CMAKE_CXX_STANDARD_REQUIRED ON		
    CMAKE_CXX_EXTENSIONS OFF
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	#PUBLIC_HEADER   "${HEADER_FILES}"        #does not preserve internal structure 
)

# Make sure the compiler can find include files for GRAPH
# when other libraries or executables link to it
target_include_directories(graph PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>    # <prefix>/include/<PROJECT_NAME>/graph
)

# Enable compiler warnings.
if (MSVC)
    # not enabled because this is not a tested target
    target_compile_options(graph PRIVATE /W4)
else()
    target_compile_options(graph PRIVATE -Wall -Wextra -Wpedantic)
endif()

###################
# UNIT TESTS for GRAPH lib
###################
#option(graph_build_tests "Build graph tests." ON)
if (BUILD_TESTS)

    add_subdirectory(tests)

endif()

###################
# GRAPH lib - Example targets
###################
#option(graph_build_examples "Build graph examples." ON)
if (BUILD_EXAMPLES)

	add_subdirectory(examples)

endif()

#############
# Install GRAPH lib
# GRAPH lib header files are deployed manually
# (to preserve internal structure)
#############

if(INSTALL_BITGRAPH)

#Install GRAPH library
install(
   TARGETS graph
   EXPORT  "${TARGETS_EXPORT_NAME}"
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib
   RUNTIME DESTINATION bin
   INCLUDES DESTINATION "${include_install_dir}"
)

#include header files
file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${source_code_dir}/graph/*.h")
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    file(RELATIVE_PATH REL_PATH ${source_code_dir} ${ITEM})
    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
    install(FILES ${ITEM} DESTINATION ${include_install_dir}/${DIR_PATH})	 
 endforeach()


endif(INSTALL_BITGRAPH)
 
 
 
 






