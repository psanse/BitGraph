/**
 * @file graph_conversions.h 
 * @brief sconversions between different graph types of the GRAPH library 
 *
 * @creation_date 25/11/16
 * @last_update 30/06/25
 * @author pss
 *
 **/

#ifndef __GRAPH_TYPE_CONVERSIONS_H__
#define	__GRAPH_TYPE_CONVERSIONS_H__

#include "graph/simple_ugraph.h"								//currently, contains the only converted graph types
#include "utils/common.h"
#include "utils/logger.h"
#include <iostream>

using namespace std;

//alias for the converted graph types
namespace bitgraph {
	using ugraph = bitgraph::Ugraph<bitarray>;						//simple undirected graph type
	using sparse_ugraph = bitgraph::Ugraph<sparse_bitarray>;		//simple sparse undirected graph type
}

namespace bitgraph {
	namespace _impl {

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
			* @details: failfast policy - exits if memory allocation fails
			* @date	25/11/16
			*/
			static void sug2ug(const sparse_ugraph& sug, ugraph& ug)
			{

				auto NV = sug.num_vertices();

				//allocation - empty graph of size NV
				ug.reset(NV);
				
				//copies adjacency (non-empty) block array
				for (auto v = 0; v < NV; ++v) {
					for (auto it = sug.adj_[v].cbegin(); it != sug.adj_[v].cend(); ++it) {

						////////////////////////////////////////////////////
						ug.adj_[v].block(it->idx_) = it->bb_;
						////////////////////////////////////////////////////

					}
				}

				//name
				ug.set_name(sug.name());				
			}


			/*
			* @brief conversion from ugraph to sparse_ugraph
			* @details: fast-fail policy- exits if error
			* @date	25/11/16
			*/
			static	void ug2sug(const ugraph& ug, sparse_ugraph& sug)
			{
				auto NV = ug.num_vertices();

				//allocation - empty graph of size NV
				sug.reset(NV);
			
				//add edges	
				auto blockID = ug.num_blocks();
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
				sug.set_name(ug.name());

			}

		};

	} //end of namespace _impl

	using _impl::GraphConversion;

}//end of namespace bitgraph


#endif