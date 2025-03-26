## Generic Build Instructions

### Setup

To build BitGraph, you must tell your build system where to find its headers and source files. The exact method depends on your build system, but it is usually straightforward.

## Build with CMake

BitGraph comes with a CMake build script ([CMakeLists.txt](https://github.com/psanse/BitGraph/blob/master/src/CMakeLists.txt)) that can be used on a wide range of platforms ("C" stands for cross-platform). In case it is not installed already, you can download CMake for free from [https://cmake.org/](https://cmake.org/).

CMake works by generating native makefiles or build projects that can be used in the compiler environment of your choice. You can either build BitGraph as a standalone project or it can be incorporated into an existing CMake build for another project.

## Standalone CMake Project

```plaintext
git clone https://github.com/psanse/BitGraph.git 
cd BitGraph   # Main directory of the cloned repository.
mkdir build   # Create a directory to hold the build output.
cd build
cmake ..      # Generate native build scripts for BitGraph.
```

If you are on a Linux OS, you should now see a Makefile in the current directory. Just type `make` to build BitGraph. You can then proceed to install BitGraph if you are a system admin.

```plaintext
make
sudo make install    # Install in ~/BitGraph/install by default
```

If you use Windows and have Visual Studio installed, a `BitGraph.sln` file and several `.vcproj` files will be created such `INSTALL.vproj`. You can then build them using Visual Studio.. Note that building the INSTALL project is equivalent to installing BitGraph..