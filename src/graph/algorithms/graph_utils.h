/**
* @file graph_utils.h
* @brief main header for namespace gfunc providing general purpose functions for graphs
* @details: 
*  - date 07/3/2017
*  - last_modified 30/01/2025  
* @author pss
*
* @todo CHECK edgeW::ew_shift_2_highest_index function  (09/01/25)
**/

#ifndef BITGRAPH_GRAPH_GRAPHS_UTILS_H
#define BITGRAPH_GRAPH_GRAPHS_UTILS_H

//#include "graph/simple_ugraph.h"
//#include "graph/simple_graph_w.h"				// must be after ugraph include
//#include "graph/simple_graph_ew.h"				// must be after ugraph include
//#include "graph/graph_traits.h"
//#include "utils/logger.h"
//#include "utils/sort_utils.h"
//#include <algorithm>
//#include <iostream>


#include "unweighted_graph_utils.h"
#include "vertex_weighted_graph_utils.h"
#include "edge_weighted_graph_utils.h"

namespace bitgraph {

	// Backward compatibility alias for graph_utils namespace
	namespace gfunc = graph_utils;

} // namespace bitgraph



#endif // BITGRAPH_GRAPH_GRAPH_UTILS_H
