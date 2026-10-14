/**
 * @file graph_unweighted.h
 * @brief Provides public aliases for unweighted BitGraph graph types.
 *
 * This header exposes convenient aliases for dense and sparse unweighted
 * graphs. Both directed and undirected graph representations are provided.
 *
 * The dense aliases use bitgraph::bitarray to represent adjacency sets,
 * whereas the sparse aliases use bitgraph::sparse_bitarray.
 *
 * Client code should normally use these aliases instead of naming the
 * underlying Graph and Ugraph class templates explicitly.
 *
 * @date created 01/02/2026
 * @date last update 20/09/2026
 * @author P. San Segundo
 */

#ifndef BITSCAN_GRAPH_UNWEIGHTED_H
#define BITSCAN_GRAPH_UNWEIGHTED_H

#include "simple_graph.h"
#include "simple_sparse_ugraph.h"

namespace bitgraph {

    // alias facade graph types
    using graph = Graph<bitarray>;                              // simple graph 
    using ugraph = Ugraph<bitarray>;                            // simple undirected graph
    using sparse_graph = Graph<sparse_bitarray>;                // simple sparse graph
    using sparse_ugraph = Ugraph<sparse_bitarray>;              // simple sparse undirected graph
}


#endif // BITSCAN_GRAPH_UNWEIGHTED_H__