//graph_sort.h: header for GraphSort class which sorts graphs by different criteria
//date: 20/11/15
//authors: pss

#ifndef __GRAPH_SORT_H__
#define __GRAPH_SORT_H__

#include "graph/graph.h"
#include "graph/algorithms/kcore.h"
#include "filter_graph_sort_type.h"			//Template Graph_t reduced to undirected types
#include "utils/logger.h"
#include "utils/common.h"
#include "decode.h"
#include <iostream>
#include <algorithm>


using namespace std;

///////////////
//
// namespace for GRAPH sort basic (enum) types
//
////////////////
namespace gbbs{
	enum sort_t						{MIN_DEG_DEGEN=0, MAX_DEG_DEGEN, MIN_DEG_DEGEN_TIE_STATIC, MAX_DEG_DEGEN_TIE_STATIC, MAX_DEG_DEGEN_TIE, KCORE, KCORE_UB, MAX_WEIGHT, MIN_WEIGHT, MAX_WEIGHT_DEG, MIN_WEIGHT_DEG, MAX_DEG_ABS, MIN_DEG_ABS, NONE};
	enum place_t					{PLACE_FL=0, PLACE_LF};
	enum pick_t						{PICK_MINFL=0, PICK_MINLF, PICK_MAXFL, PICK_MAXLF, PICK_FL, PICK_LF, PICK_MINABSFL,PICK_MAXABSFL, PICK_MINABSLF, PICK_MAXABSLF};
};


//vertex neighborhood info
struct deg_t{
	friend ostream & operator<<(ostream& o, const deg_t& d){ o<<d.index<<":("<<d.deg<<","<<d.deg_of_n<<")"; return o;}
	deg_t():index(EMPTY_ELEM), deg(0), deg_of_n(0){}
	deg_t(int ind, int deg):index(ind), deg(deg), deg_of_n(0){}
	int index;
	int deg;
	int deg_of_n;
};

typedef std::vector<int>				vint;	

///////////////////////////
//
// GraphSort class
// (Graph_t restricted to ugraph and sparse_ugraph)
//
////////////////////////////

template <class Graph_t>
class GraphSort: public filterGraphSortType<Graph_t>{
	static const int GRAPH_SORT_INFINITE=0x1FFFFFFF;
protected:
	typedef vector< deg_t >					vdeg;							
	typedef vdeg::iterator					vdeg_it;
		
	//sorting criteria
	struct degreeLess /* : public binary_function<deg_t, deg_t, bool>*/ {
			bool operator() (deg_t i,deg_t j) const { 
				return (abs(i.deg)<abs(j.deg));
			} 
	};

	struct degreeGreater/* : public binary_function<deg_t, deg_t, bool> */{
			bool operator() (deg_t i,deg_t j) const { 
				return (i.deg>j.deg);
			} 
	};

	struct degreeWithTieBreakLess /* : public binary_function<deg_t, deg_t, bool> */{
			bool operator() (deg_t i,deg_t j) const {
				if(i.deg<j.deg) return true;
				else if(i.deg==j.deg){
					if(i.deg_of_n<j.deg_of_n) return true;
				}
				return false;
			} 
	};
	
public:
	
	typedef vector< pair<gbbs::sort_t, gbbs::place_t> >				vpair;
	typedef typename vpair::iterator								vpair_it;
	
	//additional typedefs for simplified syntax (3/4/18)
	typedef GraphSort<Graph_t> _myt;
//	typedef typename Graph_t::_wt _wt;
	typedef typename Graph_t::_bbt _bbt;
	
	static void print				(const vint& order, bool revert=false, ostream& o=std::cout);			
	GraphSort						(Graph_t& gout):g(gout){}

//reorders graph (graph is CHANGED)
	int reorder						(const vint& new_order, ostream* o = NULL);					//gen. to weighted case		   
	int reorder_in_place			(const vint& new_order, ostream* o=NULL);					//only sparse unweighted
	int reorder						(const vint& new_order, Decode& d,  ostream* o = NULL);		//gen. to weighted case		
	int reorder_in_place			(const vint& new_order, Decode& d, ostream* o = NULL);		//only sparse unweighted

	int reorder						(const vint& new_order, Graph_t& gnew, Decode& d, ostream* o = NULL);	//2/10/17-sets new graph	

	//for big but not massive graphs (non-sparse data types-2/10/17)
	int reorder_edge_based			(const vint& new_order, ostream* o = NULL);	
	int reorder_edge_based			(const vint& new_order, Decode& d,  ostream* o = NULL);		//gen. to weighted case		
	int reorder_edge_based			(const vint& new_order, Graph_t& gnew, Decode& d, ostream* o = NULL);	
	//int reorder_edge_based		(const vint& new_order, ostream* o = NULL);						

//computes a reordering [OLD_INDEX]=NEW_INDEX
	vint new_order					(gbbs::sort_t alg, gbbs::place_t= gbbs::PLACE_LF);			//use by default
	vint new_order_fast				(gbbs::sort_t alg, gbbs::place_t= gbbs::PLACE_LF);			//fast version for large graphs (20/7/17)			
	vint new_order_fast_II			(gbbs::sort_t alg);											//Last to First only																					
	vint new_order_furini			(gbbs::sort_t alg, gbbs::place_t= gbbs::PLACE_LF);			//Fabio's implementation (20/7/17)
	
	//iterative variants (use get_v)
	vint new_order					(gbbs::pick_t, gbbs::place_t= gbbs::PLACE_LF);	
	vint new_order					(const _bbt& sg, gbbs::pick_t, gbbs::place_t=gbbs::PLACE_LF);																//reorders induced subgrapg sg
	vint new_order					(const _bbt& sgfrom, const _bbt& sgref, gbbs::pick_t, gbbs::place_t, bool is_degen);	
	int change_order				(const _bbt& sgfrom, const _bbt& sgref, vint& ord, gbbs::pick_t, gbbs::place_t, bool is_degen);		

private:
	vint new_order_kcore			(gbbs::place_t= gbbs::PLACE_LF);
	vint new_order_kcore_UB			(gbbs::place_t= gbbs::PLACE_FL);				//finds kcore number and uses it to create a deg ordering with vertices with kcore degrees first
	
	//weights: only for weighted graph types
	vint new_order_weighted			(gbbs::place_t= gbbs::PLACE_FL, bool max_weight=true);	
	
