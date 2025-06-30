/**
* @file: grap_globals.h
* @brief: global information for GRAPH in BitGraph
* @details: created 30/06/2025
*/

#ifndef __GRAPH_GLOBALS_H__
#define __GRAPH_GLOBALS_H__

#include <vector>	

//useful alias for STL containers

namespace bitgraph {
	using vint = std::vector<int>;

	template<class W>
	using vecw = std::vector<W>;

	//...TODO- ADD more aliases required
}

//default modulus for vertex and edge-weights [Pullman 2008]
constexpr int DEFAULT_WEIGHT_MODULUS = 200;		

//TODO...add more global constants as required



#endif // __GRAPH_GLOBALS_H__
