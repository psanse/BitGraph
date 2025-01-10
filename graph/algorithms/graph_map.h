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

///////////////////////
// 
// GraphMap for managing pairs of vertex orderings	
//
///////////////////////

class GraphMap{

public:
	enum print_t { L2R = 0, R2L, BOTH };			//streaming configuration

///////////////////////
//setters and getters

	int size()									{return l2r_.size();}
	vint& get_l2r() 							{return l2r_;}	
	vint& get_r2l()								{return r2l_;}
	const vint& get_l2r () const				{return l2r_;}	
	const vint& get_r2l () const				{return r2l_;}	

	//sets mapping (no need to build it)
	void set_l2r (vint& l, std::string name)	{l2r_=l; nameL_=name;}	
	void set_r2l (vint& r, std::string name)	{r2l_=r; nameR_=name;}

////////////////
// mapping getters
	
	int map_l2r (int v) const					{return l2r_[v]; } 
	int map_r2l (int v) const					{return r2l_[v]; } 

	/*
	* @brief maps a (bit) set of vertices (bbl) to a (bit) set of vertices (bbr)
	* @param bbl: input bitset of vertices in the space of the left ordering
	* @param bbr: output bitset of vertices in the space of the right ordering
	* @param overwrite: if TRUE, bbr is erased before mapping
	*/
	template<class bitset_t>
	bitset_t& map_l2r				(bitset_t& bbl, bitset_t& bbr, bool overwrite = true)			const;

	/*
	* @brief maps a (bit) set of vertices (bbr) to a (bit) set of vertices (bbl)
	* @param bbl: output bitset of vertices in the space of the left ordering
	* @param bbr: input bitset of vertices in the space of the right ordering
	* @param overwrite: if TRUE, bbr is erased before mapping
	*/
	template<class bitset_t>
	bitset_t& map_r2l				(bitset_t& bbl, bitset_t& bbr, bool overwrite = true)			const;

	//TODO - extend to other set representations

////////////////////
// build mapping operations

	/*
	* @brief Computes mapping between the two different sortings of a graph
	* 
	*		 I. The mappings are available by the getters get_l2r(), get_r2l()
	* 
	*		 II. Type Alg_t is a sorting algorithm (e.g. GraphFastRootSort<Graph_t>)
	* 
	* @param lhs_s, rhs_s: input sorting strategies of the left and right orderings
	* @param lhs_p, rhs_p: input placement strategies of the left and right orderings (first-to-last, last-to-first)
	* @param lhs_name, rhs_name: fancy names for the orderings
	* @returns -1 if error, 0 otherwise
	*/
	template< typename Alg_t , typename G_t = Alg_t::_gt >
	int build_mapping		(G_t&, int lhs_s, int lhs_p, int rhs_s, int rhs_p,
								string lhs_name = "", string rhs_name = ""				);

	int build_mapping		(vint& lhs_o2n, vint& rhs_o2n, string lhs_name="", string rhs_name="");
	
	//I/O
	ostream& print_mappings	(print_t type = BOTH, ostream& o=cout);
	ostream& print_names	(print_t type = BOTH, ostream& o=cout);

///////////////
//Boolean operations

	/*
	* @brief checks if the internal mapping state l2r_, r2l is consistent
	*/
	bool is_consistent();


///////////////////////
//private interface

protected:	
	void clear	()							{ l2r_.clear(); r2l_.clear(); nameL_.clear(); nameR_.clear(); }
	void reset	(std::size_t NV)			{ clear(); 	l2r_.resize(NV); r2l_.resize(NV);	}
	
	
////////////////
// data members

	vint l2r_, r2l_;					//mapping between left and right ordering
	string nameL_;					//fancy name of left ordering
	string nameR_;					//fancy name of right ordering	
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
	l2r_=old_to_new;
	r2l_=Decode::reverse(l2r_);
	nameL_=lhs_name;
	nameR_="NOT USED - SINGLE MAPPING";
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
	
	auto NV = g.number_of_vertices();
	//vint o2n_lhs, n2o_lhs, o2n_rhs, n2o_rhs;	
	//init(NV,EMPTY_ELEM /* -1 */);					/* no need to initialize sets manually*/
		
	//determine sorting lhs
	Alg_t gol(g);
	l2r_=gol.new_order(slhs, plhs);
	r2l_=Decode::reverse(l2r_);	

	nameL_=lhs_name;
	nameR_="SINGLE GRAPH MAPPING";
		
	if(!is_consistent()){
		LOG_ERROR("bizarre ordering");
		return -1;
	}
	return 0;
}

template<class bitset_t>
inline
bitset_t& GraphMap::map_l2r (bitset_t& bbl, bitset_t& bbr, bool overwrite) const{

	//cleans bbr if requested
	if(overwrite) { bbr.erase_bit();}

	//sets bitscanning configuration
	bbl.init_scan(bbo::NON_DESTRUCTIVE);

	//bitscans bbl and sets bits in bbr
	int v;
	while(v = bbl.next_bit() != EMPTY_ELEM){
		bbr.set_bit(l2r_[v]);
	}

	return bbr;
}

