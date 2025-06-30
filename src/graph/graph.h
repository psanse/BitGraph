/** 
 * @file graph.h
 * @brief  The one and only header file for the GRAPH 1.0 library
 *		    simple graphs (encoded as BITSCAN bitstrings)
 *
 * @comment: The GRAPH 1.0 library is an efficient C++ library for simple graphs
 *			 GRAPH stores the adjacency matrix un full and each row is encoded as a
 *			 bitstring. GRAPH is at the core of a number of state-of-the-art-leading 
 *			 exact algorithms for NP-hard problems. 
 *
 * @author: Pablo San Segundo (main developper)
 * @version: 1.0 
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


#include "simple_ugraph.h"
using graph = bitgraph::Graph<bitarray>;						//simple graph
using ugraph = bitgraph::Ugraph<bitarray>;						//simple undirected graph
using sparse_graph = bitgraph::Graph<sparse_bitarray>;			//simple sparse graph
using sparse_ugraph = bitgraph::Ugraph<sparse_bitarray>;		//simple sparse undirected graph

#include "simple_graph_w.h"										//MUST BE AFTER ugraph type 
using ugraph_w = bitgraph::Graph_W<ugraph, double>;				//simple vertex weighted graph with double weights
using ugraph_wi = bitgraph::Graph_W<ugraph, int>;				//simple vertex weighted graph with int weights

#include "simple_graph_ew.h"									//MUST BE AFTER ugraph type 
using ugraph_ew = bitgraph::Graph_EW<ugraph, double>;			//simple edge weighted graph with double weights
using ugraph_ewi = bitgraph::Graph_EW<ugraph, int>;				//simple edge weighted graph with int weights


#endif