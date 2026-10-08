/**
 * @file graph_fast_sort_vertex_weighted.h
 * @brief Vertex-ordering algorithms for vertex-weighted graphs.
 *
 * Defines `GraphFastRootSort_VW`, which extends `GraphFastRootSort`
 * with vertex-ordering strategies based on vertex weights.
 *
 * @author pss
 * @details Created 12/03/2021, last updated 08/10/2026.
 */

#ifndef BITGRAPH_GRAPH_GRAPH_FAST_SORT_VERTEX_WEIGHTED_H
#define BITGRAPH_GRAPH_GRAPH_FAST_SORT_VERTEX_WEIGHTED_H

#include "graph_fast_sort.h"
#include "ordering_decoder.h"	
#include "utils/logger.h"
#include <vector>

namespace bitgraph {

	namespace graph_utils {

		/**
		 * @brief Vertex-ordering utilities for vertex-weighted graphs.
		 *
		 * Extends `GraphFastRootSort` with ordering strategies based on vertex
		 * weights while retaining the unweighted ordering strategies provided by
		 * the base class.
		 *
		 * @tparam GraphW Vertex-weighted graph type. The type must provide a
		 *                `graph_type` member identifying the underlying graph type.
		 *
		 * @note The class inherits from
		 *       `GraphFastRootSort<typename GraphW::graph_type>`.
		 */
		template <class GraphW>
		class GraphFastRootSort_VW  : public GraphFastRootSort <typename GraphW::graph_type>  
		{

		public:

			using graph_w_t = GraphW;
			using graph_t = typename GraphW::graph_type;
			using base_t = GraphFastRootSort<graph_t>;
			using weight_t = typename GraphW::Weight;
			using vertex_ordering_t = typename base_t::vertex_ordering_t;

			// alias for backward compatibility with existing code
			using basic_type = GraphW;									//weighted graph type
			using ptype = base_t;										//parent type
			using Weight = weight_t;									//weight type
			using VertexOrdering = vertex_ordering_t;
			

			// specialized enums for vertex weighted sorting strategies
			enum class strategy {
				max_weight = 100,
				min_weight
			};
						
			using strategy_t = strategy;
			using base_strategy_t = typename base_t::strategy_t;
			using sort_order_t = typename base_t::sort_order_t;
			

			// enum for backward compatibility with existing code
			enum { MAX_WEIGHT = 100, MIN_WEIGHT };					
									
		public:

			/**
			 * @brief Reorders the vertices of a graph according to a given ordering.
			 *
			 * @param graph Input graph.
			 * @param new_order_o2n Vertex ordering used to relabel the graph in [OLD]->[NEW] format.
			 * @param decoder Optional decoder updated with the corresponding [NEW]->[OLD] vertex mapping..
			 * @return A reordered copy of the input graph.
			 */
			static graph_w_t reorder(
				const graph_w_t& graph,
				const vertex_ordering_t& new_order_o2n,
				OrderingDecoder* decoder = nullptr);


			/**
			 * @brief Computes a new vertex ordering using a legacy integer strategy code.
			 *
			 * Supports both the unweighted strategies inherited from the base class and
			 * the weighted strategies defined by `GraphFastRootSort_VW`.
			 *
			 * @param strategy Sorting strategy encoded as an integer.
			 * @param last_to_first If `true`, reverses the resulting ordering.
			 * @param old_to_new If `true`, returns the ordering in [OLD]->[NEW] format;
			 *                   otherwise, in [NEW]->[OLD] format.
			 * @return The computed vertex ordering.
			 *
			 * @warning Calls `std::terminate()` if `strategy` does not identify a valid
			 *          sorting strategy.
			 *
			 * @deprecated Use the typed `new_order()` overloads instead.
			 */
			[[deprecated("Use the typed new_order() overloads instead")]]
			vertex_ordering_t new_order(
				int strategy,
				bool last_to_first = true,
				bool old_to_new = true);

			/**
			 * @brief Computes a new vertex ordering using an unweighted sorting strategy.
			 *
			 * This overload forwards the request to the base `GraphFastRootSort`
			 * implementation.
			 *
			 * @param strategy Unweighted sorting strategy.
			 * @param placement Vertex placement policy.
			 * @param order Ordering format to return  (old->new or new->old).
			 * @return The computed vertex ordering.
			 *
			 * @warning Calls `std::terminate()` if `strategy` is not a valid
			 *          unweighted sorting strategy.
			 */
			vertex_ordering_t new_order(
				base_strategy_t strategy,
				placement_t placement = placement_t::last_to_first,
				sort_order_t order = sort_order_t::old_to_new)
			{
				return base_t::new_order(strategy, placement, order);
			}
						

