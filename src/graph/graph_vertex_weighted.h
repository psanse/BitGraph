/** 
 * @file graph_vertex_weighted.h
 * @brief  
 *		    
 * @comment: 
 *
 * @authors: pss (main developper)
 * @date 01/02/2026 
 * 
 **/

#ifndef __GRAPH_VERTEX_WEIGHTED_H__
#define __GRAPH_VERTEX_WEIGHTED_H__

#include "graph_types.h"
#include "graph_basic.h"
#include "simple_graph_w.h"                                     // MUST BE AFTER graph_basic.h 


namespace bitgraph {
    
    // Vertex-weighted
    using ugraph_w = Graph_W<ugraph, double>;                   // simple vertex weighted graph with double weights
    using ugraph_wi = Graph_W<ugraph, int>;                     // simple vertex weighted graph with int weights

 
}


#endif //end __GRAPH_VERTEX_WEIGHTED_H__