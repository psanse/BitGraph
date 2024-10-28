//graph_map.h: conversions between two isomporphic graphs encoded by GRAPH (14/8/17)
//			   extended to inlcude mapping to a single sorting (1/10/17)

#ifndef __GRAPH_ISOMORPHISM_MAPPINGS_H__
#define	__GRAPH_ISOMORPHISM_MAPPINGS_H__

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "../graph.h"
#include "../../utils/logger.h"
#include "../../utils/common.h"
#include "../algorithms/decode.h"
#include "../algorithms/graph_map.h"
#include "../algorithms/graph_fast_sort.h"
#include "../algorithms/graph_fast_sort_weighted.h"


using namespace std;


class GraphMap{
///////////////
//interface	
public:
	enum print_t {L2R=0, R2L, ALL};
	int size()									{return l2r.size();}
	vint get_l2r() 								{return l2r;}	
	vint get_r2l()								{return r2l;}
	const vint& get_l2r () const				{return l2r;}	
	const vint& get_r2l () const				{return r2l;}	
	void set_l2r(vint& l, std::string name)		{l2r=l; name_l=name;}	
	void set_r2l(vint& r, std::string name)		{r2l=r; name_r=name;}

	vint& get_o2n_lhs()							{ return o2nl; }			//[OLD]->[NEW] mappings for lhs graph
	vint& get_o2n_rhs()							{ return o2nr; }			//[OLD]->[NEW] mappings for rhs graph

	void clear() { l2r.clear(); r2l.clear(); name_l.clear(); name_r.clear(); }
////////////////
//mapping 
	int map_l2r(int v)const				{return l2r[v]; } 
	int map_r2l(int v)const				{return r2l[v]; } 
	template<class bitstring_t>
	bitstring_t& map_l2r(bitstring_t& bbl, bitstring_t& bbr, bool overwrite=true) const;
	template<class bitstring_t>
	bitstring_t& map_r2l(bitstring_t& bbl, bitstring_t& bbr, bool overwrite=true) const;
	template<class bitstring_array_t>
	bitstring_array_t& map_l2r(bitstring_array_t& bbl, bitstring_array_t& bbr, int from, int to) const;		/* range includes both ends */
	template<class bitstring_array_t>
	bitstring_array_t& map_r2l(bitstring_array_t& bbl, bitstring_array_t& bbr, int from, int to) const;		/* range includes both ends */
		
////////////////////

	//build driver
	template<class Graph_t, class Alg_t= GraphFastRootSort<Graph_t>>
	int build_mapping	(Graph_t&, int lhs_s, int lhs_p, int rhs_s, int rhs_p, 
										string lhs_name="", string rhs_name=""	);	

	int build_mapping	(vint& lhs_o2n, vint& rhs_o2n, string lhs_name="", string rhs_name="");
	
	//I/O
	ostream& print_mappings	(print_t type=ALL, ostream& o=cout);
	ostream& print_names	(print_t type=ALL, ostream& o=cout);

	
protected:
	void init(int NV, int val)		{l2r.assign(NV, val); r2l.assign(NV, val); name_l.clear(); name_r.clear();}
	
	bool is_consistent();

////////////////
// state
	vint l2r, r2l;					//mapping
	string name_l;
	string name_r;
	
	vint o2nl, o2nr;				//o2n transformations from the reference graph to l and r graphs
};

//////////////
//
// GraphMapSingle (mapping for single ordering)
// Original graph on the left
// Sorted graph on the right
//
//////////////
class GraphMapSingle: public GraphMap{
public:
	//overrides 
	template<class Graph_t, class Alg_t /* = GraphFastRootSort_W<Graph_t>*/ >
	int build_mapping(Graph_t&, int lhs_s, int lhs_p, int rhs_s, int rhs_p, 
						string lhs_name="", string rhs_name="");

	int build_mapping(vint& old_to_new, string lhs_name="");
};

