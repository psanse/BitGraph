cmake_minimum_required (VERSION 2.8...3.13)  	

##############
# Set layout 
##############

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/myInstall CACHE PATH "prefix directory to deploy the project")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)	

############
# project 
###########

set(LIBNAME "BitGraph")
set(LIB_MAJOR_VERS "1")
set(LIB_MINOR_VERS "0")
set(LIB_PATCH_VERS "1")

#build source
project(${LIBNAME} 
    VERSION  ${LIB_MAJOR_VERS}.${LIB_MINOR_VERS}.${LIB_PATCH_VERS}
	LANGUAGES CXX C	)

#################
# Introduce variables:
# * CMAKE_INSTALL_LIBDIR		#lib
# * CMAKE_INSTALL_BINDIR		#bin
# * CMAKE_INSTALL_INCLUDEDIR	#include
include(GNUInstallDirs)

#################
# Export layout (config mode - find_package)
# Valid for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
#################


set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

#directory for the <PROJECT-NAME>Targets.cmake file during the build - will be installed later on
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")

#############
# C++ flags 
#############

if(UNIX)

    include(CheckCXXCompilerFlag)
	
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    #CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    #elseif(COMPILER_SUPPORTS_CXX0X)
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
            message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 or greater support. Please use a different C++ compiler.")
    endif()


    ##################
    # specific flags  
    ###################

    CHECK_CXX_COMPILER_FLAG("-fpermissive" COMPILER_SUPPORTS_FPERMISSIVE)
    if(COMPILER_SUPPORTS_FPERMISSIVE)
        add_compile_options("-fpermissive")
    endif()

endif()

if(WIN32)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
endif()

#################
# Google tests (requires C++14)
#################
option(BUILD_TESTS "makes tests available" ON)
if (BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
	set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
	
	#C++14 is required for googletest 
	set(CMAKE_CXX_STANDARD 14)					
	set(CMAKE_CXX_STANDARD_REQUIRED ON)		
	
	#for tests to be run with make test or ctest
	enable_testing()	
	
	message(STATUS "Building tests with GoogleTest framework")
endif()

#############
# USEFUL GLOBAL DEFINITIONS
# (to be used inside the code)
#############

set(PATH_GRAPH_TESTS_CMAKE_GUI ${CMAKE_SOURCE_DIR}/graph/data CACHE PATH "PATH for GRAPH unit tests")
add_definitions (-DPATH_GRAPH_TESTS_CMAKE_SRC_CODE=\"${PATH_GRAPH_TESTS_CMAKE_GUI}/\") 

set(PATH_COPT_TESTS_CMAKE_GUI ${CMAKE_SOURCE_DIR}/copt/data CACHE PATH "PATH for COPT unit tests")
add_definitions (-DPATH_COPT_TESTS_CMAKE_SRC_CODE=\"${PATH_COPT_TESTS_CMAKE_GUI}/\") 

#old definitions - deprecated
#add_definitions (-D_PATH_FOR_COPT_TESTS_DATA_IN_SRC_CODE=\"${CMAKE_SOURCE_DIR}/copt/data/\") 
#add_definitions (-D_PATH_FOR_GRAPH_TESTS_DATA_IN_SRC_CODE=\"${CMAKE_SOURCE_DIR}/graph/data/\") 

##############
# LOGGER LEVEL (see utils/logger.h) employed at runtime
# comments: default level debug
##############

set(LOGGER_LEVEL "debug" CACHE STRING "Logger level used at run time")
set_property(CACHE LOGGER_LEVEL PROPERTY STRINGS debug verbose error)
message(STATUS "LOGGER_LEVEL set to '${LOGGER_LEVEL}'")

if( ${LOGGER_LEVEL} STREQUAL "debug")
    add_definitions(-DLOGGER_DEBUG_LEVEL)				#all messages (logger.h) 
endif()

if( ${LOGGER_LEVEL}  STREQUAL "verbose")
    add_definitions(-DLOGGER_VERBOSE_LEVEL)				#all messages (logger.h)  	
endif()

if( ${LOGGER_LEVEL}  STREQUAL "error")
    add_definitions(-DLOGGER_ERROR_LEVEL)				#all messages (logger.h)  	
endif()

#############
# OpenMP (Windows)
# comments: found nothing better than to set it globally for all targets
#############
option(INCLUDE_OpenMP "makes OpenMP available in Windows platforms" OFF)
if (WIN32 AND INCLUDE_OpenMP)
find_package(OpenMP)
IF (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -D_VARIADIC_MAX=10")
	message(STATUS "OpenMP found in Windows OS")
endif()
endif (WIN32 AND INCLUDE_OpenMP)


#############
# APPLE options (check)
#############
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    set(CMAKE_PREFIX_PATH "/usr/local/homebrew")
    #add_compile_options(-arch arm64)
endif (APPLE)

####################
# Subdirectories 
####################
include_directories (PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (utils)
add_subdirectory (bitscan)
add_subdirectory (graph)

add_subdirectory (tclap)            #TODO - possibly remove

###################
#INSTALL
#(find_package, config mode)
###################

# Config
#   * <prefix>/lib/cmake/<PROJECT_NAME>/<PROJECT_NAME>Config.cmake
#   * <prefix>/lib/cmake/<PROJECT_NAME>/<PROJECT_NAME>ConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"					#<prefix>/lib/cmake/<PROJECT_NAME>
)

# Config
#   * <prefix>/lib/cmake/<PROJECT_NAME>/<PROJECT_NAME>Targets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)

# Include module with function 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/${PROJECT_NAME}Config.cmake.in"
    "${project_config}"									#"${generated_dir}/${PROJECT_NAME}Config.cmake")
    INSTALL_DESTINATION "${config_install_dir}"			#<prefix>/lib/cmake/<PROJECT_NAME>
)



