# GRAPH API overview

The main types and functions of GRAPH, grouped by task. This page is a map, not a full reference: for every signature and parameter, build the Doxygen reference (see [docs/README.md](../README.md)) or read the headers in [src/graph](../../src/graph). A guide with examples is in the [GRAPH README](../../src/graph/README.md).

Include the graph types with `#include "BitGraph/graph/graph.h"`. Algorithms live in their own headers (listed below). All names are in namespace `bitgraph` unless stated.

**Conventions.** Vertices are numbered `0 .. num_vertices() - 1`. `Vertex` is `int`. Adjacency rows are bitsets from [BITSCAN](bitscan.md): `neighbors(v)` returns the row of `v`.

## Graph types

| Alias | Class | Description |
|---|---|---|
| `graph` | `Graph<bitarray>` | directed graph |
| `ugraph` | `Ugraph<bitarray>` | undirected graph |
| `sparse_graph` | `Graph<sparse_bitarray>` | sparse directed graph |
| `sparse_ugraph` | `Ugraph<sparse_bitarray>` | sparse undirected graph |
| `ugraph_vw`, `ugraph_vwi` | `Graph_VW<ugraph, double>`, `Graph_VW<ugraph, int>` | undirected, vertex-weighted |
| `ugraph_ew`, `ugraph_ewi` | `Graph_EW<ugraph, double>`, `Graph_EW<ugraph, int>` | undirected, edge-weighted |

`Ugraph` derives from `Graph`, so everything in the next sections applies to undirected graphs as well. `graph_traits<GraphT>` provides compile-time properties of a graph type, for example whether it is undirected.

## Creating and resetting

| Function | Purpose |
|---|---|
| `Graph(n)`, `Ugraph(n)` | graph with `n` isolated vertices |
| `Graph(filename)`, `Ugraph(filename)` | read a graph from a file (the format is detected) |
| `reset(n)` | reinitialize with `n` isolated vertices |
| `clear()` | clear the contents |
| `shrink_to_fit()` | release unused storage in the adjacency bitsets |
| `Ugraph::make_triangle()` | triangle |
| `Ugraph::make_complete_graph(n)` | complete graph |
| `Ugraph::make_cycle(n)` | cycle |
| `Ugraph::make_star(n)` | star |
| `graph_utils::create_complete(graph, n)` | reinitialize a graph as a complete graph (header `graph/algorithms/unweighted_graph_utils.h`) |

## Edges and neighbourhoods

| Function | Purpose |
|---|---|
| `add_edge(v, w)` | add the edge (directed from `v` to `w` in `Graph`; symmetric in `Ugraph`) |
| `remove_edge(v, w)` | remove the edge |
| `remove_edges(v)` | remove every edge incident to `v` |
| `is_edge(v, w)` | test an edge |
| `is_self_loop()` | test whether the graph contains any self-loop `(v, v)` |
| `neighbors(v)` | adjacency row of `v` (a bitset) |
| `adjacency_matrix()` | the whole bitset adjacency matrix |
| `make_bidirected()` | add the reverse of every edge (directed graphs) |
| `create_complement(out)` | the complement graph |

## Size and degrees

| Function | Purpose |
|---|---|
| `num_vertices()`, `size()` | number of vertices |
| `num_edges()` | number of edges |
| `density()` | graph density |
| `degree_out(v)`, `degree_in(v)` | out-degree and in-degree (directed graphs) |
| `Ugraph::degree(v)` | degree (undirected graphs) |
| `Ugraph::degree_up(v)` | number of neighbours of `v` with a higher index |
| `Ugraph::max_degree()` | maximum degree |
| `Ugraph::max_subgraph_degree(...)` | maximum degree in an induced subgraph |
| `Ugraph::outgoing_degree(...)` | number of edges leaving a vertex set |

## Subgraphs

| Function | Purpose |
|---|---|
| `create_subgraph(out, vertices)` | the subgraph induced by a list of vertices |
| `create_subgraph_first_k(k)` | return the subgraph induced by the first `k` vertices; `Graph` also has `create_subgraph_first_k(k, out)` |
| `remove_vertices(...)` | the subgraph induced by the vertices that are **not** in a given set |
| `Ugraph::shrink_to_fit(k)` | reduce the graph to its first `k` vertices |
| `graph_utils::neighbors_in(...)`, `neighbors_after(...)` | neighbours of a vertex inside a reference set, or with a higher index |
| `graph_utils::is_edge_free_induced_subgraph(...)`, `is_triangle_free_induced_subgraph(...)` | structural tests on induced subgraphs |
| `graph_utils::is_circulant(...)`, `circulant_jump_pattern(...)` | circulant-graph tests |
| `convert_graph(sparse, dense)` | convert a sparse undirected graph to a dense one (header `graph/algorithms/graph_conversions.h`) |

## Weighted graphs

For `ugraph_vw` / `ugraph_vwi` (vertex weights), header `graph/ugraph_vertex_weighted.h`:

