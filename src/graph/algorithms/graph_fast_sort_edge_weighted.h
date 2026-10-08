/**
* @file graph_fast_sort_edge_weighted.h
* @brief header for GraphFastRootSort_EW_W class which sorts
*		 weighted graphs according to different criteria
* @details : created 08/12/2021
* @last_update 29/01/2026 
* @author pss
* 
* @todo
* - unit tests for GraphFastRootSort_EW  (30/06/2025)
* - check everything works as expected (29/01/2026)
* 
**/

#ifndef BITGRAPH_GRAPH_GRAPH_FAST_SORT_EDGE_WEIGHTED_H
#define BITGRAPH_GRAPH_GRAPH_FAST_SORT_EDGE_WEIGHTED_H


#include "graph_fast_sort.h"
#include "ordering_decoder.h"

#include "utils/logger.h"


namespace bitgraph {

	namespace graph_utils {

		///////////////////////////
		//
		// class GraphFastRootSort_EW
		// (sorting for vertex weighted graphs)
		//
		////////////////////////////

		template <class GraphEW>
		class GraphFastRootSort_EW : public GraphFastRootSort <typename GraphEW::graph_type> {

		public:
			
			using graph_ew_t = GraphEW;
			using graph_t = typename GraphEW::graph_type;
			using base_t = GraphFastRootSort<graph_t>;
			using weight_t = typename GraphEW::Weight;
			using vertex_ordering_t = typename base_t::vertex_ordering_t;

			// alias for backward compatibility with existing code
			using basic_type = GraphEW;									//weighted graph type
			using ptype = base_t;										//parent type
			using Weight = weight_t;									//weight type
			using VertexOrdering = vertex_ordering_t;
								
		
			GraphFastRootSort_EW(graph_type& gew) 
				: base_t(gew.graph()), 
				graph_ew_(gew)
			{}

			// move and copy semantics disallowed
			GraphFastRootSort_EW(const GraphFastRootSort_EW&) = delete;
			GraphFastRootSort_EW& operator=	(const GraphFastRootSort_EW&) = delete;
			GraphFastRootSort_EW(GraphFastRootSort_EW&&)	noexcept = delete;
			GraphFastRootSort_EW& operator=	(GraphFastRootSort_EW&&)	noexcept = delete;

			~GraphFastRootSort_EW() = default;

			using base_t::new_order;
		
		public:
							
			static graph_ew_t reorder(
				const graph_ew_t& graph,
				const vertex_ordering_t& new_order_o2n,
				OrderingDecoder* decoder = nullptr);

			void reorder(
				const vertex_ordering_t& new_order, 
				graph_ew_t& gew,
				OrderingDecoder* d = NULL);				
			
		private:

			graph_ew_t& graph_ew_;

		};

	}//end of namespace graph_utils

	using graph_utils::GraphFastRootSort_EW;

}//end of namespace bitgraph


/////////////////////////////////////
// Necessary header implementations for generic code

namespace bitgraph {

	namespace graph_utils {

		template<class GraphEW>
		inline auto
			GraphFastRootSort_EW<GraphEW>::reorder(
				const graph_ew_t& graph,
				const vertex_ordering_t& new_order_o2n,
				OrderingDecoder* decoder) -> graph_ew_t
		{
			const int NV = graph.number_of_vertices();

			graph_ew_t reordered_graph;
			reordered_graph.reset(NV, weight_t{1});		

			// Copy graph metadata.
			reordered_graph.set_name(graph.get_name(), false /* no path separation */);
			reordered_graph.set_path(graph.get_path());

			// Reorder graph topology.
			// Currently intended for simple undirected graphs.
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
				reordered_graph.set_wv(
					new_order_o2n[v],
					graph.get_wv(v));
			}

			// Reorder edge weights.
			for (vertex_t u = 0; u < NV - 1; ++u) {
				for (vertex_t v = u + 1; v < NV; ++v) {
					if (graph.is_edge(u, v)) {

						const auto weight = graph.get_we(u, v);

						reordered_graph.set_we(
							new_order_o2n[u],
							new_order_o2n[v],
							weight);

						reordered_graph.set_we(
							new_order_o2n[v],
							new_order_o2n[u],
							weight);
					}
				}
			}

			// Store decoding information: [NEW] -> [OLD].
			if (decoder != nullptr) {
				vertex_ordering_t new_order_n2o = new_order_o2n;
				Decode::reverse_in_place(new_order_n2o);
				decoder->insert_ordering(new_order_n2o);
			}

			return reordered_graph;
		}



		template <class GraphEW >
		inline
			void GraphFastRootSort_EW<GraphEW>::reorder(
				const vertex_ordering_t& new_order,
				graph_ew_t& gew,
				OrderingDecoder* d)
		{
			/////////////////////
			// EXPERIMENTAL-ONLY FOR SIMPLE GRAPHS
			//
			// PARAMS
			// @new_order: MUST BE mapping [OLD]->[NEW]!

			int NV = graph_ew_.number_of_vertices();
			gew.init(NV, graph_type::NOWT);
			gew.set_name(graph_ew_.get_name(), false /* no path separation */);
			gew.set_path(graph_ew_.get_path());

			//only for undirected graphs
			for (int i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (graph_ew_.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
						gew.add_edge(new_order[i], new_order[j]);
					}
				}
			}

			///////////////
			//stores decoding information [NEW]->[OLD]
			if (d != NULL) {
				VertexOrdering aux(new_order);
				Decode::reverse_in_place(aux);								//maps [NEW] to [OLD]		
				d->insert_ordering(aux);
			}

			/////////////////////
			//weights (vertices) -update
			for (int i = 0; i < NV; i++) {
				gew.set_wv(new_order[i], graph_ew_.get_wv(i));
			}

			/////////////////////
			//weights (edges)- 	
			for (int i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (graph_ew_.is_edge(i, j)) {
						gew.set_we(new_order[i], new_order[j], graph_ew_.get_we(i, j));
						gew.set_we(new_order[j], new_order[i], graph_ew_.get_we(i, j));
					}
				}
			}
		}

	} // end of namespace graph_utils

}//end of namespace bitgraph



#endif  // BITGRAPH_GRAPH_GRAPH_FAST_SORT_EDGE_WEIGHTED_H__

