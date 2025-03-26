## Generic Build Instructions

### Setup

To build BitGraph, you must tell your build system where to find its headers and source files. The exact method depends on which build system you use, but it is usually straightforward.

## Build with CMake

BitGraph comes with a CMake build script ([CMakeLists.txt](https://github.com/psanse/BitGraph/blob/master/src/CMakeLists.txt)) that can be used on a wide range of platforms ("C" stands for cross-platform.). In case it is not installed already, you can download CMake for free from [https://cmake.org/](https://cmake.org/).

CMake works by generating native makefiles or build projects that can be used in the compiler environment of your choice. You can either build BitGraph as a standalone project or it can be incorporated into an existing CMake build for another project.