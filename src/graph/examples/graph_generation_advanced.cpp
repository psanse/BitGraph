/**
 * @file graph_generation_advanced.cpp
 * @brief An advanced example of random graph generation.
 *
 * RandomGen<GraphT>::create_graph(graph, n, p) generates random graphs
 * according to the graph type:
 *
 * - Ugraph: G(n, m) model, with exactly m edges determined by the
 *   target edge density p.
 * - Directed graphs: G(n, p) model, where each possible directed edge
 *   is included independently with probability p.
 *
 * Self-loops are not generated.
 */

#include <iomanip>
#include <iostream>

#include "graph/algorithms/graph_gen.h"

namespace {

template <class GraphT>
void show_graph(const char* label, const GraphT& graph)
{
    std::cout << label << '\n';
    graph.print_data();

    std::cout << "  degrees: ";
    for (bitgraph::vertex_t vertex = 0;
         vertex < graph.num_vertices();
         ++vertex) {
        std::cout << graph.degree(vertex) << ' ';
    }
    std::cout << "\n\n";
}

} // namespace

int main()
{
    // The same parameter bundle used by gen_random_bench.cpp.  Keeping the
    // values together makes it easy to turn this example into a benchmark.
    const bitgraph::random_attr_t random_attributes(
        12, 12,       // vertex range [nLB, nUB]
        0.25, 0.25,   // density range [pLB, pUB]
        1,            // repetitions
        1, 0.10);     // increments (unused when each range has one value)

    const std::size_t vertex_count = random_attributes.nLB;
    const double edge_probability = random_attributes.pLB;

    std::cout << std::fixed << std::setprecision(3)
              << "Generating G(" << vertex_count << ", "
              << edge_probability << ")\n"
              << "random attributes: " << random_attributes << '\n';

    // The benchmark example uses this same call; the return-value overload is
    // convenient when the graph is needed immediately.
   const auto dense = bitgraph::RandomGen<bitgraph::ugraph>::create_graph(
        vertex_count, edge_probability);
    show_graph("dense undirected graph", dense);

    // The generator is independent of the storage strategy.  For a sparse
    // graph, only the graph type changes.
    const auto sparse =
        bitgraph::RandomGen<bitgraph::sparse_ugraph>::create_graph(
            vertex_count, edge_probability);
    show_graph("sparse undirected graph", sparse);

    // A vertex permutation changes labels but preserves the graph's structure.
    bitgraph::ugraph isomorphic;
    bitgraph::RandomGen<bitgraph::ugraph>::create_isomorphism(isomorphic,
                                                               dense);
    std::cout << "isomorphic copy: " << isomorphic.num_vertices()
              << " vertices, " << isomorphic.num_edges() << " edges\n";

    return 0;
}
