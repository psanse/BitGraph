#########################
#headers for GRAPH lib
########################
file(GLOB USEFUL_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.h 
                               ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/*.h
)

#removed this file, currently  is deprecated
list(REMOVE_ITEM USEFUL_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/graph_sort.h) 

###################
# GRAPH lib
###################
add_library  (graph STATIC
simple_graph_w.cpp
simple_graph_ew.cpp
formats/mmio.cpp
algorithms/decode.cpp
${USEFUL_HEADER_FILES}
)

set_target_properties(graph 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	#PUBLIC_HEADER   "${USEFUL_HEADER_FILES}"        #does not preserve internal structure 
)

# Make sure the compiler can find include files for GRAPH
# when other libraries or executables link to it
target_include_directories(graph PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>    # <prefix>/include/<PROJECT_NAME>/graph
)

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
 
 
 
 






