/*
 * @file graph_conversions.h 
 * @brief sconversions between different graph types of the GRAPH library 
 *
 * @creation_date 25/11/16
 * @last_update 27/01/25
 * @author pss
 *
 */

#ifndef __GRAPH_TYPE_CONVERSIONS_H__
#define	__GRAPH_TYPE_CONVERSIONS_H__

#include "graph/simple_ugraph.h"								//currently, contains the only converted graph types
#include "utils/common.h"
#include "utils/logger.h"
#include <iostream>

using namespace std;

//alias for the converted graph types
using ugraph = Ugraph<bitarray>;						//simple undirected graph type
using sparse_ugraph = Ugraph<sparse_bitarray>;			//simple sparse undirected graph type

////////////////////////
//
// GraphConversion 
// 
// Conversions between different graph types of the GRAPH library
// (stateless - globals)
// 
// Comment: namespace is not used for a clean friendship declaration in the Graph classes
//
////////////////////////

class GraphConversion {
public:

	//constructor - deleted
	GraphConversion() = delete;

	/*
	* @brief conversion from sparse_ugraph to ugraph
	* @returns 0 if conversion is successful, -1 otherwise
	* @date	25/11/16
	*/
	static int sug2ug(const sparse_ugraph& sug, ugraph& ug)
	{

		auto NV = sug.number_of_vertices();

		//allocation - empty graph of size NV
		if (ug.reset(NV) == -1) {
			LOG_ERROR("memory for graph not allocated- GraphConversion::sug2ug");
			return -1;
		}

		//copies adjacency (non-empty) block array
		for (auto v = 0; v < NV; ++v) {
			for (auto it = sug.adj_[v].cbegin(); it != sug.adj_[v].cend(); ++it) {

				////////////////////////////////////////////////////
				ug.adj_[v].block(it->idx_) = it->bb_;
				////////////////////////////////////////////////////

			}
		}

		//name
		ug.name(sug.name());

		return 0;
	}


	/*
	* @brief conversion from ugraph to sparse_ugraph
	* @returns 0 if conversion is successful, -1 otherwise
	* @date	25/11/16
	*/
	static	int ug2sug(const ugraph& ug, sparse_ugraph& sug)
	{
		auto NV = ug.number_of_vertices();

		//allocation - empty graph of size NV
		if (sug.reset(NV) == -1) {
			LOG_ERROR("memory for graph not allocated- GraphConversion::sug2ug");
			return -1;
		}

		//add edges	
		auto blockID = ug.number_of_blocks();
		for (auto v = 0; v < NV; ++v) {
			for (auto nBB = 0; nBB < blockID; ++nBB) {

				BITBOARD bb = ug.adj_[v].block(nBB);
				if (bb != 0) {
					////////////////////////////////////////////////////
					sug.adj_[v].bitset().emplace_back(nBB, bb);
					////////////////////////////////////////////////////
				}

			}
		}

		//name
		sug.name(ug.name());

		return 0;
	}

};



#endif