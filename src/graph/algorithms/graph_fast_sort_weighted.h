/**
* @file graph_fast_sort_weighted.h
* @brief header for GraphFastRootSort_W_W class which sorts weighted graphs by different criteria
* @details: created 12/03/21, last_update 28/02/25
* @author pss
**/

#ifndef BITGRAPH_GRAPH_FAST_SORT_VERTEX_WEIGHTED_H
#define BITGRAPH_GRAPH_FAST_SORT_VERTEX_WEIGHTED_H

#include "graph_fast_sort.h"
#include "ordering_decoder.h"	
#include "utils/logger.h"
#include <vector>

namespace bitgraph {

	namespace graph_utils {

		///////////////////////////
		//
		// class GraphFastRootSort_W
		// (ordering of vertices for vertex weighted graphs)
		//
		////////////////////////////

		template <class GraphW>
		class GraphFastRootSort_W : public GraphFastRootSort <typename GraphW::graph_type> {

		public:
			using basic_type = GraphW;									//weighted graph type
			using type = GraphFastRootSort_W<GraphW>;					//own type	
			using ugtype = typename basic_type::graph_type;				//non-weighted graph type	
			using ptype = GraphFastRootSort <ugtype>;					//parent type
			using Weight = typename GraphW::Weight;						//weight type

			using graph_w_t = basic_type;

			//using ptype::vertex_ordering_t;
			using VertexOrdering = typename ptype::vertex_ordering_t;

			enum class strategy { 
				max_weight = 100,
				min_weight 
			};				

			// sorting strategies for weighted graphs
			// backward compatibility with existing code
			enum { MAX_WEIGHT = 100, MIN_WEIGHT };						//sorting algorithms for weighted graphs	

			

			////////////////
			// public interface 
		public:
			/*
			* @brief Computes a new_order
			* @param alg sorting algorithm
			* @param ltf  last to first if TRUE
			* @param o2n  old to new if TRUE
			*/
			VertexOrdering new_order(
				int alg, 
				bool ltf = true,
				bool o2n = true) override;

			/*
			* @brief Creates a weighted graph isomorphism for a given ordering
			* @param gn output isomorphic weighted graph
			* @param new_order given ordering in [OLD]->[NEW] format
			* @param d ptr to decode object to store the ordering
			* @comments only for simple undirected graphs with no weights
			* @return 0 if successful
			*/
			int reorder(
				const VertexOrdering& new_order, 
				graph_w_t& gn,
				Decode* d = nullptr) ;
						

			////////////////////////
			//construction / destruction
			GraphFastRootSort_W(graph_w_t& gw) : GraphFastRootSort<typename graph_w_t::graph_type>(gw.graph()), gw_(gw) {}

			//move and copy semantics
			GraphFastRootSort_W(const GraphFastRootSort_W&) = delete;
			GraphFastRootSort_W& operator=	(const GraphFastRootSort_W&) = delete;
			GraphFastRootSort_W(GraphFastRootSort_W&&)		noexcept = delete;
			GraphFastRootSort_W& operator=	(GraphFastRootSort_W&&)		noexcept = delete;

			~GraphFastRootSort_W() = default;

			//////////
			// setters / getters
			const graph_w_t& graph() const { return gw_; }

			////////
			//internals
		private:
			/*
			* @brief Non-degenerate maximum weight sorting of vertices
			* @param ltf last to first if TRUE
			* @comments uses stable sort
			* @returns reference to the new ordering in @nodes_ ([OLD]->[NEW] format)
			*/
			const VertexOrdering& sort_by_non_increasing_weight(bool ltf = true);

			/*
			* @brief Non-degenerate minimum weight sorting of vertices
			* @param ltf last to first if TRUE
			* @comments uses stable sort
			* @returns reference to the new ordering in @nodes_ ([OLD]->[NEW] format)
			*/
			const VertexOrdering& sort_by_non_decreasing_weight(bool ltf = true);

			////////////////
			// data members	
		private:
			const graph_w_t& gw_;
		};

	}//end of namespace graph_utils	

	using graph_utils::GraphFastRootSort_W;		//alias for the GraphFastRootSort_W class

}//end of namespace bitgraph

////////////////////////////
// Necessary header implementations for generic code

namespace bitgraph {

	namespace graph_utils {

