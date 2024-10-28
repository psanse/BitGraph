//graph_fast_sort.h: header for GraphFastRootSort class which sorts graphs by different criteria
//date: 12/03/15
//authors: pss
//EXPERIMENTAL-CHANGE STACK TO VECTOR!!!   (18/03/19: changed and simplified, TEST PROPERLY) 
//TODO: CHECK how the static order affects the degen order

#ifndef __GRAPH_FAST_SORT_H__
#define __GRAPH_FAST_SORT_H__

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include "../graph.h"
#include "../kcore.h"
#include "filter_graph_sort_type.h"			//limits template Graph_t to undirected types
#include "utils/logger.h"
#include "utils/common.h"					//sort functors
#include "decode.h"


using namespace std;

/////////////////
////
//// namespace for GRAPH sort basic (enum) types
////
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
//
//typedef std::vector<int>				vint;	

///////////////////////////
//
// GraphFastRootSort class
// (Graph_t restricted to ugraph and sparse_ugraph)
//
////////////////////////////

template <class Graph_t>
class GraphFastRootSort{
	
public:
	enum {PRINT_DEGREE=0, PRINT_SUPPORT, PRINT_NODES};
	enum {MIN_DEGEN = 0, MAX_DEGEN, MIN_DEGEN_COMPO, MAX_DEGEN_COMPO, MAX, MIN, MAX_WITH_SUPPORT, MIN_WITH_SUPPORT, NONE };
	enum {FIRST_TO_LAST = 0, LAST_TO_FIRST=1 };
	enum {NEW_TO_OLD = 0, OLD_TO_NEW = 1 };
	
	static int SORT_SUBGRAPH_NON_INC_DEG(Graph_t& g, vint& lhs, vint& rhs, bool ftl= true);				//EXPERIMENTAL-sorting subgraphs
	static int SORT_SUBGRAPH_NON_DEC_DEG(Graph_t& g, vint& lhs, vint& rhs, bool ltf = true);			//EXPERIMENTAL-sorting subgraphs

	static void fill_vertices(vint&, int NV);															//fills vector with all vertices in NV [0..NV-1]
////////////////
//static infertace
public:
	static int compute_deg(const Graph_t& g, vint& deg);

////////////////
// data members	
protected:
	Graph_t& g;		

	int NV;
	vector<int> nb_neigh;
	vector<int> deg_neigh;

	typename Graph_t::_bbt node_active_state;			//1bit-active, 0bit-passive
	vector<int> nodes;

////////////////
// interface	
public:

////////////////////////
//construction / allocation
	GraphFastRootSort(Graph_t& gout)	:g(gout),	  NV(g.number_of_vertices()), nb_neigh(NULL), deg_neigh(NULL) { init();}
	~GraphFastRootSort(){}

	const vector<int>& get_degree() const { return nb_neigh; }

protected:
	int init();
	
public:
/////////////////////////
// useful operations	
	void fill_stack_root();												//fills stack with all vertices
	void compute_deg_root();											//computes number of neighbors
	void compute_support_root();										//computes sum of the number of neighbors 
	
	void fill_stack(vint& nodes_out) { nodes = nodes_out;}
	
////////////
//static: 
//1. requires computation of support and deg
//2. internally sets node stack 1...NV
	
	int  sort_non_increasing_deg(bool rev=false);						
	int  sort_non_decreasing_deg(bool rev=false);
	int  sort_non_increasing_deg_with_support_tb(bool rev=false);
	int  sort_non_decreasing_deg_with_support_tb(bool rev=false);

///////////
//degen: 
// 1. corrupt deg info: will need to be recomputed for sequences of sorts

	int  sort_degen_non_decreasing_degree(bool rev=false);				
	int  sort_degen_non_increasing_degree(bool rev=false);				
	
	//use a prior node stack info as reference order
	int sort_degen_composite_non_decreasing_degree( bool rev = false);		
	int sort_degen_composite_non_increasing_degree( bool rev = false);	
	
	//extension-TODO@substitute original primitives for n=0. n can be positive or negative (09/12/2020)
	// int sort_non_increasing_deg (vint& rhs, vint& lhs,  bool rev = false);
		  