	/*template<class Weight_t=int>*/
	vint new_order_weighted_deg		(gbbs::place_t= gbbs::PLACE_FL, bool max_weight=true);	
	

public:

//computes a reordering of the subgraph not accesible by vertex index
	vint new_subg_order				(gbbs::sort_t, _bbt&,  gbbs::place_t= gbbs::PLACE_LF);					//cannot be used as input to REORDER functions
	
///////////////
//composite orderings
	int reorder_composite			(vpair&, Decode& d,  ostream* o = NULL);
	
///////////////
// degree computation -**TODO-change to a Graph method?
	int sum_of_neighbor_deg			(int v);																//computes support(sum of degree of neighbors)
	int sum_of_neighbor_deg			(int v, const _bbt& subgraph);		

////////////////
// vertex selection primitives
	int get_v						(gbbs::pick_t= gbbs::PICK_MINFL);	
	int	get_v						(_bbt& sg, gbbs::pick_t= gbbs::PICK_MINFL);	
	int get_v						(_bbt& sgfrom, const _bbt& sgref, gbbs::pick_t= gbbs::PICK_MINFL);

protected:
////////////////
// data members	
	Graph_t& g;																									
};

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order_weighted (gbbs::place_t place, bool max_weight){
/////////////////////
// Orders vertices by weights (absolute stable ordering)
// 
// RETURNS a valid ordering O[OLD_INDEX]=NEW_INDEX

	struct wset_t{
		int v;
		typename Graph_t::_wt  wv;
		void print(ostream& o)	{o<<v<<":"<<wv<<" ";}
	};

	const int NV=g.number_of_vertices();
	vector<wset_t> lv(NV);
	for(int i=0; i<NV; i++){
		lv[i].v=i;
		lv[i].wv=g.weight(i);
		
	}

	struct weightMore: 
		public binary_function<wset_t, wset_t, bool>{
			bool operator() (wset_t i, wset_t j) const {
				if(i.wv>j.wv) return true;
				return false;
			} 
	};
	
	struct weightLess: 
		public binary_function<wset_t, wset_t, bool>{
			bool operator() (wset_t i,wset_t j) const {
				if(i.wv<j.wv) return true;
				return false;
			} 
	};

	//max or min depending on the flag
	if(max_weight)
		sort(lv.begin(), lv.end(), weightMore() );
	else sort(lv.begin(), lv.end(), weightLess() );

	/*for(vector<wset_t>::iterator it=lv.begin(); it!=lv.end(); it++){
		(*it).print(cout);
	}
	cout<<endl;*/
	
	//copy results and leave
	vint vres(NV);
	int k;
	(place==gbbs::place_t::PLACE_FL)? k=0: k=NV-1;
	for(int i=0; i<NV; i++){
		vres[lv[i].v]=k;
		(place==gbbs::place_t::PLACE_FL)? k++: k--;
	}

	return vres;
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order_weighted_deg	(gbbs::place_t place, bool max_weight){
/////////////////////
// Orders vertices by weights*deg (absolute stable ordering)
// 
// RETURNS a valid ordering O[OLD_INDEX]=NEW_INDEX

	struct wset_t{
		int v;
		typename Graph_t::_wt wv;
		int degv;
		void print(ostream& o)	{o<<v<<":"<<wv<<" ";}
	};
	const int NV=g.number_of_vertices();
	vector<wset_t> lv(NV);
	for(int i=0; i<NV; i++){
		lv[i].v=i;
		lv[i].wv=g.weight(i);
		lv[i].degv=g.degree(i);
	}

	struct weightDegMore: 
		public binary_function<wset_t, wset_t, bool>{
			bool operator() (wset_t i,wset_t j) const {
				if(i.wv*i.degv>j.wv*j.degv) return true;
				return false;
			} 
	};


	struct weightDegLess: 
		public binary_function<wset_t, wset_t, bool>{
			bool operator() (wset_t i,wset_t j) const {
				if(i.wv*i.degv<j.wv*j.degv) return true;
				return false;
			} 
	};

	//max or min depending on the flag
	if(max_weight)
		sort(lv.begin(), lv.end(), weightDegMore() );
	else sort(lv.begin(), lv.end(), weightDegLess() );

	/*for(vector<wset_t>::iterator it=lv.begin(); it!=lv.end(); it++){
		(*it).print(cout);
	}
	cout<<endl;*/
	
	//copy results and leave
	vint vres(NV);
	int k;
	(place==gbbs::place_t::PLACE_FL)? k=0: k=NV-1;
	for(int i=0; i<NV; i++){
		vres[lv[i].v]=k;
		(place==gbbs::place_t::PLACE_FL)? k++: k--;
	}

	return vres;
}


template<class Graph_t>
int GraphSort<Graph_t>::sum_of_neighbor_deg(int v){
/////////////////////////
//Sum of degrees of neighbors to v in the current graph considered

	int ndeg=0,vadj=EMPTY_ELEM;
	if(g.neighbors(v).init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			vadj=((g.neighbors(v)).next_bit());
			if(vadj==EMPTY_ELEM) break;
			ndeg+=g.degree(vadj);
		}
	}
return ndeg;
}

template<class Graph_t>
int GraphSort<Graph_t>::sum_of_neighbor_deg(int v, const _bbt& sg){
/////////////////////////
//Sum of degrees of neighbors to v in the current graph considered, circumscribed to sg

	int ndeg=0,vadj=EMPTY_ELEM;
	_bbt nset(g.number_of_vertices());
	AND(sg, g.neighbors(v), nset);
	if(nset.init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			vadj=nset.next_bit();
			if(vadj==EMPTY_ELEM) break;
			ndeg+=g.degree(vadj, sg);
		}
	}
return ndeg;
}

template<class Graph_t>
void GraphSort<Graph_t>::print(const std::vector<int>& new_order, bool revert, ostream& o){
	o<<"new order: ";
	if(revert){
		copy(new_order.rbegin(), new_order.rend(), ostream_iterator<int>(o, " "));
	}else{
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(o, " "));
	}
o<<endl;
}

