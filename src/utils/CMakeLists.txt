#########################
#headers for UTILS lib
#########################

file(GLOB HEADER_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/*.h 
                                ${CMAKE_CURRENT_SOURCE_DIR}/info/*.h
)

#creates utils library
add_library (utils STATIC
    benchmark.cpp 
    result.cpp
    test_analyser.cpp
    info_analyser.cpp
    common.cpp
    common_types.cpp
    info/info_base.cpp
    info/info_clq.cpp                                   #move to clq algorithms and remove from here
    #logger.cpp 
    ${HEADER_FILES}
)

##############
# LOGGER LEVEL - default level debug
# (utils/logger.h) 

set(LOGGER_LEVEL "debug" CACHE STRING "Logger level used at run time")
set_property(CACHE LOGGER_LEVEL PROPERTY STRINGS debug verbose error)
message(STATUS "LOGGER_LEVEL is '${LOGGER_LEVEL}'")

#################
# Make sure the compiler can find include files for our utils library
# when other libraries or executables link to bitscan

target_include_directories(utils PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>    									# <prefix>/include/<PROJECT_NAME>/mylib
)

target_compile_definitions(utils
    PUBLIC
    $<$<STREQUAL:${LOGGER_LEVEL},debug>:LOGGER_DEBUG_LEVEL>         #all messages (logger.h)
    $<$<STREQUAL:${LOGGER_LEVEL},verbose>:LOGGER_VERBOSE_LEVEL>     #all except debug (logger.h)                     
    $<$<STREQUAL:${LOGGER_LEVEL},error>:LOGGER_ERROR_LEVEL>         #warning and error (logger.h)
)

set_target_properties(utils 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    #PUBLIC_HEADER   "${HEADER_FILES}"        #does not preserve internal structure       
)

#############
# Install UTILS lib
# UTILS lib header files are deployed manually
# (to preserve internal structure)
#############

if(INSTALL_BITGRAPH)

#Install UTILS library
install(
   TARGETS utils
   EXPORT  "${TARGETS_EXPORT_NAME}"
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib
   RUNTIME DESTINATION bin
   INCLUDES DESTINATION "${include_install_dir}"
)

#include header files
file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${source_code_dir}/utils/*.h")
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    file(RELATIVE_PATH REL_PATH ${source_code_dir} ${ITEM})
    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
    install(FILES ${ITEM} DESTINATION ${include_install_dir}/${DIR_PATH})	 
 endforeach()
 
endif(INSTALL_BITGRAPH)


 

#############
# UNIT TESTS for UTILS lib
#############
#option(utils_build_tests "Build utils tests." ON)
if (BUILD_TESTS)

	add_subdirectory(tests)

endif()

