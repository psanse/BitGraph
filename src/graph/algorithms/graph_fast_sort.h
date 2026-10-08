/**
* @file graph_fast_sort.h
* @brief header for GraphFastRootSort class which sorts graphs by different criteria
* @details: changed nodes_ stack to vector (18/03/19)
*			- now working with subgraphs (30/11/2025)
* @date: created 12/03/15,  last_update 27/01/25
* @author pss
**/

#ifndef BITGRAPH_GRAPH_GRAPH_FAST_SORT_H
#define BITGRAPH_GRAPH_GRAPH_FAST_SORT_H
							
#include "graph/graph_traits.h"		
#include "graph/vertex_utils.h"
#include "graph/graph_types.h"
#include "utils/logger.h"
#include "utils/collection_utils.h"
#include "utils/sort_utils.h"
#include "ordering_decoder.h"
#include "bitscan/bbtypes.h"					//for EMPTY_ELEM constant	
#include "bitscan/bbobject.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include <numeric>								//std::iota


namespace bitgraph {

	namespace graph_utils {

		///////////////////////////
		//
		// GraphFastRootSort class
		// (GraphT should be restricted to ugraph and sparse_ugraph types)
		//
		////////////////////////////

		template <class GraphT>
		class GraphFastRootSort {
			
			//restrict to ugraph and sparse_ugraph types
			static_assert(
				graph_traits<GraphT>::is_undirected,
				"GraphFastRootSort requires an undirected graph type"
			);
					
		public:
			using graph_t = GraphT;														
			using vertex_bitset_t = typename GraphT::vertex_set_type;						
			using vertex_ordering_t = bitgraph::vertex_ordering;
			using vertex_degrees_t = std::vector<degree_t>;
			using vertex_supports_t = std::vector<degree_t>;

			// for backward compatibility with existing code
			using graph_type = graph_t;
						
					
			// new enum for sorting algorithms 
			enum class strategy {
				min_degeneracy,
				max_degeneracy,
				min_degeneracy_composite,
				max_degeneracy_composite,
				max,
				min,
				max_with_support,
				min_with_support,
				none
			};

			enum class placement {
				first_to_last,
				last_to_first
			};

			enum class print_mode {
				print_degree,
				print_support,
				print_nodes
			};

			enum class sort_order {
				new_to_old = 0,
				old_to_new
			};
						
			using strategy_t = strategy;
			using placement_t = placement;
			using sort_order_t = sort_order;

			// for backward compatibility with existing code
			using strategy_type = strategy_t;
			using placement_type = placement_t;


			// non-structured enums are used for easy conversion to int when needed
			// and backward compatibility with existing code
			enum { PRINT_DEGREE = 0, PRINT_SUPPORT, PRINT_NODES };
			enum { MIN_DEGEN = 0, MAX_DEGEN, MIN_DEGEN_COMPO, MAX_DEGEN_COMPO, MAX, MIN, MAX_WITH_SUPPORT, MIN_WITH_SUPPORT, NONE };
			enum { FIRST_TO_LAST = 0, LAST_TO_FIRST };
			enum { NEW_TO_OLD = 0, OLD_TO_NEW };								

			////////////////////////
			//static methods 

			/**
			 * @brief Computes the degree of every vertex in the graph.
			 * @param graph Input graph.
			 * @param deg Output vector where `deg[v]` is the degree of vertex `v`.
			 */
			static void compute_deg(
				const graph_t& graph, 
				vertex_degrees_t& deg);
			
			/**
			 * @brief Reorders the vertices of a graph according to a given ordering.
			 *
			 * @param graph Input graph.
			 * @param new_order_o2n Vertex ordering used to relabel the graph in [OLD]->[NEW] format.
			 * @param decoder Optional decoder updated with the corresponding [NEW]->[OLD] vertex mapping..
			 * @return A reordered copy of the input graph.
			 */
			static graph_t reorder(
				const graph_t& graph,
				const vertex_ordering_t& new_order_o2n,
				OrderingDecoder* decoder = nullptr);

			///////////////
			// drivers - the real public interface

			/**
			 * @brief Computes a new vertex ordering.
			 *
			 * @param strategy Sorting strategy encoded as an integer.
			 * @param last_to_first If `true`, vertices are placed from last to first;
			 *                      otherwise, from first to last.
			 * @param old_to_new If `true`, returns the ordering in [OLD]->[NEW] format;
			 *                   otherwise, in [NEW]->[OLD] format.
			 * @return The computed vertex ordering.
			 *
			 * @warning The function terminates execution if `strategy` is not a valid
			 *          sorting strategy.
			 *
			 * @deprecated Use the overload accepting `strategy_t` and `placement_t`.
			 */
			[[deprecated("Use new_order(strategy_t, placement_t, bool) instead")]]
			vertex_ordering_t new_order(
				int strategy, 
				bool last_to_first = true, 
				bool old_to_new = true);

