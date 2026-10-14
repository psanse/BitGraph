/**
 * @file graph_types_simple.cpp
 * @brief Introduces the four basic unweighted GRAPH representations.
 *
 * The GRAPH library provides dense and sparse storage for both directed and
 * undirected graphs.  This example creates the same small structure in each
 * representation so that the differences between the public graph types are
 * easy to compare.
 */

#include <iostream>

#include "graph/graph.h"

namespace {

// Print the common information for a directed graph.
template <class GraphT>
void print_directed_summary(const char* name, const GraphT& graph)
{
    std::cout << name << " (directed)\n"
              << "  vertices: " << graph.num_vertices() << '\n'
              << "  edges: " << graph.num_edges() << '\n'
              << "  density: " << graph.density() << '\n'
              << "  vertex 0 out-degree: " << graph.degree_out(0) << '\n'
              << "  vertex 0 in-degree: " << graph.degree_in(0) << '\n'
              << "  edge 0 -> 1: " << std::boolalpha
              << graph.is_edge(0, 1) << "\n\n";
}

// Print the common information for an undirected graph.
template <class GraphT>
void print_undirected_summary(const char* name, const GraphT& graph)
{
    std::cout << name << " (undirected)\n"
              << "  vertices: " << graph.num_vertices() << '\n'
              << "  edges: " << graph.num_edges() << '\n'
              << "  density: " << graph.density() << '\n'
              << "  vertex 0 degree: " << graph.degree(0) << '\n'
              << "  edge 0 -- 1: " << std::boolalpha
              << graph.is_edge(0, 1) << "\n\n";
}

} // namespace

int main()
{
    // The same four edges are used in every graph.  The direction matters for
    // graph and sparse_graph, while ugraph and sparse_ugraph store each edge
    // symmetrically and report ordinary undirected degrees.
    constexpr int vertex_count = 4;

    bitgraph::graph dense_directed(vertex_count);
    dense_directed.add_edge(0, 1);
    dense_directed.add_edge(1, 2);
    dense_directed.add_edge(2, 3);
    dense_directed.add_edge(0, 3);

    bitgraph::sparse_graph sparse_directed(vertex_count);
    sparse_directed.add_edge(0, 1);
    sparse_directed.add_edge(1, 2);
    sparse_directed.add_edge(2, 3);
    sparse_directed.add_edge(0, 3);

    bitgraph::ugraph dense_undirected(vertex_count);
    dense_undirected.add_edge(0, 1);
    dense_undirected.add_edge(1, 2);
    dense_undirected.add_edge(2, 3);
    dense_undirected.add_edge(0, 3);

    bitgraph::sparse_ugraph sparse_undirected(vertex_count);
    sparse_undirected.add_edge(0, 1);
    sparse_undirected.add_edge(1, 2);
    sparse_undirected.add_edge(2, 3);
    sparse_undirected.add_edge(0, 3);

    print_directed_summary("graph", dense_directed);
    print_directed_summary("sparse_graph", sparse_directed);
    print_undirected_summary("ugraph", dense_undirected);
    print_undirected_summary("sparse_ugraph", sparse_undirected);

    return 0;
}
