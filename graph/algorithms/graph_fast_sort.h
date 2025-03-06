/**
* @file graph_fast_sort.h
* @brief header for GraphFastRootSort class which sorts graphs by different criteria
* @details: changed nodes_ stack to vector (18/03/19)
* @details: created 12/03/15,  last_update 27/01/25
* @author pss
* 
* @TODO add further primitives for composite orderings in subgraphs (29/12/24)
* @TOD0 add new_order(...) for subgraphs (29/12/24)
**/

#ifndef __GRAPH_FAST_SORT_H__
#define __GRAPH_FAST_SORT_H__


#include "utils/logger.h"
#include "utils/common.h"						
#include "decode.h"
#include "bitscan/bbtypes.h"					//for EMPTY_ELEM constant	
#include "bitscan/bbobject.h"
#include <algorithm>
#include <iostream>
#include <vector>

//#include "filter_graph_sort_type.h"			//limits template Graph_t to undirected types

using namespace std;

//alias
using vint = std::vector<int>;

///////////////////////////
//
// GraphFastRootSort class
// (Graph_t restricted to ugraph and sparse_ugraph)
//
////////////////////////////

template <class Graph_t>
class GraphFastRootSort{
	
public:
	using basic_type = Graph_t;											//graph type
	using type = GraphFastRootSort< Graph_t>;							//own type		
	using bb_type = typename Graph_t::_bbt;								//bitset type

	enum  	{PRINT_DEGREE=0, PRINT_SUPPORT, PRINT_NODES};
	enum  	{MIN_DEGEN = 0, MAX_DEGEN, MIN_DEGEN_COMPO, MAX_DEGEN_COMPO, MAX, MIN, MAX_WITH_SUPPORT, MIN_WITH_SUPPORT, NONE };
	
////////////////////////
//static methods 

	/*
	* @brief Computes the degree of the vertices of a graph 
	* @param g input graph G=(V, E)
	* @param deg output vector of size |V| (v[i] = deg(vi))
	*/
	static int compute_deg(const Graph_t& g, vint& deg);		
		

///////////////
// drivers - the real public interface
	
	/*
	* @brief Computes a new ordering 
	* @param alg sorting algorithm
	* @param ltf last to first ordering if TRUE
	* @param o2n old to new ordering	if TRUE
	* @return new ordering in [OLD]->[NEW] format
	*/
	virtual vint new_order(int alg, bool ltf = true, bool o2n = true);

	/*
	* @brief Creates an isomorphism for a given ordering
	* @param gn output isomorphic graph
	* @param new_order given ordering in [OLD]->[NEW] format
	* @param d ptr to decode object to store the ordering
	* @comments only for simple undirected graphs with no weights
	* @return 0 if successful
	*/
	int reorder(const vint& new_order, Graph_t& gn, Decode* d = nullptr);

////////////////////////
//construction/destructions

explicit GraphFastRootSort(Graph_t& gout) :
		g_(gout),  
		NV_(g_.number_of_vertices())
	{
		nb_neigh_.assign(NV_, 0);
		deg_neigh_.assign(NV_, 0);
		node_active_state_.reset(NV_); 
	}

	//move and copy semantics not allowed
	GraphFastRootSort				(const GraphFastRootSort&)	= delete;
	GraphFastRootSort& operator=	(const GraphFastRootSort&)	= delete;
	GraphFastRootSort				(GraphFastRootSort&&)		= delete;
	GraphFastRootSort& operator=	(GraphFastRootSort&&)		= delete;

	//destructor
	~GraphFastRootSort() = default;
		
////////////////////////
//setters / getters

	const vint&	degree				() const		{ return nb_neigh_; }
	const vint&	support				() const		{ return deg_neigh_; }
	const Graph_t& graph			() const		{ return g_; }
	std::size_t number_of_vertices	() const		{ return NV_; }

///////////////////////
// allocation
protected:

	/**
	* @brief Restores context for NV_ vertices
	**/
	int reset();

public:
/////////////////////////
// main operations - sorting, etc.

	/*
	* @brief Sets trivial ordering [1..NV] in @nodes_,
	*		 a starting point for all sorting primitives	
	*		
	*/
	void set_ordering();	

	/*
	* @brief Sets an ordering in [OLD]->[NEW] format in @nodes_.
	*		 This will be the given ordering in composite orderings
	*/
	void set_ordering(vint& nodes) { nodes_ = nodes; }

