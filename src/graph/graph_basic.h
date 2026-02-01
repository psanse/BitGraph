/** 
 * @file graph_basic.h
 * @brief  
 *		    
 * @comment:  
 *
 * @author: pss
 * @date 01/02/2026 
 **/

#ifndef __GRAPH_BASIC_H__
#define __GRAPH_BASIC_H__

#include "graph_types.h"

#include "simple_ugraph.h"

namespace bitgraph {
    	

	// note that implementations of Graph<Bitset> and Ugraph<BitsetT> are in simple_graph and simple_ugraph.h 
	// there are no specializations for BitsetT = BBScan>   

    // specialized implementations of graph types for sparse_bitarray







    // alias facade graph types
    using graph = Graph<bitarray>;                              // simple graph 
    using ugraph = Ugraph<bitarray>;                            // simple undirected graph
    using sparse_graph = Graph<sparse_bitarray>;                // simple sparse graph
    using sparse_ugraph = Ugraph<sparse_bitarray>;              // simple sparse undirected graph
}

///////////////////////////////
// implementation in header file


#endif // __GRAPH_BASIC_H__