			/**
			 * @brief Computes a new vertex ordering.
			 *
			 * @param strategy Sorting strategy.
			 * @param placement Vertex placement policy.
			 * @param order If `true`, returns the ordering in [OLD]->[NEW] format;
			 *                   otherwise, in [NEW]->[OLD] format.
			 * @return The computed vertex ordering.
			 */
			 vertex_ordering_t new_order(
				strategy_t strategy,
				placement_t placement = placement_t::last_to_first,
				sort_order order = sort_order::old_to_new)
			{
				return new_order(
					static_cast<int>(strategy), 
					placement == placement_t::last_to_first,
					order == sort_order::old_to_new);
			}

			/**
			 * @brief Computes a new ordering for the vertices of an induced subgraph.
			 *
			 * Only the vertices contained in `vertex_set` are reordered.
			 *
			 * @param strategy Sorting strategy encoded as an integer.
			 * @param vertex_set Bitset encoding the vertices of the induced subgraph.
			 * @param last_to_first If `true`, vertices are placed from last to first;
			 *            otherwise, from first to last.
			 * @param old_to_new If `true`, returns the ordering in [OLD]->[NEW] format;
			 *            otherwise, in [NEW]->[OLD] format.
			 * @return The computed vertex ordering.
			 *
			 * @details The ordering is computed as follows:
			 *          1. Construct the subgraph induced by the vertices in `bbsg`.
			 *          2. Compute a vertex ordering for the induced subgraph using
			 *             the available sorting primitives.
			 *          3. Map the resulting ordering back to the original graph.
			 */

			[[deprecated("Use new_order(strategy_t,  vertex_bitset_t, placement_t, bool) instead")]]
			 vertex_ordering_t new_order(
				int strategy, 
				vertex_bitset_t& vertex_set,
				bool last_to_first = true, 
				bool old_to_new = true);

			/**
			 * @brief Computes a new ordering for the vertices of an induced subgraph.
			 *
			 * Only the vertices contained in `vertex_set` are reordered.
			 *
			 * @param strategy Sorting strategy.
			 * @param vertex_set Bitset encoding the vertices of the induced subgraph.			 *            
			 * @param placement Vertex placement policy.
			 * @param order If `true`, returns the ordering in [OLD]->[NEW] format;
			 *                   otherwise, in [NEW]->[OLD] format.
			 * @return The computed vertex ordering.
			 */
			 vertex_ordering_t new_order(
				strategy_t strategy,
				vertex_bitset_t& vertex_set,
				placement_t placement = placement_t::last_to_first,
				sort_order order = sort_order::old_to_new) 
			{
				return new_order(
					static_cast<int>(strategy),
					vertex_set,
					placement == placement_t::last_to_first,
					order == sort_order::old_to_new);
			}
			
			/**
			 * @brief Reorders the graph according to an old-to-new vertex ordering.
			 *
			 * @param new_order_o2n Vertex permutation in `[old] -> [new]` format.
			 * @param decoder Optional decoder receiving the ordering information.
			 * @return A graph whose vertices are relabeled according to
			 *         @p new_order_o2n.
			 *
			 * @pre @p new_order_o2n is a valid permutation of the graph vertices.
			 */
			graph_t reorder(
				const vertex_ordering_t& new_order_o2n,
				OrderingDecoder* decoder = nullptr) const
			{
				return GraphFastRootSort<graph_t>::reorder(
					this->graph(),
					new_order_o2n, 
					decoder);				
			}

			////////////////////////
			//construction/destructions

			/**
			 * @brief Constructs a vertex-ordering algorithm for a graph.
			 *
			 * The sorter stores a non-owning reference to @p graph. The graph must remain
			 * alive and must not be structurally modified while the sorter is in use.
			 *
			 * @param graph Graph to analyze.
			 */
			explicit GraphFastRootSort(graph_t& gout)
				: graph_(gout),
				  NV_(graph_.num_vertices())
			{
				nb_neigh_.assign(static_cast<size_t>(NV_), 0);
				deg_neigh_.assign(static_cast<size_t>(NV_), 0);
				node_active_state_.reset(static_cast<size_t>(NV_));
			}

			//move and copy semantics not allowed
			GraphFastRootSort(const GraphFastRootSort&) = delete;
			GraphFastRootSort& operator=	(const GraphFastRootSort&) = delete;
			GraphFastRootSort(GraphFastRootSort&&) = delete;
			GraphFastRootSort& operator=	(GraphFastRootSort&&) = delete;

			///////////////////////
			//destructor
			~GraphFastRootSort() = default;

			////////////////////////
			//setters / getters

