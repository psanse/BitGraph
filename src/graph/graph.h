/** 
 * @file graph.h
 * @brief  The one-and-only header file for the main facade graph types of the BitScan library
 *         To be used by consumer code
 *		    
 * @comment: BitGraph is an efficient C++ library for simple graphs
 *			 BitGraph stores the adjacency matrix un full and each row is encoded as a
 *			 bitstring. BitGraph is at the core of a number of state-of-the-art-leading 
 *			 exact algorithms for NP-hard problems. 
 *
 * @dev: Pablo San Segundo (main developper)
 * @details: last_update 30/01/2026 
 * 
 *
 * Permission to use, modify and distribute this software is
 * granted provided that this copyright notice appears in all 
 * copies, in source code or in binaries. For precise terms 
 * see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any 
 * kind, express or implied, and with no claim as to its
 * suitability for any purpose.
 **/

#ifndef __GRAPH_INCLUDE_H__
#define __GRAPH_INCLUDE_H__

#include "graph_types.h"

#include "simple_ugraph.h"
#include "simple_graph_w.h"                                     // MUST BE AFTER ugraph type 
#include "simple_graph_ew.h"                                    // MUST BE AFTER ugraph type  

namespace bitgraph {

    // Core graphs
    using graph = Graph<bitarray>;                              // simple graph 
    using ugraph = Ugraph<bitarray>;                            // simple undirected graph
    using sparse_graph = Graph<sparse_bitarray>;                // simple sparse graph
    using sparse_ugraph = Ugraph<sparse_bitarray>;              // simple sparse undirected graph

    // Vertex-weighted
    using ugraph_w = Graph_W<ugraph, double>;                   // simple vertex weighted graph with double weights
    using ugraph_wi = Graph_W<ugraph, int>;                     // simple vertex weighted graph with int weights

    // Edge-weighted
    using ugraph_ew = Graph_EW<ugraph, double>;                 // simple edge weighted graph with double weights
    using ugraph_ewi = Graph_EW<ugraph, int>;                   // simple edge weighted graph with integer weights
}

//legacy code - to be removed in future versions

//#include "simple_ugraph.h"
//namespace bitgraph {
//	using graph = bitgraph::Graph<bitarray>;						//simple graph
//	using ugraph = bitgraph::Ugraph<bitarray>;						//simple undirected graph
//	using sparse_graph = bitgraph::Graph<sparse_bitarray>;			//simple sparse graph
//	using sparse_ugraph = bitgraph::Ugraph<sparse_bitarray>;		//simple sparse undirected graph
//}
//
//#include "simple_graph_w.h"										//MUST BE AFTER ugraph type 
//namespace bitgraph {
//	using ugraph_w = bitgraph::Graph_W<ugraph, double>;				//simple vertex weighted graph with double weights
//	using ugraph_wi = bitgraph::Graph_W<ugraph, int>;				//simple vertex weighted graph with int weights
//}
//
//#include "simple_graph_ew.h"									//MUST BE AFTER ugraph type 
//namespace bitgraph {
//	using ugraph_ew = bitgraph::Graph_EW<ugraph, double>;			//simple edge weighted graph with double weights
//	using ugraph_ewi = bitgraph::Graph_EW<ugraph, int>;				//simple edge weighted graph with int weights
//}

#endif