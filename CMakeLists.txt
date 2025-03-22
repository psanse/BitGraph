cmake_minimum_required (VERSION 2.8...3.13)  	

if(${CMAKE_VERSION} VERSION_GREATER "3.24.0") 
   cmake_policy(SET CMP0135 NEW) 
endif()
	
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install CACHE PATH "prefix directory to install hte BitGraph libraries")	

############
# project 
###########

set(LIBNAME "BitGraph")
set(LIB_MAJOR_VERS "1")
set(LIB_MINOR_VERS "0")
set(LIB_PATCH_VERS "1")

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
# Google tests 
option(BUILD_TESTS "builds tests with googletest" ON)

###################
# INSTALL BitGraph (find_package, config mode)
option(INSTALL_BITGRAPH "Enable installation of BitGraph. (Projects embedding BitGraph may want to turn this OFF.)" ON)

###################
# INSTALL googletest - default off
set(INSTALL_GTEST "BitGraph embeds googletest, no installation required" OFF CACHE BOOL "" FORCE)

#############
# OpenMP (Windows)
option(INCLUDE_OPENMP "makes OpenMP available (only for Windows platforms)" OFF)

#############
# APPLE options 
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    set(CMAKE_PREFIX_PATH "/usr/local/homebrew")
    #add_compile_options(-arch arm64)
endif (APPLE)

####################
# Subdirectories 
####################
add_subdirectory (src) 