| Function | Purpose |
|---|---|
| `set_weight(v, w)`, `set_weights(...)` | set one vertex weight, or all of them |
| `weight(v)`, `weights()` | read one weight, or the whole vector |
| `maximum_weight(v)` | return the maximum weight and store a vertex that attains it in `v` |
| `is_unit_weighted()` | test whether all weights are 1 |
| `transform_weights(f)` | apply a function to all weights |
| `read_weights(filename)` | read the weights from a file |
| `set_modulus_weights()` | Pullan-style modulus weights |
| `print_weights(os)` | print the weights |

For `ugraph_ew` / `ugraph_ewi` (edge weights), header `graph/ugraph_edge_weighted.h`:

| Function | Purpose |
|---|---|
| `add_edge(v, w, weight)` | add an edge with a weight (replaces the weight if the edge exists) |
| `set_weight(v, w, weight)`, `set_weights(weight)` | set one edge weight, or all of them |
| `weight(v, w)`, `weights()` | read one weight, or the whole matrix; weights are symmetric |
| `is_unit_weighted()`, `is_consistent()` | consistency and unit-weight tests |
| `transform_weights(f)` | apply a function to all weights |
| `print_edges(os)` | print the edges with their weights |

## Reading and writing

| Function | Purpose |
|---|---|
| `Graph(filename)` | read a file; DIMACS, Matrix Market and edge list are detected automatically |
| `read_dimacs(...)`, `read_mtx(...)`, `read_EDGES(...)`, `read_01(...)` | read one specific format (`read_01` is a 0-1 adjacency matrix with the vertex count in the first line) |
| `write_dimacs(os)`, `write_EDGES(os)` | write DIMACS or an edge list (`Graph` and `Ugraph`) |
| `Ugraph::write_mtx(os)` | write Matrix Market |
| `io::yed::graph_to_gml(...)`, `graph_to_gml_clique`, `graph_to_gml_color`, `graph_to_gml_layered`, `graph_to_gml_circular` | write GML files for the yED editor (header `graph/formats/yED_format.h`) |
| `print_data()`, `print_edges()`, `print_adj()` | print summary, edges or adjacency matrix |
| `Ugraph::print_degrees()`, `print_adjacency_matrix()` | print degrees or the adjacency matrix |

## Orderings (header `graph/algorithms/graph_fast_sort.h`)

| Name | Purpose |
|---|---|
| `GraphSort<GraphT>` (`graph_utils::GraphFastRootSort`) | degree and degeneracy orderings of an undirected graph |
| `new_order(strategy, placement, sort_order)` | compute an ordering. Strategies: `min`, `max`, `min_degeneracy`, `max_degeneracy`, `min_degeneracy_composite`, `max_degeneracy_composite`, `max_with_support`, `min_with_support`. Placement: `first_to_last` or `last_to_first`. Format: `new_to_old` or `old_to_new`. |
| `GraphSort<GraphT>::reorder(graph, order)` | static: return the graph relabelled by an old-to-new ordering |
| `compute_deg(graph, degrees)` | static: degree of every vertex |
| `graph_utils::GraphFastRootSort_VW<GraphW>` | ordering utilities for vertex-weighted graphs |
| `graph_utils::GraphFastRootSort_EW<GraphW>` | ordering utilities for edge-weighted graphs |

The integer-based `new_order(int, bool, bool)` overloads are deprecated.

## Vertex relabelling (headers `graph/algorithms/ordering_decoder.h`, `ordering_map.h`)

| Name | Purpose |
|---|---|
| `OrderingDecoder` | decode vertices through a composition of orderings |
| `add_ordering(order)`, `decode(v)` | add an ordering to the composition; map a vertex back through it |
| `OrderingDecoder::inverse_ordering(order)`, `invert_ordering(order)` | the inverse permutation, returned or in place |
| `OrderingMap` | map vertices between two orderings of a graph, in both directions |
| `map_left_to_right(...)`, `map_right_to_left(...)`, `is_consistent()` | map vertex sets; check the mapping |

## Generators (header `graph/algorithms/graph_gen.h`)

| Function | Purpose |
|---|---|
| `RandomGen<GraphT>::create_graph(n, p)` | uniform random graph `G(n, p)` |
| `RandomGen<GraphT>::create_isomorphism(...)` | an isomorphic copy with randomly permuted vertices |
| `RandomGen<GraphT>::create_graph_benchmark(...)` | generate and save a collection of random graphs |
| `experimental::SparseRandomGen`, `VertexWeightGen`, `EdgeWeightGen` | **experimental** generators for massive sparse graphs and for weights |

## Algorithm information and benchmarks

| Name | Purpose |
|---|---|
| `AlgorithmParameters`, `BasicAlgorithmInfo` | configuration and run information for graph algorithms (header `graph/algorithms/algorithm_info.h`) |
| `BasicGraphBenchmark` | base class for collections of graph benchmark instances (header `graph/graph_benchmark.h`) |

## Experimental

`experimental::VertexRange` (an internal closed range of vertices) and the experimental generators above are not stable and may change in any release.
