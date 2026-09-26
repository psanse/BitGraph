/**
* @file unweighted_graph_utils.h
* @brief main header for namespace gfunc providing general purpose functions for graphs
* @details: 
*  - date 07/3/2017
*  - last_modified 30/01/2025  
* @author pss
*
* @todo CHECK edgeW::ew_shift_2_highest_index function  (09/01/25)
**/

#ifndef BITGRAPH_GRAPH_UNWEIGHTED_GRAPH_UTILS_H
#define BITGRAPH_GRAPH_UNWEIGHTED_GRAPH_UTILS_H

#include "graph/graph_unweighted.h"
#include "graph/graph_traits.h"
#include "utils/logger.h"
#include "utils/sort_utils.h"
#include <algorithm>
#include <iostream>

namespace bitgraph {

	namespace graph_utils {

		/**
		 * @brief Collects the neighbors of a vertex contained in a reference set.
		 *
		 * For directed graphs, neighbors are the outgoing neighbors of @p vertex.
		 * Vertex and edge weights, when present, are ignored.
		 *
		 * @tparam GraphT Graph type providing `neighbors(vertex)` and
		 *         `vertex_set_type`.
		 * @param graph Input graph.
		 * @param vertex Vertex whose neighborhood is examined.
		 * @param reference Set restricting the returned vertices.
		 * @param result Receives the vertices in `N(vertex) ∩ reference`.
		 * @return Number of vertices written to @p result.
		 */
		template<class GraphT>
		std::size_t neighbors_in(
			const GraphT& graph,
			vertex_t vertex,
			const typename GraphT::vertex_set_type& reference,
			vertices& result)
		{
			result.clear();
			result.reserve(
				static_cast<std::size_t>(graph.num_vertices()));

			const int block_count = graph.num_blocks();

			for (int block = 0; block < block_count; ++block) {
				BITBOARD bits =
					graph.neighbors(vertex).block(block) &
					reference.block(block);

				const vertex_t offset =
					static_cast<vertex_t>(WMUL(block));

				while (bits != 0) {
					const int position = bblock::lsb(bits);

					result.push_back(
						offset + static_cast<vertex_t>(position));

					// Remove the least-significant set bit.
					bits &= bits - 1;
				}
			}

			return result.size();
		}

		// aliasfor backward compatibility

		template<class GraphT>
		std::size_t neighbors(
			const GraphT& graph,
			vertex_t vertex,
			const typename GraphT::vertex_set_type& reference,
			vertices& result)
		{
			return neighbors_in(
				graph,
				vertex,
				reference,
				result);
		}
		
		/**
		 * @brief Collects neighbors of @p vertex whose indices are greater than
		 *        @p vertex and which belong to @p reference.
		 *
		 * Computes
		 * `N(vertex) ∩ reference ∩ {vertex + 1, ..., graph.num_vertices() - 1}`.
		 * For directed graphs, `N(vertex)` denotes the outgoing neighborhood.
		 * Graph weights, if present, are ignored.
		 *
		 * @tparam GraphT Graph type providing `neighbors()`, `num_blocks()`, and
		 *         `vertex_set_type`.
		 * @param graph Input graph.
		 * @param vertex Vertex whose neighborhood is examined.
		 * @param reference Bitset restricting the returned vertices.
		 * @param result Receives the selected vertices in increasing index order.
		 *        Its previous contents are discarded.
		 * @return Number of vertices written to @p result.
		 *
		 * @pre `0 <= vertex < graph.num_vertices()`.
		 * @pre @p reference has a capacity compatible with @p graph.
		 * @date: created 03/09/18,
		 * @last_update: 26/09/26
		 */
		template<class GraphT>
		std::size_t neighbors_after(
			const GraphT& graph,
			vertex_t vertex,
			const typename GraphT::vertex_set_type& reference,
			vertices& result)
		{
			assert(vertex >= 0 && vertex < graph.num_vertices());

			result.clear();
			result.reserve(
				static_cast<std::size_t>(graph.num_vertices()));

			const auto& neighborhood = graph.neighbors(vertex);
			const block_index_t first_block = WDIV(vertex);
			const int block_count = graph.num_blocks();

			// Process the block containing vertex, excluding vertex itself and all
			// preceding positions.
			BITBOARD bits =
				neighborhood.block(first_block) &
				reference.block(first_block) &
				bblock::MASK_1_HIGH(WMOD(vertex) + 1);
				//Tables::mask_high[WMOD(vertex)];

			const int first_offset = WMUL(first_block);

			while (bits != 0) {
				const int position = bblock::lsb(bits);
				result.emplace_back(first_offset + position);

				// Remove the least-significant set bit.
				bits &= bits - 1;
			}

			// All positions in subsequent blocks are greater than vertex.
			for (block_index_t block = first_block + 1;
				block < block_count;
				++block)
			{
				bits =
					neighborhood.block(block) &
					reference.block(block);

				const int offset = WMUL(block);

				while (bits != 0) {
					const int position = bblock::lsb(bits);
					result.emplace_back(offset + position);

					bits &= bits - 1;
				}
			}
			return result.size();
		}

