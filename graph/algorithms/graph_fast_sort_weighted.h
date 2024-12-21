//graph_fast_sort_weighted.h: header for GraphFastRootSort_W_W class which sorts weighted graphs by different criteria
//date: 12/03/2021
//authors: pss

#ifndef __GRAPH_FAST_SORT_WEIGHTED_H__
#define __GRAPH_FAST_SORT_WEIGHTED_H__

#include "../algorithms/graph_fast_sort.h"
#include "decode.h"
#include "utils/common.h"						
#include "utils/logger.h"
#include <vector>
//#include "filter_graph_sort_type.h"			//limits template Graph_t to undirected types

using namespace std;

///////////////////////////
//
// class GraphFastRootSort_W
// (sorting for vertex weighted graphs)
//
////////////////////////////

template <class GraphW_t>
class GraphFastRootSort_W: public GraphFastRootSort <typename GraphW_t::_gt>{

public:
	using type = GraphFastRootSort_W<GraphW_t>;
	using basic_type = GraphW_t;
	using gtype = typename basic_type::_gt;
	using ptype = typename GraphFastRootSort <gtype>;
	
	//typedef GraphFastRootSort <typename GraphW_t::_gt> _mypt;
	enum class sort_algw_t { MAX_WEIGHT = 100 };

////////////////
// public interface 
public:

	vint new_order	(int alg, bool ltf = true, bool o2n = true)		override;							
	int  reorder	(const vint& new_order, GraphW_t& gn, Decode* d = NULL);		// (new) interface for the framework- TODO@build an in-place reordering as in the old GraphSort 	

public:
////////////////////////
//construction / allocation
	GraphFastRootSort_W(GraphW_t& gwout): gw(gwout), GraphFastRootSort<typename GraphW_t::_gt>(gwout.graph()){ }
	~GraphFastRootSort_W() = default;

	GraphFastRootSort_W	(const GraphFastRootSort_W&) = delete;
	GraphFastRootSort_W& operator=(const GraphFastRootSort_W&) = delete;
	GraphFastRootSort_W	(GraphFastRootSort_W&&) = delete;
	GraphFastRootSort_W& operator=(GraphFastRootSort_W&&) = delete;

private:
	////////////////
	//sorting 
	vint sort_by_weight(bool ltf = true, bool o2n = true);

////////////////
// data members	
private:
	const GraphW_t& gw;
};

template <class GraphW_t >
inline
vint GraphFastRootSort_W<GraphW_t>::new_order (int alg, bool ltf, bool o2n){
/////////////////
// Computes new order of vertices accordig to @alg
//
// PARAMS
// @alg:algorithm
// @ltf:last to first
// @o2n:old to new

	vector<int> order;

	switch (alg) {
	case ptype::sort_alg_t::NONE:
	case ptype::sort_alg_t::MIN_DEGEN:
	case ptype::sort_alg_t::MIN_DEGEN_COMPO:
	case ptype::sort_alg_t::MAX_DEGEN:
	case ptype::sort_alg_t::MAX_DEGEN_COMPO:
	case ptype::sort_alg_t::MAX:
	case ptype::sort_alg_t::MIN:
	case ptype::sort_alg_t::MAX_WITH_SUPPORT:
	case ptype::sort_alg_t::MIN_WITH_SUPPORT:
		
		order= ptype::new_order( alg,  ltf,  o2n);
		break;
    
	case sort_algw_t::MAX_WEIGHT:
		order = sort_by_weight( ltf, o2n);
		break;

	default:
		LOG_ERROR("unknown algorithm - GraphFastRootSort_W<GraphW_t>::new_order(...)");
		LOG_ERROR("exiting...");
		exit(-1);
	}
	return order;
}

template <class GraphW_t >
inline
int GraphFastRootSort_W<GraphW_t>::reorder(const vint& new_order, GraphW_t& gn, Decode* d) {
	/////////////////////
	// EXPERIMENTAL-ONLY FOR SIMPLE GRAPHS
	//
	// PARAMS
	// @new_order: MUST BE mapping [OLD]->[NEW]!

	int NV = gw.number_of_vertices();
	gn.init(NV, 1.0);												//assigns unit weights- TODO@CHECK BEST INIT STRATEGY						
	gn.set_name(gw.get_name(), false /* no path separation */);
	gn.set_path(gw.get_path());

	//only for undirected graphs
	for (int i = 0; i < NV - 1; i++) {
		for (int j = i + 1; j < NV; j++) {
			if (gw.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
				//switch edges according to new numbering
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}

	///////////////
	//stores decoding information [NEW]->[OLD]
	if (d != NULL) {
		vint aux(new_order);
		Decode::reverse_in_place(aux);								//maps [NEW] to [OLD]		
		d->insert_ordering(aux);
	}

	/////////////////////
	//weights (vertices) -update
	for (int i = 0; i <NV; i++) {
		gn.set_w(new_order[i], gw.get_w(i));
	}
	
	/////////////////////
	//weights (edges)- TODO@add to edge-weighted graphs
	
	//reorder edge-weights if required (CHECK@-eff)
	/*if (g.is_edge_weighted()) {
		gn.init_we();
		for (int i = 0; i < size - 1; i++) {
			for (int j = i + 1; j < size; j++) {
				if (gn.is_edge(new_order[i], new_order[j])) {
					gn.set_we(new_order[i], new_order[j], g.get_we(i, j));
					gn.set_we(new_order[j], new_order[i], g.get_we(i, j));
				}
			}
		}
	}*/

	//g = gn;

	////new order to stream if available
	//if (o != NULL)
	//	copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));

	return 0;
}


template <class GraphW_t >
inline
vint GraphFastRootSort_W<GraphW_t>::sort_by_weight(bool ltf, bool o2n) {
	/////////////////////
	// Orders vertices by weights (absolute stable ordering)
	// Default is FIRST TO LAST, e.g., @lft is MAXIMUM WEIGHT LAST
	// 
	// 
	// RETURNS a valid ordering O[OLD_INDEX]=NEW_INDEX

	vint order;
	const int NV = gw.number_of_vertices();
	ptype::fill_vertices(order, NV);
	
	if (ltf) {
		com::has_smaller_val<int, std::vector<typename GraphW_t::_wt>> pred(gw.get_weights());
		std::stable_sort(order.begin(), order.end(), pred);
	}
	else {
		com::has_greater_val<int, std::vector<typename GraphW_t::_wt>> pred(gw.get_weights());
		std::stable_sort(order.begin(), order.end(), pred);
	}
	   
	if (o2n) { Decode::reverse_in_place(order); }
	return order;
}



#endif