			/**
			 * @brief Computes a new vertex ordering using a weighted sorting strategy.
			 *
			 * @param strategy Weighted sorting strategy.
			 * @param placement Vertex placement policy.
			 * @param old_to_new If `true`, returns the ordering in [OLD]->[NEW] format;
			 *                   otherwise, in [NEW]->[OLD] format.
			 * @return The computed vertex ordering.
			 *
			 * @warning Calls `std::terminate()` if `strategy` is not a valid
			 *          weighted sorting strategy.
			 */
			vertex_ordering_t new_order(
				strategy_t strategy,
				placement_t placement = placement_t::last_to_first,
				sort_order_t order = sort_order_t::old_to_new);
	
			/**
			 * @brief Reorders the vertices of the associated graph.
			 *
			 * Creates an isomorphic graph by relabeling its vertices according to
			 * `new_order_o2n`. This overload operates on the graph associated with
			 * the sorter and forwards the operation to the static `reorder()` function.
			 *
			 * @param new_order_o2n Vertex ordering in [OLD]->[NEW] format.
			 * @param decoder Optional decoder updated with the corresponding
			 *                [NEW]->[OLD] vertex mapping.
			 * @return The reordered graph.
			 */
			graph_w_t reorder(
				const vertex_ordering_t& new_order_o2n,
				OrderingDecoder* decoder = nullptr) const
			{
				return GraphFastRootSort_VW::reorder(
					this->graph(),
					new_order_o2n,
					decoder);
			}
					

			////////////////////////
			//construction / destruction
			GraphFastRootSort_VW(graph_w_t& gw) 
				: base_t(gw.graph()), graphw_(gw) {}

			// move and copy semantics disallowed
			GraphFastRootSort_VW(const GraphFastRootSort_VW&) = delete;
			GraphFastRootSort_VW& operator=	(const GraphFastRootSort_VW&) = delete;
			GraphFastRootSort_VW(GraphFastRootSort_VW&&)	noexcept = delete;
			GraphFastRootSort_VW& operator=	(GraphFastRootSort_VW&&)	noexcept = delete;

			~GraphFastRootSort_VW() = default;

			//////////
			// setters / getters
			
			const graph_w_t& graph() const { return graphw_; }
						
		private:
			
			/**
			 * @brief Computes a non-increasing vertex ordering by weight.
			 *
			 * Vertices are ordered by non-increasing weight using a stable sort.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Reference to the computed vertex ordering stored in `nodes_`,
			 *         in [NEW]->[OLD] format.
			 */
			const vertex_ordering_t& sort_by_non_increasing_weight(bool reverse = true);

			/**
			 * @brief Computes a non-decreasing vertex ordering by weight.
			 *
			 * Vertices are ordered by non-decreasing weight using a stable sort.
			 *
			 * @param reverse If `true`, reverses the resulting ordering.
			 * @return Reference to the computed vertex ordering stored in `nodes_`,
			 *         in [NEW]->[OLD] format.
			 */
			const vertex_ordering_t& sort_by_non_decreasing_weight(bool reverse = true);

			////////////////
			// data members	
		private:
			const graph_w_t& graphw_;
		};

	}//end of namespace graph_utils	

	using graph_utils::GraphFastRootSort_VW;		//alias for the GraphFastRootSort_VW class

}//end of namespace bitgraph

////////////////////////////
// Necessary header implementations for generic code

namespace bitgraph {

	namespace graph_utils {

		template <class GraphW >
		inline auto
			GraphFastRootSort_VW<GraphW>::new_order(
				int strategy,
				bool last_to_first, 
				bool old_to_new) -> vertex_ordering_t 
		{
			this->nodes_.clear();

			switch (strategy) {
			case ptype::NONE:
			case ptype::MIN_DEGEN:
			case ptype::MIN_DEGEN_COMPO:
			case ptype::MAX_DEGEN:
			case ptype::MAX_DEGEN_COMPO:
			case ptype::MAX:
			case ptype::MIN:
			case ptype::MAX_WITH_SUPPORT:
			case ptype::MIN_WITH_SUPPORT:

				ptype::new_order(strategy, last_to_first, old_to_new);			//sorts the graph according to non-weighted criteria
				break;

			case MAX_WEIGHT:						//currently the only sorting algorithm for weighted graphs
				sort_by_non_increasing_weight(last_to_first);
				if (!old_to_new) { Decode::reverse_in_place(this->nodes_); }
				break;
			case MIN_WEIGHT:						//currently the only sorting algorithm for weighted graphs
				sort_by_non_decreasing_weight(last_to_first);
				if (!old_to_new) { Decode::reverse_in_place(this->nodes_); }
				break;

			default:
				LOG_ERROR("unknown algorithm - GraphFastRootSort_VW<GraphW>::new_order(...)");
				std::terminate();
			}
			return this->nodes_;
		}