		/**
		 * @brief Reinitializes a graph as a complete graph.
		 *
		 * Creates a graph with @p vertex_count vertices and every possible edge
		 * between distinct vertices. For directed graphs, both arcs `(u, v)` and
		 * `(v, u)` are created. For undirected graphs, each edge `{u, v}` is created
		 * once. Self-loops are not created.
		 *
		 * Any previous graph contents are discarded.
		 *
		 * @tparam GraphT Supported graph type.
		 * @param graph Graph to reinitialize.
		 * @param vertex_count Number of vertices in the resulting graph.
		 *
		 * @post `graph.num_vertices() == vertex_count`.
		 */
		template<class GraphT>
		void create_complete(
			GraphT& graph,
			std::size_t vertex_count)
		{
			detail::create_complete_impl(
				graph,
				vertex_count,
				graph_traits<GraphT>::is_undirected_tag{});
		}
		
		/**
		 * @brief Returns the jump pattern of vertex v for the current labeling 0..n-1.
		 *
		 * jump[d] == 1  <=>  edge(v, (v+d) mod n) exists, for d=1..n-1
		 *
		 * Required graph interface:
		 *   - int number_of_vertices() const;
		 *   - bool is_edge(int u, int v) const;
		 */
		template <class Graph>
		inline
			std::vector<unsigned char> circulant_jump_pattern(
				const Graph& g, 
				int v)
		{
			const int n = g.num_vertices();
			std::vector<unsigned char> jump(static_cast<std::size_t>(n), 0);

			for (int d = 1; d < n; ++d) {
				const int u = (v + d) % n;
				jump[d] = g.is_edge(v, u) ? 1 : 0;
			}

			return jump;
		}

		/**
		 * @brief Checks whether an undirected simple graph is circulant
		 *        with the current labeling 0..n-1.
		 *
		 * Criterion:
		 *   all vertices must have the same modular jump pattern.
		 *
		 * @param g input graph
		 * @param[out] step_set optional full jump list S
		 * @return true iff the graph is circulant with current labeling
		 * @details:
		 *	 - usually for undirected graphs, only half of the jump pattern is needed (d < n/2) due to symmetry,
		 *     but this function checks the full pattern for generality and to allow directed graphs in the future.
		  */
		template <class Graph>
		inline
			bool is_circulant(
				const Graph& g, 
				std::vector<Vertex>* step_set = nullptr)
		{
			const int NV = g.num_vertices();

			if (NV <= 0) return false;
			if (step_set) step_set->clear();

			// simple graph: no loops
			for (int v = 0; v < NV; ++v) {
				if (g.is_edge(v, v)) return false;
			}

			if (NV == 1) return true;

			// reference jump pattern
			const auto ref = circulant_jump_pattern(g, 0);

			// undirected circulant => symmetry d <-> n-d
			for (int d = 1; d < NV; ++d) {
				if (ref[d] != ref[(NV - d) % NV]) {
					return false;
				}
			}

			// every vertex must have the same jump pattern
			for (Vertex v = 1; v < NV; ++v) {
				const auto pat = circulant_jump_pattern(g, v);
				if (pat != ref) return false;
			}

			// output
			if (step_set) {
				for (int d = 1; d < NV; ++d) {
					if (ref[d]) step_set->push_back(d);
				}
			}

			return true;
		}

		/*
		* @brief Determines if an induced subgraph has no edges
		*		 If this is not the case provides the first edge found
		*
		* @param g: a simple graph
		* @param bbsg: bitset of vertices that induces the subgraph
		* @param edge: output first-found edge (pair of vertices) in the induced subgraph
		* @returns TRUE if the induced subgraph has no edges, FALSE otherwise
		* 
		* @todo - change returned edge to std::pair<int,int> (29/01/2026)
		*/
		template<class GraphT>
		bool is_edgeFree_subgraph(
			GraphT& g, 
			const typename GraphT::vertex_set_type& bbsg,
			std::vector<Vertex>& edge) 
		{
			//cleans output edge
			edge.clear();

			//decodes subgraph to vector
			Vertices lv;
			bbsg.extract(lv);

			//singleton input bitset - induced subgraph has no edges
			int NV = static_cast<int>(lv.size());
			if (NV <= 1) { return true; }

			//searches for an edge in the induced subgraph
			for (int i = 0; i < NV - 1; ++i) {
				for (int j = i + 1; j < NV; ++j) {

					if (g.is_edge(lv[i], lv[j])) {
						edge.emplace_back(lv[i]);
						edge.emplace_back(lv[j]);
						return false;
					}
				}
			}

			return true;
		}

