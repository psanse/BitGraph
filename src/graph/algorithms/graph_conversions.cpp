 /**
  * @file graph_conversions.cpp
  * @brief Implements conversions between dense and sparse undirected graphs.
  *
  * This file implements the public convert_graph() overloads. Adjacency blocks
  * are copied between the ugraph and sparse_ugraph representations without
  * scanning individual edges.
  *
  * Each conversion resets the destination graph, preserves vertex indices and
  * graph metadata, and invalidates the destination's cached edge count because
  * adjacency storage is modified directly.
  *
  * Direct access to graph adjacency storage is confined to the internal
  * detail::GraphConversion helper.
  *
  * @date Created: 25/11/2016
  * @date Last updated: 22/09/2026
  * @author Pablo San Segundo
  */

#ifndef BITGRAPH_GRAPH_TYPE_CONVERSIONS_H
#define	BITGRAPH_GRAPH_TYPE_CONVERSIONS_H

#include "graph_conversions.h"

#include "utils/common.h"
#include "utils/logger.h"
#include <iostream>

namespace bitgraph {
	
	namespace detail {

		////////////////////////
		//
		// GraphConversion 
		// 
		// Conversions between different (unweighted) graph types of the GRAPH library
		// (stateless - globals)
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

				const int NV = sug.num_vertices();

				//allocation - empty graph of size NV
				ug.reset(static_cast<size_t>(NV), sug.path() + sug.name());

				//copies adjacency (non-empty) block array
				for (vertex_t v = 0; v < NV; ++v) {
					for (auto it = sug.adj_[v].cbegin(); it != sug.adj_[v].cend(); ++it) {

						////////////////////////////////////////////////////
						ug.adj_[v].block(it->idx_) = it->bb_;
						////////////////////////////////////////////////////

					}
				}

				// Direct block writes bypass add_edge().
				ug.edge_count_valid_ = false;		
			}


			/*
			* @brief conversion from ugraph to sparse_ugraph
			* @details: fast-fail policy- exits if error
			* @date	25/11/16
			*/
			static	void ug2sug(const ugraph& ug, sparse_ugraph& sug)
			{
				const int NV = ug.num_vertices();

				//allocation - empty graph of size NV
				sug.reset(NV, ug.path() + ug.name());

				//add edges	
				const block_index_t num_blocks = ug.num_blocks();
				for (vertex_t v = 0; v < NV; ++v) {
					for (block_index_t block = 0; block < num_blocks; ++block) {
						const BITBOARD bits = ug.adj_[v].block(block);
						
						if (bits != 0) {
							////////////////////////////////////////////////////
							sug.adj_[v].bitset().emplace_back(block, bits);
							////////////////////////////////////////////////////
						}

					}
				}

				// Direct block writes bypass add_edge().
				sug.edge_count_valid_ = false;		
			}

		};

	} //end of namespace detail

	void convert_graph(
		const sparse_ugraph& source,
		ugraph& destination)
	{
		detail::GraphConversion::sug2ug(source, destination);
	}

	void convert_graph(
		const ugraph& source,
		sparse_ugraph& destination)
	{
		detail::GraphConversion::ug2sug(source, destination);
	}

}//end of namespace bitgraph


#endif