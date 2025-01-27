/**
* @file graph_fast_sort_edge_weighted.h
* @brief header for GraphFastRootSort_EW_W class which sorts
*		 weighted graphs according to different criteria
* @date 08/12/2021
* @last_update 27/01/2025
* @author pss
**/

#ifndef __GRAPH_FAST_SORT_EDGE_WEIGHTED_H__
#define __GRAPH_FAST_SORT_EDGE_WEIGHTED_H__

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include "graph/algorithms/graph_fast_sort.h"
#include "graph/kcore.h"
#include "filter_graph_sort_type.h"			//limits template Graph_t to undirected types
#include "utils/logger.h"
#include "utils/common.h"					//sort functors
#include "decode.h"

using namespace std;

///////////////////////////
//
// class GraphFastRootSort_EW
// (sorting for vertex weighted graphs)
//
////////////////////////////

template <class GraphEW_t>
class GraphFastRootSort_EW: public GraphFastRootSort <typename GraphEW_t::_gt>{

public:
	typedef GraphFastRootSort <typename GraphEW_t::_gt> _mypt;
	enum { MAX_WEIGHT=100 };

////////////////
// data members	
private:
	GraphEW_t& m_gw;

public:
////////////////////////
//construction / allocation
	GraphFastRootSort_EW(GraphEW_t& gwout): m_gw(gwout), _mypt(gwout.graph()){ }
	~GraphFastRootSort_EW(){}
	
////////////////
// interface (public)	
public:
	
///////////
//overrides
	vint new_order	(int alg, bool ltf=true, bool o2n=true);								/* interface for the framework */
	int reorder		(const vint& new_order, GraphEW_t& gn, Decode* d=NULL);					// (new) interface for the framework- TODO@build an in-place reordering as in the old GraphSort 	

private:

};


template <class GraphEW_t >
inline
vint GraphFastRootSort_EW<GraphEW_t>::new_order (int alg, bool ltf, bool o2n){
/////////////////
// Computes new order of vertices accordig to @alg
//
// PARAMS
// @alg:algorithm
// @ltf:last to first
// @o2n:old to new

	vector<int> order;

	switch (alg) {
	case _mypt::NONE:
	case _mypt::MIN_DEGEN:
	case _mypt::MIN_DEGEN_COMPO:
	case _mypt::MAX_DEGEN:
	case _mypt::MAX_DEGEN_COMPO:
	case _mypt::MAX:
	case _mypt::MIN:
	case _mypt::MAX_WITH_SUPPORT:
	case _mypt::MIN_WITH_SUPPORT:

		order= _mypt::new_order( alg,  ltf,  o2n);
		break;
    
	default:
		LOG_ERROR("bizarre algorithm- GraphFastRootSort_EW<Graph_t>::new_order(...), exiting...");
		exit(-1);
	}
	return order;
}

template <class GraphEW_t >
inline
int GraphFastRootSort_EW<GraphEW_t>::reorder(const vint& new_order, GraphEW_t& gn, Decode* d) {
	/////////////////////
	// EXPERIMENTAL-ONLY FOR SIMPLE GRAPHS
	//
	// PARAMS
	// @new_order: MUST BE mapping [OLD]->[NEW]!

	int NV = m_gw.number_of_vertices();
	gn.init(NV, GraphEW_t::NOWT);
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
		vint aux(new_order);
		Decode::reverse_in_place(aux);								//maps [NEW] to [OLD]		
		d->insert_ordering(aux);
	}

	/////////////////////
	//weights (vertices) -update
	for (int i = 0; i <NV; i++) {
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




#endif  //__GRAPH_FAST_SORT_EDGE_WEIGHTED_H__