			const std::vector<degree_t>& degree() const noexcept { return nb_neigh_; }
			const std::vector<degree_t>& support() const noexcept{ return deg_neigh_; }
			const graph_t& graph() const noexcept { return graph_; }
			vertex_t num_vertices() const noexcept { return NV_; }
						
			////////
			// I/O
		

			/**
			 * @brief Prints internal sorting information to an output stream.
			 *
			 * @param mode Information to print.
			 * @param os Output stream.
			 * @param trailing_newline If `true`, appends a newline after the output.
			 * @return Reference to the output stream.
			 */
			std::ostream& print(
				print_mode mode,
				std::ostream& os,
				bool trailing_newline = true) const;
			
		protected:

			/////////////////////////
			// main operations - sorting, etc.
			// (internals, not part of the public API)

			/**
			* @brief Restores context for NV_ vertices
			**/
			void reset();

			/**
			 * @brief Sets the trivial vertex ordering in `nodes_`.
			 *
			 * Initializes `nodes_` with the identity ordering [0, NV_-1],
			 * used as the starting point by sorting primitives that require
			 * an existing ordering.
			 */
			void set_ordering();

			/*
			* @brief Sets an ordering in [OLD]->[NEW] format in @nodes_.
			*		 This will be the given ordering in composite orderings
			*/
			void set_ordering(const vertex_ordering_t& nodes) { nodes_ = nodes; }
			void set_ordering(vertex_ordering_t&& nodes) { nodes_ = std::move(nodes); }
						
			/**
			* @brief Computes the degree of each vertex
			**/
			const vertex_degrees_t& compute_deg_root();

			/**
			 * @brief Computes the support of every vertex.
			 *
			 * The support of a vertex is the sum of the degrees of its neighbors.
			 *
			 * @return Vector containing the support value of each vertex.
			 *
			 * @pre Degree information in `nb_neigh_` must be up to date before calling
			 *      this function.
			 */
			const vertex_supports_t& compute_support_root();

			/**
			 * @brief Computes a non-increasing degree ordering.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_increasing_deg(bool reverse);

			/**
			 * @brief Computes a non-decreasing degree ordering.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_decreasing_deg(bool reverse);

			/**
			 * @brief Computes a non-increasing degree ordering using support as a tie-breaker.
			 *
			 * Vertices are ordered primarily by non-increasing degree. Ties between
			 * vertices with the same degree are broken using their support, defined as
			 * the sum of the degrees of their neighbors:
			 *
			 * \f[
			 *   support(v) = \sum_{u \in N(v)} deg(u)
			 * \f]
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree and support information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_increasing_deg_with_support_tb(bool reverse);

			/**
			 * @brief Computes a non-decreasing degree ordering using support as a tie-breaker.
			 *
			 * Vertices are ordered primarily by non-decreasing degree. Ties between
			 * vertices with the same degree are broken using their support, defined as
			 * the sum of the degrees of their neighbors.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree and support information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_decreasing_deg_with_support_tb(bool reverse);

			/**
			 * @brief Computes a degenerate non-decreasing degree ordering.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @note Cached degree information is modified and not restored after the call.
			 * @todo Optimize this implementation.
			 */
			const vertex_ordering_t& sort_degen_non_decreasing_deg(bool reverse);
			
			/**
			 * @brief Computes a degenerate non-increasing degree ordering.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree information must be computed before calling this function.
			 * @note Cached degree information is modified and not restored after the call.
			 */
			const vertex_ordering_t& sort_degen_non_increasing_deg(bool reverse);

			/**
			 * @brief Experimental alternative implementation.
			 * @details This implementation does not require cached degree information of vertices in @nb_neigh_.
			 *
			 * @warning Experimental API. May change or be removed without notice.
			 */
			const vertex_ordering_t& sort_degen_non_decreasing_deg_B(bool reverse);

			/**
			 * @brief Computes a composite degenerate non-decreasing degree ordering.
			 *
			 * The ordering is computed using the current vertex ordering as the
			 * tie-breaking criterion during the degenerate degree ordering.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre An initial vertex ordering must be set before calling this function,
			 *      for example with `set_ordering(...)`.
			 */
			const vertex_ordering_t& sort_degen_composite_non_decreasing_deg(bool reverse);

			/**
			 * @brief Computes a composite degenerate non-increasing degree ordering.
			 *
			 * The ordering is computed using the current vertex ordering as the
			 * tie-breaking criterion during the degenerate degree ordering.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre An initial vertex ordering must be set before calling this function,
			 *      for example with `set_ordering(...)`.
			 */
			const vertex_ordering_t& sort_degen_composite_non_increasing_deg(bool reverse);

			/////////////////
			// Subgrah ordering 
			// 
			// TODO - add further primitives for composites, etc...

