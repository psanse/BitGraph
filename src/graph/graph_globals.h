/**
* @file: grap_globals.h
* @brief: global information for GRAPH in BitGraph
* @details: created 30/06/2025
*/

#ifndef __GRAPH_GLOBALS_H__
#define __GRAPH_GLOBALS_H__

//useful alias for STL containers

namespace bitgraph {
	using vint = std::vector<int>;

	template<class W>
	using vecw = std::vector<W>;

	//...
}

//////////////////////////////////////////////
constexpr int DEFAULT_WEIGHT_MODULUS = 200;			//default modulus for edge-weights [Pullman 2008]
///////////////////////////////////////////////





#endif // __GRAPH_GLOBALS_H__
