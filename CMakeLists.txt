cmake_minimum_required (VERSION 2.8...3.14)  	

if(${CMAKE_VERSION} VERSION_GREATER "3.24.0") 
   cmake_policy(SET CMP0135 NEW) 
endif()
	
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install CACHE PATH "prefix directory to install BitGraph - default set to a local dir in the package which includes a linking example")	

#C++14 required for BitGraph and googletest
set(CMAKE_CXX_STANDARD 14)					
set(CMAKE_CXX_STANDARD_REQUIRED ON)		
set(CMAKE_CXX_EXTENSIONS OFF)

############
# project 
###########

set(LIBNAME "BitGraph")
set(LIB_MAJOR_VERS "1")
set(LIB_MINOR_VERS "4")
set(LIB_PATCH_VERS "3")

project(${LIBNAME} 
    VERSION  ${LIB_MAJOR_VERS}.${LIB_MINOR_VERS}.${LIB_PATCH_VERS}
	LANGUAGES CXX C	)
	

#################
# Introduce variables for export:
# * CMAKE_INSTALL_LIBDIR		#lib
# * CMAKE_INSTALL_BINDIR		#bin
# * CMAKE_INSTALL_INCLUDEDIR	#include
include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake"
)

#################
# UNIT TESTS (using googletest) 

# Tests and examples are enabled by default only when BitGraph is the
# top-level project.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(BITGRAPH_STANDALONE ON)
else()
    set(BITGRAPH_STANDALONE OFF)
endif()

option(
    BUILD_TESTING
    "Build BitGraph tests"
    ${BITGRAPH_STANDALONE}
)

option(
    BUILD_EXAMPLES
    "Build BitGraph examples"
    ${BITGRAPH_STANDALONE}
)


# Defines BUILD_TESTING, when not already defined, and calls
# enable_testing() when (standard) BUILD_TESTING is enabled.

if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(googletest)

    message(STATUS "Building tests with GoogleTest framework")
endif()


# if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)	
	# #Root dir - BitGraph project, build tests									
	# SET(BUILD_TESTS ON CACHE BOOL "builds tests for BitGraph (uses googletest)" FORCE)
	# SET(BUILD_EXAMPLES ON CACHE BOOL "builds examples for BitGraph " FORCE)
	# #SET(INSTALL_BITGRAPH ON CACHE BOOL "installs BitGraph if required" FORCE)

# else()
	# #not Root dir - Project embedding BitGraph, building tests / examples and installing BitGraph is not enabled
	# SET(BUILD_TESTS OFF CACHE BOOL "builds tests for BitGraph (uses googletest)" FORCE)
	# SET(BUILD_EXAMPLES OFF CACHE BOOL "builds examples for BitGraph " FORCE)
	# #SET(INSTALL_BITGRAPH OFF CACHE BOOL "installs BitGraph if required" FORCE)
# endif()


###################
# INSTALL BitGraph (find_package, config mode)
option(INSTALL_BITGRAPH "Enable installation of BitGraph. (Projects embedding BitGraph may want to turn this OFF.)" ON)

###################
# INSTALL googletest - default off
option(INSTALL_GTEST "Enable installation of googletest (disabled by default)" OFF)

#############
# OpenMP (Windows)
option(INCLUDE_OPENMP "Makes OpenMP available (only for Windows platforms)" OFF)

#############
# APPLE options  - extend to other SO
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Set architectures for Apple platforms (x86_64;arm64)" FORCE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Set minimum deployment target for Apple platforms (10.15)" FORCE)
    set(CMAKE_PREFIX_PATH "/usr/local/homebrew")
    #add_compile_options(-arch arm64)
endif (APPLE)


####################
# Subdirectories 
####################
add_subdirectory (src)
