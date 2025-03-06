cmake_minimum_required (VERSION 2.8...3.4)  		#version 2.8 for VENUS server
project (bitscan_graph_libs CXX)

#############
# C++11 flag
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

##################
# specific flags  
###################

CHECK_CXX_COMPILER_FLAG("-fpermissive" COMPILER_SUPPORTS_FPERMISSIVE)
if(COMPILER_SUPPORTS_FPERMISSIVE)
    add_compile_options("-fpermissive")
endif()

if(WIN32)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
endif()

#############
# USEFUL GLOBAL DEFINITIONS
# (use inside the code)
#############

set(PATH_GRAPH_TESTS_CMAKE_GUI ${CMAKE_SOURCE_DIR}/graph/data CACHE PATH "PATH for GRAPH unit tests")
add_definitions (-DPATH_GRAPH_TESTS_CMAKE_SRC_CODE=\"${PATH_GRAPH_TESTS_CMAKE_GUI}/\") 

set(PATH_COPT_TESTS_CMAKE_GUI ${CMAKE_SOURCE_DIR}/copt/data CACHE PATH "PATH for COPT unit tests")
add_definitions (-DPATH_COPT_TESTS_CMAKE_SRC_CODE=\"${PATH_COPT_TESTS_CMAKE_GUI}/\") 

#old definitions - deprecated
#add_definitions (-D_PATH_FOR_COPT_TESTS_DATA_IN_SRC_CODE=\"${CMAKE_SOURCE_DIR}/copt/data/\") 
#add_definitions (-D_PATH_FOR_GRAPH_TESTS_DATA_IN_SRC_CODE=\"${CMAKE_SOURCE_DIR}/graph/data/\") 

##############
# LOGGER LEVEL (see utils/logger.h) configurable from CMake at run time
# comments: there is no checking of the value if it is not set from the CMake GUI
##############

set(LOG_LEVEL "debug" CACHE STRING "LOG_LEVEL chosen in CMake at run time")
set_property(CACHE LOG_LEVEL PROPERTY STRINGS debug verbose error)
message("LOGGER_LEVEL='${LOG_LEVEL}'")

if( ${LOG_LEVEL} STREQUAL "debug")
    add_definitions(-DLOGGER_DEBUG_LEVEL)				#all messages (logger.h) 
endif()

if( ${LOG_LEVEL}  STREQUAL "verbose")
    add_definitions(-DLOGGER_VERBOSE_LEVEL)				#all messages (logger.h)  	
endif()

if( ${LOG_LEVEL}  STREQUAL "error")
    add_definitions(-DLOGGER_ERROR_LEVEL)				#all messages (logger.h)  	
endif()

#############
# OpenMP (Windows)
# comments: found nothing better than to set it globally for all targets
#############

if (WIN32)
#find_package(OpenMP)
#IF (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -D_VARIADIC_MAX=10")
#endif()
endif (WIN32)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    set(CMAKE_PREFIX_PATH "/usr/local/homebrew")
    #add_compile_options(-arch arm64)
endif (APPLE)


#########################################
# Subdirectories for the different libs
#########################################

include_directories (PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (googletest)       #TODO - import some other way
add_subdirectory (utils)
add_subdirectory (bitscan)
add_subdirectory (tclap)            #TODO - possibly remove
add_subdirectory (graph)

#####################################
# Set install prefix path for all libs
######################################

set(INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/example_external_linking" CACHE PATH "Directory chosen to deploy project")
set(CMAKE_INSTALL_PREFIX ${INSTALL_PATH})

 



#####################################################
# DEPRECATED CODE
####################################################

####################
# deploy all header files (inlcuding gtest) in include directory preserving internal structure
# (old CMake)
###################

#file(GLOB_RECURSE DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/*.h")
# foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
#    file(RELATIVE_PATH REL_PATH ${PROJECT_SOURCE_DIR} ${ITEM})
#    get_filename_component(DIR_PATH ${REL_PATH} DIRECTORY)
#    install(FILES ${ITEM} DESTINATION include/${DIR_PATH})	 
# endforeach()

#############
#
# Deploy UTILS,  BITSCAN and GRAPH libraries with EXPORT
# (using modern CMake)
#
#############

# Install utils library
#install(TARGETS utils bitscan graph EXPORT allConfig)

#export(TARGETS utils bitscan graph
#    #NAMESPACE allConfig::
#    FILE "${CMAKE_BINARY_DIR}/allConfig.cmake"
#)

#set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/example)

#install(EXPORT allConfig
#    DESTINATION "${CMAKE_INSTALL_PREFIX}/allConfig/cmake"
#   #NAMESPACE allConfig::
#)



