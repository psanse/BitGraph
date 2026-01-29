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

#ifndef __GRAPH_FAST_SORT_EDGE_WEIGHTED_H__
#define __GRAPH_FAST_SORT_EDGE_WEIGHTED_H__

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include "graph_fast_sort.h"
#include "kcore.h"
#include "filter_graph_sort_type.h"			//limits template Graph_t to undirected types
#include "utils/logger.h"
#include "utils/common.h"					//sort functors
#include "decode.h"

namespace bitgraph {

	namespace _impl {

		///////////////////////////
		//
		// class GraphFastRootSort_EW
		// (sorting for vertex weighted graphs)
		//
		////////////////////////////

		template <class GraphEWT>
		class GraphFastRootSort_EW : public GraphFastRootSort <typename GraphEWT::graph_type> {

		public:
			using graph_type = typename GraphEWT::graph_type;								
			using BaseT = GraphFastRootSort<graph_type>;
			
			using typename BaseT::VertexOrdering;
				
			////////////////
			// data members	
		private:
			graph_type& m_gw;

			//////////////////

		public:
			////////////////////////
			//construction / allocation
			GraphFastRootSort_EW(graph_type& gwout) : m_gw(gwout), BaseT(gwout.graph()) {}
			~GraphFastRootSort_EW() {}

			////////////////
			// interface 	
		public:

			///////////
			//overrides

			VertexOrdering new_order(int alg, bool ltf = true, bool o2n = true) override ;								// interface for the framework 
			int reorder(const VertexOrdering& new_order, graph_type& gn, Decode* d = NULL) override;					// (new) interface for the framework- TODO@build an in-place reordering as in the old GraphSort 	
			
		private:

		};

	}//end of namespace _impl

	using _impl::GraphFastRootSort_EW;

}//end of namespace bitgraph


/////////////////////////////////////
// Necessary header implementations for generic code

namespace bitgraph {

	template <class GraphEWT >
	inline auto
	GraphFastRootSort_EW<GraphEWT>::new_order(int alg, bool ltf, bool o2n) -> VertexOrdering
	{
		/////////////////
		// Computes new order of vertices accordig to @alg
		//
		// PARAMS
		// @alg:algorithm
		// @ltf:last to first
		// @o2n:old to new

		vector<int> order;

		switch (alg) {
		case BaseT::NONE:
		case BaseT::MIN_DEGEN:
		case BaseT::MIN_DEGEN_COMPO:
		case BaseT::MAX_DEGEN:
		case BaseT::MAX_DEGEN_COMPO:
		case BaseT::MAX:
		case BaseT::MIN:
		case BaseT::MAX_WITH_SUPPORT:
		case BaseT::MIN_WITH_SUPPORT:

			order = BaseT::new_order(alg, ltf, o2n);
			break;

		default:
			LOG_ERROR("bizarre algorithm- GraphFastRootSort_EW<Graph_t>::new_order(...), exiting...");
			exit(-1);
		}
		return order;
	}

	template <class GraphEWT >
	inline
		int GraphFastRootSort_EW<GraphEWT>::reorder(const VertexOrdering& new_order, graph_type& gn, Decode* d) {
		/////////////////////
		// EXPERIMENTAL-ONLY FOR SIMPLE GRAPHS
		//
		// PARAMS
		// @new_order: MUST BE mapping [OLD]->[NEW]!

		int NV = m_gw.number_of_vertices();
		gn.init(NV, graph_type::NOWT);
		gn.set_name(m_gw.get_name(), false /* no path separation */);
		gn.set_path(m_gw.get_path());

		//only for undirected graphs
		for (int i = 0; i < NV - 1; i++) {
			for (int j = i + 1; j < NV; j++) {
				if (m_gw.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
					gn.add_edge(new_order[i], new_order[j]);
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
			gn.set_wv(new_order[i], m_gw.get_wv(i));
		}

		/////////////////////
		//weights (edges)- 	
		for (int i = 0; i < NV - 1; i++) {
			for (int j = i + 1; j < NV; j++) {
				if (m_gw.is_edge(i, j)) {
					gn.set_we(new_order[i], new_order[j], m_gw.get_we(i, j));
					gn.set_we(new_order[j], new_order[i], m_gw.get_we(i, j));
				}
			}
		}

		return 0;
	}

}//end of namespace bitgraph




#endif  //__GRAPH_FAST_SORT_EDGE_WEIGHTED_H__

