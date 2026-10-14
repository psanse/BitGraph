/**
 * @file graph_weighted_simple.cpp
 * @brief Demonstrates vertex-weighted and edge-weighted undirected graphs.
 *
 * Weighted graph types keep the graph topology together with their weights.
 * Vertex weights describe individual vertices, while edge weights describe
 * the connections between pairs of vertices.
 */

#include <iostream>
#include <vector>

#include "graph/graph.h"

int main()
{
    using bitgraph::ugraph_ewi;
    using bitgraph::ugraph_vwi;

    // A vertex-weighted graph stores one weight for every vertex.  The
    // topology is still created with the usual undirected add_edge() method.
    ugraph_vwi vertex_weighted(4);
    vertex_weighted.set_weights(std::vector<int>{10, 20, 30, 40});
    vertex_weighted.add_edge(0, 1);
    vertex_weighted.add_edge(1, 2);
    vertex_weighted.add_edge(2, 3);

    std::cout << "vertex-weighted graph\n"
              << "  vertices: " << vertex_weighted.num_vertices() << '\n'
              << "  edges: " << vertex_weighted.num_edges() << '\n'
              << "  vertex weights: ";
  
    for (const auto weight : vertex_weighted.weights()) {
        std::cout << weight << ' ';
    }
    std::cout << "\n  weight(1): " << vertex_weighted.weight(1) << "\n\n";

    // An edge-weighted graph stores a weight for each existing edge.  For an
    // undirected graph, weight(v,w) and weight(w,v) are kept symmetric.
    ugraph_ewi edge_weighted(4);
    edge_weighted.add_edge(0, 1, 5);
    edge_weighted.add_edge(1, 2, 7);
    edge_weighted.add_edge(2, 3, 9);

    std::cout << "edge-weighted graph\n"
              << "  vertices: " << edge_weighted.num_vertices() << '\n'
              << "  edges: " << edge_weighted.num_edges() << '\n'
              << "  weight(1, 2): " << edge_weighted.weight(1, 2) << '\n'
              << "  weight(2, 1): " << edge_weighted.weight(2, 1)
              << " (symmetric)\n";

    return 0;
}
