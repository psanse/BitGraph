#set useful headers 
set(useful_headers_for_IDE ./common.h
./prec_timer.h
./logger.h
)

#created utils library
add_library (utils benchmark.cpp 
#logger.cpp 
result.cpp
test_analyser.cpp
common.cpp
${useful_headers_for_IDE}
)

#make logger level configurable from CMake at run time
#to note: there is no checking of the value if it is not set from the CMake GUI
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


# Make sure the compiler can find include files for our utils library
# when other libraries or executables link to bitscan
target_include_directories (utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(utils 
    PROPERTIES
     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)


option(utils_build_tests "Build utils tests." OFF)
#set(utils_build_tests ON)
if (utils_build_tests)

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (test_utils ./tests/gtest_main.cc
#./tests/test_batch.cpp
#./tests/test_benchmark.cpp
./tests/test_common.cpp
#./tests/test_file.cpp
#./tests/test_logger.cpp
# ./tests/test_result.cpp
#./tests/test_stress_timer.cpp
#./tests/test_tanal.cpp
#./tests/test_thread.cpp
#./test_analyser.cpp
)

target_include_directories (test_utils PUBLIC ../googletest/include/)
target_link_libraries (test_utils LINK_PUBLIC utils gtest)

endif()

install(TARGETS utils DESTINATION lib)
