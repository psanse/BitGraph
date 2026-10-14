# GRAPH

**Graphs stored as bitsets, for algorithms that run on bit operations.**

GRAPH is the graph library of [BitGraph](../../README.md). It represents a graph by its adjacency matrix, with every row held as a bitset from [BITSCAN](../bitscan/README.md). Neighbourhood queries, induced subgraphs and candidate filtering then become bitwise operations that process 64 vertices at a time.

## Contents

- [Why GRAPH](#why-graph)
- [Features](#features)
- [Getting started](#getting-started)
- [Graph types](#graph-types)
- [Working with graphs](#working-with-graphs)
- [Weighted graphs](#weighted-graphs)
- [Vertex orderings](#vertex-orderings)
- [File formats](#file-formats)
- [Examples and tests](#examples-and-tests)
- [Folder layout](#folder-layout)
- [License and citation](#license-and-citation)

## Why GRAPH

Row *v* of the adjacency matrix **is** the set of neighbours of *v*, stored as a bitset. So the questions that dominate exact algorithms for cliques, colouring and constraint satisfaction are one-liners over bitsets:

```cpp
#include <iostream>
#include "BitGraph/graph/graph.h"

int main() {
    using namespace bitgraph;

    ugraph g(6);                              // undirected graph, 6 vertices
    g.add_edge(0, 1);  g.add_edge(0, 2);  g.add_edge(0, 3);
    g.add_edge(1, 2);  g.add_edge(1, 3);  g.add_edge(3, 4);

    // Adjacency rows are bitsets: the common neighbours of 0 and 1 take one AND
    bitarray common = g.neighbors(0);
    common &= g.neighbors(1);

    // Enumerate the set bits
    bitarray::scan scan(common);
    for (int v = scan.next_bit(); v != BBObject::noBit; v = scan.next_bit()) {
        std::cout << v << ' ';                // prints: 2 3
    }
    std::cout << '\n';
}
```

And because dense and sparse graphs share the same interface, an algorithm written once runs on both:

```cpp
template <class GraphT>
int common_neighbours(const GraphT& g, int u, int v) {
    auto row = g.neighbors(u);                // copy of the adjacency row (a bitset)
    row &= g.neighbors(v);
    return row.count();
}

bitgraph::ugraph        dense(1000);          // every 64-bit block is stored
bitgraph::sparse_ugraph sparse(1000000);      // only the non-empty blocks are stored
// common_neighbours(dense, 0, 1) and common_neighbours(sparse, 0, 1) both compile
```

## Features

- **Dense and sparse graphs.** `graph` (directed), `ugraph` (undirected) and their sparse counterparts `sparse_graph` and `sparse_ugraph`, which keep very large graphs with few edges small.
- **Weighted graphs.** Undirected vertex-weighted (`ugraph_vw`, `ugraph_vwi`) and edge-weighted (`ugraph_ew`, `ugraph_ewi`) graphs. Edge weights are kept symmetric.
- **File formats.** Read and write DIMACS, Matrix Market and edge-list files. Reading detects the format automatically. A yED (GML) writer is also provided.
- **Orderings.** Degree and degeneracy orderings, and reversible vertex relabelling.
- **Generators.** Complete graphs, cycles, stars and triangles, and uniform random graphs, dense or sparse.
- **Portable C++14.** Built and tested with GCC and MSVC.

## Getting started

Include the facade header and build with CMake. After installing BitGraph:

```cmake
find_package(BitGraph CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE BitGraph::graph BitGraph::bitscan BitGraph::utils)
```

```cpp
#include "BitGraph/graph/graph.h"      // graph types: graph, ugraph, sparse_*, weighted graphs
```

`graph.h` provides the graph types. Algorithms are in separate headers that you include as you need them, for example `graph/algorithms/graph_gen.h` for random graph generation and `graph/algorithms/graph_fast_sort.h` for orderings. See the [main README](../../README.md#installation) for installation.

## Graph types

| Type | Description |
|---|---|
| `graph` | directed graph |
| `ugraph` | undirected graph |
| `sparse_graph` | sparse directed graph |
| `sparse_ugraph` | sparse undirected graph |
| `ugraph_vw`, `ugraph_vwi` | undirected, vertex-weighted (`double` and `int` weights) |
| `ugraph_ew`, `ugraph_ewi` | undirected, edge-weighted (`double` and `int` weights) |

Pick the dense types for small and medium graphs, and the sparse types for large or massive graphs with few edges.

## Working with graphs

Declaring graphs:

```cpp
using namespace bitgraph;

ugraph ug(10);                    // undirected graph with 10 vertices
ug.add_edge(0, 1);
ug.add_edge(0, 2);
ug.add_edge(7, 8);
ug.print_data();                  // n:= 10  m:= 3  p:= 0.067

graph g(100);                     // directed graph
g.add_edge(0, 1);
g.add_edge(0, 2);
g.add_edge(1, 2);
g.make_bidirected();              // every edge becomes bidirected

sparse_ugraph sg(1000000);        // 1M vertices, only the edges are stored
sg.add_edge(0, 1);
```

Edges can be added or removed in constant time in the dense types, and in logarithmic time in the sparse types. Graphs can be copied and assigned.

Generators:

```cpp
#include "BitGraph/graph/algorithms/graph_gen.h"      // for RandomGen

auto cycle = ugraph::make_cycle(10);
cycle.shrink_to_fit(5);                               // subgraph induced by the first 5 vertices
cycle.print_data();                                   // n:= 5  m:= 4  p:= 0.400

auto k5 = ugraph::make_complete_graph(5);             // also make_triangle(), make_star(n)
ugraph r = RandomGen<ugraph>::create_graph(100, 0.3); // uniform random graph, density 0.3
```

## Weighted graphs

```cpp
using namespace bitgraph;

ugraph_vwi vw(4);                                     // one weight per vertex
vw.set_weights(std::vector<int>{10, 20, 30, 40});
vw.add_edge(0, 1);
// vw.weight(1) == 20

ugraph_ewi ew(4);                                     // one weight per edge
ew.add_edge(0, 1, 5);
ew.add_edge(1, 2, 7);
// ew.weight(1, 2) == 7  and  ew.weight(2, 1) == 7  (symmetric)
```

## Vertex orderings

```cpp
#include "BitGraph/graph/algorithms/graph_fast_sort.h"

using namespace bitgraph;
using sorter_t = GraphSort<ugraph>;

sorter_t sorter(g);
auto order = sorter.new_order(sorter_t::strategy::min_degeneracy,    // degeneracy ordering
                              sorter_t::placement::first_to_last,
                              sorter_t::sort_order::new_to_old);     // order[i] = vertex at position i
```

Strategies include `min`, `max`, `min_degeneracy` and `max_degeneracy`. The `OrderingDecoder` and `OrderingMap` classes convert between the original and the relabelled vertex numbering.

## File formats

GRAPH reads DIMACS, Matrix Market and edge-list files. To load a graph, give the file name to the constructor; the format is detected automatically:

```cpp
bitgraph::ugraph g("brock200_1.clq");           // 200 vertices, 14834 edges
```

To write a graph in another format:

```cpp
#include <fstream>

std::ofstream out("brock200_1.mtx");
g.write_mtx(out);              // also write_dimacs(out) and write_EDGES(out)
```

Sample graphs in each format are in [data/](data).

## Examples and tests

| Example | Shows |
|---|---|
| [graph_types_simple.cpp](examples/graph_types_simple.cpp) | the four basic graph types, degrees and edge queries |
| [graph_generation_simple.cpp](examples/graph_generation_simple.cpp) | classic graphs and random graph generation |
| [graph_weighted_simple.cpp](examples/graph_weighted_simple.cpp) | vertex- and edge-weighted graphs |
| [graph_sorting_simple.cpp](examples/graph_sorting_simple.cpp) | degree and degeneracy orderings |
| [graph_reading_simple.cpp](examples/graph_reading_simple.cpp) | reading a graph file in any supported format |
| [graph_formats.cpp](examples/graph_formats.cpp) | converting a graph file between formats |

The unit tests are in [tests/](tests) and run with `ctest` from the build directory.

## Folder layout

```
graph/
├── graph.h                      facade header: include this one
├── graph_types.h                vertex and ordering type aliases
├── graph_unweighted.h           graph, ugraph, sparse_graph, sparse_ugraph
├── ugraph_vertex_weighted.h     vertex-weighted graphs
├── ugraph_edge_weighted.h       edge-weighted graphs
├── simple_graph.h, simple_ugraph.h, simple_sparse_ugraph.h, ...   graph class templates
├── algorithms/                  orderings, generators, conversions, graph utilities
├── formats/                     DIMACS, Matrix Market, edge list and yED readers and writers
├── detail/                      implementation details (not public API)
├── data/                        sample graphs
├── examples/
└── tests/
```

## License and citation

Released under the [GNU GPL v3](../../LICENSE). To cite BitGraph, see [CITATION.cff](../../CITATION.cff) and the [citation notes](../../README.md#citation). Back to the [main README](../../README.md).