	/**
	* @brief Computes the degree of each vertex
	**/ 
	const vint& compute_deg_root();											

	/**
	* @brief Computes support for all vertices (sum of the number of neighbors)
	* @comments May include the same vertex twice	 
	**/
	const vint& compute_support_root();									
		
	
	/**
	* @brief Computes a non_increasing_degree (non-degenerate) ordering 
	* @param rev reverse ordering if TRUE
	* @important requires prior computation of deg
	* @return output ordering in [OLD]->[NEW] format
	**/
	const vint&  sort_non_increasing_deg(bool rev);		

	/**
	* @brief Computes a non-decreasing degree (non-degenerate) ordering 
	* @param rev reverse ordering if TRUE
	* @important requires prior computation of deg
	* @return ouptut ordering in [OLD]->[NEW] format
	**/
	const vint&  sort_non_decreasing_deg(bool rev);

	/*
	* @brief Computes a non-increasing degree (non-degenerate) ordering with tiebreak by supprt 
	* @param rev reverse ordering if TRUE
	* @important requires prior computation of deg and support
	* @return output ordering in [OLD]->[NEW] format
	*/
	const vint&  sort_non_increasing_deg_with_support_tb(bool rev);

	/**
	* @brief Computes a non-decreasing degree (non-degenerate) ordering with tiebreak by supprt
	* @param rev reverse ordering if TRUE
	* @important requires prior computation of deg and support
	* @return output ordering in [OLD]->[NEW] format
	**/
	const vint&  sort_non_decreasing_deg_with_support_tb(bool rev);
		
	/**
	* @brief Degenerate non-decreasing degree ordering
	* @comments deg info is not restored after the call
	* @return output ordering in [OLD]->[NEW] format
	* TODO - optimize
	**/
	const vint&  sort_degen_non_decreasing_deg(bool rev);				
	const vint&  sort_degen_non_increasing_deg(bool rev);

	//Expermimental alternative implementation - CHECK efficiency
	//Does not required cached degree info of vertices in @nb_neigh_
	const vint& sort_degen_non_decreasing_deg_B(bool rev);					
	
	/**
	*@brief Composite non-decreasing degree degenerate ordering based on a prior given ordering 
	*@param rev reverse ordering if TRUE
	*@comments the vertex ordering has to be set (with set_ordering(...)) prior to the call
	*@return output ordering in [OLD]->[NEW] format
	**/
	const vint& sort_degen_composite_non_decreasing_deg( bool rev);	


	/**
	*@brief Composite non-increasing degree degenerate ordering based on a prior given ordering
	*@param rev reverse ordering if TRUE
	*@comments the vertex ordering has to be set (with set_ordering(...)) prior to the call
	*@return output ordering in [OLD]->[NEW] format
	**/
	const vint& sort_degen_composite_non_increasing_deg( bool rev );	
	
	/////////////////
	// Subgrah ordering 
	// 
	// TODO - add further primitives for composites, etc...
	
	/**
	*@brief sorts the first k vertices by non-increasing degree (non-degenerate) 
	*@param first_k  first k < |V|  vertices to sort ([0..k-1])
	*@param rev reverse ordering if TRUE
	*@return output ordering in [OLD]->[NEW] format
	**/
    const vint&  sort_non_increasing_deg(int first_k, bool rev );
	
	/**
	*@brief sorts [first, last] consecutive vertices by non-increasing degree (non-degenerate)
	*@param first  first vertex to sort (0-based index) - in  [0, |V|-1] 
	*@param last  last vertex to sort	(0-based index)	- in  [0, |V|-1], > first 
	*@param rev reverse ordering if TRUE
	*@return output ordering in [OLD]->[NEW] format
	**/
	const vint& sort_non_increasing_deg(int first, int last, bool rev);

	/**
	*@brief sorts the first k vertices by non-decreasing degree (non-degenerate) 
	*@param first_k  first k < |V|  vertices to sort ([0..k-1])
	*@param rev reverse ordering if TRUE
	*@return output ordering in [OLD]->[NEW] format
	**/
	const vint&  sort_non_decreasing_deg(int first_k, bool rev);

