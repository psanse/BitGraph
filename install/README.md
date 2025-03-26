# Linking to an existing CMake project

This folder is an independent CMake project that illustrates how to use the default installation of BitGraph. The installation of Bitgraph is configured by default in this [install](https://github.com/psanse/BitGraph/tree/master/install) folder. The project contains two target source code files in the [src](https://github.com/psanse/BitGraph/tree/master/install/src) folder. The CMake code uses `find_package(BitGraph CONFIG REQUIRED)` to look for the BitGraph root folder containing the installed libraries.

```plaintext
find_package(BitGraph CONFIG REQUIRED)     #set the install path of BitGraph in the variable BitGraph_dir

# target graph_example
add_executable(graph_example src/graph.cpp)

target_link_libraries (graph_example LINK_PUBLIC 
    BitGraph::graph
    BitGraph::bitscan
    BitGraph::utils				)

```

For `find_package` to succeed the installation path of BitGraph has to be provided to the CMake build in the CMake variable `BitGraph_DIR`, e.g. in the build

```plaintext
cmake .. -DBitGraph_DIR=<PATH_BitGraph>

```

This example is also the default installation folder of BitGraph. In this setup, `BitGraph_DIR` should be set to <path\_to\_this\_folder>/lib/cmake/BitGraph.
