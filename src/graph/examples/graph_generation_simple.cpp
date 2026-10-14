/**
 * @file graph_generation_simple.cpp
 * @brief Demonstrates built-in and random graph generation.
 *
 * The first part uses named factories for common undirected graphs.  The
 * second part uses RandomGen with dense and sparse graph representations.
 * All examples are intentionally small so that the resulting statistics are
 * easy to inspect.
 */

#include <iomanip>
#include <iostream>

#include "graph/algorithms/graph_gen.h"

namespace {

// Print only the basic statistics needed to compare generated graphs.
template <class GraphT>
void print_summary(const char* name, const GraphT& graph)
{
    std::cout << name << '\n'
              << "  vertices: " << graph.num_vertices() << '\n'
              << "  edges: " << graph.num_edges() << '\n'
              << "  density: " << graph.density() << "\n\n";
}

} // namespace

int main()
{
    using bitgraph::RandomGen;
    using bitgraph::graph;
    using bitgraph::sparse_ugraph;
    using bitgraph::ugraph;

    std::cout << std::fixed << std::setprecision(3);

    // Named factories create common undirected graph structures directly.
    print_summary("triangle", ugraph::make_triangle());
    print_summary("complete graph", ugraph::make_complete_graph(5));
    print_summary("cycle", ugraph::make_cycle(6));
    print_summary("star", ugraph::make_star(6));

    // RandomGen creates G(n,p), where p is the probability of an edge.
    // The graph type determines whether the generated graph is directed or
    // undirected and whether its storage is dense or sparse.
    graph dense_directed = RandomGen<graph>::create_graph(6, 0.400);
    print_summary("random dense directed graph G(6, 0.4)", dense_directed);

    ugraph dense_undirected = RandomGen<ugraph>::create_graph(6, 0.400);
    print_summary("random dense undirected graph G(6, 0.4)", dense_undirected);

    sparse_ugraph sparse_undirected =
        RandomGen<sparse_ugraph>::create_graph(6, 0.400);
    print_summary("random sparse undirected graph G(6, 0.4)", sparse_undirected);

    return 0;
}