	/**
	*@brief sorts [first, last] consecutive vertices by non-decreasing degree (non-degenerate)
	*@param first  first vertex to sort (0-based index) - in  [0, |V|-1]
	*@param last  last vertex to sort	(0-based index)	- in  [0, |V|-1], > first
	*@param rev reverse ordering if TRUE
	*@return output ordering in [OLD]->[NEW] format
	**/
	const vint& sort_non_decreasing_deg(int first, int last, bool rev);

	//TODO - add tiebreak support for subgraph ordering 
	//int  sort_non_increasing_deg_with_support_tb(int n, bool rev = false);
	//int  sort_non_decreasing_deg_with_support_tb(int n, bool rev = false);

////////////////////////
// I/O
	ostream& print(int type, ostream& o, bool eofl = true) const;


	
		
/////////////////////////////////////////////
// data members	
protected:

	Graph_t& g_;											//ideally CONST but some operations like get_neighbors are non-const (TODO!)
	std::size_t NV_;										//number of vertices cached - g_.number_of_vertices()  

	vint nb_neigh_;											//stores the degree of the vertices		
	vint deg_neigh_;										//stores the support of the vertices (degree of neighbors)
	bb_type node_active_state_;								//bitset for active vertices: 1bit-active, 0bit-passive. Used in degenerate orderings	
	vint nodes_;											//stores the ordering

};//end of GraphFastRootSort class

////////////////////////////////////////////////////////////
// Inline implementations in header file