    //int  sort_non_increasing_deg(int n, bool rev = false);
	//int  sort_non_decreasing_deg(int n, bool rev = false);
	//int  sort_non_increasing_deg_with_support_tb(int n, bool rev = false);
	//int  sort_non_decreasing_deg_with_support_tb(int n, bool rev = false);

///////////
//drivers
	vint new_order	(int alg, bool ltf=true, bool o2n=true);								/* interface for the framework */
	int reorder		(const vint& new_order, Graph_t& gn, Decode* d=NULL);					// (new) interface for the framework- TODO@build an in-place reordering as in the old GraphSort 	
	
////////////////////////
// I/O
	ostream& print(int type, ostream& o);
};

template<class Graph_t>
inline
void  GraphFastRootSort<Graph_t>::fill_vertices(vint& lv, int NV) {
	lv.clear();
	for (int i = 0; i < NV; i++) {
		lv.push_back(i);
	}
}

template<class Graph_t>
inline
vint GraphFastRootSort<Graph_t>::new_order (int alg, bool ltf, bool o2n){
/////////////////
//returns [OLD]->[NEW] mapping
	
	nodes.clear();
	if (alg == NONE) {
		LOG_ERROR("NONE alg. sorting petition detected, returning trivial isomorphism- GraphFastRootSort<Graph_t>::new_order()");
		for (int i = 0; i < NV; i++) {
			nodes.push_back(i);
		}
		return nodes;
	}
		
	if(alg==MIN_DEGEN){
		fill_stack_root();
		compute_deg_root();
		sort_degen_non_decreasing_degree(ltf);					//checked with framework!
	}else if (alg == MIN_DEGEN_COMPO) {
		compute_deg_root();
		compute_support_root();
		sort_non_decreasing_deg_with_support_tb(false /* MUST BE*/);
		sort_degen_composite_non_decreasing_degree(ltf);
	}else if (alg == MAX_DEGEN) {
		fill_stack_root();
		compute_deg_root();
		sort_degen_non_increasing_degree(ltf);
	}
	else if (alg == MAX_DEGEN_COMPO) {
		compute_deg_root();
		compute_support_root();
		sort_non_increasing_deg_with_support_tb(false /* MUST BE*/);
		sort_degen_composite_non_increasing_degree(ltf);
	}
	else if(alg == MAX){	
		compute_deg_root();
		sort_non_increasing_deg(ltf);
	}
	else if (alg == MIN) {		
		compute_deg_root();
		sort_non_decreasing_deg(ltf);
	}
	else if (alg == MAX_WITH_SUPPORT) {
		compute_deg_root();
		compute_support_root();
		sort_non_increasing_deg_with_support_tb(ltf);
	}
	else if (alg == MIN_WITH_SUPPORT) {
		compute_deg_root();
		compute_support_root();
		sort_non_decreasing_deg_with_support_tb(ltf);
	}
	else {
		LOG_ERROR(" GraphFastRootSort<Graph_t>::new_order-bizarre algorithm");
	}
		
	if (o2n) { Decode::reverse_in_place(nodes); }             //[OLD] to [NEW]
	return nodes;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_degree(bool rev){
	node_active_state.set_bit(0, NV-1);			//all active, pending to be ordered
	int min_deg=NV, v=EMPTY_ELEM;
	nodes.clear();

	for(int i=0; i<NV; i++){
		min_deg=NV;
		for(int j=0; j<NV; j++){
			if(node_active_state.is_bit(j) && nb_neigh[j] < min_deg){
				min_deg=nb_neigh[j];
				v=j;
			}
		}
		nodes.push_back(v);
		node_active_state.erase_bit(v);
		typename Graph_t::_bbt& bbn=g.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int w=bbn.next_bit();
			if(w==EMPTY_ELEM) break;
			if(node_active_state.is_bit(w))
				nb_neigh[w]--;
		}
	}//endFor

	if(rev){
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::sort_degen_non_increasing_degree(bool rev){
	node_active_state.set_bit(0, NV-1);											//all active, pending to be ordered
	int max_deg=0, v=EMPTY_ELEM;
	nodes.clear();

	for(int i=0; i<NV; i++){
		max_deg=-1;
		for(int j=0; j<NV; j++){
			if(node_active_state.is_bit(j) && nb_neigh[j] > max_deg){			 /* MUST BE >= */
				max_deg=nb_neigh[j];
				v=j;
			}
		}
		nodes.push_back(v);
		node_active_state.erase_bit(v);
		typename Graph_t::_bbt& bbn=g.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int w=bbn.next_bit();
			if(w==EMPTY_ELEM) break;
			if(node_active_state.is_bit(w))
				nb_neigh[w]--;
		}
	}//endFor

	if(rev){
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}

template<class Graph_t>
inline int GraphFastRootSort<Graph_t>::sort_degen_composite_non_decreasing_degree(bool rev)
{
	//////////////////
	// param@order:MUST BE a root ordering (mapping of all vertices, size- NV)

	node_active_state.set_bit(0, NV - 1);			//all active, pending to be ordered
	int min_deg = NV, v = EMPTY_ELEM;
	vint nodes_ori = nodes;
	nodes.clear();

	for (int i = 0; i < NV; i++) {
		min_deg = NV;
		for (int j = 0; j < NV; j++) {
			int u = nodes_ori[j];
			if (node_active_state.is_bit(u) && nb_neigh[u] < min_deg) {
				min_deg = nb_neigh[u];
				v = u;
			}
		}
		nodes.push_back(v);
		node_active_state.erase_bit(v);
		typename Graph_t::_bbt& bbn = g.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while (true) {
			int w = bbn.next_bit();
			if (w == EMPTY_ELEM) break;
			if (node_active_state.is_bit(w))
				nb_neigh[w]--;
		}
	}//endFor

	if (rev) {
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}

template<class Graph_t>
inline int GraphFastRootSort<Graph_t>::sort_degen_composite_non_increasing_degree(bool rev)
{
	//////////////////
	// param@order:MUST BE a root ordering (mapping of all vertices, size- NV)

	node_active_state.set_bit(0, NV - 1);											//all active, pending to be ordered
	int max_deg = 0, v = EMPTY_ELEM;
	vint nodes_ori = nodes;
	nodes.clear();

	//com::stl::print_collection<vint>(nodes_ori, cout, true);
	//cin.get();
	

	for (int i = 0; i < NV; i++) {
		max_deg = -1;
		for (int j = 0; j < NV; j++) {
			int u = nodes_ori[j];
			if (node_active_state.is_bit(u) && nb_neigh[u] > max_deg) {			 /* MUST BE >= */
				max_deg = nb_neigh[u];
				v = u;
			}
		}
		nodes.push_back(v);
		node_active_state.erase_bit(v);
		typename Graph_t::_bbt& bbn = g.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while (true) {
			int w = bbn.next_bit();
			if (w == EMPTY_ELEM) break;
			if (node_active_state.is_bit(w))
				nb_neigh[w]--;
		}

		//print(PRINT_DEGREE, cout);
		//cin.get();

	}//endFor

	if (rev) {
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}


template<class Graph_t>
inline
void GraphFastRootSort<Graph_t>::fill_stack_root(){
	nodes.clear();
	for(int i=0; i<NV; i++){
			nodes.push_back(i);
	}
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::sort_non_increasing_deg(bool rev){
	fill_stack_root();
	com::has_greater_val<int, int> pred(nb_neigh);
	std::stable_sort(nodes.begin(),  nodes.end(), pred);
	if(rev){
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}


template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(bool rev){
	fill_stack_root();
	com::has_smaller_val<int, int> pred(nb_neigh);
	std::stable_sort(nodes.begin(),  nodes.end(), pred);
	
	if(rev){
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::sort_non_increasing_deg_with_support_tb(bool rev ){
	fill_stack_root();
	com::has_greater_val_with_tb<int, int> pred(nb_neigh, deg_neigh);
	std::stable_sort(nodes.begin(),  nodes.end(), pred);

	if(rev){
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::sort_non_decreasing_deg_with_support_tb(bool rev){
	fill_stack_root();
	com::has_smaller_val_with_tb<int, int> pred(nb_neigh, deg_neigh);
	std::stable_sort(nodes.begin(),  nodes.end(), pred);

	if(rev){
		std::reverse(nodes.begin(), nodes.end());
	}
	return 0;
}

template<class Graph_t>
inline
void GraphFastRootSort<Graph_t>::compute_deg_root(){
	for(int elem=0; elem<NV; elem++){		
		nb_neigh[elem]=g.get_neighbors(elem).popcn64();

		//Graph_t::_bbt& bbn=g.get_neighbors(elem);		
	/*	bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int w=bbn.next_bit();
			if(w==EMPTY_ELEM) break;
			nb_neigh[elem]++;	
		}*/
	}
}

template<class Graph_t>
inline
void GraphFastRootSort<Graph_t>::compute_support_root(){
/////////////
// support: sum of number of neighbors for each vertex 
//       	(STATIC concept: can include the same vertex twice!)	 
	
	for(int elem=0; elem<NV; elem++){
		deg_neigh[elem]=0;
		typename Graph_t::_bbt& bbn=g.get_neighbors(elem);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int w=bbn.next_bit();
			if(w==EMPTY_ELEM) break;
			deg_neigh[elem]+=nb_neigh[w];	
		}
	}
}

template<class Graph_t>
inline
ostream& GraphFastRootSort<Graph_t>::print(int type, ostream& o){
	if(type==PRINT_DEGREE){
		for(int i=0; i<NV; i++){
			o<<nb_neigh[i]<<" ";
		}		
	}
	
	if(type==PRINT_SUPPORT){
		for(int i=0; i<NV; i++){
			o<<deg_neigh[i]<<" ";
		}		
	}

	if(type==PRINT_NODES){
		com::stl::print_collection<vint>(nodes, o);		
	}
	
	o<<endl;
	return o;
}

template<class Graph_t>
inline int GraphFastRootSort<Graph_t>::compute_deg(const Graph_t & g, vint & deg)
{
	int NV = g.number_of_vertices();	
	deg.assign(NV, -1);
	for (int v = 0; v < NV; v++) {
		deg[v] = g.get_neighbors(v).popcn64();
	}
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::init(){
	
	nb_neigh.assign(NV, 0);
	deg_neigh.assign(NV, 0);

	//nb_neigh= new int[NV];
	//deg_neigh= new int[NV];
	//for(int i=0; i<NV; i++){
	//	nb_neigh[i]=0;
	//	deg_neigh[i]=0;
	//}

	//if( nb_neigh==NULL ||  deg_neigh==NULL){
	//	perror("error: ");
	//	return -1;
	//}
	
	//nodes.init(NV);
	node_active_state.init(NV);	
	return 0;
}
//
//template<class Graph_t>
//inline
//void GraphFastRootSort<Graph_t>::clear(){
//	if(nb_neigh){
//		delete [] nb_neigh;
//	}
//	nb_neigh=NULL;
//
//	if(deg_neigh){
//		delete [] deg_neigh;
//	}
//	deg_neigh=NULL;
//}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::reorder(const vint& new_order, Graph_t& gn, Decode* d) {
	/////////////////////
	// EXPERIMENTAL-ONLY FOR SIMPLE GRAPHS
	//
	// param@new_order: mapping [OLD]->[NEW]
		
	int NV = g.number_of_vertices();
	gn.init(NV);
	gn.set_name(g.get_name(), false /* no path separation */ );	
	gn.set_path(g.get_path());
		
	//only for undirected graphs
	for (int i = 0; i < NV - 1; i++) {
		for (int j = i + 1; j < NV; j++) {
			if (g.is_edge(i, j)) {								//in O(log) for sparse graphs, should be specialized for that case
				//switch edges according to new numbering
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}

///////////////
//stores decoding information [NEW]->[OLD]
	if (d != NULL) {
		vint aux(new_order);
		Decode::reverse_in_place(aux);		//maps [NEW] to [OLD]		
		d->insert_ordering(aux);
	}

//////////////////////////////////
//WEIGHTS-old framework

	//reorder weights if required
	//if (g.is_weighted_v()) {
	//	gn.init_wv();
	//	for (int i = 0; i < size; i++) {
	//		gn.set_wv(new_order[i], g.get_wv(i));
	//	}
	//}


	////reorder edge-weights if required (CHECK@-eff)
	//if (g.is_edge_weighted()) {
	//	gn.init_we();
	//	for (int i = 0; i < size - 1; i++) {
	//		for (int j = i + 1; j < size; j++) {
	//			if (gn.is_edge(new_order[i], new_order[j])) {
	//				gn.set_we(new_order[i], new_order[j], g.get_we(i, j));
	//				gn.set_we(new_order[j], new_order[i], g.get_we(i, j));
	//			}
	//		}
	//	}
	//}

	//g = gn;

	////new order to stream if available
	//if (o != NULL)
	//	copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));

	return 0;
}

//template<>
//inline
//int GraphFastRootSort<ugraph_w>::reorder(const vint& new_order, ugraph_w& gn, Decode* d) {
//	/////////////////////
//	// EXPERIMENTAL-ONLY FOR SIMPLE GRAPHS
//	//
//	// param@new_order: mapping [OLD]->[NEW]
//
//	int NV = g.number_of_vertices();
//	gn.init(NV, 1.0);												//assigns unit weights						
//	gn.set_name(g.get_name(), false /* no path separation */);
//	gn.set_path(g.get_path());
//
//	//only for undirected graphs
//	for (int i = 0; i < NV - 1; i++) {
//		for (int j = i + 1; j < NV; j++) {
//			if (g.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
//				//switch edges according to new numbering
//				gn.add_edge(new_order[i], new_order[j]);
//			}
//		}
//	}
//
//	///////////////
//	//stores decoding information [NEW]->[OLD]
//	if (d != NULL) {
//		vint aux(new_order);
//		Decode::reverse_in_place(aux);				//maps [NEW] to [OLD]		
//		d->insert_ordering(aux);
//	}
//
//	/////////////////////
//	//weights (vertices) -update
//	for (int i = 0; i <NV; i++) {
//		gn.set_w(new_order[i], g.get_w(i));
//	}
//	
//	/////////////////////
//	//weights (edges)- TODO@add to edge-weighted graphs
//	
//	//reorder edge-weights if required (CHECK@-eff)
//	/*if (g.is_edge_weighted()) {
//		gn.init_we();
//		for (int i = 0; i < size - 1; i++) {
//			for (int j = i + 1; j < size; j++) {
//				if (gn.is_edge(new_order[i], new_order[j])) {
//					gn.set_we(new_order[i], new_order[j], g.get_we(i, j));
//					gn.set_we(new_order[j], new_order[i], g.get_we(i, j));
//				}
//			}
//		}
//	}*/
//
//	//g = gn;
//
//	////new order to stream if available
//	//if (o != NULL)
//	//	copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
//
//	return 0;
//}


///////////////////////////
//
// STATIC (stateless)
// 
// Experimental
///////////////////////////
template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::SORT_SUBGRAPH_NON_INC_DEG(Graph_t& g, vint& lhs, vint& rhs, bool ftl) {
///////////////////////////
// degenerate sorting  of  @lhs according to the degree of vertices wrt to @lhs and @rhs 
// degeneracy is only related to @lhs
	

	const int NV = g.number_of_vertices();
	vector<int> nb_neigh(NV, EMPTY_ELEM);
	//int* nb_neigh = new int[NV];
	/*for (int i = 0; i < NV; i++) {
		nb_neigh[i]=EMPTY_ELEM;
	}*/
	
	int deg = 0, vf=EMPTY_ELEM;
	vint res;

	//////////////////////////////////////////
	//determine degrees of lhs in rhs
	for (int i = 0; i < lhs.size(); i++) {
		int v = lhs[i];
		
		deg = 0;
		/////////////
		//deg in rhs
		for (int j = 0; j < rhs.size(); j++) {
			if (g.is_edge(v, rhs[j])) deg++;
		}	

		//////////////
		//deg in lhs
		for (int j = 0; j < lhs.size(); j++) {
			if (g.is_edge(v, lhs[j])) deg++;
		}

		nb_neigh[v] = deg;

	}	
	com::has_smaller_val<int, int> p(nb_neigh);

	//I/O
	/*com::stl::print_array(nb_neigh, nb_neigh+ g.number_of_vertices());
	LOG_INFO("-----------------");*/
	
	
	//////////////////////////////////////////
	for (int iter = 0; iter < lhs.size(); iter++) {
			
		vf = *std::max_element(lhs.begin(), lhs.end(), p);				
		res.push_back(vf);

		//update degs in lhs
		for (int i = 0; i < lhs.size(); i++) {
			if (g.is_edge(vf, lhs[i])) nb_neigh[lhs[i]]--;
		}
	
		nb_neigh[vf] = EMPTY_ELEM;
		
		

		//I/O
	/*	com::stl::print_array(nb_neigh, nb_neigh + g.number_of_vertices());
		LOG_INFO("-----------------");
		cin.get();*/
	}
	/////////////////////////////////////////////////////////////

	//assert
	if (res.size() != lhs.size()) {
		LOG_ERROR("bizarre sorting- GraphFastRootSort<Graph_t>::sort_non_increasing_deg(...), exiting...");
		exit(-1);
	}	
	
	
	lhs = res;
	if (ftl == false) {
		reverse(lhs.begin(), lhs.end());
	}
	
	//I/O
	/*stringstream sstr;
	com::stl::print_collection<vint>(lhs, sstr);
	LOG_INFO(sstr.str());
	LOG_INFO("GraphFastRootSort<Graph_t>::SORT_NON_INCREASING_DEG(...)---");
	cin.get();*/

	//delete[] nb_neigh;
	
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::SORT_SUBGRAPH_NON_DEC_DEG(Graph_t& g, vint& lhs, vint& rhs, bool ltf) {
	///////////////////////////
	// degenerate sorting  of  @lhs according to the degree of vertices wrt to @lhs and @rhs 
	// degeneracy is only related to @lhs

	const unsigned int MAX_INT = 0xFFFFFFF;
	const int NV = g.number_of_vertices();
	int* nb_neigh = new int[NV];

	for (int i = 0; i < NV; i++) {
		nb_neigh[i] = EMPTY_ELEM;
	}

	int deg = 0, vf = EMPTY_ELEM;
	vint res;

	//////////////////////////////////////////
	//determine degrees of lhs in rhs
	for (int i = 0; i < lhs.size(); i++) {
		int v = lhs[i];

		deg = 0;
		/////////////
		//deg in rhs
		for (int j = 0; j < rhs.size(); j++) {
			if (g.is_edge(v, rhs[j])) deg++;
		}

		//////////////
		//deg in lhs
		for (int j = 0; j < lhs.size(); j++) {
			if (g.is_edge(v, lhs[j])) deg++;
		}

		nb_neigh[v] = deg;

	}
	com::has_smaller_val<int, int /* vertex degrees */> p(nb_neigh);

	//I/O
	/*com::stl::print_array(nb_neigh, nb_neigh+ g.number_of_vertices());
	LOG_INFO("-----------------");*/


	//////////////////////////////////////////
	for (int iter = 0; iter < lhs.size(); iter++) {

		vf = *std::min_element(lhs.begin(), lhs.end(), p);
		res.push_back(vf);
		
		//update degs in lhs
		for (int i = 0; i < lhs.size(); i++) {
			if (g.is_edge(vf, lhs[i])) nb_neigh[lhs[i]]--;
		}

		nb_neigh[vf] = MAX_INT;
				

		//I/O
		/*com::stl::print_array(nb_neigh, nb_neigh + g.number_of_vertices());
		LOG_INFO("-----------------");
		cin.get();*/

	}
	/////////////////////////////////////////////////////////////

	//assert
	if (res.size() != lhs.size()) {
		LOG_ERROR("bizarre sorting- GraphFastRootSort<Graph_t>::sort_non_increasing_deg(...), exiting...");
		exit(-1);
	}


	lhs = res;

	///////////////
	//LAST TO FIRST by default
	if (ltf) {
		reverse(lhs.begin(), lhs.end());
	}

	//I/O
	/*stringstream sstr;
	com::stl::print_collection<vint>(lhs, sstr);
	LOG_INFO(sstr.str());
	LOG_INFO("GraphFastRootSort<Graph_t>::SORT_NON_INCREASING_DEG(...)---");
	cin.get();*/

	delete[] nb_neigh;

	return 0;
}






#endif