		/*
		* @brief Determines if an induced subgraph has no triangles
		*		 If this is not the case provides the first triangle found
		*
		* @param g: a simple graph
		* @param bbsg: bitset of vertices that induces the subgraph
		* @param triangle: output first-found triangle (triplet of vertices) in the induced subgraph
		* @returns TRUE if the induced subgraph has no edges, FALSE otherwise
		*/
		template<class GraphT>
		bool is_triangleFree_subgraph(
			GraphT& g,
			const typename GraphT::vertex_set_type& bbsg,
			std::vector<Vertex>& triangle) 
		{

			//cleans output 
			triangle.clear();

			//decodes subgraph to vector
			vertices lv;
			bbsg.extract(lv);

			//input bitset of size 2 - induced subgraph has no triangles
			auto NV = lv.size();
			if (NV <= 2) { return true; }

			//searches for an edge in the induced subgraph
			for (auto i = 0; i < NV - 2; ++i) {
				for (auto j = i + 1; j < NV - 1; ++j) {

					if (g.is_edge(lv[i], lv[j])) {		//determines an edge

						for (auto k = j + 1; k < NV; ++k) {

							if (g.is_edge(lv[j], lv[k]) && g.is_edge(lv[i], lv[k])) {		//determines a triangle with edge (i,j)
								triangle.emplace_back(lv[i]);
								triangle.emplace_back(lv[j]);
								triangle.emplace_back(lv[k]);
								return false;
							}
						}

					}
				}
			}
			return true;
		}
		

		/*
		* @brief Sorts a set of vertices (lv) according to their degree wrt to a reference set (lref) of vertices.
		*
		*		  No restrictions between lv and lref (can have non-empty intersection)
		*
		* @param g: a simple graph
		* @param lv: list of vertices to be sorted
		* @param lref: list of reference vertices
		* @param min_sort: sorting criteria
		* @returns sorted list of vertices lv
		*/
		template<class GraphT>
		Vertices& sort_deg(
			const GraphT& g, 
			Vertices& lv, 
			const Vertices& lref,
			bool min_sort = true) 
		{
			int deg = 0;

			//determine the degree of each vertex in lv wrt to ref
			std::vector<int> ldeg(g.num_vertices(), 0);
			for (auto v : lv) {
				deg = 0;
				for (auto w : lref) {
					if (g.is_edge(v, w)) {
						deg++;
					}
				}
				ldeg[v] = deg;
			}

			//I/O
			//utils::stl::print_collection(ldeg); 

			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv.begin(), lv.end(), utils::has_smaller_val<int, vector<int>>(ldeg));
			}
			else {
				std::sort(lv.begin(), lv.end(), utils::has_greater_val<int, vector<int>>(ldeg));
			}

			return lv;
		}

		/*
		* @brief Sorts a set of vertices (lv) according to their degree wrt to a reference set (bbref) of vertices.
		*
		*		  No restrictions between lv and bbref (can have non-empty intersection)
		*
		* @param g: a simple graph
		* @param lv: list of vertices to be sorted
		* @param bbref: list (bitset) of reference vertices
		* @param min_sort: sorting criteria
		* @returns sorted list of vertices lv
		*/
		template<class GraphT>
		Vertices& sort_deg(
			const GraphT& g,
			Vertices& lv, 
			typename  GraphT::vertex_bitset_t& bbref,
			bool min_sort = true)
		{

			//int deg = 0;

			//determine the degree of each vertex in lv wrt to ref
			std::vector<int> ldeg(g.num_vertices(), 0);
			for (auto v : lv) {
				ldeg[v] = g.degree(v, bbref);
			}

			//I/O
			//utils::stl::print_collection(ldeg); 

			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv.begin(), lv.end(), utils::has_smaller_val<int, vector<int>>(ldeg));
			}
			else {
				std::sort(lv.begin(), lv.end(), utils::has_greater_val<int, vector<int>>(ldeg));
			}

			return lv;
		}

		
		//////////////////////////////////////

		namespace detail {
			
			template<class GraphT>
			void create_complete_impl(
				GraphT& graph,
				std::size_t vertex_count,
				std::false_type /* directed */)
			{
				graph.reset(vertex_count);

				const vertex_t count =
					static_cast<vertex_t>(vertex_count);

				for (vertex_t v = 0; v < count; ++v) {
					for (vertex_t w = 0; w < count; ++w) {
						if (v != w) {
							graph.add_edge(v, w);
						}
					}
				}
			}

			template<class GraphT>
			void create_complete_impl(
				GraphT& graph,
				std::size_t vertex_count,
				std::true_type /* undirected */)
			{
				graph.reset(vertex_count);

				const vertex_t count =
					static_cast<vertex_t>(vertex_count);

				for (vertex_t v = 0; v + 1 < count; ++v) {
					for (vertex_t w = v + 1; w < count;	++w)
					{
						graph.add_edge(v, w);
					}
				}
			}									

		} // end namespace detail
				
	} //namespace graph_utils

} //namespace bitgraph

#endif // BITGRAPH_GRAPH_UNWEIGHTED_GRAPH_UTILS_H