template<class Graph_t>
inline
vint GraphFastRootSort<Graph_t>::new_order (int alg, bool ltf, bool o2n)
{
	nodes_.clear();

	switch (alg) {
	case NONE:								//trivial case- with exit condition!
 		nodes_.reserve(NV_);						
		for (int i = 0; i < NV_; i++) {
			nodes_.emplace_back(i);
		}
		
		///////////////////////
		LOG_WARNING("NONE alg. sorting petition detected, returning trivial isomorphism- GraphFastRootSort<Graph_t>::new_order()");
		return nodes_;
		///////////////////////

		break;
	case MIN_DEGEN:
		set_ordering();
		compute_deg_root();
		sort_degen_non_decreasing_deg(ltf);			//checked with framework - (20/12/19 - what does this mean?)
		break;
	case MIN_DEGEN_COMPO:
		compute_deg_root();
		compute_support_root();
		sort_non_decreasing_deg_with_support_tb(false /* MUST BE*/);
		sort_degen_composite_non_decreasing_deg(ltf);
		break;
	case MAX_DEGEN:
		set_ordering();
		compute_deg_root();
		sort_degen_non_increasing_deg(ltf);
		break;
	case MAX_DEGEN_COMPO:
		compute_deg_root();
		compute_support_root();
		sort_non_increasing_deg_with_support_tb(false /* MUST BE*/);
		sort_degen_composite_non_increasing_deg(ltf);
		break;
	case MAX:
		compute_deg_root();
		sort_non_increasing_deg(ltf);
		break;
	case MIN:
		compute_deg_root();
		sort_non_decreasing_deg(ltf);
		break;
	case MAX_WITH_SUPPORT:
		compute_deg_root();
		compute_support_root();
		sort_non_increasing_deg_with_support_tb(ltf);
		break;
	case MIN_WITH_SUPPORT:
		compute_deg_root();
		compute_support_root();
		sort_non_decreasing_deg_with_support_tb(ltf);
		break;
	default:
		LOGG_ERROR("unknown sorting algorithm : ", alg, "- GraphFastRootSort<Graph_t>::new_order");
		LOG_ERROR("exiting...");
		exit(-1);
	}
		
	//conversion [NEW] to [OLD] if required
	if (!o2n) { Decode::reverse_in_place(nodes_); }            
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_deg(bool rev){
	
	//initialization
	node_active_state_.set_bit(0, NV_ - 1);					//all vertices active, pending to be ordered
	nodes_.clear();
	nodes_.reserve(NV_);

	int max_deg = NV_;
	int v = BBObject::noBit;
	do{
		max_deg = NV_;

		//selects an active vertex with minimum degree
		for (auto j = 0; j < NV_; j++) {
			if (node_active_state_.is_bit(j) && nb_neigh_[j] < max_deg) {			 
				max_deg = nb_neigh_[j];
				v = j;
			}
		}

		//////////////////////////////////
		nodes_.emplace_back(v);
		if ( nodes_.size() == NV_) { break; }			//exit condition

		node_active_state_.erase_bit(v);
		///////////////////////////////////

		//update degree info of the remaining active vertices
		bb_type& bbn = g_.get_neighbors(v);
	
		if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {			
			int w = BBObject::noBit;
			while ((w = bbn.next_bit()) != BBObject::noBit) {
				if (node_active_state_.is_bit(w)) {
					nb_neigh_[w]--;
				}
			}
		}
		
	} while (true);


	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}

	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_degen_non_increasing_deg(bool rev){
	
	//initialization
	node_active_state_.set_bit(0, NV_ - 1);										//all vertices active, pending to be ordered
	nodes_.clear();
	nodes_.reserve(NV_);

	//main loop
	int max_deg = 0, v = BBObject::noBit;
	do{
		//finds vertex with maximum degree
		max_deg = -1;
		for(auto j = 0; j < NV_; j++){
			if(node_active_state_.is_bit(j) && nb_neigh_[j] > max_deg){			 
				max_deg = nb_neigh_[j];
				v = j;
			}
		}

		//////////////////////////////////
		nodes_.emplace_back(v);
		if ( nodes_.size() == NV_) { break; }			//exit condition

		node_active_state_.erase_bit(v);
		//////////////////////////////////

		//updates neighborhood info in remaining vertices
		bb_type& bbn = g_.get_neighbors(v);
		if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
			int w = BBObject::noBit;
			while ((w = bbn.next_bit()) != BBObject::noBit) {
				if (node_active_state_.is_bit(w)) {
					nb_neigh_[w]--;
				}
			}
		}

	} while (true);

	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline const vint& GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_deg_B(bool rev)
{

	int min_deg = NV_, deg = 0;
	node_active_state_.set_bit(0, NV_ - 1);					//all active, pending to be ordered
	nodes_.clear();

	//main loop
	do {

		min_deg = NV_;
		int w = BBObject::noBit;
		int v = BBObject::noBit;

		//selects an active vertex with minimum degree
		if (node_active_state_.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
			LOG_ERROR("init scan failed -  GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_deg_B");
			LOG_ERROR("exting");
			std::exit(-1);
		}

		while ((w = node_active_state_.next_bit()) != BBObject::noBit) {
			deg = g_.degree(w, node_active_state_);
			if (min_deg > deg) {											// >= is possible
				min_deg = deg;
				v = w;
			}
		}

		//////////////////////////////////
		node_active_state_.erase_bit(v);
		nodes_.emplace_back(v);
		//////////////////////////////////

	} while (nodes_.size() < NV_);

	if (rev) {
		std::reverse(nodes_.begin(), nodes_.end());
	}

	return nodes_;
}

template<class Graph_t>
inline 
const vint& GraphFastRootSort<Graph_t>::sort_degen_composite_non_decreasing_deg(bool rev)
{
	node_active_state_.set_bit(0, NV_ - 1);			//all active, pending to be ordered
	int min_deg = NV_, v = EMPTY_ELEM;
	vint nodes_ori = nodes_;
	nodes_.clear();

	for (int i = 0; i < NV_; i++) {

		//finds vertex with minimum degree with TB according to the given ordering in nodes_ori 
		min_deg = NV_;
		for (int j = 0; j < NV_; j++) {
			int u = nodes_ori[j];
			if (node_active_state_.is_bit(u) && nb_neigh_[u] < min_deg) {
				min_deg = nb_neigh_[u];
				v = u;
			}
		}

		//updates context
		nodes_.emplace_back(v);
		node_active_state_.erase_bit(v);

		//updates neighborhood info in remaining vertices
		bb_type& bbn = g_.get_neighbors(v);
		if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
			int w = BBObject::noBit;
			while ( (w = bbn.next_bit()) != BBObject::noBit ) {
				if (node_active_state_.is_bit(w)) {
					nb_neigh_[w]--;
				}
			}
		}
	}

	if (rev) {
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline 
const vint& GraphFastRootSort<Graph_t>::sort_degen_composite_non_increasing_deg(bool rev)
{
	node_active_state_.set_bit(0, NV_ - 1);											//all active, pending to be ordered
	int max_deg = 0, v = EMPTY_ELEM;
	vint nodes_ori = nodes_;
	nodes_.clear();
		
	for (int i = 0; i < NV_; i++) {
		//finds vertex with maximum degree
		max_deg = -1;
		for (int j = 0; j < NV_; j++) {
			int u = nodes_ori[j];
			if (node_active_state_.is_bit(u) && nb_neigh_[u] > max_deg) {			 
				max_deg = nb_neigh_[u];
				v = u;
			}
		}

		//updates context
		nodes_.emplace_back(v);
		node_active_state_.erase_bit(v);

		//updates neighborhood info in remaining vertices
		bb_type& bbn = g_.get_neighbors(v);
		if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
			int w = BBObject::noBit;
			while ((w = bbn.next_bit()) != BBObject::noBit) {
				if (node_active_state_.is_bit(w)) {
					nb_neigh_[w]--;
				}
			}
		}			
	}

	if (rev) {
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg(int first_k, bool rev)
{		
	
	vint kord;
	::com::sort::fill_vertices(kord, first_k);

	//////////////////////////////////////////////////////
	::com::has_greater_val<int, vint> pred(nb_neigh_);
	//////////////////////////////////////////////////////

	std::stable_sort(kord.begin(), kord.end(), pred);
	if (rev) {
		std::reverse(kord.begin(), kord.end());
	}

	//generates the ordering in @nodes_ - first k vertices at the beginning
	nodes_= kord;
	nodes_.reserve(NV_);
	for (int v = first_k; v < NV_; v++) {
		nodes_.emplace_back(v);
	}
	return nodes_;
}

template<class Graph_t>
inline const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg(int first, int last,  bool rev) {

	vint kord;
	kord.reserve(last - first + 1);
	for (int i = first; i <= last; i++) {
		kord.emplace_back(i);
	}

	//////////////////////////////////////////////////////
	::com::has_greater_val<int, vint> pred(nb_neigh_);
	//////////////////////////////////////////////////////

	std::stable_sort(kord.begin(), kord.end(), pred);
	if (rev) {
		std::reverse(kord.begin(), kord.end());
	}

	//generates the ordering - first k vertices at the beginning
	set_ordering();				//trivial ordering in @nodes_
	int index = first;
	for (auto v : kord) {
		nodes_[index++] = v;	//substitute in nodes_ the sorted vertices
	}
		
	return nodes_;
}

template<class Graph_t>
inline const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(int first_k, bool rev){	
	vint kord;
	::com::sort::fill_vertices(kord, first_k);

	//////////////////////////////////////////////////////
	::com::has_smaller_val<int, vint> pred(nb_neigh_);
	//////////////////////////////////////////////////////

	std::stable_sort(kord.begin(), kord.end(), pred);
	if (rev) {
		std::reverse(kord.begin(), kord.end());
	}

	//generates the ordering - first k vertices at the beginning
	nodes_ = kord;
	nodes_.reserve(NV_);
	for (int v = first_k; v < NV_; v++) {
		nodes_.emplace_back(v);
	}

	return nodes_;
}

template<class Graph_t>
inline const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(int first, int last, bool rev) {

	vint kord;
	kord.reserve(last - first + 1);
	for (int i = first; i <= last; i++) {
		kord.emplace_back(i);
	}

	//////////////////////////////////////////////////////
	::com::has_smaller_val<int, vint> pred(nb_neigh_);
	//////////////////////////////////////////////////////

	std::stable_sort(kord.begin(), kord.end(), pred);
	if (rev) {
		std::reverse(kord.begin(), kord.end());
	}

	//generates the ordering - first k vertices at the beginning
	set_ordering();				//trivial ordering in @nodes_
	int index = first;
	for (auto v : kord) {
		nodes_[index++] = v;	//substitute in @nodes_ the sorted vertices
	}

	return nodes_;
}


template<class Graph_t>
inline
void GraphFastRootSort<Graph_t>::set_ordering(){
	nodes_.clear();
	nodes_.reserve(NV_);
	for(int i=0; i<NV_; i++){
			nodes_.emplace_back(i);
	}
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg(bool rev){
	set_ordering();
	::com::has_greater_val<int, vint> pred(nb_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);
	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}


template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(bool rev){
	set_ordering();
	::com::has_smaller_val<int, vint> pred(nb_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);
	
	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg_with_support_tb(bool rev ){
	set_ordering();
	::com::has_greater_val_with_tb<int, vint> pred(nb_neigh_, deg_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);

	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg_with_support_tb(bool rev){
	set_ordering();
	::com::has_smaller_val_with_tb<int, vint> pred(nb_neigh_, deg_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);

	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::compute_deg_root(){

	for(auto elem = 0; elem < NV_;  ++elem){		
		nb_neigh_[elem] = g_.get_neighbors(elem).size();
	}

	return nb_neigh_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::compute_support_root()
{
	for(auto elem = 0; elem < NV_; ++elem){
		deg_neigh_[elem] = 0;
		bb_type& bbn = g_.get_neighbors(elem);
		if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
			int w = BBObject::noBit;
			while ((w = bbn.next_bit()) != EMPTY_ELEM) {
				deg_neigh_[elem] += nb_neigh_[w];
			}
		}
	}

	return deg_neigh_;
}

template<class Graph_t>
inline
ostream& GraphFastRootSort<Graph_t>::print (int type, ostream& o, bool eofl) const
{	
	switch (type) {
	case PRINT_DEGREE:
		for (auto elem : nb_neigh_) {
			o << elem << " ";
		}		
		break;
	case PRINT_SUPPORT:
		for (auto elem : deg_neigh_) {
			o << elem << " ";
		}		
		break;
	case PRINT_NODES:
		::com::stl::print_collection<vint>(nodes_, o);		//formatted print with size in brackets
		break;
	default:
		LOG_ERROR("unknown print type- GraphFastRootSort<Graph_t>::print()");
		LOG_ERROR("exiting...");
		exit(-1);
	}

	if (eofl) { o << endl; }
	return o;
}

template<class Graph_t>
inline int GraphFastRootSort<Graph_t>::compute_deg(const Graph_t & g, vint & deg){
	auto NV = g.number_of_vertices();	
	deg.assign(NV, -1);
	for (auto v = 0; v < NV; v++) {
		deg[v] = g.get_neighbors(v).size();
	}
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::reset(){

	nb_neigh_.clear();
	nb_neigh_.resize(NV_);
	
	deg_neigh_.clear();
	deg_neigh_.resize(NV_);
	
	node_active_state_.set_bit(0, NV_ - 1);		//all active, pending to be ordered
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::reorder(const vint& new_order, Graph_t& gn, Decode* d) 
{
	std::size_t NV = g_.number_of_vertices();
	gn.reset(NV);
	gn.name(g_.name());	
	gn.path(g_.path());
	
	///generate isomorphism (only for undirected graphs) 
	for (auto i = 0; i < NV - 1; i++) {
		for (auto j = i + 1; j < NV; j++) {
			if (g_.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
				//////////////////////////////////////////////
				gn.add_edge(new_order[i], new_order[j]);			//maps the new edges according to the new given order
				//////////////////////////////////////////////
			}
		}
	}

///////////////
//stores decoding information [NEW]->[OLD]
	if (d != nullptr) {
		vint aux(new_order);						//new_order is in format [OLD]->[NEW]
		Decode::reverse_in_place(aux);				//aux is in format [NEW]->[OLD]		
		d->insert_ordering(aux);
	}

	return 0;
}


/////////////////
//	
// namespace for GRAPH sort basic (enum) types
// (deprecated code - 22/12/24)
//
//////////////////
//namespace gbbs{
//	enum sort_t						{MIN_DEG_DEGEN=0, MAX_DEG_DEGEN, MIN_DEG_DEGEN_TIE_STATIC, MAX_DEG_DEGEN_TIE_STATIC, MAX_DEG_DEGEN_TIE, KCORE, KCORE_UB, MAX_WEIGHT, MIN_WEIGHT, MAX_WEIGHT_DEG, MIN_WEIGHT_DEG, MAX_DEG_ABS, MIN_DEG_ABS, NONE};
//	enum place_t					{PLACE_FL=0, PLACE_LF};
//	enum pick_t						{PICK_MINFL=0, PICK_MINLF, PICK_MAXFL, PICK_MAXLF, PICK_FL, PICK_LF, PICK_MINABSFL,PICK_MAXABSFL, PICK_MINABSLF, PICK_MAXABSLF};
//};
//
////vertex neighborhood info
//struct deg_t{
//	friend ostream & operator<<(ostream& o, const deg_t& d){ o<<d.index<<":("<<d.deg<<","<<d.deg_of_n<<")"; return o;}
//	deg_t():index(EMPTY_ELEM), deg(0), deg_of_n(0){}
//	deg_t(int ind, int deg):index(ind), deg(deg), deg_of_n(0){}
//	int index;
//	int deg;
//	int deg_of_n;
//};





#endif