			/**
			 * @brief Sorts the first `first_k` vertices by non-increasing degree.
			 *
			 * Only the vertices in the range [0, first_k - 1] are reordered.
			 * Vertices outside this range remain unchanged.
			 *
			 * @param first_k Number of initial vertices to sort. Must satisfy
			 *                `0 <= first_k < |V|`.
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_increasing_deg(int first_k, bool reverse);

			/**
			 * @brief Sorts a range of consecutive vertices by non-increasing degree.
			 *
			 * Only the vertices in the inclusive range [`first`, `last`] are reordered.
			 * Vertices outside this range remain unchanged.
			 *
			 * @param first First vertex in the range to sort. Must satisfy
			 *              `0 <= first < |V|`.
			 * @param last Last vertex in the range to sort. Must satisfy
			 *             `first < last < |V|`.
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_increasing_deg(
				vertex_t first, 
				vertex_t last,
				bool reverse);

			/**
			 * @brief Sorts the first `first_k` vertices by non-decreasing degree.
			 *
			 * Only the vertices in the range [0, first_k - 1] are reordered.
			 * Vertices outside this range remain unchanged.
			 *
			 * @param first_k Number of initial vertices to sort. Must satisfy
			 *                `0 < first_k < |V|`.
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_decreasing_deg(int first_k, bool reverse);

			/**
			 * @brief Sorts a range of consecutive vertices by non-decreasing degree.
			 *
			 * Only the vertices in the inclusive range [`first`, `last`] are reordered.
			 * Vertices outside this range remain unchanged.
			 *
			 * @param first First vertex in the range to sort. Must satisfy
			 *              `0 <= first < |V|`.
			 * @param last Last vertex in the range to sort. Must satisfy
			 *             `first < last < |V|`.
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Vertex ordering in [NEW]->[OLD] format.
			 *
			 * @pre Degree information must be computed before calling this function.
			 */
			const vertex_ordering_t& sort_non_decreasing_deg(
				vertex_t first,
				vertex_t last,
				bool reverse);

			//TODO - add tiebreak support for subgraph ordering 
			//int  sort_non_increasing_deg_with_support_tb(int n, bool reverse = false);
			//int  sort_non_decreasing_deg_with_support_tb(int n, bool reverse = false);
							
		protected:
					
			graph_t& graph_;										// ideally CONST but some operations like neighbors() are non-const (TODO!)
			vertex_t NV_;											// number of vertices cached - graph_.num_vertices()  

			vertex_degrees_t nb_neigh_;								// stores the degree of the vertices		
			vertex_supports_t deg_neigh_;							// stores the support of the vertices (degree of neighbors)
			vertex_bitset_t node_active_state_;						// bitset for active vertices: 1bit-active, 0bit-passive. Used in degenerate orderings	
			vertex_ordering_t nodes_;								// stores the ordering

		}; // GraphFastRootSort class

	} // namespace graph_utils

	using graph_utils::GraphFastRootSort;

}//end of namespace bitgraph


////////////////////////////////////////////////////////////
// Necessary header implementations for generic code

namespace bitgraph {

