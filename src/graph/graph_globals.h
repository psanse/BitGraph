/**
* @file: graph_globals.h
* @brief: global information for GRAPH in BitGraph
* @details: created 30/06/2025
*/

#ifndef __GRAPH_GLOBALS_H__
#define __GRAPH_GLOBALS_H__

#include <vector>	

//useful alias / globlas for GRAPH

namespace bitgraph {

	using VertexList = std::vector<int>;
	using VertexMapping = std::vector<int>;
	using VertexOrdering = std::vector<int>;

	enum : int {
		DEFAULT_WEIGHT_MODULUS = 200		//default modulus for vertex and edge-weights [Pullman 2008]

		// @todo add more integal global constants as required
	};
}

#endif // __GRAPH_GLOBALS_H__