inline
int GraphMapSingle::build_mapping(vint& old_to_new, string lhs_name){
	l2r=old_to_new;
	r2l=Decode::reverse(l2r);
	name_l=lhs_name;
	name_r="SINGLE GRAPH MAPPING";
	return 0;
}

template<class Graph_t, class Alg_t>
inline
int GraphMapSingle::build_mapping(Graph_t& g, int slhs, int plhs,int srhs, int prhs,
										string lhs_name, string rhs_name){
////////////////////////
// builds mapping for a graph g and a sorting provided by slhs and plhs 
// (the right parameters are not used, rhs_name is assigned "SINGLE_GRAPH_MAPPING")
//
//
// RETURNS 0 ok, -1 Error
//
// COMMENTS: g may be weighted (weights are also ordered)
	
	int NV=g.number_of_vertices();
	//vint o2n_lhs, n2o_lhs, o2n_rhs, n2o_rhs;	
	//init(NV,EMPTY_ELEM /* -1 */);					/* no need to initialize sets manually*/
		
	//determine sorting lhs
	Alg_t gol(g);
	l2r=gol.new_order(slhs, plhs);
	r2l=Decode::reverse(l2r);	

	name_l=lhs_name;
	name_r="SINGLE GRAPH MAPPING";
		
	if(!is_consistent()){
		LOG_ERROR("bizarre ordering");
		return -1;
	}
	return 0;
}

template<class bitstring_t>
inline
bitstring_t& GraphMap::map_l2r(bitstring_t& bbl, bitstring_t& bbr, bool overwrite) const{
	if(overwrite) {bbr.erase_bit();}
	bbl.init_scan(bbo::NON_DESTRUCTIVE);
	while(true){
		int v=bbl.next_bit();
		if(v==EMPTY_ELEM) break;
		bbr.set_bit(l2r[v]);
	}
	return bbr;
}

template<class bitstring_t>
inline
bitstring_t& GraphMap::map_r2l(bitstring_t& bbl, bitstring_t& bbr, bool overwrite) const{
	if(overwrite) {bbl.erase_bit();}
	bbr.init_scan(bbo::NON_DESTRUCTIVE);
	while(true){
		int v=bbr.next_bit();
		if(v==EMPTY_ELEM) break;
		bbl.set_bit(r2l[v]);
	}
	return bbl;
}

template<class bitstring_array_t>
inline
bitstring_array_t& GraphMap::map_l2r(bitstring_array_t& bbal, bitstring_array_t& bbar, int from, int to) const{
//////////////////
// assumes bbal and bbar are correctly allocated
	for(int c=from; c<=to; c++){
		map_l2r(bbal.pbb[c],bbar.pbb[c]);
	}
	return bbar;
}

template<class bitstring_array_t>
inline
bitstring_array_t& GraphMap::map_r2l(bitstring_array_t& bbal, bitstring_array_t& bbar, int from, int to)const {
//////////////////
// maps a set of bitarrays within range (both ends included)
//
// COMMENTS: no assertion for capacity

	for(int c=from; c<=to; c++){
		map_r2l(bbal.pbb[c],bbar.pbb[c]);
	}
	return bbal;
}

inline
bool GraphMap::is_consistent(){
/////////////////////
// checks all reverse mappings for consistency
	for(int v=0; v<l2r.size(); v++){
		if(v!=r2l[l2r[v]]) return false;
	}
	return true;
}

