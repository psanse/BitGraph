## Generic Build Instructions

### Setup

To build BitGraph, you must tell your build system where to find its headers and source files. The exact method depends on your build system, but it is usually straightforward.

## Build with CMake

BitGraph comes with a CMake build script ([CMakeLists.txt](https://github.com/psanse/BitGraph/blob/master/src/CMakeLists.txt)) that can be used on a wide range of platforms ("C" stands for cross-platform). In case it is not installed already, you can download CMake for free from [https://cmake.org/](https://cmake.org/).

CMake works by generating native makefiles or build projects that can be used in the compiler environment of your choice. You can either build BitGraph as a standalone project or use it in an existing CMake project.

## Standalone CMake Project

```plaintext
git clone https://github.com/psanse/BitGraph.git 
cd BitGraph   # Main directory of the cloned repository.
mkdir build   # Create a directory to hold the build output.
cd build
cmake ..      # Generate native build scripts for BitGraph.

```

If you are on a Linux OS, you should now see a Makefile in the current directory. Just type `make` to build BitGraph. You can then proceed to install BitGraph if you are a system admin. The installation default directory of libraries and headers is the [install](https://github.com/psanse/BitGraph/tree/master/install) folder in the root directory of BitGraph.

```plaintext
make
sudo make install    # Install in ~/BitGraph/install by default

```

If you use Windows and have Visual Studio installed, a `BitGraph.sln` file and several `.vcproj` files will be created such `INSTALL.vproj`. You can then build them using Visual Studio. Note that building the INSTALL project is equivalent to installing BitGraph.

## Linking BitGraph Into An Existing CMake Project

If you want to use BitGraph in a project that already uses CMake, the easiest way is to first install libraries and headers as explained above. Import BitGraph by using `find_package`. For example, if `find_package(BitGraph CONFIG REQUIRED)` succeeds, you can use the libraries as `BitGraph::utils`, `BitGraph::bitscan` and `BitGraph::graph`. The folder [install](https://github.com/psanse/BitGraph/tree/master/install) includes a CMakeLists.txt and a target source file example.

An alternative approach is to use CMake to download BitGraph as part of the build's configure step. Here is a basic example of CMake code:

```plaintext
include(FetchContent)

FetchContent_Declare(
  BitGraph
  GIT_REPOSITORY https://github.com/psanse/BitGraph
  GIT_TAG master			# A fixed release source will be provided shortly
)
FetchContent_MakeAvailable(BitGraph)
include_directories (PUBLIC ${FETCHCONTENT_BASE_DIR}/BitGraph-src/src)

# Now simply link against BitGraph.
add_executable(example main.cpp)
target_link_libraries(example graph bitscan utils)

```

Note that this approach requires CMake 3.14 or later due to its use of the `FetchContent_MakeAvailable()` command.
