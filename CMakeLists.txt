#cmake_minimum_required (VERSION 2.8...3.14)  	
cmake_minimum_required (VERSION 3.14)  

# Use extraction-time timestamps for downloaded archives when supported.
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

############
# project 
###########

set(LIBNAME "BitGraph")
set(LIB_MAJOR_VERS "1")
set(LIB_MINOR_VERS "4")
set(LIB_PATCH_VERS "3")

project(
    ${LIBNAME} 
    VERSION  ${LIB_MAJOR_VERS}.${LIB_MINOR_VERS}.${LIB_PATCH_VERS}
	LANGUAGES CXX C	
)


#C++14 required for BitGraph and googletest
set(CMAKE_CXX_STANDARD 14)					
set(CMAKE_CXX_STANDARD_REQUIRED ON)		
set(CMAKE_CXX_EXTENSIONS OFF)


#################
# Installation directories
#################

# Use a local installation directory only when BitGraph is the top-level
# project and the user has not explicitly selected another install prefix.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR
   AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(
        CMAKE_INSTALL_PREFIX
        "${CMAKE_SOURCE_DIR}/install"
        CACHE PATH
        "Prefix directory for installing BitGraph"
        FORCE
    )
endif()


# Defines:
# - CMAKE_INSTALL_LIBDIR        #lib
# - CMAKE_INSTALL_BINDIR        #bin
# - CMAKE_INSTALL_INCLUDEDIR    #include
include(GNUInstallDirs)


list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake"
)

#################
# Standalone build
#################

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

#################
# GoogleTest
#################

option(
    INSTALL_GTEST
    "Install GoogleTest with BitGraph"
    OFF
)

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


#################
# BitGraph installation (find_package, config mode)
#################

option(
    INSTALL_BITGRAPH 
    "Enable installation of BitGraph. (Projects embedding BitGraph may want to turn this OFF.)"
    ${BITGRAPH_STANDALONE}
)


#################
# OpenMP
#################

option(
    INCLUDE_OPENMP 
    "Makes OpenMP available (only for Windows platforms)" 
    OFF
)

#################
# Apple configuration
#################

if(APPLE)
    set(
        CMAKE_OSX_ARCHITECTURES
        "x86_64;arm64"
        CACHE STRING
        "Architectures for Apple platforms"
    )

    set(
        CMAKE_OSX_DEPLOYMENT_TARGET
        "10.15"
        CACHE STRING
        "Minimum macOS deployment target"
    )

    list(APPEND CMAKE_PREFIX_PATH
        "/usr/local/homebrew"
    )
endif()


####################
# Subdirectories 
####################
add_subdirectory (src)