template<class Graph_t>
inline
int GraphSort<Graph_t>::reorder(const vint& new_order, ostream* o){
/////////////////////
// reordering in place
// new order logs to "o"
//
// Extended to the weighted case (9/10/16)
// Extended to the edge-weighted case (14/08/18)  (experimental)
//
// REMARKS: 
// 1-Experimental (uses auxiliary graph: should be done truly in place)
// 2-only for undirected graphs

	//control
	if(new_order.size()!=g.number_of_vertices()){
		LOG_ERROR("reorder: cannot reorder graph");
		return -1;
	}

	int size=g.number_of_vertices();
	Graph_t gn(size);
	gn.name(g.name());
	
		
	//only for undirected graphs
	for(int i=0; i<size-1; i++){
		for(int j=i+1; j<size; j++){
			if(g.is_edge(i,j)){								//in O(log) for sparse graphs, should be specialized for that case
				//switch edges according to new numbering
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}

	//reorder weights if required
	//if(g.is_weighted_v()){
	//	gn.init_wv();
	//	for(int i=0; i<size; i++){
	//		//gn.set_wv(new_order[i], g.get_wv(i));
	//		gn.set_w(new_order[i], g.get_wv(i));
	//	}
	//}

	
	//reorder edge-weights if required (CHECK@-eff)
	/*if(g.is_edge_weighted()){
		gn.init_we();
		for(int i=0; i<size-1; i++){
			for(int j=i+1; j<size; j++){
				if(gn.is_edge(new_order[i],new_order[j])){
					gn.set_we(new_order[i],new_order[j],g.get_we(i,j));
					gn.set_we(new_order[j],new_order[i],g.get_we(i,j));
				}
			}
		}
	}*/

	g=gn;
			
	//new order to stream if available
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}


template<>
inline
int GraphSort<ugraph_w>::reorder(const vint& new_order, ostream* o) {
	/////////////////////
	// reordering in place
	// new order logs to "o"
	//
	// Extended to the weighted case (9/10/16)
	// Extended to the edge-weighted case (14/08/18)  (experimental)
	//
	// REMARKS: 
	// 1-Experimental (uses auxiliary graph: should be done truly in place)
	// 2-only for undirected graphs

		//control
	if (new_order.size() != g.number_of_vertices()) {
		LOG_ERROR("reorder: cannot reorder graph");
		return -1;
	}

	int size = g.number_of_vertices();
	ugraph_w gn(size);
	gn.name(g.graph().name());


	//only for undirected graphs
	for (int i = 0; i < size - 1; i++) {
		for (int j = i + 1; j < size; j++) {
			if (g.is_edge(i, j)) {								//in O(log) for sparse graphs, should be specialized for that case
				//switch edges according to new numbering
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}

	//reorder weights if required
	//if (g.is_weighted_v()) {
		//gn.init_wv();
		gn.set_weight();												//allocates and sets unit weights
		for (int i = 0; i < size; i++) {
			gn.set_weight(new_order[i], g.weight(i));
		}
	//}


	//reorder edge-weights if required (CHECK@-eff)
	/*if(g.is_edge_weighted()){
		gn.init_we();
		for(int i=0; i<size-1; i++){
			for(int j=i+1; j<size; j++){
				if(gn.is_edge(new_order[i],new_order[j])){
					gn.set_we(new_order[i],new_order[j],g.get_we(i,j));
					gn.set_we(new_order[j],new_order[i],g.get_we(i,j));
				}
			}
		}
	}*/

	g = gn;

	//new order to stream if available
	if (o != NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));

	return 0;
}



template<class Graph_t>
inline
int GraphSort<Graph_t>::reorder_edge_based(const vint& new_order, ostream* o){
/////////////////////
// reordering in place
// new order logs to "o"
//
// Extended to the weighted case (9/10/16)
//
// REMARKS: 
// 1-Experimental (uses auxiliary graph: should be done truly in place)
// 2-only for undirected graphs

	//control
	const int NV=g.number_of_vertices();
	if(new_order.size()!=NV){
		LOG_ERROR("GraphSort<Graph_t>::reorder_edge_based()- bizarre size of nodelist, cannot reorder graph");
		return -1;
	}
		
	Graph_t gn(NV);
	gn.name(g.name());
	

	for(int v=0; v<NV-1; v++){
		_bbt& nn=g.neighbors(v);
		nn.init_scan(v, bbo::NON_DESTRUCTIVE);
		while(true){
			int w=nn.next_bit();
			if(w==EMPTY_ELEM) break;

			gn.add_edge(new_order[v], new_order[w]);			
		}
	}
	
	//reorder weights if required
	if(g.is_weighted_v()){
		gn.init_wv();
		for(int i=0; i<NV; i++){
			gn.set_wv(new_order[i], g.get_wv(i));
		}
	}

	g=gn;
			
	//new order to stream if available
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}


template<class Graph_t>
inline
int GraphSort<Graph_t>::reorder (const vint& new_order, Decode& d,  ostream* o){
/////////////////////
// reordering in place and stores a way of decoding the new vertex indexes 
// new order logs to "o"
//
// Extended to the weighted case (9/10/16)
// Extended to the edge-weighted case (14/08/18)
//
// REMARKS: 
// 1-Experimental (uses auxiliary graph: should be done truly in place)
// 2-only for undirected graphs
// 3-Assumes Decode state is properly updated before the call


	//control
	if(new_order.size()!=g.number_of_vertices()){
		LOG_ERROR("reorder: cannot reorder graph");
		return -1;
	}

	int size=g.number_of_vertices();
	Graph_t gn(size);
	gn.name(g.name());
	

	//only for undirected graphs
	for(int i=0; i<size-1; i++){
		for(int j=i+1; j<size; j++){
			if(g.is_edge(i,j)){								//in O(log) for sparse graphs, should be specialized for that case
				//switch edges according to new numbering
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}

	//reorder weights if required
	//if(g.is_weighted_v()){
	//	gn.init_wv();
	//	for(int i=0; i<size; i++){
	//		//gn.set_wv(new_order[i], g.get_wv(i));
	//		gn.set_w(new_order[i], g.get_wv(i));
	//	}
	//}

	//reorder edge-weights if required (CHECK@-eff)
	/*if(g.is_edge_weighted()){
		gn.init_we();
		for(int i=0; i<size-1; i++){
			for(int j=i+1; j<size; j++){
				if(gn.is_edge(new_order[i],new_order[j])){
					gn.set_we(new_order[i],new_order[j],g.get_we(i,j));
					gn.set_we(new_order[j],new_order[i],g.get_we(i,j));
				}
			}
		}
	}*/



	//copy back to the original graph
	g=gn;
			
	//decode
	vint aux(new_order);
	Decode::reverse_in_place(aux);				//changes to [NEW_INDEX]=OLD_INDEX
	d.insert_ordering(aux);

	
	//new order to stream if available
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}


template<>
inline
int GraphSort<ugraph_w>::reorder(const vint& new_order, Decode& d, ostream* o) {
	/////////////////////
	// reordering in place and stores a way of decoding the new vertex indexes 
	// new order logs to "o"
	//
	// Extended to the weighted case (9/10/16)
	// Extended to the edge-weighted case (14/08/18)
	//
	// REMARKS: 
	// 1-Experimental (uses auxiliary graph: should be done truly in place)
	// 2-only for undirected graphs
	// 3-Assumes Decode state is properly updated before the call


		//control
	if (new_order.size() != g.number_of_vertices()) {
		LOG_ERROR("reorder: cannot reorder graph");
		return -1;
	}

	int size = g.number_of_vertices();
	ugraph_w gn(size);
	gn.name(g.graph().name());


	//only for undirected graphs
	for (int i = 0; i < size - 1; i++) {
		for (int j = i + 1; j < size; j++) {
			if (g.is_edge(i, j)) {								//in O(log) for sparse graphs, should be specialized for that case
				//switch edges according to new numbering
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}

	//reorder weights if required
	//if (g.is_weighted_v()) {
		//gn.init_wv();
		gn.set_weight();											//allocates weight space and assigns unit weights
		for (int i = 0; i < size; i++) {
			//gn.set_wv(new_order[i], g.get_wv(i));
			gn.set_weight(new_order[i], g.weight(i));
		}
	//}

	
	//copy back to the original graph
	g = gn;

	//decode
	vint aux(new_order);
	Decode::reverse_in_place(aux);				//changes to [NEW_INDEX]=OLD_INDEX
	d.insert_ordering(aux);


	//new order to stream if available
	if (o != NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));

	return 0;
}

template<class Graph_t>
inline
int GraphSort<Graph_t>::reorder_edge_based (const vint& new_order, Decode& d,  ostream* o){
/////////////////////
// reordering in place and stores a way of decoding the new vertex indexes 
// new order logs to "o"
//
// Extended to the weighted case (9/10/16)
// Extended to the edge-weighted case (14/08/18) -Experimental
//
// REMARKS: 
// 1-Experimental (uses auxiliary graph: should be done truly in place)
// 2-only for undirected graphs
// 3-Assumes Decode state is properly updated before the call
//
// /* TODO-OPTIMIZE IN PLACE- Since edges are few, possibly store all edges and then overwrite? */


	//control
	const int NV=g.number_of_vertices();
	if(new_order.size()!=NV){
		LOG_ERROR("reorder: cannot reorder graph");
		return -1;
	}
		
	Graph_t gn(NV);
	gn.name(g.name());
	

	for(int v=0; v<NV-1; v++){
		_bbt& nn=g.neighbors(v);
		nn.init_scan(v, bbo::NON_DESTRUCTIVE);
		while(true){
			int w=nn.next_bit();
			if(w==EMPTY_ELEM) break;

			gn.add_edge(new_order[v], new_order[w]);			
		}
	}
	

	//reorder weights if required
	if(g.is_weighted_v()){
		gn.init_wv();
		for(int i=0; i<NV; i++){
			gn.set_wv(new_order[i], g.get_wv(i));
		}
	}

	//reorder edge-weights if required (CHECK@-eff)
	if(g.is_edge_weighted()){
		gn.init_we();
		for(int i=0; i<NV-1; i++){
			for(int j=i+1; j<NV; j++){
				gn.set_we(new_order[i],new_order[j],g.get_we(i,j));
				gn.set_we(new_order[j],new_order[i],g.get_we(i,j));
			}
		}
	}

	//copy back to the original graph
	g=gn;
		
		
	//decode
	vint aux(new_order);
	Decode::reverse_in_place(aux);				//changes to [NEW_INDEX]=OLD_INDEX
	d.insert_ordering(aux);

	
	//new order to stream if available
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}

template<class Graph_t>
inline
int GraphSort<Graph_t>::reorder (const vint& new_order, Graph_t& gn, Decode& d, ostream* o){
/////////////////////
// reorders to a new graph: gn
// decode contains [NEW]->[OLD] mapping as last element
//
// RETURNS 0 ok, -1 error
//
// date:2/10/17

	
	const int NV=g.number_of_vertices();
	if(new_order.size()!=NV){							 /* early exit */
		LOG_ERROR("reorder: cannot reorder graph");
		return -1;
	}
	gn.init(NV);
	gn.name(g.name());
	

	//only for undirected graphs
	for(int i=0; i<NV-1; i++){
		for(int j=i+1; j<NV; j++){
			if(g.is_edge(i,j)){								//in O(log) for sparse graphs, should be specialized for that case
				gn.add_edge(new_order[i], new_order[j]);   
			}
		}
	}

	//reorder weights if required
	if(g.is_weighted_v()){
		gn.init_wv();
		for(int i=0; i<NV; i++){
			gn.set_wv(new_order[i], g.get_wv(i));
		}
	}
					
	//decode- **TODO optimize (too much copying)
	vint aux(new_order);
	Decode::reverse_in_place(aux);				//changes to [NEW_INDEX]=OLD_INDEX
	d.insert_ordering(aux);
		
	//new order to stream if available
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}

template<class Graph_t>
inline
int GraphSort<Graph_t>::reorder_edge_based (const vint& new_order, Graph_t& gn, Decode& d, ostream* o){
/////////////////////
// reorders to a new graph: gn
// decode contains [NEW]->[OLD] mapping as last element
//
// RETURNS 0 ok, -1 error
//
// date:30/10/17
	
	const int NV=g.number_of_vertices();
	if(new_order.size()!=NV){							 /* early exit */
		LOG_ERROR("reorder: cannot reorder graph");
		return -1;
	}
	gn.init(NV);
	gn.name(g.name());

	//edge based
	for(int v=0; v<NV-1; v++){
		_bbt& nn=g.neighbors(v);
		nn.init_scan(v, bbo::NON_DESTRUCTIVE);
		while(true){
			int w=nn.next_bit();
			if(w==EMPTY_ELEM) break;

			gn.add_edge(new_order[v], new_order[w]);			
		}
	}
	

	//reorder weights if required
	if(g.is_weighted_v()){
		gn.init_wv();
		for(int i=0; i<NV; i++){
			gn.set_wv(new_order[i], g.get_wv(i));
		}
	}
					
	//decode info (one copy too much?)
	vint aux(new_order);
	Decode::reverse_in_place(aux);						//changes to [NEW_INDEX]=OLD_INDEX
	d.insert_ordering(aux);
		
	//I/O
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}

template<>
inline
int GraphSort<sparse_ugraph>::reorder(const vint& new_order, ostream* o){
/////////////////////
// specialization for sparse graphs
//
// REMARKS: 
// 1-Experimental (uses auxiliary graph: should be done truly in place)
// 2-only for undirected graphs



	//control
	if(new_order.size()!=g.number_of_vertices()){
		LOG_ERROR("reorder: cannot reorder sparse undirected graph");
		return -1;
	}

	int size=g.number_of_vertices();
	sparse_ugraph gn(size);
	gn.name(g.name());
	
	//only for undirected graphs
	int j=EMPTY_ELEM;
	for(int i=0; i<size-1; i++){
		sparse_bitarray neigh=g.neighbors(i);
		if(neigh.init_scan(i, bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
			while(true){
				j=neigh.next_bit();
				if(j==EMPTY_ELEM)
						break;
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}
	g=gn;
			
	//new order to stream if available
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}

template<>
inline
int GraphSort<sparse_ugraph>::reorder(const vint& new_order, Decode& d, ostream* o){
/////////////////////
// specialization for sparse graphs with decoding
//
// REMARKS: 
// 1-Experimental (uses auxiliary graph: should be done truly in place)
// 2-only for undirected graphs
	
	//control
	if(new_order.size()!=g.number_of_vertices()){
		LOG_ERROR("reorder: cannot reorder sparse undirected graph");
		return -1;
	}

	int size=g.number_of_vertices();
	sparse_ugraph gn(size);
	gn.name(g.name());
	
	//only for undirected graphs
	int j=EMPTY_ELEM;
	for(int i=0; i<size-1; i++){
		sparse_bitarray neigh=g.neighbors(i);
		if(neigh.init_scan(i, bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
			while(true){
				j=neigh.next_bit();
				if(j==EMPTY_ELEM)
						break;
				gn.add_edge(new_order[i], new_order[j]);
			}
		}
	}
	g=gn;
		
	//decode
	vint aux(new_order);
	Decode::reverse_in_place(aux);				//changes to [NEW_INDEX]=OLD_INDEX
	d.insert_ordering(aux);
				
	//new order to stream if available
	if(o!=NULL)
		copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
	
return 0;		
}

template<class Graph_t>
inline
int GraphSort<Graph_t>::reorder_composite (vpair& lord, Decode& d,  ostream* o ){
//////////////////
// iterates over the list and reorders the graph accordingly
// stores in decoder each change in format [NEW_INDEX]=OLD_INDEX
// for a later decoding
//
// RETURNS -1 if ERROR, 0 if OK

	d.clear();
	
	for(vpair_it it=lord.begin(); it!=lord.end(); it++){
		pair<gbbs::sort_t, gbbs::place_t> ord=*it;

		//sort
		switch(ord.first){
		case gbbs::MIN_DEG_DEGEN:
			reorder(new_order(gbbs::MIN_DEG_DEGEN,ord.second),d,o);
			break;
		case gbbs::MAX_DEG_DEGEN:
			reorder(new_order(gbbs::MAX_DEG_DEGEN,ord.second),d,o);
			break;
		case gbbs::MIN_DEG_DEGEN_TIE_STATIC:
			reorder(new_order(gbbs::MIN_DEG_DEGEN_TIE_STATIC,ord.second),d,o);
			break;
		case gbbs::NONE:
			reorder(new_order(gbbs::NONE,ord.second),d,o);
			break;
		//others
		default:
			LOG_ERROR("GraphSort::reorder_composite: unknown algorithm");
			return -1;
		}
	}

	return 0;	//OK
}


template<>
inline
int GraphSort<ugraph>::reorder_in_place(const vint& new_order, ostream* o){
	struct this_type_is_not_available_for_GraphSort{};
	return 0;
}

template<>
inline
int GraphSort<ugraph>::reorder_in_place(const vint& new_order,  Decode& d,  ostream* o){
	struct this_type_is_not_available_for_GraphSort{};
	return 0;
}

template<>
inline
int GraphSort<sparse_ugraph>::reorder_in_place(const vint& new_order, ostream* o){
/////////////////////
// Reorders graph in place (only for large sparse UNDIRECTED graphs)
// date: 22/12/14
// author: Alvaro Lopez
//
// COMMENTS: Optimize for space requirements

	//control
	int N=g.number_of_vertices();
	if(new_order.size()!=N){
        LOG_ERROR("reorder_in_place: cannot reorder sparse undirected graph");
        return -1;
    }
	   	
	//Deletes lower triangle of adjacency matrix
	LOG_DEBUG("deleting low triangle--------------------");
    for(int i=0; i<N; i++){
        g.neighbors(i).clear_bit(0,i);
		g.neighbors(i).shrink_to_fit();
	}
		
	LOG_DEBUG("new order upper to lower triangle--------------");		
	sparse_bitarray neigh;
    int j=EMPTY_ELEM;
    for(int i=0; i<N; i++){
        neigh=g.neighbors(i);
		//reorders using upper triangle information
        if(neigh.init_scan(i, bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		    while(true){
                j=neigh.next_bit();
                if(j==EMPTY_ELEM)
                        break;
			
				//writes new edge in lower triangle
                if(new_order[i]>new_order[j]){
					g.neighbors(new_order[i]).set_bit(new_order[j]);
				} else{
                    g.neighbors(new_order[j]).set_bit(new_order[i]);
				}
            }
        }
		//Deletes each neighborhood once read
		g.neighbors(i).clear_bit(i, N-1);
		g.neighbors(i).shrink_to_fit();
    }

    //Makes the graph bidirected: copies information from lower to upper triangle of the adjacency matrix
	LOG_DEBUG("making graph bidirected--------------------");	
    for(int i=0; i<N; i++){
        neigh=g.neighbors(i);
        if(neigh.init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){ 
            while(true){
                j=neigh.next_bit();
                if((j==EMPTY_ELEM) || (j>i))
                        break;
				g.neighbors(j).set_bit(i);
	        }
        }
    }
	   
    //new order to stream if available
    if(o!=NULL)
        copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
  return 0;        
}

template<>
inline
int GraphSort<sparse_ugraph>::reorder_in_place(const vint& new_order, Decode& d, ostream* o){
/////////////////////
// Reorders graph in place and stores a way of decoding the vertices (only for large sparse UNDIRECTED graphs)
// date: 22/12/14
// author: Alvaro Lopez
//
// COMMENTS: Opimmize for space requirements


    //control
	int N=g.number_of_vertices();
	if(new_order.size()!=N){
        LOG_ERROR("reorder_in_place: cannot reorder sparse undirected graph");
        return -1;
    }
	   	
	//Deletes lower triangle of adjacency matrix
	LOG_DEBUG("deleting low triangle--------------------");
    for(int i=0; i<N; i++){
        g.neighbors(i).clear_bit(0,i);
		g.neighbors(i).shrink_to_fit();
	}
		
	LOG_DEBUG("new order upper to lower triangle--------------");		
	sparse_bitarray neigh;
    int j=EMPTY_ELEM;
    for(int i=0; i<N; i++){
        neigh=g.neighbors(i);
		//reorders using upper triangle information
        if(neigh.init_scan(i, bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		    while(true){
                j=neigh.next_bit();
                if(j==EMPTY_ELEM)
                        break;
			
				//writes new edge in lower triangle
                if(new_order[i]>new_order[j]){
					g.neighbors(new_order[i]).set_bit(new_order[j]);
				} else{
                    g.neighbors(new_order[j]).set_bit(new_order[i]);
				}
            }
        }
		//Deletes each neighborhood once read
		g.neighbors(i).clear_bit(i, N-1);
		g.neighbors(i).shrink_to_fit();
    }

    //Makes the graph bidirected: copies in54.-51kl,.5formation from lower to upper triangle
	LOG_DEBUG("making graph bidirected--------------------");	
    for(int i=0; i<N; i++){
        neigh=g.neighbors(i);
        if(neigh.init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){ 
            while(true){
                j=neigh.next_bit();
                if((j==EMPTY_ELEM) || (j>i))
                        break;
				g.neighbors(j).set_bit(i);
	        }
        }
    }

	//decode info
	vint aux(new_order);
	Decode::reverse_in_place(aux);				//changes to [NEW_INDEX]=OLD_INDEX
	d.insert_ordering(aux);
	   
    //new order to stream if available
    if(o!=NULL)
        copy(new_order.begin(), new_order.end(), ostream_iterator<int>(*o, " "));
  return 0;    
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order_fast (gbbs::sort_t alg, gbbs::place_t place){
/////////////////////
// fast variant currently only for basic cases (i.e. MIN_WIDTH)
// 
// COMMENTS-currently only for non-sparse graphs
	
	const int NV=g.number_of_vertices();
	vint new_order(NV);
	vdeg degs(NV);	
	int k=0; 
	if(place==gbbs::PLACE_LF) k=NV-1;
		
	//computes degree of vertices
	for(int i=0; i<NV; i++){
		deg_t vt;
		vt.index=i;
		vt.deg=g.degree(i);
		degs[i]=vt;		
	}
		
	//computes order
//	_bbt bbn(NV);
//	bbn.set_bit(0,NV-1);
	int counter=NV;
	switch(alg){
	case gbbs::MIN_DEG_DEGEN:
		while(counter-->0){
			vdeg_it itv=min_element(degs.begin(), degs.end(), degreeLess());			
			int v=itv->index;
			itv->deg=2*NV;									//so that it will not be chosen again; specific for MIN_DEG	
				
			new_order[v]=k;
			(place==gbbs::PLACE_LF)? k-- : k++;
		//	bbn.erase_bit(v);
				
			//recompute degrees-A
			//AND(g.neighbors(v),bbn, neigh);
			//neigh.init_scan(bbo::DESTRUCTIVE);
			//while(true){
			//	int w=neigh.next_bit_del();
			//	if(w==EMPTY_ELEM) break;
			//	degs[w].deg--;
			//	if(degs[w].deg<0){
			//		LOG_ERROR("GraphFastSort<Graph_t>::new_order()-bizarre deg value-----------------");
			//		cin.get();
			//	}
			//}
			
			//recompute degrees-B
		/*	AND(g.neighbors(v),bbn, neigh);
			neigh.init_scan(bbo::DESTRUCTIVE);*/
			_bbt& vneigh=g.neighbors(v);
			vneigh.init_scan(bbo::NON_DESTRUCTIVE);
			while(true){
				int w=vneigh.next_bit();
				if(w==EMPTY_ELEM) break;
		//		if(bbn.is_bit(w))
						degs[w].deg--;
				/*if(degs[w].deg<0){
					LOG_ERROR("GraphFastSort<Graph_t>::new_order()-bizarre deg value-----------------");
					cin.get();
				}*/
			}
		}
		break;
	default:
		LOG_ERROR("GraphFastSort<Graph_t>::new_order_fast: unknown ordering strategy");
		vint vempty;
		vempty.swap(new_order);
	}
return new_order;
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order_fast_II (gbbs::sort_t alg){
/////////////////////
// fast variant currently only for basic cases (i.e. MIN_WIDTH, Last to First)
// 
// COMMENTS-currently only for non-sparse graphs
//
		
	const int NV=g.number_of_vertices();
	vint new_order(NV);
	deg_t* degs= new deg_t[NV];
	int* pos_degs= new int[NV];
	int k=NV-1;							//gbbs::place_t place:LF

	//int k=0; 
	//if(place==gbbs::PLACE_LF) k=NV-1;
		

	//computes degree of vertices
	for(int i=0; i<NV; i++){ 
		deg_t vt(i,g.degree(i));
		degs[i]=vt;	
		pos_degs[i]=i;
	}
	
	
	int counter=NV;
	switch(alg){
	case gbbs::MIN_DEG_DEGEN:
		while(k>0){
			deg_t* pv=min_element(&degs[0], &degs[k+1] /* end: init NV*/, degreeLess());			
			int v=pv->index; 
			new_order[v]=k;
		//	(place==gbbs::PLACE_LF)? k-- : k++;
			
			//swap
			if(pv != &degs[k]){
				int ori_v=pv-&degs[0];
				pos_degs[degs[k].index]=ori_v;
				pos_degs[v]=k;
				deg_t temp=degs[ori_v]; degs[ori_v]=degs[k]; degs[k]=temp;
			}

			k--;
						
			
			//	com::stl::print_collection(position_in_deg); cout<<endl;

			//sparse update of degrees selectively
			_bbt& vneigh=g.neighbors(v);
			bool first_time=false;
			if(degs[v].deg>counter || first_time ){
			//	LOG_INFO("updating by degrees");
				for(int i=0; i<counter; i++){
					if(vneigh.is_bit(degs[i].index))
						degs[i].deg--;
				}
				first_time=true;
			}else{
				vneigh.init_scan(bbo::NON_DESTRUCTIVE);
				while(true){
					int w=vneigh.next_bit();
					if(w==EMPTY_ELEM) break;
					degs[pos_degs[w]].deg--;
					/*if(degs[w].deg<0){
					LOG_ERROR("GraphFastSort<Graph_t>::new_order()-bizarre deg value-----------------");
					cin.get();
					}*/
				}
			}
		}
		break;
	default:
		LOG_ERROR("GraphFastSort<Graph_t>::new_order: unknown ordering strategy");
		vint vempty;
		vempty.swap(new_order);
	}

	delete [] degs;
	delete [] pos_degs;
return new_order;
}


template<typename Graph_t>
vint GraphSort<Graph_t>::new_order_furini (gbbs::sort_t alg, gbbs::place_t place){
/////////////////////
// Furini´s implementation of MIN_WIDTH for comparision purposes
// 
// COMMENTS-currently only for sparse graphs
	
	const int NV=g.number_of_vertices();
	double *degree = new double[NV];
	int *order = new int[NV];
	int *position = new int[NV];
	vint new_order(NV);

	int pos=NV-1;
	if(place==gbbs::PLACE_FL) pos=0;

	for(int i=0;i<NV;i++){
		degree[i] =g.degree(i);
		order[i] = i;
	}
	
	for (int ii = 0; ii < NV; ii++){
		com::sort::SORT_NON_DECR(order,degree, NV);

		for (int iii = 0; iii < NV; iii++){
			position[order[iii]] = iii;
		}

		degree[0] = NV+1;
		new_order[order[0]]= ((place==gbbs::PLACE_FL)? pos++ : pos--);

		bitarray& bbnn = g.neighbors(order[0]);
		bbnn.init_scan(bbo::NON_DESTRUCTIVE);
		while (true){
			int v = bbnn.next_bit();
			if (v == EMPTY_ELEM) break;
			degree[ position[  v  ] ] --;
		}
	}

	delete[]degree;
	delete[]order;
	delete[]position;
	
return new_order;
}


template<typename Graph_t>
vint GraphSort<Graph_t>::new_order (gbbs::sort_t alg, gbbs::place_t place){
/////////////////////////////
// Sorts vertices by different strategies always picking them by non-decreasing index
// PARAMS: LF (last to first) TRUE places each vertex taken at the end of the new order; if  FALSE at the beginning
//         LF=TRUE is the degeneracy ordering (reverse min-width ordering)
//
// As usual new order[OLD_INDEX]=NEW_INDEX
// RETURNS: Empty vertex set if ERROR, else new ordering
//
// REMARKS
// 1.Had to make tie-breaks more efficient (28/8/14)
// 2.There was a lot to do! Basically degrees with respect to the vertex removed and support can be recomputed over the updated degrees.
	
	
	//KCORE ordering special case: does not requiere degree computation
	if(alg==gbbs::KCORE){
		return new_order_kcore(place);
	}

	if(alg==gbbs::KCORE_UB){
		return new_order_kcore_UB(gbbs::PLACE_FL);					//absolute ordering
	}

	if(alg==gbbs::MAX_WEIGHT){										//9/10/16
		return new_order_weighted(place, true);	
	}else if(alg==gbbs::MIN_WEIGHT){
		return new_order_weighted(place, false);	
	}else if(alg==gbbs::MAX_WEIGHT_DEG){
		return new_order_weighted_deg(place, true);	
	}else if(alg==gbbs::MIN_WEIGHT_DEG){
		return new_order_weighted_deg(place, false);
	}
			
	//remaining cases: all require explicit degree computation
	const int NV=g.number_of_vertices();
	vint new_order(NV);
	vdeg degs;	
	int k; 
	(place==gbbs::PLACE_LF)? k=g.number_of_vertices()-1 : k=0;
	
	//computes degree of vertices
	for(int i=0; i<NV; i++){
		deg_t vt;
		vt.index=i;
		vt.deg=g.degree(i);
		if(alg==gbbs::MIN_DEG_DEGEN_TIE_STATIC)
				vt.deg_of_n=sum_of_neighbor_deg(vt.index);
		degs.push_back(vt);		
	}

	//makes a copy in case it is needed inside the switch
	vdeg vd(degs);
		
	//computes order
	_bbt bbn(NV);
	bbn.set_bit(0,NV-1);
	switch(alg){
	case gbbs::MIN_DEG_DEGEN:
		while(!degs.empty()){
			vdeg_it it1=min_element(degs.begin(), degs.end(), degreeLess());
			new_order[it1->index]=k;
			(place==gbbs::PLACE_LF)? k-- : k++;
			bbn.erase_bit(it1->index);
			degs.erase(it1);

			//recompute degrees
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index, bbn);
			}
		}
		break;
	case gbbs::MAX_DEG_DEGEN:
		while(!degs.empty()){
			vdeg_it it1=max_element(degs.begin(), degs.end(), degreeLess());
			new_order[it1->index]=k;
			(place==gbbs::PLACE_LF)? k-- : k++;
			bbn.erase_bit(it1->index);
			degs.erase(it1);

			//recompute degrees
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index, bbn);
			}
		}
		break;
	case gbbs::MAX_DEG_ABS:
			sort(vd.begin(), vd.end(), degreeGreater());
			for(int i=0; i<vd.size(); i++){
				new_order[vd[i].index]=k;
				(place==gbbs::PLACE_LF)? k-- : k++;
			}
		break;
	case gbbs::MIN_DEG_ABS:
		sort(vd.begin(), vd.end(), degreeLess());
			for(int i=0; i<vd.size(); i++){
				new_order[vd[i].index]=k;
				(place==gbbs::PLACE_LF)? k-- : k++;
			}
		break;
	case gbbs::MIN_DEG_DEGEN_TIE_STATIC:
		while(!degs.empty()){
			vdeg_it it_sel=min_element(degs.begin(), degs.end(), degreeWithTieBreakLess());
			int v_sel=it_sel->index;
			new_order[v_sel]=k;
			(place==gbbs::PLACE_LF)? k-- : k++;
			bbn.erase_bit(v_sel);
			degs.erase(it_sel);
							
			//recompute degrees 
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index,bbn);
			}
		}
		break;
	case gbbs::MAX_DEG_DEGEN_TIE_STATIC:
		while(!degs.empty()){
			vdeg_it it_sel=max_element(degs.begin(), degs.end(), degreeWithTieBreakLess());				
			int v_sel=it_sel->index;
			new_order[v_sel]=k;
			(place==gbbs::PLACE_LF)? k-- : k++;
			bbn.erase_bit(v_sel);
			degs.erase(it_sel);
							
			//recompute degrees 
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index,bbn);
			}
		}
		break;
	case gbbs::MAX_DEG_DEGEN_TIE:		//computes support on the fly (28/11/16) ***CHECK
		while(!degs.empty()){
			vdeg_it it_sel=max_element(degs.begin(), degs.end(), degreeWithTieBreakLess());				
			int v_sel=it_sel->index;
			new_order[v_sel]=k;
			(place==gbbs::PLACE_LF)? k-- : k++;
			bbn.erase_bit(v_sel);
			degs.erase(it_sel);
							
			//recompute degrees 
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index,bbn);
			}

			//recompute support
			for(int i=0; i<degs.size(); i++){
				degs[i].deg_of_n=this->sum_of_neighbor_deg(degs[i].index,bbn);						
			}

		}
		break;
	case gbbs::NONE:							//to implement reverse ordering
		//warning for petition in which vertex order remains as is but is computed nevertheless
		if(place==gbbs::PLACE_FL){
			LOG_WARNING("GraphSort<Graph_t>::new_order: NONE + PLACE_FL->order unchanged but will be processed");
		}

		for(int i=0; i<new_order.size(); i++){
			new_order[i]=k;
			(place==gbbs::PLACE_LF)? k-- : k++;
		}
		break;
	default:
		LOG_ERROR("GraphSort<Graph_t>::new_order: unknown ordering strategy");
		vint vempty;
		vempty.swap(new_order);
	}
return new_order;
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order (gbbs::pick_t pick, gbbs::place_t place){
//////////////////////
// Determines new order in format [OLD_VERTEX]= NEW_VERTEX incrementally (vertex by vertex)
// using the primitives for vertex selection

	int v_sel=EMPTY_ELEM; int k=0;
	int NV=g.number_of_vertices();
	vint res(NV);
	_bbt sg(NV);
	sg.set_bit(0, NV-1);
	
	(place==gbbs::PLACE_LF)?  k=NV-1 : k=0;
	while(true){
		v_sel=get_v(sg, pick);
		if(v_sel==EMPTY_ELEM) break;
		res[v_sel]=k;
		(place==gbbs::PLACE_LF)?  k-- : k++;
	sg.erase_bit(v_sel);
	}
	
return res;
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order	(const _bbt& sg_in, gbbs::pick_t pick, gbbs::place_t place){
////////////////////////
// Reorders INDUCED SUBGRAPH by sg_in (returns a full ordering which may be used in a reorder operation)

	int v_sel=EMPTY_ELEM; int k=0;
	int NV=g.number_of_vertices();
	vint res;
	_bbt sg(sg_in);
	
	//inits vector with current order
	for(int i=0; i<NV; i++){
		res.push_back(i);
	}

	//get vertex list to reorder
	vint vsg;
	sg.to_vector(vsg);

	(place==gbbs::PLACE_LF)?  k=vsg.size()-1 : k=0;
	while(1){
		v_sel=get_v(sg, pick);
		if(v_sel==EMPTY_ELEM) break;
		(place==gbbs::PLACE_LF)?  res[v_sel]=vsg[k--] : res[v_sel]=vsg[k++];			//swaps related vertices
	sg.erase_bit(v_sel);
	}

return res;
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order (const _bbt& sgfrom_in, const _bbt& sgref_in, gbbs::pick_t pick, gbbs::place_t place, bool is_degen){
///////////////////////
// Reorders, by the usual criteria, vertices in sg in connection to subgraph INDUCED by sgref
// Typically sgref should include sg but this is not strictly neccessary
// 
// PARAMS
//  BOOL is_degen: TRUE removes vertex selected in sg from sgref after being picked 
//				    User case for non degenerate (is_degen=FALSE) is a max or min degree ordering which takes 
//					into account all vertices in each choice.
//
// User case: reorder a subset of vertices using global structure criteria

	int v_sel=EMPTY_ELEM; int k=0;
	int NV=g.number_of_vertices();
	vint res;
	_bbt sgfrom(sgfrom_in);
	_bbt sgref(sgref_in);
	
	//inits vector with current order
	for(int i=0; i<NV; i++){
		res.push_back(i);
	}

	//get vertex list to reorder
	vint vsg;
	sgfrom.to_vector(vsg);

	(place==gbbs::PLACE_LF)?  k=vsg.size()-1 : k=0;
	while(true){
		v_sel=get_v(sgfrom,sgref,pick);  
		if(v_sel==EMPTY_ELEM) break;
		(place==gbbs::PLACE_LF)?  res[v_sel]=vsg[k--] : res[v_sel]=vsg[k++];			//swaps related vertices
		sgfrom.erase_bit(v_sel);
		if(is_degen) sgref.erase_bit(v_sel);									//implements degeneracy by modifying the reference induced subgraph
	}

	return res;
}

template<typename Graph_t>
int GraphSort<Graph_t>::change_order (const _bbt& sgfrom_in, const _bbt& sgref_in, vint& ord, gbbs::pick_t pick, gbbs::place_t place, bool is_degen){
///////////////////////
// Modifies the valid ordering in PARAM ord, by the usual criteria, vertices in sg in connection to subgraph INDUCED by sgref
// Typically sgref should include sg but this is not strictly neccessary
//
// REMARKS: the resulting ord should also be a valid ordering (ord[OLD_VERTEX]=NEW_VERTEX)
//
// first update: 2/10/16
//
// RETURNS 0 is ok, -1 if ERROR
// 
// PARAMS
// BOOL is_degen: TRUE removes vertex selected in sg from sgref after being picked 
//				    User case for non degenerate (is_degen=FALSE) is a max or min degree ordering which takes 
//					into account all vertices in each choice.
//
// User case: reorder a subset of vertices using global structure criteria

	int v_sel=EMPTY_ELEM; int k=0;
	int NV=g.number_of_vertices();
	_bbt sgfrom(sgfrom_in);
	_bbt sgref(sgref_in);

	//check size of ordering
	if(ord.size()!=NV){
		LOG_ERROR("change order()::Bizarre input order");
		return -1;
	}

	//get vertex list to reorder
	vint vsg;
	sgfrom.to_vector(vsg);

	(place==gbbs::PLACE_LF)?  k=vsg.size()-1 : k=0;
	while(true){
		v_sel=get_v(sgfrom,sgref,pick);  
		if(v_sel==EMPTY_ELEM) break;
		(place==gbbs::PLACE_LF)?  ord[v_sel]=vsg[k--] : ord[v_sel]=vsg[k++];			//swaps related vertices
		sgfrom.erase_bit(v_sel);
		if(is_degen) sgref.erase_bit(v_sel);									//implements degeneracy by modifying the reference induced subgraph
	}

	return 0;	//ok
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order_kcore (gbbs::place_t place){
/////////////////////
// Ret
	int nV=g.number_of_vertices();
	vint new_order(nV);

	KCore<Graph_t> kc(g);
	kc.kcore();
	const vint& kco=kc.get_kcore_ordering();		//ordered by non decreasing
	
////////////////
//translates to [OLD_INDEX]=NEW_INDEX
	int l=0;
	if(place==gbbs::PLACE_LF){				//the standard use in clique
		for(vint::const_reverse_iterator it=kco.rbegin(); it!=kco.rend(); ++it){
			new_order[*it]=l++;
		}
	}else{
		//PLACE_FL
		LOG_WARNING("new_order_kcore: non typical ordering by increasing kcore");
		for(vint::const_iterator it=kco.begin(); it!=kco.end(); ++it){
			new_order[*it]=l++;
		}
	}

	return new_order;
}

template<typename Graph_t>
vint GraphSort<Graph_t>::new_order_kcore_UB ( gbbs::place_t place){
///////////////////////
// Creates an ordering using new kcore_UB:
// 1-First it computes kcore number of the graph (K(G))
// 2-Uses kcore number to place vertices with kcore number K(G) LAST in the
//   internal vertex ordering (m_ver)
//
// date of creation: 7/3/16
// last update: 7/3/16

	int nV=g.number_of_vertices();
	vint new_order(nV);

	//main computations
	KCore<Graph_t> kc(g);
	kc.kcore();
	int kcn=kc.get_kcore_number();
	kc.kcore_UB(kcn);
	const vint& kco=kc.get_kcore_ordering();		//degeneracy sorting is from last to first

////////////////
//translates to [OLD_INDEX]=NEW_INDEX
	int l=0;
	if(place==gbbs::PLACE_LF){					
		//degeneracy sorting from first to last
		LOG_INFO("GraphSort<Graph_t>::new_order_kcore_UB-using reverse ordering, please check!");
		for(vint::const_reverse_iterator it=kco.rbegin(); it!=kco.rend(); ++it){
			new_order[*it]=l++;
		}
	}else{ //PLACE_FL	
		//degeneracy sorting from last to first (the default application in our clique algorithms which take vertices in reverse order)
		for(vint::const_iterator it=kco.begin(); it!=kco.end(); ++it){
			new_order[*it]=l++;
		}
	}
	return new_order;
}




template<typename Graph_t>
vint GraphSort<Graph_t>::new_subg_order (gbbs::sort_t alg, _bbt& sg,  gbbs::place_t place){
/////////////////
// Returns a list of vertices in a subgraph (as a bitstring of vertices) ordered by non decreasing param-alg criteria
// CANNOT be used in REORDER functions
//
// date of creation: 9/3/15
// date of refactoring: 21/11/15
//
// On ERROR: The empty graph
//
// COMMENTS: **EXPERIMENTAL***
// 1-All sorting criteria are referred solely to the subgraph passed
// 2-NOT OPTIMIZED (sparse_graph specialization, erase elements in vectors, recompute degree at each iteration)
// 3-The returned list CANNOT be accessed by vertex index

// TODO: Other possible orderings (absolute and degenerate)

	deg_t vt;
	vdeg degs;
	
	//consistency check
	if(sg.is_empty()){
		LOG_WARNING("new_subg_order: empty subgraph");
		vint vempty;
		return vempty;
	}
	
	//computes initial relevant degree information of vertices
	sg.init_scan(bbo::NON_DESTRUCTIVE);
	while(true){
		vt.index=sg.next_bit();
		if(vt.index==EMPTY_ELEM) break;

		vt.deg=g.degree(vt.index, sg);
		if(alg==gbbs::MIN_DEG_DEGEN_TIE_STATIC){
			vt.deg_of_n=sum_of_neighbor_deg(vt.index, sg);
		}
		degs.push_back(vt);	
	}
	
	//computes direct order
	vint new_order;
	_bbt bbn(sg); 
	switch(alg){
	case gbbs::MIN_DEG_DEGEN:
		while(!degs.empty()){
			vdeg_it it1=min_element(degs.begin(), degs.end(), degreeLess());
			new_order.push_back(it1->index);
			bbn.erase_bit(it1->index);
			degs.erase(it1);

			//recompute degrees
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index, bbn);
			}
		}
		break;
	case gbbs::MAX_DEG_DEGEN:
		while(!degs.empty()){
			vdeg_it it1=max_element(degs.begin(), degs.end(), degreeLess());
			new_order.push_back(it1->index);
			bbn.erase_bit(it1->index);
			degs.erase(it1);

			//recompute degrees
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index, bbn);
			}
		}
		break;
	case gbbs::MIN_DEG_DEGEN_TIE_STATIC:
		//degrees of supporters fixed at the beginning of the search
		while(!degs.empty()){
			vdeg_it it1=min_element(degs.begin(), degs.end(), degreeWithTieBreakLess());
			new_order.push_back(it1->index);
			bbn.erase_bit(it1->index);
			degs.erase(it1);
			
			//recompute degrees
			for(int i=0; i<degs.size(); i++){
				degs[i].deg=g.degree(degs[i].index, bbn);
			}
		}
		break;
	case gbbs::NONE:
		//warning for petition in which vertex order of subgraph remains as is but is computed nevertheless
		if(place==gbbs::PLACE_FL){
			LOG_WARNING("GraphSort<Graph_t>::new_subg_order: NONE + PLACE_FL->order unchanged but will be processed");
		}
		
		sg.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			vt.index=sg.next_bit();
			if(vt.index==EMPTY_ELEM) break;
			new_order.push_back(vt.index);
		}
		break;
	default:
		LOG_ERROR("new_subg_order: unknown ordering strategy");
		vint vempty;
		return vempty;
	}

	//reverse for place parameter consistency
	if(place==gbbs::PLACE_LF)
		reverse(new_order.begin(), new_order.end());
		
return new_order;
}


///////////////
// 
// vertex selection primitives
//
////////////////
template<typename Graph_t>
int GraphSort<Graph_t>::get_v(gbbs::pick_t pick){
///////////////////
// Picks vertex from the graph according to pick strategy (ties lexicographical)
// Breaks ties first found

	int opt_val;
	int v_sel=EMPTY_ELEM; 
	int NV=g.number_of_vertices();
	
	switch(pick){
	case gbbs::PICK_MINFL:	
		opt_val=GRAPH_SORT_INFINITE;
		for(int v=0; v<NV; v++){
			int deg=g.degree(v);
			if(deg<opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MINLF:	
		opt_val=GRAPH_SORT_INFINITE;
		for(int v=NV-1; v>=0; v--){
			int deg=g.degree(v);
			if(deg<opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MAXFL:	
		opt_val=-1;
		for(int v=0; v<NV; v++){
			int deg=g.degree(v);
			if(deg>opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MAXLF:	
		opt_val=-1;
		for(int v=NV-1; v>=0; v--){
			int deg=g.degree(v);
			if(deg>opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	default:
		LOG_ERROR("GraphSort<Graph_t>::sel_v_by_deg: unknown vertex selection criteria");
	}
		
	return v_sel;	
}

template<typename Graph_t>
int GraphSort<Graph_t>::get_v(_bbt& sg, gbbs::pick_t pick){	
// ////////////////////
// Picks vertex from induced subgraph according to pick degree criteria (ties lexicographical)
//
// OBSERVATIONS: PROPERTIES ARE ALWAYS LOCAL TO THE INDUCED SUBGRAPH (sg)

	int opt_val;
	int v_sel=EMPTY_ELEM; 
	int NV=g.number_of_vertices();

	switch(pick){
	case gbbs::PICK_MINFL:	
		opt_val=GRAPH_SORT_INFINITE;
		sg.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int v=sg.next_bit();
			if(v==EMPTY_ELEM) break;
			int deg=g.degree(v, sg);
			if(deg<opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MINLF:	
		opt_val=GRAPH_SORT_INFINITE;
		sg.init_scan(bbo::NON_DESTRUCTIVE_REVERSE);
		while(true){
			int v=sg.previous_bit();
			if(v==EMPTY_ELEM) break;
			int deg=g.degree(v, sg);
			if(deg<opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MAXFL:	
		opt_val=-1;
		sg.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int v=sg.next_bit();
			if(v==EMPTY_ELEM) break;
			int deg=g.degree(v, sg);
			if(deg>opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MAXLF:	
		opt_val=-1;
		sg.init_scan(bbo::NON_DESTRUCTIVE_REVERSE);
		while(true){
			int v=sg.previous_bit();
			if(v==EMPTY_ELEM) break;
			int deg=g.degree(v, sg);
			if(deg>opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	default:
		LOG_ERROR("GraphSort<Graph_t>::sel_v_by_deg: unknown vertex selection criteria");
	}

	return v_sel;
}

template<typename Graph_t>
int GraphSort<Graph_t>::get_v (_bbt& sgfrom, const _bbt& sgref, gbbs::pick_t pick){
////////////////////////////
// Picks vertex from induced subgraph sgfrom according to pick degree criteria (ties lexicographical)
// related to induced graph sgref (sgfrom and sgref are not necessarily disjoint) 
	
	int opt_val;
	int v_sel=EMPTY_ELEM; 
	int NV=g.number_of_vertices();
	_bbt neigh(NV);


	switch(pick){
	case gbbs::PICK_MINFL:	
		opt_val=GRAPH_SORT_INFINITE;
		sgfrom.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int v=sgfrom.next_bit();
			if(v==EMPTY_ELEM) break;
			AND(g.neighbors(v), sgref, neigh);
			int deg=neigh.popcn64();
			if(deg<opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MINLF:	
		opt_val=GRAPH_SORT_INFINITE;
		sgfrom.init_scan(bbo::NON_DESTRUCTIVE_REVERSE);
		while(true){
			int v=sgfrom.previous_bit();
			if(v==EMPTY_ELEM) break;
			AND(g.neighbors(v), sgref, neigh);
			int deg=neigh.popcn64();
			if(deg<opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MAXFL:	
		opt_val=-1;
		sgfrom.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int v=sgfrom.next_bit();
			if(v==EMPTY_ELEM) break;
			AND(g.neighbors(v),sgref, neigh);
			int deg=neigh.popcn64();
			if(deg>opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	case gbbs::PICK_MAXLF:	
		opt_val=-1;
		sgfrom.init_scan(bbo::NON_DESTRUCTIVE_REVERSE);
		while(true){
			int v=sgfrom.previous_bit();
			if(v==EMPTY_ELEM) break;
			AND(g.neighbors(v),sgref, neigh);
			int deg=neigh.popcn64();
			if(deg>opt_val){
				opt_val=deg;
				v_sel=v;
			}
		}
		break;
	default:
		LOG_ERROR("GraphSort<Graph_t>::sel_v_by_deg: unknown vertex selection criteria");
	}
	

	return v_sel;
}
	


#endif