template<class bitset_t>
inline
bitset_t& GraphMap::map_r2l (bitset_t& bbl, bitset_t& bbr, bool overwrite) const{

	//cleans bbr if requested
	if (overwrite) { bbl.erase_bit(); }

	//sets bitscanning configuration
	bbr.init_scan(bbo::NON_DESTRUCTIVE);

	//bitscans bbl and sets bits in bbr
	int v;
	while (v = bbr.next_bit() != EMPTY_ELEM) {
		bbl.set_bit(r2l_[v]);
	}
		
	return bbl;
}


inline
bool GraphMap::is_consistent(){

	for(auto v = 0;  v < l2r_.size(); ++v){
		if (v != r2l_[l2r_[v]]) {
			return false;
		}
	}

	return true;
}

template<class Alg_t, class G_t>
inline
int GraphMap::build_mapping(G_t& g, int slhs, int plhs, int srhs, int prhs,
								string lhs_name, string rhs_name				)
{
	vint o2n_lhs, n2o_lhs, o2n_rhs, n2o_rhs;

	auto NV = g.number_of_vertices();
	
	reset(NV);
		
	//determine sorting lhs
	Alg_t gol (g);
	o2n_lhs = gol.new_order (slhs, plhs);
	n2o_lhs = Decode::reverse(o2n_lhs);	

	//determine sorting rhs
	Alg_t gor(g);
	o2n_rhs = gor.new_order (srhs, prhs );
	n2o_rhs = Decode::reverse(o2n_rhs);	

	//determines direct and reverse mappings independently
	for(auto v = 0; v < NV; v++){
		l2r_[v]= o2n_rhs[ n2o_lhs[v] ];				// l->r
	}
	for(auto v = 0; v < NV; v++){
		r2l_[v]= o2n_lhs[ n2o_rhs[v] ] ;			// r->l 
	}

	nameL_=lhs_name;
	nameR_=rhs_name;
	
	if(!is_consistent()){
		LOG_ERROR("bad ordering - GraphMap::build_mapping");
		return -1;
	}

	//I/O
	//cout<<"N2O_D"; com::stl::print_collection(n2o_d); cout<<endl;
	//cout<<"O2N_D";com::stl::print_collection(o2n_d); cout<<endl;
	//cout<<"O2N_W";com::stl::print_collection(o2n_w); cout<<endl;
	//cout<<"N2O_W";com::stl::print_collection(n2o_w); cout<<endl;

	return 0;
}

inline
int GraphMap::build_mapping(vint& o2n_lhs, vint& o2n_rhs, string lhs_name, string rhs_name){
/////////////////////////
// builds mapping between the two different sortings
//
// RETURNS 0 ok, -1 Error
		
	vint n2o_lhs, n2o_rhs;

	auto NV = o2n_lhs.size();	

	reset(NV);
		
	//determine sorting lhs
	n2o_lhs=Decode::reverse(o2n_lhs);	

	//determine sorting rhs
	n2o_rhs=Decode::reverse(o2n_rhs);	

	//determines direct and reverse mappings independently
	for(int v=0; v<NV; v++){
		l2r_[v]= o2n_rhs[ n2o_lhs[v] ];				// l->r 
	}
	for(int v=0; v<NV; v++){
		r2l_[v]= o2n_lhs[ n2o_rhs[v] ] ;				// r->l 
	}

	nameL_=lhs_name;
	nameR_=rhs_name;
		

	//assert
	if (!is_consistent()) {
		LOG_ERROR("bad ordering - GraphMap::build_mapping");
		return -1;
	}

	//I/O
	//cout<<"N2O_D"; com::stl::print_collection(n2o_d); cout<<endl;
	//cout<<"O2N_D";com::stl::print_collection(o2n_d); cout<<endl;
	//cout<<"O2N_W";com::stl::print_collection(o2n_w); cout<<endl;
	//cout<<"N2O_W";com::stl::print_collection(n2o_w); cout<<endl;

	return 0;
}

inline
ostream& GraphMap::print_mappings(print_t type, ostream& o){
	switch(type){
	case L2R:
		o << "\n*****************" << endl;
		o << "L->R" << endl;
		com::stl::print_collection(l2r_,o, true);
		o << "\n*****************" << endl;
		break;
	case R2L:
		o << "\n*****************" << endl;
		o << "R->L" << endl;
		com::stl::print_collection(r2l_,o, true);
		o << "******************" << endl;
		break;
	case BOTH:
		o << "\n*****************" << endl;
		o << "L->R and R->L" << endl;
		com::stl::print_collection(l2r_,o, true);
		com::stl::print_collection(r2l_,o, true); 
		o << "*****************" << endl;
		break;
	default:
		LOG_WARNING("bad printing type - GraphMap::print_mappings");
	}

	return o;
}

inline
ostream& GraphMap::print_names(print_t type, ostream& o){
	switch(type){
	case L2R:
		o<<"L:"<<nameL_;
		break;
	case R2L:
		o<<"R:"<<nameR_;
		break;
	case BOTH:
		o<<"L:"<<nameL_; o<<endl;
		o<<"R:"<<nameR_;
		break;
	default:
		LOG_WARNING("bad printing type - GraphMap::print_names");
	}
		
	return o;
}

#endif