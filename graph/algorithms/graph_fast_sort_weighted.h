/*
* graph_fast_sort_weighted.h: header for GraphFastRootSort_W_W class which sorts weighted graphs by different criteria
* @created 12/03/21
* @last_update 20/12/24
* @dev pss
*/

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
// (ordering of vertices for vertex weighted graphs)
//
////////////////////////////

template <class GraphW_t>
class GraphFastRootSort_W: public GraphFastRootSort <typename GraphW_t::_gt>{

public:
	using basic_type = GraphW_t;									//weighted graph type
	using type = GraphFastRootSort_W<GraphW_t>;						//own type	
	using ugtype = typename basic_type::_gt;						//non-weighted graph type	
	using ptype = typename GraphFastRootSort <ugtype>;				//parent type
	using wtype = typename GraphW_t::_wt;							//weight type

	enum  { MAX_WEIGHT = 100, MIN_WEIGHT };							//sorting algorithms for weighted graphs	

////////////////
// public interface 
public:
	/*
	* @brief Computes a new_order 
	* @param alg sorting algorithm
	* @param ltf  last to first if TRUE
	* @param o2n  old to new if TRUE
	*/
	vint new_order	(int alg, bool ltf = true, bool o2n = true)		override;							
	
	/*
	* @brief Creates a weighted graph isomorphism for a given ordering
	* @param gn output isomorphic weighted graph
	* @param new_order given ordering in [OLD]->[NEW] format
	* @param d ptr to decode object to store the ordering
	* @comments only for simple undirected graphs with no weights
	* @return 0 if successful
	*/
	int  reorder (const vint& new_order, GraphW_t& gn, Decode* d = nullptr);		

////////////////////////
//construction / allocation
	GraphFastRootSort_W(GraphW_t& gw): gw_(gw), GraphFastRootSort<typename GraphW_t::_gt>(gw.graph()){ }
	~GraphFastRootSort_W() = default;

	GraphFastRootSort_W	(const GraphFastRootSort_W&) = delete;
	GraphFastRootSort_W& operator=(const GraphFastRootSort_W&) = delete;
	GraphFastRootSort_W	(GraphFastRootSort_W&&) = delete;
	GraphFastRootSort_W& operator=(GraphFastRootSort_W&&) = delete;

	const GraphW_t& get_graph() const { return gw_; }
	
private:
	/*
	* @brief Non-degenerate maximum weight sorting of vertices 
	* @param ltf last to first if TRUE 
	* @comments uses stable sort
	* @returns reference to the new ordering in @nodes_ ([OLD]->[NEW] format)
	*/
	const vint& sort_by_non_increasing_weight(bool ltf = true);

	/*
	* @brief Non-degenerate minimum weight sorting of vertices
	* @param ltf last to first if TRUE
	* @comments uses stable sort
	* @returns reference to the new ordering in @nodes_ ([OLD]->[NEW] format)
	*/
	const vint& sort_by_non_decreasing_weight(bool ltf = true);

////////////////
// data members	
private:
	const GraphW_t& gw_;
};

template <class GraphW_t >
inline
vint GraphFastRootSort_W<GraphW_t>::new_order (int alg, bool ltf, bool o2n){
	nodes_.clear();											//clears the ordering

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
		if (!o2n) { Decode::reverse_in_place(nodes_); }
		break;
	case MIN_WEIGHT:						//currently the only sorting algorithm for weighted graphs
		sort_by_non_decreasing_weight(ltf);
		if (!o2n) { Decode::reverse_in_place(nodes_); }
		break;

	default:
		LOG_ERROR("unknown algorithm - GraphFastRootSort_W<GraphW_t>::new_order(...)");
		LOG_ERROR("exiting...");
		exit(-1);
	}
	return nodes_;
}

template <class GraphW_t >
inline
int GraphFastRootSort_W<GraphW_t>::reorder(const vint& new_order, GraphW_t& gn, Decode* d) {
	
	int NV = gw_.number_of_vertices();
	gn.init(NV, 1.0);												//assigns unit weights(1.0) 						
	gn.set_name(gw_.get_name());
	gn.set_path(gw_.get_path());

	//generate isomorphism (only for undirected graphs)
	for (int i = 0; i < NV - 1; i++) {
		for (int j = i + 1; j < NV; j++) {
			if (gw_.is_edge(i, j)) {								//is_edge is in O(log) for sparse graphs, should be specialized for that case
				//switch edges according to new numbering
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}
	
	/////////////////////
	//vertex weights update
	for (int i = 0; i <NV; i++) {
		gn.set_w(new_order[i], gw_.get_w(i));
	}

	///////////////
	//stores decoding information [NEW]->[OLD]
	if (d != nullptr) {
		vint aux(new_order);										//@new_order is in format [OLD]->[NEW]
		Decode::reverse_in_place(aux);								//@aux is in format [NEW] to [OLD]		
		d->insert_ordering(aux);
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

template<class GraphW_t>
inline 
const vint& GraphFastRootSort_W<GraphW_t>::sort_by_non_increasing_weight(bool ltf)
{
	//set trivial ordering [1..NV] in @nodes_ as starting point 
	ptype::set_ordering();

	/////////////////////////////////////////////////////////////////////////////
	com::has_greater_val< int, std::vector<wtype> > pred(gw_.get_weights());
	std::stable_sort(nodes_.begin(), nodes_.end(), pred);
	/////////////////////////////////////////////////////////////////////////////
	
	//reverse order if required
	if (ltf) { std::reverse(nodes_.begin(), nodes_.end());  }

	return nodes_;
}

template<class GraphW_t>
inline const vint& GraphFastRootSort_W<GraphW_t>::sort_by_non_decreasing_weight(bool ltf)
{
	//set trivial ordering [1..NV] in @nodes_ as starting point 
	ptype::set_ordering();

	/////////////////////////////////////////////////////////////////////////////
	com::has_smaller_val< int, std::vector<wtype> > pred(gw_.get_weights());
	std::stable_sort(nodes_.begin(), nodes_.end(), pred);
	/////////////////////////////////////////////////////////////////////////////

	//reverse order if required
	if (ltf) { std::reverse(nodes_.begin(), nodes_.end()); }

	return nodes_;
}

#endif