	namespace graph_utils {


		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::new_order(
				int strategy, 
				bool last_to_first, 
				bool old_to_new) -> vertex_ordering_t
		{
			nodes_.clear();

			switch (strategy) {
			case NONE:								//trivial case- with exit condition!
				nodes_.reserve(NV_);
				for (vertex_t u = 0; u < NV_; ++u) {
					nodes_.push_back(u);
				}
								
				LOG_WARNING(
					"NONE strategy. Sorting request detected; returning trivial ordering - "
					"GraphFastRootSort<GraphT>::new_order()");
				return nodes_;				
								
			case MIN_DEGEN:
				set_ordering();
				compute_deg_root();
				sort_degen_non_decreasing_deg(last_to_first);			//checked with framework - (20/12/19 - what does this mean?)
				break;
			case MIN_DEGEN_COMPO:
				compute_deg_root();
				compute_support_root();
				sort_non_decreasing_deg_with_support_tb(false /* MUST BE*/);
				sort_degen_composite_non_decreasing_deg(last_to_first);
				break;
			case MAX_DEGEN:
				set_ordering();
				compute_deg_root();
				sort_degen_non_increasing_deg(last_to_first);
				break;
			case MAX_DEGEN_COMPO:
				compute_deg_root();
				compute_support_root();
				sort_non_increasing_deg_with_support_tb(false /* MUST BE*/);
				sort_degen_composite_non_increasing_deg(last_to_first);
				break;
			case MAX:
				compute_deg_root();
				sort_non_increasing_deg(last_to_first);
				break;
			case MIN:
				compute_deg_root();
				sort_non_decreasing_deg(last_to_first);
				break;
			case MAX_WITH_SUPPORT:
				compute_deg_root();
				compute_support_root();
				sort_non_increasing_deg_with_support_tb(last_to_first);
				break;
			case MIN_WITH_SUPPORT:
				compute_deg_root();
				compute_support_root();
				sort_non_decreasing_deg_with_support_tb(last_to_first);
				break;
			default:
				LOGG_ERROR(
					"unknown sorting algorithm : ",
					strategy, 
					"- GraphFastRootSort<GraphT>::new_order");
				
				std::terminate();
			}

			// Convert [NEW] -> [OLD] to [OLD] -> [NEW] if required.
			// Sorting primitives always produce [NEW] -> [OLD].
			if (old_to_new) { 
				Decode::reverse_in_place(nodes_); 
			}

			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_degen_non_decreasing_deg(bool reverse) -> const vertex_ordering_t&
		{

			//initialization
			node_active_state_.set_bit(0, NV_ - 1);					//all vertices active, pending to be ordered
			nodes_.clear();
			nodes_.reserve(NV_);

			do {
				degree_t min_deg = NV_;
				vertex_t v = BBObject::noBit;

				//selects an active vertex with minimum degree
				for (vertex_t w = 0; w < NV_; w++) {
					if (node_active_state_.is_bit(w) && nb_neigh_[w] < min_deg) {
						min_deg = nb_neigh_[w];
						v = w;
					}
				}

				assert(v != BBObject::noBit &&
					"no active vertex found - GraphFastRootSort<GraphT>::sort_degen_non_decreasing_deg()");

				
				nodes_.push_back(v);
				node_active_state_.erase_bit(v);
				if (nodes_.size() == static_cast<std::size_t>(NV_)) {		//exit condition
					break;
				}
							

				// Update the degree of the remaining active neighbors.
				vertex_bitset_t& neighbors = graph_.neighbors(v);

				neighbors.init_scan(BBObject::NON_DESTRUCTIVE);
				int w = BBObject::noBit;
				while ((w = neighbors.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						--nb_neigh_[w];
					}
				}
			} while (true);


			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}

			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_degen_non_increasing_deg(bool reverse) -> const vertex_ordering_t&
		{

			// Initialization: all vertices active, pending to be ordered.
			node_active_state_.set_bit(0, NV_ - 1);										
			nodes_.clear();
			nodes_.reserve(NV_);

			//main loop
			
			do {

				degree_t max_deg = -1;
				vertex_t v = BBObject::noBit;

				// Select an active vertex with maximum current degree.
				for (vertex_t w = 0; w < NV_; w++) {
					if (node_active_state_.is_bit(w) && nb_neigh_[w] > max_deg) {
						max_deg = nb_neigh_[w];
						v = w;
					}
				}

				assert(v != BBObject::noBit &&
					"no active vertex found - "
					"GraphFastRootSort<GraphT>::sort_degen_non_increasing_deg()");

				nodes_.push_back(v);
				node_active_state_.erase_bit(v);

				if (nodes_.size() == static_cast<std::size_t>(NV_)) {	 //exit condition
					break;
				}
				
				// Update the degree of the remaining active neighbors.
				vertex_bitset_t& neighbors = graph_.neighbors(v);
				neighbors.init_scan(BBObject::NON_DESTRUCTIVE);

				vertex_t w = BBObject::noBit;
				while ((w = neighbors.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						--nb_neigh_[w];
					}
				}

			} while (true);

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}
			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_degen_non_decreasing_deg_B(bool reverse) -> const vertex_ordering_t&
		{
						
			node_active_state_.set_bit(0, NV_ - 1);					//all active, pending to be ordered
			nodes_.clear();
			nodes_.reserve(NV_);

			//main loop
			do {

				degree_t min_deg = NV_;
				vertex_t v = BBObject::noBit;

				//  find an active vertex with minimum degree
				node_active_state_.init_scan(BBObject::NON_DESTRUCTIVE);
				
				vertex_t w = BBObject::noBit;
				while ((w = node_active_state_.next_bit()) != BBObject::noBit) {
					deg = graph_.degree(w, node_active_state_);
					if (deg < min_deg) {											// >= is possible
						min_deg = deg;
						v = w;
					}
				}

				assert(v != BBObject::noBit &&
					"no active vertex found - "
					"GraphFastRootSort<GraphT>::sort_degen_non_decreasing_deg_B()");

				node_active_state_.erase_bit(v);
				nodes_.push_back(v);

			} while (nodes_.size() < static_cast<std::size_t>(NV_));

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}

			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_degen_composite_non_decreasing_deg(bool reverse) -> const vertex_ordering_t&
		{
			node_active_state_.set_bit(0, NV_ - 1);			//all active, pending to be ordered
			
			
			vertex_ordering_t nodes_ori = std::move(nodes_);
			nodes_.clear();
			nodes_.reserve(NV_);
					

			for (vertex_t i = 0; i < NV_; ++i) {

				degree_t min_deg = NV_;
				vertex_t v = BBObject::noBit;

				// Select an active vertex with minimum current degree.
				// Ties are broken according to the prior ordering in nodes_ori.				
				for (auto j = 0; j < NV_; j++) {
					const vertex_t u = nodes_ori[j];

					if (node_active_state_.is_bit(u) &&
						nb_neigh_[u] < min_deg) {
						
						min_deg = nb_neigh_[u];
						v = u;
					}
				}

				assert(v != BBObject::noBit &&
					"no active vertex found - "
					"GraphFastRootSort<GraphT>::"
					"sort_degen_composite_non_decreasing_deg()");

			
				nodes_.push_back(v);
				node_active_state_.erase_bit(v);

				// Update degrees of the remaining active neighbors.
				vertex_bitset_t& neighbors = graph_.neighbors(v);
				neighbors.init_scan(BBObject::NON_DESTRUCTIVE);

				vertex_t w = BBObject::noBit;
				while ((w = neighbors.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						--nb_neigh_[w];
					}
				}
			}

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}
			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_degen_composite_non_increasing_deg(bool reverse) -> const vertex_ordering_t&
		{
			node_active_state_.set_bit(0, NV_ - 1);   // all active

			vertex_ordering_t nodes_ori = std::move(nodes_);
			nodes_.clear();
			nodes_.reserve(NV_);
			
			for (vertex_t i = 0; i < NV_; ++i){ 

				degree_t max_deg = -1;
				vertex_t v = BBObject::noBit;
				
				// Select an active vertex with maximum current degree.
				// Ties are broken according to the prior ordering in nodes_ori.
				for (vertex_t j = 0; j < NV_; ++j) {
					const vertex_t u = nodes_ori[j];

					if (node_active_state_.is_bit(u)
						&& nb_neigh_[u] > max_deg) {

						max_deg = nb_neigh_[u];
						v = u;
					}
				}

				assert(v != BBObject::noBit &&
					"no active vertex found - "
					"GraphFastRootSort<GraphT>::"
					"sort_degen_composite_non_increasing_deg()");
							
				nodes_.push_back(v);
				node_active_state_.erase_bit(v);

				// Update degrees of the remaining active neighbors.
				vertex_bitset_t& neighbors = graph_.neighbors(v);
				neighbors.init_scan(BBObject::NON_DESTRUCTIVE);

				vertex_t w = BBObject::noBit;
				while ((w = neighbors.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						--nb_neigh_[w];
					}
				}
			}

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}

			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_increasing_deg(int first_k, bool reverse) -> const vertex_ordering_t&
		{
			assert(
				0 <= first_k &&
				first_k < NV_ &&
				"Invalid first_k - "
				" GraphFastRootSort<GraphT>::sort_non_increasing_deg()");

			vertex_ordering_t kord;
			fill_vertices(kord, first_k);

		
			utils::has_greater_val<int, vertex_ordering_t> pred(nb_neigh_);
			std::stable_sort(kord.begin(), kord.end(), pred);

			if (reverse) {
				std::reverse(kord.begin(), kord.end());
			}

			// First k vertices are reordered; the remaining vertices keep
			// their original positions.
			nodes_ = std::move(kord);
			nodes_.reserve(NV_);

			for (vertex_t v = first_k; v < NV_; v++) {
				nodes_.push_back(v);
			}

			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_increasing_deg(
				vertex_t first,
				vertex_t last,
				bool reverse) -> const vertex_ordering_t&
		{

			assert(
				0 <= first &&
				first < last &&
				last < NV_ &&
				"Invalid vertex range [first, last] - "
				"GraphFastRootSort<GraphT>::sort_non_increasing_deg()");				

			vertex_ordering_t kord;
			kord.reserve(last - first + 1);

			for (vertex_t v = first; v <= last; ++v) {
				kord.push_back(v);
			}

			
			utils::has_greater_val<int, vertex_ordering_t> pred(nb_neigh_);
			std::stable_sort(kord.begin(), kord.end(), pred);

			if (reverse) {
				std::reverse(kord.begin(), kord.end());
			}

			// Start from the identity ordering and replace only the range
			// [first, last] with the sorted vertices.
			set_ordering();					
			
			vertex_t index = first;
			for (vertex_t v : kord) {
				nodes_[index++] = v;			// substitute in nodes_ the sorted vertices
			}

			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_decreasing_deg(int first_k, bool reverse) -> const vertex_ordering_t&
		{

			assert(
				0 <= first_k &&
				first_k < NV_ &&
				"Invalid first_k - "
				" GraphFastRootSort<GraphT>::sort_non_decreasing_deg()");

			vertex_ordering_t kord;
			fill_vertices(kord, first_k);

			utils::has_smaller_val<int, vertex_ordering_t> pred(nb_neigh_);
			std::stable_sort(kord.begin(), kord.end(), pred);

			if (reverse) {
				std::reverse(kord.begin(), kord.end());
			}

			// Vertices [0, first_k - 1] are reordered; the remaining
			// vertices preserve their original order.
			nodes_ = std::move(kord);
			nodes_.reserve(NV_);

			for (vertex_t v = first_k; v < NV_; ++v) {
				nodes_.push_back(v);
			}

			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_decreasing_deg(
				vertex_t first, 
				vertex_t last, 
				bool reverse)  -> const vertex_ordering_t&
		{

			assert(
				0 <= first &&
				first < last &&
				last < NV_ &&
				"Invalid vertex range [first, last] - "
				"GraphFastRootSort<GraphT>::sort_non_decreasing_deg()");

			vertex_ordering_t kord;
			kord.reserve(last - first + 1);
			for (vertex_t v = first; v <= last; v++) {
				kord.push_back(v);
			}

			utils::has_smaller_val<int, vertex_ordering_t> pred(nb_neigh_);
			std::stable_sort(kord.begin(), kord.end(), pred);

			if (reverse) {
				std::reverse(kord.begin(), kord.end());
			}

			// Start from the identity ordering and replace only the range
			// [first, last] with the sorted vertices
			set_ordering();				

			vertex_t index = first;
			for (vertex_t v : kord) {
				nodes_[index++] = v;	//substitute in @nodes_ the sorted vertices
			}

			return nodes_;
		}


		template<class GraphT>
		inline
			void GraphFastRootSort<GraphT>::set_ordering() {
			nodes_.clear();
			nodes_.reserve(NV_);

			for (vertex_t v = 0; v < NV_; v++) {
				nodes_.push_back(v);
			}
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_increasing_deg(bool reverse) -> const vertex_ordering_t&
		{
			set_ordering();
			utils::has_greater_val<vertex_t, vertex_ordering_t> pred(nb_neigh_);
			std::stable_sort(nodes_.begin(), nodes_.end(), pred);

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}
			return nodes_;
		}


		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_decreasing_deg(bool reverse) -> const vertex_ordering_t&
		{
			set_ordering();
			utils::has_smaller_val<int, vertex_ordering_t> pred(nb_neigh_);
			std::stable_sort(nodes_.begin(), nodes_.end(), pred);

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}
			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_increasing_deg_with_support_tb(bool reverse) -> const vertex_ordering_t&
		{
			set_ordering();
			utils::has_greater_val_with_tb<int, vertex_ordering_t> pred(nb_neigh_, deg_neigh_);
			std::stable_sort(nodes_.begin(), nodes_.end(), pred);

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}
			return nodes_;
		}

		template<class GraphT>
		inline
			auto GraphFastRootSort<GraphT>::sort_non_decreasing_deg_with_support_tb(bool reverse) -> const vertex_ordering_t&
		{
			set_ordering();
			utils::has_smaller_val_with_tb<int, vertex_ordering_t> pred(nb_neigh_, deg_neigh_);
			std::stable_sort(nodes_.begin(), nodes_.end(), pred);

			if (reverse) {
				std::reverse(nodes_.begin(), nodes_.end());
			}
			return nodes_;
		}

		template<class GraphT>
		inline auto
			GraphFastRootSort<GraphT>::compute_deg_root() -> const vertex_degrees_t& {

			for (vertex_t v = 0; v < NV_; ++v) {
				nb_neigh_[v] = graph_.neighbors(v).count();
			}

			return nb_neigh_;
		}

		template<class GraphT>
		inline auto
			GraphFastRootSort<GraphT>::compute_support_root() -> const vertex_supports_t&
		{
			for (vertex_t v = 0; v < NV_; ++v) {
				deg_neigh_[v] = 0;

				vertex_bitset_t& neighbors = graph_.neighbors(v);
				neighbors.init_scan(BBObject::NON_DESTRUCTIVE);

				vertex_t w = BBObject::noBit;
				while ((w = neighbors.next_bit()) != BBObject::noBit) {
					deg_neigh_[v] += nb_neigh_[w];
				}
			}

			return deg_neigh_;
		}

		template<class GraphT>
		inline
			std::ostream& GraphFastRootSort<GraphT>::print(
				print_mode mode,
				std::ostream& os,
				bool trailing_newline) const
		{
			switch (mode) {
			case print_mode::print_degree:
				bitgraph::utils::print_collection(nb_neigh_, os, false);
				break;
			case print_mode::print_support:
				bitgraph::utils::print_collection(deg_neigh_, os, false);
				break;
			case print_mode::print_nodes:
				bitgraph::utils::print_collection(nodes_, os, false);
				break;
			default:
				LOG_ERROR("unknown print mode- GraphFastRootSort<GraphT>::print()");
				std::terminate();
			}

			if (trailing_newline) {
				os << std::endl; 
			}
			return os;
		}

		template<class GraphT>
		inline void GraphFastRootSort<GraphT>::compute_deg(
			const graph_t& g,
			vertex_degrees_t& deg)
		{
			const int vertex_count = g.num_vertices();
			deg.resize(vertex_count);

			for (vertex_t v = 0; v < vertex_count; v++) {
				deg[v] = static_cast<int>(g.neighbors(v).size());
			}
		}

		template<class GraphT>
		inline
			void GraphFastRootSort<GraphT>::reset() {

			nb_neigh_.assign(NV_, 0);
			deg_neigh_.assign(NV_, 0);
						
			node_active_state_.set_bit(0, NV_ - 1);		// all active, pending to be ordered
		}

		template<class GraphT>
		inline auto
			GraphFastRootSort<GraphT>::new_order(
				int strategy, 
				vertex_bitset_t& vertex_set,
				bool last_to_first,
				bool old_to_new)  -> vertex_ordering_t
		{
			//convert vertex_set to vector
			vertex_ordering_t lv;
			vertex_set.extract(lv);
						
			assert(!lv.empty() && "empty subgraph detected- GraphFastRootSort<GraphT>::new_order()");
			
			// create the induced subgraph of size |vertex_set|
			graph_t induced_subgraph;			
			this->graph_.create_subgraph(induced_subgraph, lv);
		
			// create a new ordering for the subgraph based on existing primitives
			GraphFastRootSort<graph_t> sort(induced_subgraph);
			vertex_ordering_t ord_induced = sort.new_order(strategy, last_to_first, false /* n2o format*/);

			// map the ordering @ord back to the original graph
			vertex_ordering_t ord(NV_);
			std::iota(ord.begin(), ord.end(), 0);

			// map the subgraph ordering back to the original graph.
			for (std::size_t i = 0; i < lv.size(); ++i) {
				ord[lv[i]] = lv[ord_induced[i]];
			}

			////build reverse mapping from sg to the original graph g
			//vertex_ordering_t sg_to_g = ord;
			//int v = bbo::noBit;
			//int index_in_sg = 0;
			//vertex_set.init_scan(bbo::NON_DESTRUCTIVE);
			//while ((v = vertex_set.next_bit()) != bbo::noBit) {
			//	sg_to_g[index_in_sg++] = v;
			//}

			////mapping of ord_sg to ord ([NEW]->[OLD] format)
			//v = bbo::noBit;
			//index_in_sg = 0;
			//vertex_set.init_scan(bbo::NON_DESTRUCTIVE);
			//while ((v = vertex_set.next_bit()) != bbo::noBit) {
			//	int new_index_in_sg = ord_sg[index_in_sg++];
			//	ord[v] = sg_to_g[new_index_in_sg];
			//}
						
#ifndef NDEBUG
			assert(static_cast<int>(ord.size()) == NV_ 
				&& "ERROR: ord.size() != N - GraphFastRootSort<GraphT>::new_order");

			// Verify that vertices outside the subgraph remain unchanged.
			for (vertex_t v = 0; v < NV_; ++v) {
				if (!vertex_set.is_bit(v)) {
					assert(ord[v] == v
						&& "ERROR: vertex outside vertex_set reordered - "
						" GraphFastRootSort<GraphT>::new_order");				
				}
			}
#endif

			// Convert [NEW]->[OLD] to [OLD]->[NEW] if requested.
			if (old_to_new) {
				Decode::reverse_in_place(ord);
			}

			return ord;
		}

		template<class GraphT>
		inline
		auto GraphFastRootSort<GraphT>::reorder(
				const graph_t& graph,
				const vertex_ordering_t& o2n, 
				OrderingDecoder* decoder) -> graph_t
		{
			const int NV = graph.num_vertices();

			graph_t gres;
			gres.reset(NV);
			gres.set_name(graph.name());
			gres.set_path(graph.path());	
		
			// Create isomorphism (only for undirected graphs) 
			for (vertex_t u = 0; u < NV - 1; ++u) {
				for (vertex_t v = u + 1; v < NV; ++v) {

					//in is_edge is O(log) for sparse graphs, should be specialized for that case
					if (graph.is_edge(u, v)) {						
						gres.add_edge(o2n[u], o2n[v]);						
					}
				}
			}

			// Store decoding information: [NEW] -> [OLD].
			if (decoder != nullptr) {
				decoder->add_ordering(OrderingDecoder::inverse_ordering(o2n));  // add_odering requires [NEW] -> [OLD] 
			}
			
			return gres;
		}

	} // namespace graph_utils

}//end of namespace bitgraph	





#endif  // BITGRAPH_GRAPH_GRAPH_FAST_SORT_H
