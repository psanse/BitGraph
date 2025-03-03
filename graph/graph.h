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
 *
 *
 **/

#ifndef __GRAPH_INCLUDE_H__
#define __GRAPH_INCLUDE_H__

//#include "simple_ugraph.h"
#include "simple_sparse_graph.h"
#include "simple_sparse_ugraph.h"

typedef Graph<bitarray> graph;						//simple graph
typedef Ugraph<bitarray> ugraph;					//simple undirected graph
typedef Graph<sparse_bitarray> sparse_graph;		//simple sparse graph
typedef Ugraph<sparse_bitarray> sparse_ugraph;		//simple sparse undirected graph


#include "simple_graph_w.h"							//MUST BE AFTER ugraph type 
typedef Graph_W<ugraph, double> ugraph_w;			//simple vertex weighted graph with double weights
typedef Graph_W<ugraph, int> ugraph_wi;				//simple vertex weighted graph with int weights

#include "simple_graph_ew.h"						//MUST BE AFTER ugraph type 
typedef Graph_EW<ugraph, double> ugraph_ew;			//simple edge weighted graph with double weights
typedef Graph_EW<ugraph, int> ugraph_ewi;			//simple edge weighted graph with int weights

#endif