		template <class GraphW >
		inline auto
			GraphFastRootSort_W<GraphW>::new_order(int alg, bool ltf, bool o2n) -> VertexOrdering {
			this->nodes_.clear();

			switch (alg) {
			case ptype::NONE:
			case ptype::MIN_DEGEN:
			case ptype::MIN_DEGEN_COMPO:
			case ptype::MAX_DEGEN:
			case ptype::MAX_DEGEN_COMPO:
			case ptype::MAX:
			case ptype::MIN:
			case ptype::MAX_WITH_SUPPORT:
			case ptype::MIN_WITH_SUPPORT:

				ptype::new_order(alg, ltf, o2n);				//sorts the graph according to non-weighted criteria
				break;

			case MAX_WEIGHT:						//currently the only sorting algorithm for weighted graphs
				sort_by_non_increasing_weight(ltf);
				if (!o2n) { Decode::reverse_in_place(this->nodes_); }
				break;
			case MIN_WEIGHT:						//currently the only sorting algorithm for weighted graphs
				sort_by_non_decreasing_weight(ltf);
				if (!o2n) { Decode::reverse_in_place(this->nodes_); }
				break;

			default:
				LOG_ERROR("unknown algorithm - GraphFastRootSort_W<GraphW>::new_order(...)");
				LOG_ERROR("exiting...");
				exit(-1);
			}
			return this->nodes_;
		}

		template <class GraphW >
		inline
			int GraphFastRootSort_W<GraphW>::reorder(
				const VertexOrdering& new_order,
				graph_w_t& gn, Decode* d)
		{

			int NV = gw_.num_vertices();

			//assigns unit weights(1.0) 	
			gn.reset(NV, 1.0);

			//copy graph data
			gn.set_name(gw_.name());
			gn.set_path(gw_.path());

			//generate isomorphism (only for undirected graphs)
			for (auto i = 0; i < NV - 1; ++i) {
				for (auto j = i + 1; j < NV; ++j) {
					if (gw_.is_edge(i, j)) {								//is_edge is in O(log) for sparse graphs, should be specialized for that case
						//switch edges according to new numbering
						gn.add_edge(new_order[i], new_order[j]);
					}
				}
			}

			/////////////////////
			//vertex weights update
			for (auto i = 0; i < NV; i++) {
				gn.set_weight(new_order[i], gw_.weight(i));
			}

			///////////////
			//stores decoding information [NEW]->[OLD]
			if (d != nullptr) {
				VertexOrdering aux(new_order);										//@new_order is in format [OLD]->[NEW]
				Decode::reverse_in_place(aux);								//@aux is in format [NEW] to [OLD]		
				d->add_ordering(aux);
			}

			return 0;
		}

		template<class GraphW_t>
		inline auto
			GraphFastRootSort_W<GraphW_t>::sort_by_non_increasing_weight(bool ltf) -> const VertexOrdering&
		{
			//set trivial ordering [1..NV] in @nodes_ as starting point 
			ptype::set_ordering();

			/////////////////////////////////////////////////////////////////////////////
			utils::has_greater_val< int, std::vector<Weight> > pred(gw_.weight());
			std::stable_sort(this->nodes_.begin(), this->nodes_.end(), pred);
			/////////////////////////////////////////////////////////////////////////////

			//reverse order if required
			if (ltf) { std::reverse(this->nodes_.begin(), this->nodes_.end()); }

			return this->nodes_;
		}

		template<class GraphW_t>
		inline auto
			GraphFastRootSort_W<GraphW_t>::sort_by_non_decreasing_weight(bool ltf) -> const VertexOrdering&
		{
			//set trivial ordering [1..NV] in @nodes_ as starting point 
			ptype::set_ordering();

			/////////////////////////////////////////////////////////////////////////////
			utils::has_smaller_val< int, std::vector<Weight> > pred(gw_.weight());
			std::stable_sort(this->nodes_.begin(), this->nodes_.end(), pred);
			/////////////////////////////////////////////////////////////////////////////

			//reverse order if required
			if (ltf) { std::reverse(this->nodes_.begin(), this->nodes_.end()); }

			return this->nodes_;
		}

	}// end of namespace graph_utils

}//end of namespace bitgraph	

#endif // BITGRAPH_GRAPH_FAST_SORT_VERTEX_WEIGHTED_H

