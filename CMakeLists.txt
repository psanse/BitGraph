cmake_minimum_required (VERSION 2.8...3.13)  	

if(${CMAKE_VERSION} VERSION_GREATER "3.24.0") 
   cmake_policy(SET CMP0135 NEW) 
endif()
	
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install CACHE PATH "prefix directory to install BitGraph - default set to a local dir in the package which includes a linking example")	

############
# project 
###########

set(LIBNAME "BitGraph")
set(LIB_MAJOR_VERS "1")
set(LIB_MINOR_VERS "1")
set(LIB_PATCH_VERS "0")

project(${LIBNAME} 
    VERSION  ${LIB_MAJOR_VERS}.${LIB_MINOR_VERS}.${LIB_PATCH_VERS}
	LANGUAGES CXX C	)

#################
# Introduce variables for export:
# * CMAKE_INSTALL_LIBDIR		#lib
# * CMAKE_INSTALL_BINDIR		#bin
# * CMAKE_INSTALL_INCLUDEDIR	#include
include(GNUInstallDirs)

#################
# UNIT TESTS (using googletest) 

#Tests and examples are not built by default in Projects which embed BitGraph
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)	
	#Root dir - BitGraph project, build tests									
	SET(BUILD_TESTS ON CACHE BOOL "builds tests for BitGraph (uses googletest)" FORCE)
	SET(BUILD_EXAMPLES ON CACHE BOOL "builds examples for BitGraph " FORCE)
	SET(INSTALL_BITGRAPH ON CACHE BOOL "installs BitGraph if required" FORCE)

else()
	#not Root dir - Project embedding BitGraph, building tests / examples and installing BitGraph is not enabled
	SET(BUILD_TESTS OFF CACHE BOOL "builds tests for BitGraph (uses googletest)" FORCE)
	SET(BUILD_EXAMPLES OFF CACHE BOOL "builds examples for BitGraph " FORCE)
	SET(INSTALL_BITGRAPH OFF CACHE BOOL "installs BitGraph if required" FORCE)
endif()

###################
# INSTALL BitGraph (find_package, config mode)
#option(INSTALL_BITGRAPH "Enable installation of BitGraph. (Projects embedding BitGraph may want to turn this OFF.)" ON)

###################
# INSTALL googletest - default off
option(INSTALL_GTEST "Enable installation of googletest (disabled by default)" OFF)

#############
# OpenMP (Windows)
option(INCLUDE_OPENMP "Makes OpenMP available (only for Windows platforms)" OFF)

#############
# APPLE options  - extend to other SO
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    set(CMAKE_PREFIX_PATH "/usr/local/homebrew")
    #add_compile_options(-arch arm64)
endif (APPLE)

####################
# Subdirectories 
####################
add_subdirectory (src) 








