##############
# Source layout
##############

set(BITGRAPH_ROOT_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

#################
# Export layout
#################

# Installed package layout:
#
#   <prefix>/lib/cmake/BitGraph/
#   <prefix>/lib/
#   <prefix>/include/BitGraph/

set(TARGETS_EXPORT_NAME
    "${PROJECT_NAME}Targets"
)

set(namespace
    "${PROJECT_NAME}::"
)

set(config_install_dir
    "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

set(include_install_dir
    "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)

# Directory for generated package-configuration files.
set(generated_dir
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

set(version_config
    "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake"
)

set(project_config
    "${generated_dir}/${PROJECT_NAME}Config.cmake"
)

#################
# OpenMP
#################

if(WIN32 AND INCLUDE_OPENMP)
    find_package(OpenMP)

    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found on Windows")
    else()
        message(WARNING
            "INCLUDE_OPENMP is enabled, but OpenMP was not found"
        )
    endif()
endif()

####################
# Library components
####################

add_subdirectory(utils)
add_subdirectory(bitscan)
add_subdirectory(graph)

###################
# Installation
###################

include(cmake_install_bitgraph)



# ###############
# DEPRECATED  (to remove)
#################




# ##############
# # Set layout 

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)	

# set(source_code_dir ${CMAKE_CURRENT_SOURCE_DIR})

# #################
# # Export layout (config mode - find_package)
# # Valid for all platforms:
# #   * <prefix>/lib*/cmake/<PROJECT_NAME>
# #   * <prefix>/lib*/
# #   * <prefix>/include/

# set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
# set(namespace "${PROJECT_NAME}::")
# set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
# set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# #directory for the <PROJECT-NAME>Targets.cmake file during the build - will be installed later on
# set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
# set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
# set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")

# #############
# # C++ flags 
# #############

# if(UNIX)

    # include(CheckCXXCompilerFlag)
	
    # CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    # #CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    # if(COMPILER_SUPPORTS_CXX11)
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    # #elseif(COMPILER_SUPPORTS_CXX0X)
    # #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    # else()
            # message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 or greater support. Please use a different C++ compiler.")
    # endif()


    # ##################
    # # specific flags  
    # ###################

    # CHECK_CXX_COMPILER_FLAG("-fpermissive" COMPILER_SUPPORTS_FPERMISSIVE)
    # if(COMPILER_SUPPORTS_FPERMISSIVE)
        # add_compile_options("-fpermissive")
    # endif()

# endif(UNIX)

# if(WIN32)
  # set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
# endif(WIN32)


# #############
# # USEFUL PATHS (to be used inside the code)

# set(PATH_GRAPH_TESTS_CMAKE_GUI ${CMAKE_CURRENT_SOURCE_DIR}/graph/data CACHE PATH "PATH for GRAPH unit tests")
# add_definitions (-DPATH_GRAPH_TESTS_CMAKE_SRC_CODE=\"${PATH_GRAPH_TESTS_CMAKE_GUI}/\") 

# set(PATH_COPT_TESTS_CMAKE_GUI ${CMAKE_CURRENT_SOURCE_DIR}/copt/data CACHE PATH "PATH for COPT unit tests")
# add_definitions (-DPATH_COPT_TESTS_CMAKE_SRC_CODE=\"${PATH_COPT_TESTS_CMAKE_GUI}/\") 


# #############
# # OpenMP (Windows)

# if (WIN32 AND INCLUDE_OPENMP)
	# find_package(OpenMP)
	# IF (OPENMP_FOUND)
		# set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -D_VARIADIC_MAX=10")
		# message(STATUS "OpenMP found in Windows OS")
	# endif()
# endif (WIN32 AND INCLUDE_OPENMP)

# ####################
# # Subdirectories 
# ####################
# set(BITGRAPH_ROOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# add_subdirectory (utils)
# add_subdirectory (bitscan)
# add_subdirectory (graph)

# ###################
# # INSTALL (find_package - config mode)
# include(cmake_install_bitgraph)




