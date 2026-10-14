/**
 * @file graph_sorting_simple.cpp
 * @brief Demonstrates simple vertex orderings for undirected graphs.
 *
 * GraphFastRootSort provides degree-based and degeneracy-based orderings for
 * the dense and sparse undirected graph types.  The ordering returned in this
 * example is [NEW]->[OLD]: each position contains the original vertex placed
 * at that position.
 */

#include <iostream>

#include "graph/algorithms/graph_fast_sort.h"
#include "graph/graph.h"

namespace {

// Print a vector without exposing any of the sorter implementation details.
void print_order(const char* label, const bitgraph::vertex_ordering& ordering)
{
    std::cout << "  " << label << ": ";
    for (const auto vertex : ordering) {
        std::cout << vertex << ' ';
    }
    std::cout << '\n';
}

template <class GraphT>
void demonstrate_sorting(const char* name, const GraphT& source)
{
    using sorter_t = bitgraph::GraphSort<GraphT>;

    // The sorter keeps a reference to its graph, so make a local copy for
    // each demonstration and leave the source graph unchanged.
    GraphT graph = source;
    sorter_t sorter(graph);

    std::cout << name << '\n'
              << "  degrees: ";
    for (bitgraph::vertex_t vertex = 0;
         vertex < graph.num_vertices();
         ++vertex) {
        std::cout << graph.degree(vertex) << ' ';
    }
    std::cout << '\n';

    // Degree ordering places vertices from lower degree to higher degree.
    const auto min_degree = sorter.new_order(
        sorter_t::strategy::min,
        sorter_t::placement::first_to_last,
        sorter_t::sort_order::new_to_old);
    print_order("non-decreasing degree", min_degree);

    // Degeneracy ordering repeatedly removes a lowest-degree vertex.  It is
    // useful in algorithms that process a graph vertex by vertex.
    const auto min_degeneracy = sorter.new_order(
        sorter_t::strategy::min_degeneracy,
        sorter_t::placement::first_to_last,
        sorter_t::sort_order::new_to_old);
    print_order("minimum degeneracy", min_degeneracy);

    std::cout << '\n';
}

} // namespace

int main()
{
    // This graph has different degrees and enough structure to make the
    // degree and degeneracy orderings visible in the output.
    constexpr int vertex_count = 6;

    bitgraph::ugraph dense(vertex_count);
    dense.add_edge(0, 1);
    dense.add_edge(0, 2);
    dense.add_edge(0, 3);
    dense.add_edge(1, 2);
    dense.add_edge(1, 4);
    dense.add_edge(3, 5);

    bitgraph::sparse_ugraph sparse(vertex_count);
    sparse.add_edge(0, 1);
    sparse.add_edge(0, 2);
    sparse.add_edge(0, 3);
    sparse.add_edge(1, 2);
    sparse.add_edge(1, 4);
    sparse.add_edge(3, 5);

    demonstrate_sorting("dense ugraph", dense);
    demonstrate_sorting("sparse_ugraph", sparse);

    return 0;
}