		template <class GraphW >
		inline auto
			GraphFastRootSort_VW<GraphW>::new_order(
				strategy_t strategy,
				placement_t placement,
				sort_order_t order) -> vertex_ordering_t
		{
			switch (strategy) {
			case strategy::max_weight:
				sort_by_non_increasing_weight(
					placement == placement_t::last_to_first);
				break;

			case strategy::min_weight:
				sort_by_non_decreasing_weight(
					placement == placement_t::last_to_first);
				break;

			default:
				LOG_ERROR(
					"unknown sorting algorithm -"
					"GraphFastRootSort_VW<GraphW>::new_order(...)");
				std::terminate();
			}

			// Assuming weighted sorting primitives produce [NEW] -> [OLD].
			if (order == sort_order_t::old_to_new) {
				Decode::reverse_in_place(this->nodes_);
			}

			return this->nodes_;
		}

		template<class GraphW>
		inline auto GraphFastRootSort_VW<GraphW>::reorder(
			const graph_w_t& graph, 
			const vertex_ordering_t& new_order_o2n, 
			OrderingDecoder* decoder) -> graph_w_t
		{

			const int NV = graph.num_vertices();

			graph_w_t reordered_graph;
			reordered_graph.reset(NV, weight_t{ 1 });

			// Copy graph metadata.
			reordered_graph.set_name(graph.name());
			reordered_graph.set_path(graph.path());

			// Generate the reordered graph.
			for (vertex_t u = 0; u < NV - 1; ++u) {
				for (vertex_t v = u + 1; v < NV; ++v) {
					if (graph.is_edge(u, v)) {
						reordered_graph.add_edge(
							new_order_o2n[u],
							new_order_o2n[v]);
					}
				}
			}

			// Reorder vertex weights.
			for (vertex_t v = 0; v < NV; ++v) {
				reordered_graph.set_weight(
					new_order_o2n[v],
					graph.weight(v));
			}

			// Store decoding information: [NEW] -> [OLD].
			if (decoder != nullptr) {
				vertex_ordering_t new_order_n2o = new_order_o2n;
				Decode::reverse_in_place(new_order_n2o);
				decoder->add_ordering(new_order_n2o);
			}

			return reordered_graph;			
		}

		template<class GraphW>
		inline auto
			GraphFastRootSort_VW<GraphW>::sort_by_non_increasing_weight(
				bool reverse) -> const vertex_ordering_t&
		{
			// Set the trivial ordering [0, NV - 1] as the starting point.
			base_t::set_ordering();
					
			utils::has_greater_val< vertex_t, std::vector<weight_t> > pred(
				graphw_.weight());

			std::stable_sort(this->nodes_.begin(), this->nodes_.end(), pred);
			

			//reverse order if required
			if (reverse) {
				std::reverse(this->nodes_.begin(), this->nodes_.end()); 
			}

			return this->nodes_;
		}

		template<class GraphW>
		inline auto
			GraphFastRootSort_VW<GraphW>::sort_by_non_decreasing_weight(
				bool reverse) -> const vertex_ordering_t&
		{
			
			// Set the trivial ordering [0, NV - 1] as the starting point.
			base_t::set_ordering();
					
			utils::has_smaller_val< int, std::vector<Weight> > pred(
				graphw_.weight());

			std::stable_sort(this->nodes_.begin(), this->nodes_.end(), pred);
			
			//reverse order if required
			if (reverse) {
				std::reverse(this->nodes_.begin(), this->nodes_.end());
			}

			return this->nodes_;
		}

	}// end of namespace graph_utils

}//end of namespace bitgraph	

#endif //  BITGRAPH_GRAPH_GRAPH_FAST_SORT_VERTEX_WEIGHTED_H