template<class Graph_t, class Alg_t>
inline
int GraphMap::build_mapping(Graph_t& g, int slhs, int plhs, int srhs, int prhs,
							string lhs_name, string rhs_name){
/////////////////////////
// builds mapping between the two different sortings of g
//
// RETURNS 0 ok, -1 Error
//
// COMMENTS: g may be weighted (weights are also ordered)
	
	int NV=g.number_of_vertices();
	vint o2n_lhs, n2o_lhs, o2n_rhs, n2o_rhs;	
	init(NV,EMPTY_ELEM /* -1 */);
		
	//determine sorting lhs
	Alg_t gol(g);
	o2n_lhs=gol.new_order(slhs, plhs);
	n2o_lhs=Decode::reverse(o2n_lhs);	

	//determine sorting rhs
	Alg_t gor(g);
	o2n_rhs=gor.new_order(srhs, prhs);
	n2o_rhs=Decode::reverse(o2n_rhs);	

	//determines direct and reverse mappings independently
	for(int v=0; v<NV; v++){
		l2r[v]= o2n_rhs[ n2o_lhs[v] ];				/* l->r */
	}
	for(int v=0; v<NV; v++){
		r2l[v]= o2n_lhs[ n2o_rhs[v] ] ;				/* r->l */
	}

	name_l=lhs_name;
	name_r=rhs_name;
		
	//I/O
	//cout<<"N2O_D"; com::stl::print_collection(n2o_d); cout<<endl;
	//cout<<"O2N_D";com::stl::print_collection(o2n_d); cout<<endl;
	//cout<<"O2N_W";com::stl::print_collection(o2n_w); cout<<endl;
	//cout<<"N2O_W";com::stl::print_collection(n2o_w); cout<<endl;

	if(!is_consistent()){
		LOG_ERROR("bizarre ordering");
		return -1;
	}

	//stores input o2n transformations (could be useful in the future)
	this->o2nl = o2n_lhs;
	this->o2nr = o2n_rhs;

	return 0;
}

inline
int GraphMap::build_mapping(vint& o2n_lhs, vint& o2n_rhs, string lhs_name, string rhs_name){
/////////////////////////
// builds mapping between the two different sortings
//
// RETURNS 0 ok, -1 Error
		
	
	int NV=o2n_lhs.size();
	vint n2o_lhs, n2o_rhs;	
	init(NV,EMPTY_ELEM /* -1 */);
		
	//determine sorting lhs
	n2o_lhs=Decode::reverse(o2n_lhs);	

	//determine sorting rhs
	n2o_rhs=Decode::reverse(o2n_rhs);	

	//determines direct and reverse mappings independently
	for(int v=0; v<NV; v++){
		l2r[v]= o2n_rhs[ n2o_lhs[v] ];				/* l->r */
	}
	for(int v=0; v<NV; v++){
		r2l[v]= o2n_lhs[ n2o_rhs[v] ] ;				/* r->l */
	}

	name_l=lhs_name;
	name_r=rhs_name;
		
	//I/O
	//cout<<"N2O_D"; com::stl::print_collection(n2o_d); cout<<endl;
	//cout<<"O2N_D";com::stl::print_collection(o2n_d); cout<<endl;
	//cout<<"O2N_W";com::stl::print_collection(o2n_w); cout<<endl;
	//cout<<"N2O_W";com::stl::print_collection(n2o_w); cout<<endl;

	if(!is_consistent()){
		LOG_ERROR("bizarre ordering");
		return -1;
	}

	//stores input o2n transformations  (could be useful in the future)
	o2nl = o2n_lhs;
	o2nr = o2n_rhs;

	return 0;
}

inline
ostream& GraphMap::print_mappings(print_t type, ostream& o){
	switch(type){
	case L2R:
		com::stl::print_collection(l2r,o);
		break;
	case R2L:
		com::stl::print_collection(r2l,o);
		break;
	case ALL:
		com::stl::print_collection(l2r,o); o<<endl;
		com::stl::print_collection(r2l,o);
		break;
	default:
		LOG_ERROR("GraphMap::print_mappings()-bizarre print type");
	}
	return o;
}

inline
ostream& GraphMap::print_names(print_t type, ostream& o){
	switch(type){
	case L2R:
		o<<"L:"<<name_l;
		break;
	case R2L:
		o<<"R:"<<name_r;
		break;
	case ALL:
		o<<"L:"<<name_l; o<<endl;
		o<<"R:"<<name_r;
		break;
	default:
		LOG_ERROR("GraphMap::print_names()-bizarre print type");
	}
	return o;
}

#endif