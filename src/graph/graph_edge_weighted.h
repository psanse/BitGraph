/** 
 * @file graph_edge_weighted.h
 * @brief  
 *		    
 * @comment:  
 *
 * @author: pss (main developper)
 * @date 01/02/2026 
 **/

#ifndef __GRAPH_INCLUDE_H__
#define __GRAPH_INCLUDE_H__

#include "graph_types.h"
#include "graph_basic.h"
#include "simple_graph_ew.h"                                    // MUST BE AFTER graph_basic.h

namespace bitgraph {

	// implementation of simple undirected graph
     


      
	// edge-weighted facade types
    using ugraph_ew = Graph_EW<ugraph, double>;                 // simple edge weighted graph with double weights
    using ugraph_ewi = Graph_EW<ugraph, int>;                   // simple edge weighted graph with integer weights
}

//////////////////////
// implementation in header files



#endif // __GRAPH_INCLUDE_H__