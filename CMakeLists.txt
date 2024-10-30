#cmake_minimum_required (VERSION 3.4)
cmake_minimum_required (VERSION 2.8)   #for VENUS server
project (clique)

#############
#
# c++11 flag
#
#############
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

CHECK_CXX_COMPILER_FLAG("-fpermissive" COMPILER_SUPPORTS_FPERMISSIVE)
if(COMPILER_SUPPORTS_FPERMISSIVE)
    add_compile_options("-fpermissive")
endif()

#############
#
# USEFUL GLOBAL DEFINITIONS
# (use inside the code)
#############
add_definitions (-D_PATH_FOR_COPT_TESTS_DATA_IN_SRC_CODE=\"${CMAKE_SOURCE_DIR}/copt/data\") 
add_definitions (-D_PATH_FOR_GRAPH_TESTS_DATA_IN_SRC_CODE=\"${CMAKE_SOURCE_DIR}/graph/data\") 

#############
#
# OpenMP (VSTUDIO)
# For VSTUDIO found nothing better than to set it globally for all targets
#############

if (WIN32)
#find_package(OpenMP)
#IF (OPENMP_FOUND)
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -D_VARIADIC_MAX=10")
#endif()
endif (WIN32)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    set(CMAKE_PREFIX_PATH "/usr/local/homebrew")
    #add_compile_options(-arch arm64)
endif (APPLE)

include_directories (PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (googletest)
add_subdirectory (utils)
add_subdirectory (bitscan)
add_subdirectory (tclap)
add_subdirectory (graph)
 
if(WIN32)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
endif()

# add_library(copt_full SHARED copt graph bitscan utils)
# set_target_properties(copt_full PROPERTIES LINKER_LANGUAGE CXX)
# install(TARGETS copt_full DESTINATION lib)
                                   
# file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/*.h")
# foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
	# get_filename_component(absdir ${ITEM} DIRECTORY)
	  # file(RELATIVE_PATH dir ${PROJECT_SOURCE_DIR} ${absdir})
      # install( FILES ${ITEM} DESTINATION include/${dir})
# endforeach()
