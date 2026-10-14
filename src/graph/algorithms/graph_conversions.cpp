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

namespace bitgraph {
	
	namespace detail {

		/**
		 * @brief Utility class for converting between dense and sparse undirected graphs.
		 *
		 * @details Provides static conversion functions between `ugraph` and
		 *          `sparse_ugraph`, preserving the graph's vertices, adjacency
		 *          information, and name.
		 *
		 *          Conversions operate directly on bitblocks for efficiency,
		 *          bypassing the standard edge insertion interface.
		 *
		 *          The class is stateless and cannot be instantiated.
		 *
		 * @note Conversion functions invalidate the cached edge count of the
		 *       destination graph.
		 */
		class GraphConversion {
		public:

			/** @brief Deleted constructor: utility class cannot be instantiated. */
			GraphConversion() = delete;

			/**
			 * @brief Converts a sparse undirected graph to its dense representation.
			 *
			 * @param sug Source sparse undirected graph.
			 * @param ug Destination dense undirected graph (overwritten).
			 *
			 * @details Resets the destination graph and copies all non-zero adjacency
			 *          bitblocks directly into the dense representation.
			 *          Vertex indices and graph metadata are preserved.
			 *
			 * @note Invalidates the destination graph's cached edge count.
			 * @warning Memory allocation failures follow the library's fail-fast policy.
			 *
			 * @date 25/11/2016
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


			/**
			 * @brief Converts a dense undirected graph to its sparse representation.
			 *
			 * @param ug Source dense undirected graph.
			 * @param sug Destination sparse undirected graph (overwritten).
			 *
			 * @details Resets the destination graph and copies only non-zero adjacency
			 *          bitblocks into the sparse representation.
			 *          Vertex indices and graph metadata are preserved.
			 *
			 * @note Invalidates the destination graph's cached edge count.
			 * @warning Memory allocation failures follow the library's fail-fast policy.
			 *
			 * @date 25/11/2016
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


#endif // BITGRAPH_GRAPH_TYPE_CONVERSIONS_H