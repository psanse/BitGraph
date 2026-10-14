/**
 * @file graph.h
 * @brief Public facade header BitGraph graph types.
 *
 * @details
 * This header is the main consumer-facing entry point for BitGraph graph
 * representations. It brings together the graph type definitions, unweighted
 * graph aliases, and vertex- and edge-weighted undirected graph
 * specializations.
 *
 * BitGraph represents graph adjacency through bitset-backed rows, enabling
 * compact storage and fast set-oriented operations used by exact algorithms
 * for difficult graph problems. The concrete aliases exposed by the included
 * headers cover directed and undirected unweighted graphs, sparse variants,
 * and weighted undirected graphs with common numeric weight types.
 *
 * This facade contains type declarations and includes; graph construction and
 * algorithm behavior are documented in the specialized headers it includes.
 *
 * @author pss (main developer)
 * @date Last updated: 1 February 2026
 *
 * @see graph_types.h
 * @see graph_unweighted.h
 * @see ugraph_vertex_weighted.h
 * @see ugraph_edge_weighted.h
 */

#ifndef BITSCAN_GRAPH_MAIN_HEADER_H
#define BITSCAN_GRAPH_MAIN_HEADER_H

#include "graph_types.h"
#include "graph_unweighted.h"				
#include "ugraph_vertex_weighted.h"
#include "ugraph_edge_weighted.h"

#endif // BITSCAN_GRAPH_MAIN_HEADER_H
