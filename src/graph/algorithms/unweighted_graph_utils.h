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
		 * @brief Specifies the ordering used when sorting vertices by degree.
		 */
		enum class degree_order {
			nondecreasing,  ///< Lower-degree vertices appear first.
			nonincreasing   ///< Higher-degree vertices appear first.
		};


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
		 * @brief Computes the cyclic adjacency pattern of a vertex.
		 *
		 * For every cyclic offset `d` in `[1, n)`, the returned pattern satisfies:
		 *
		 * `pattern[d] == 1` if and only if
		 * `graph.is_edge(vertex, (vertex + d) % n)` is true.
		 *
		 * Position zero is always zero because it would correspond to the vertex
		 * itself.
		 *
		 * @tparam GraphT Graph type providing `num_vertices()` and `is_edge()`.
		 * @param graph Input graph whose vertices are labeled `[0, n)`.
		 * @param vertex Vertex whose cyclic adjacency pattern is computed.
		 * @return Vector of size `graph.num_vertices()` containing zeros and ones.
		 *
		 * @pre `0 <= vertex < graph.num_vertices()`.
		 */
		template<class GraphT>
		std::vector<unsigned char> circulant_jump_pattern(
			const GraphT& graph,
			vertex_t vertex)
		{
			assert(
				vertex >= 0 &&
				vertex < graph.num_vertices());

			const std::size_t vertex_count =
				static_cast<std::size_t>(graph.num_vertices());

			std::vector<unsigned char> pattern(
				vertex_count,
				static_cast<unsigned char>(0));

			for (std::size_t offset = 1;
				offset < vertex_count;
				++offset)
			{
				const vertex_t target = static_cast<vertex_t>(
					(static_cast<std::size_t>(vertex) + offset) %
					vertex_count);

				pattern[offset] =
					graph.is_edge(vertex, target)
					? static_cast<unsigned char>(1)
					: static_cast<unsigned char>(0);
			}

			return pattern;
		}

		/**
		 * @brief Determines whether a simple undirected graph is circulant under its
		 *        current vertex labeling.
		 *
		 * A graph with vertices labeled `[0, n)` is considered circulant when every
		 * vertex has the same cyclic adjacency pattern. Equivalently, adjacency is
		 * invariant under the cyclic relabeling `v -> (v + 1) mod n`.
		 *
		 * When @p step_set is not null and the graph is circulant, it receives the
		 * complete set of offsets `d` in `[1, n)` for which `(0, d)` is an edge.
		 * For an undirected circulant graph, offset `d` is present if and only if
		 * offset `n - d` is present.
		 *
		 * @tparam GraphT Undirected graph type providing `num_vertices()` and
		 *         `is_edge()`.
		 * @param graph Input graph.
		 * @param[out] step_set Optional destination for the complete set of cyclic
		 *             adjacency offsets. It is cleared before the graph is examined.
		 *
		 * @retval true The graph is circulant under its current labeling.
		 * @retval false The graph is empty, contains a self-loop, or is not circulant
		 *         under its current labeling.
		 *
		 * @note Vertex and edge weights, if present, are ignored.
		 * @note The empty graph is considered non-circulant by this function.
		 */
		template<class GraphT>
		bool is_circulant(
			const GraphT& graph,
			std::vector<vertex_t>* step_set = nullptr)
		{
			static_assert(
				graph_traits<GraphT>::is_undirected,
				"is_circulant() requires an undirected graph type");

			if (step_set != nullptr) {
				step_set->clear();
			}

			const int vertex_count =
				graph.num_vertices();

			if (vertex_count <= 0) {
				return false;
			}

			// A simple graph must not contain self-loops.
			for (vertex_t vertex = 0;
				vertex < vertex_count;
				++vertex)
			{
				if (graph.is_edge(vertex, vertex)) {
					return false;
				}
			}

			// singleton vertex graph is circulant
			if (vertex_count == 1) {
				return true;
			}

			const auto reference_pattern =
				circulant_jump_pattern(graph, 0);

			// An undirected cyclic pattern must be symmetric under d -> n - d.
			for (vertex_t offset = 1;
				offset < vertex_count;
				++offset)
			{
				const vertex_t reverse_offset =
					vertex_count - offset;

				if (reference_pattern[offset] !=
					reference_pattern[reverse_offset])
				{
					return false;
				}
			}

			// Every vertex must have the same cyclic adjacency pattern.
			for (vertex_t vertex = 1;
				vertex < vertex_count;
				++vertex)
			{
				if (circulant_jump_pattern(graph, vertex) !=
					reference_pattern)
				{
					return false;
				}
			}

			if (step_set != nullptr) {
				for (vertex_t offset = 1;
					offset < vertex_count;
					++offset)
				{
					if (reference_pattern[offset] != 0) {
						step_set->push_back(offset);
					}
				}
			}

			return true;
		}

		/**
		 * @brief Determines whether an induced subgraph contains no edges.
		 *
		 * Examines the subgraph induced by @p vertex_set. If an edge is found, its
		 * endpoints are written to @p edge.
		 *
		 * @tparam GraphT Undirected graph type.
		 * @param graph Input graph.
		 * @param vertex_set Set of vertices inducing the subgraph.
		 * @param[out] edge Receives the endpoints of the first edge found. Its value
		 *             is meaningful only when the function returns `false`.
		 *
		 * @retval true The induced subgraph contains no edges.
		 * @retval false The induced subgraph contains an edge, whose endpoints are
		 *         returned in @p edge.
		 *
		 * @note Vertex and edge weights, if present, are ignored.
		 * @note The running time is O(k^2), where `k` is the number of vertices in
		 *       @p vertex_set.
		 */
		template<class GraphT>
		bool is_edge_free_induced_subgraph(
			const GraphT& graph,
			const typename GraphT::vertex_set_type& vertex_set,
			std::pair<vertex_t, vertex_t>& edge)
		{
			static_assert(
				graph_traits<GraphT>::is_undirected,
				"is_edge_free_induced_subgraph() requires an undirected graph");

			edge = std::pair<vertex_t, vertex_t>{};

			vertices induced_vertices;
			vertex_set.extract(induced_vertices);

			const std::size_t vertex_count =
				induced_vertices.size();

			for (std::size_t i = 0; i + 1 < vertex_count; ++i) {
				for (std::size_t j = i + 1; j < vertex_count; ++j)
				{
					const vertex_t first = induced_vertices[i];
					const vertex_t second = induced_vertices[j];

					if (graph.is_edge(first, second)) {
						edge = std::make_pair(first, second);
						return false;
					}
				}
			}			

			return true;
		}

		/*
		* @brief API for backward compatibility - to be removed in the future
		*/
		template<class GraphT>
		bool is_edgeFree_subgraph(
			GraphT& graph,
			const typename GraphT::vertex_set_type& vertex_set,
			std::pair<vertex_t, vertex_t>& edge)
		{
			return is_edge_free_induced_subgraph<GraphT>(
				graph,
				vertex_set,
				edge
			);
		}

		/**
		 * @brief Determines whether an induced subgraph is triangle-free.
		 *
		 * Examines the subgraph induced by @p vertex_set. If a triangle is found,
		 * its three vertices are written to @p triangle.
		 *
		 * @tparam GraphT Undirected graph type.
		 * @param graph Input graph.
		 * @param vertex_set Set of vertices inducing the subgraph.
		 * @param[out] triangle Receives the vertices of the first triangle found.
		 *             Its previous contents are discarded.
		 *
		 * @retval true The induced subgraph contains no triangle; @p triangle is empty.
		 * @retval false A triangle was found; @p triangle contains its three vertices.
		 *
		 * @note Vertex and edge weights, if present, are ignored.
		 * @note The running time is O(k^3), where `k` is the number of vertices in
		 *       @p vertex_set.
		 */
		template<class GraphT>
		bool is_triangle_free_induced_subgraph(
			const GraphT& graph,
			const typename GraphT::vertex_set_type& vertex_set,
			vertices& triangle)
		{
			static_assert(
				graph_traits<GraphT>::is_undirected,
				"is_triangle_free_induced_subgraph() requires an undirected graph");

			triangle.clear();
			triangle.reserve(3);

			vertices induced_vertices;
			vertex_set.extract(induced_vertices);

			const std::size_t vertex_count =
				induced_vertices.size();

			for (std::size_t i = 0;
				i + 2 < vertex_count;
				++i)
			{
				const vertex_t first = induced_vertices[i];

				for (std::size_t j = i + 1;
					j + 1 < vertex_count;
					++j)
				{
					const vertex_t second = induced_vertices[j];

					if (!graph.is_edge(first, second)) {
						continue;
					}

					for (std::size_t k = j + 1;
						k < vertex_count;
						++k)
					{
						const vertex_t third = induced_vertices[k];

						if (graph.is_edge(first, third) &&
							graph.is_edge(second, third))
						{
							triangle.emplace_back(first);
							triangle.emplace_back(second);
							triangle.emplace_back(third);

							return false;
						}
					}
				}
			}

			return true;
		}

		/**
		* @brief API for backward compatibility - to be removed in the future
		*/
		template<class GraphT>
		bool is_triangleFree_subgraph(
			GraphT& graph,
			const typename GraphT::vertex_set_type& vertex_set,
			std::vector<vertex_t>& triangle)
		{
			return is_triangle_free_induced_subgraph(
				graph,
				vertex_set,
				triangle
			);
		}	
			

		/**
		 * @brief Sorts vertices by their degree into a reference vertex set.
		 *
		 * For each vertex `v` in @p vertices_to_sort, computes the number of vertices
		 * `w` in @p reference_vertices for which `graph.is_edge(v, w)` is true, and
		 * sorts according to that value.
		 *
		 * The two vertex collections may overlap. Vertices having the same computed
		 * degree retain their original relative order.
		 *
		 * For directed graphs, the computed value is the outgoing degree from `v`
		 * into @p reference_vertices. Graph weights, if present, are ignored.
		 *
		 * @tparam GraphT Graph type providing `num_vertices()` and `is_edge()`.
		 * @param graph Input graph.
		 * @param vertices_to_sort Vertices to reorder in place.
		 * @param reference_vertices Vertices used to compute the restricted degrees.
		 * @param order Requested sorting order.
		 * @return Reference to @p vertices_to_sort.
		 *
		 * @pre Every vertex in both collections is valid for @p graph.
		 */
		template<class GraphT>
		vertices& sort_by_degree(
			const GraphT& graph,
			vertices& vertices_to_sort,
			const vertices& reference_vertices,
			degree_order order = degree_order::nondecreasing)
		{
			const std::size_t vertex_count =
				static_cast<std::size_t>(graph.num_vertices());

			std::vector<std::size_t> degrees(vertex_count, 0);

			for (const vertex_t vertex : vertices_to_sort) {
				assert(
					vertex >= 0 &&
					static_cast<std::size_t>(vertex) < vertex_count);

				std::size_t degree = 0;

				for (const vertex_t reference_vertex :
				reference_vertices)
				{
					assert(
						reference_vertex >= 0 &&
						static_cast<std::size_t>(reference_vertex) <
						vertex_count);

					if (graph.is_edge(vertex, reference_vertex)) {
						++degree;
					}
				}

				degrees[static_cast<std::size_t>(vertex)] = degree;
			}

			// in case of equal degrees, the original order is preserved
			std::stable_sort(
				vertices_to_sort.begin(),
				vertices_to_sort.end(),
				[&](vertex_t lhs, vertex_t rhs) {
					if (order == degree_order::nondecreasing) {
						return degrees[lhs] < degrees[rhs];
					}

					return degrees[lhs] > degrees[rhs];
				});

			return vertices_to_sort;
		}

		/**
		* @ brief API for backward compatibility - to be removed in the future
		**/
		template<class GraphT>
		vertices& sort_deg(
			const GraphT& graph,
			vertices& vertices_to_sort,
			const vertices& reference_vertices,
			bool min_sort = true)
		{

			return sort_by_degree(
				graph,
				vertices_to_sort,
				reference_vertices,
				min_sort
				? degree_order::nondecreasing
				: degree_order::nonincreasing);
		}

		/**
		 * @brief Sorts vertices by their degree into a reference vertex set.
		 *
		 * For each vertex `v` in @p vertices_to_sort, computes its degree into
		 * @p reference_vertices and sorts the vertices according to that value.
		 *
		 * The collection and reference set may overlap. Vertices having the same
		 * computed degree retain their original relative order.
		 *
		 * For directed graphs, the computed value is the outgoing degree from `v`
		 * into @p reference_vertices. Graph weights, if present, are ignored.
		 *
		 * @tparam GraphT Graph type providing `num_vertices()` and
		 *         `degree(vertex, vertex_set)`.
		 * @param graph Input graph.
		 * @param vertices_to_sort Vertices to reorder in place.
		 * @param reference_vertices Bitset containing the reference vertices.
		 * @param order Requested sorting order.
		 * @return Reference to @p vertices_to_sort.
		 *
		 * @pre Every vertex in @p vertices_to_sort is valid for @p graph.
		 * @pre @p reference_vertices is compatible with the graph vertex range.
		 */
		template<class GraphT>
		vertices& sort_by_degree(
			const GraphT& graph,
			vertices& vertices_to_sort,
			const typename GraphT::vertex_set_type& reference_vertices,
			degree_order order = degree_order::nondecreasing)
		{
			const std::size_t vertex_count =
				static_cast<std::size_t>(graph.num_vertices());

			std::vector<std::size_t> degrees(vertex_count, 0);

			for (const vertex_t vertex : vertices_to_sort) {
				assert(
					vertex >= 0 &&
					static_cast<std::size_t>(vertex) < vertex_count);

				degrees[static_cast<std::size_t>(vertex)] =
					static_cast<std::size_t>(
						graph.degree(vertex, reference_vertices));
			}

			// in case of equal degrees, the original order is preserved
			std::stable_sort(
				vertices_to_sort.begin(),
				vertices_to_sort.end(),
				[&](vertex_t lhs, vertex_t rhs) {
					const std::size_t lhs_degree =
						degrees[static_cast<std::size_t>(lhs)];

					const std::size_t rhs_degree =
						degrees[static_cast<std::size_t>(rhs)];

					if (order == degree_order::nondecreasing) {
						return lhs_degree < rhs_degree;
					}

					return lhs_degree > rhs_degree;
				});

			return vertices_to_sort;
		}


		/**
		* @ brief API for backward compatibility - to be removed in the future
		**/
		template<class GraphT>
		vertices& sort_deg(
			const GraphT& graph,
			vertices& vertices_to_sort,
			const typename GraphT::vertex_set_type& reference_vertices,
			bool min_sort = true)
		{
			return sort_by_degree(
				graph,
				vertices_to_sort,
				reference_vertices,
				min_sort
				? degree_order::nondecreasing
				: degree_order::nonincreasing);
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
