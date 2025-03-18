#########################
#headers for UTILS lib
#########################
set(useful_headers_for_IDE 
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/prec_timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/info/info_base.h
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
    ${useful_headers_for_IDE}
)

# Make sure the compiler can find include files for our utils library
# when other libraries or executables link to bitscan

#target_include_directories (utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(utils PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/utils>    # <prefix>/include/mylib
)

set_target_properties(utils 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    #RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    #PUBLIC_HEADER   "${useful_headers_for_IDE}"        #does not preserve internal structure       
)

#############
# Install UTILS lib
# UTILS lib header files are deployed manually
# (to preserve internal structure)
#############

install(TARGETS utils
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	#RUNTIME DESTINATION bin
	#PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/utils        //header files deployed without internal structure
)

 file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/utils/*.h")
 foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    file(RELATIVE_PATH REL_PATH ${PROJECT_SOURCE_DIR} ${ITEM})
    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
    install(FILES ${ITEM} DESTINATION include/${DIR_PATH})	 
 endforeach()

#############
# UNIT TESTS for UTILS lib
#############
option(utils_build_tests "Build utils tests." ON)
if (utils_build_tests)

	add_subdirectory(tests)

endif()

