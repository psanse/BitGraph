/**
 * @file graph_reading_simple.cpp
 * @brief Demonstrates reading an undirected graph from a supported file format.
 *
 * The GRAPH reader tries the supported input formats when constructing a
 * `ugraph`: DIMACS, Matrix Market, and edge-list files are accepted. Pass the
 * input filename as the only command-line argument.
 */

#include <iostream>
#include <string>

#include "graph/graph.h"

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: graph_reading_simple <graph-file>\n"
                  << "Supported formats: DIMACS, Matrix Market, edge list\n";
        return 1;
    }

    const std::string filename = argv[1];

    // The graph constructor detects and reads the supported input format.
    bitgraph::ugraph graph(filename);

    std::cout << "Read: " << filename << '\n'
              << "  vertices: " << graph.num_vertices() << '\n'
              << "  edges: " << graph.num_edges() << '\n'
              << "  density: " << graph.density() << '\n';

    return 0;
}
