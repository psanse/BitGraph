//graph_func.h: header for general purpose algorithms related to graphs
//date of creation: 07/3/17
//last update: 12/3/17
//authors: pss

#ifndef __GRAPH_FUNC_H__
#define __GRAPH_FUNC_H__

#include <iostream>
#include <algorithm>
#include <numeric>							/* accumulate */

#include "../graph.h"
#include "../kcore.h"
#include "filter_graph_sort_type.h"			//Template Graph_t reduced to undirected types
#include "utils/logger.h"
#include "utils/common.h"

using namespace std;


namespace gfunc{
////////////////////////
// stateless functions for GRAPH
	
	
	template<class Graph_t>
	int sort_deg(const Graph_t& g, vint& lv, const vint& ref, bool min_sort=true){
	/////////////////
	// sorts in place nodes in lv wrt to connections with ref
	//
	// COMMENTS-ref and lv have no restrictions	
		int deg=0;
		vint ldeg(g.number_of_vertices(),0);
		for(int i=0; i<lv.size(); i++){
			//compute deg in ref
			deg=0;
			for(int j=0; j<ref.size(); j++){
				if (g.is_edge(lv[i],ref[j])){
					deg++;
				}
			}
			ldeg[lv[i]]=deg;
		}

		//com::stl::print_collection(ldeg); 
		
		//sorting according to ldeg
		if(min_sort){
			std::sort(lv.begin(), lv.end(), com::has_smaller_val<int, int>(ldeg));			
		}else{
			std::sort(lv.begin(), lv.end(), com::has_greater_val<int, int>(ldeg));	
		}
		
		return 0;
	}

	template<class Graph_t>
	int sort_deg(const Graph_t& g, vint& lv, typename  Graph_t::_bbt& bbref, bool min_sort=true){
	/////////////////
	// sorts in place nodes in lv wrt to connections with bbref
	//
	// COMMENTS-bbref and lv have no restrictions	
		int deg=0;
		vint ldeg(g.number_of_vertices(),0);
		for(int i=0; i<lv.size(); i++){
			ldeg[lv[i]]=g.degree(lv[i],bbref);			
		}

		//com::stl::print_collection(ldeg); 
		
		//sorting according to ldeg
		if(min_sort){
			std::sort(lv.begin(), lv.end(), com::has_smaller_val<int, int>(ldeg));			
		}else{
			std::sort(lv.begin(), lv.end(), com::has_greater_val<int, int>(ldeg));	
		}
		
		return 0;
	}

	template<class Graph_t>
	int neighbors(const Graph_t& g, int v,  typename  Graph_t::_bbt& bbref, vector<int>& lv_n){
	/////////////////
	// Computes list of neighbors of v in bitstet bbref and stores them in lvn
	// (only for undirected graphs)
	//
	// RETURNS: number of neighbors
	//
	// date@03/09/18
	
		lv_n.clear();
		const int MAXNBB=g.number_of_blocks();

		for(int nBB=0; nBB<MAXNBB;nBB++){
			BITBOARD bb=g.get_neighbors(v).get_bitboard(nBB) & bbref.get_bitboard(nBB);
			int vBB=WMUL(nBB);
			//decode information
			while(true){
				int v64=BitBoard::lsb64_intrinsic(bb);
				if (v64==EMPTY_ELEM) break;
				
				lv_n.push_back(vBB+v64);
				bb^=Tables::mask[v64];
			}
		}		
		
		return lv_n.size();
	}

	template<class Graph_t>
	int neighbors_post(const Graph_t& g, int v,  typename  Graph_t::_bbt& bbref, vector<int>& lv_n){
	/////////////////
	// Computes list of neighbors of v of bbref and that COME AFTER v, and stores them in lvn 
	// (only for undirected graphs)
	//
	// RETURNS: number of neighbors
	//
	// date@03/09/18
	
		lv_n.clear();
		const int MAXNBB=g.number_of_blocks();

		//first nBB-trimming with unrolling
		int vBBo=WDIV(v);
		BITBOARD bb=g.get_neighbors(v).get_bitboard(vBBo) & bbref.get_bitboard(vBBo);
		bb &= Tables::mask_left[WMOD(v)];															/* trims preceding vertifces*/
	
		while(true){
			int v64=BitBoard::lsb64_intrinsic(bb);
			if (v64==EMPTY_ELEM) break;
			lv_n.push_back(vBBo + v64);
			bb^=Tables::mask[v64];
		}

		//rest of blocks
		for(int nBB=vBBo+1; nBB<MAXNBB;nBB++){
			BITBOARD bb=g.get_neighbors(v).get_bitboard(nBB) & bbref.get_bitboard(nBB);
			int vBB=WMUL(nBB);			
			while(true){
				int v64=BitBoard::lsb64_intrinsic(bb);
				if (v64==EMPTY_ELEM) break;
				
				lv_n.push_back(vBB+v64);
				bb^=Tables::mask[v64];
			}
		}			
		return lv_n.size();
	}
		
	template<class Graph_t>
	int create_complete(Graph_t& g, int N) {
		if (g.init(N) == -1) return -1;
		for (int i = 0; i < N; i++) {
			for (int j = 0; i < N; j++) {
				g.add_edge(i, j);
			}
		}
		return 0;
	}

	inline
	int create_complete(ugraph& g, int N) {
		if (g.init(N) == -1) return -1;
		for (int i = 0; i < N-1; i++) {
			for (int j = i+1; j < N; j++) {
				g.add_edge(i, j);
			}
		}
		return 0;
	}

	template<class Graph_t>
	bool is_edge_free(Graph_t& g, typename Graph_t::_bbt& bbsg, vint& edge) {
		/////////////////////////
		//RETURNS TRUE if the induced subgraph @bbsg has no edges

		vint lv;
		edge.clear();
		bbsg.to_vector(lv);
		size_t S = lv.size();
		
		if (S <= 1) return true;

		for (int i = 0; i < S -1; i++) {
			for (int j = i + 1; j < S; j++) {
				if (g.is_edge(lv[i], lv[j])) {
					edge.push_back(lv[i]);
					edge.push_back(lv[j]);
					return false;
				}
			}
		}
		return true;
	}

	template<class Graph_t>
	bool is_triangle_free(Graph_t& g, typename Graph_t::_bbt& bbsg, vint& triangle) {
		/////////////////////////
		//RETURNS TRUE if the induced subgraph @bbsg has no triangles

		vint lv;
		triangle.clear();
		bbsg.to_vector(lv);
		size_t S = lv.size();

		if (S <= 2) return true;

		for (int i = 0; i < S - 2; i++) {
			for (int j = i + 1; j < S  - 1; j++) {
				if (g.is_edge(lv[i], lv[j])) {
					for (int k = j + 1; k < S ; k++) {
						if (g.is_edge(lv[j], lv[k]) && g.is_edge(lv[i], lv[k])) {
							triangle.push_back(lv[i]);
							triangle.push_back(lv[j]);
							triangle.push_back(lv[k]);
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	
			
//////////////
//weighted graphs 
// first commit: 9/1/17

	template<typename Graph_t>
	typename Graph_t::_wt wsum(const Graph_t& g, typename Graph_t::_bbt& bbref){
		typename Graph_t::_wt wv=0;
		bbref.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int v=bbref.next_bit();
			if(v==EMPTY_ELEM) return wv;
			wv+=g.get_w(v);
		}
		return wv;
	}

	template<typename Graph_t>
	struct accum_w{
		typename Graph_t::_wt operator ()( typename Graph_t::_wt accum, int v) const{return (accum + g.get_w(v));}
		accum_w(const Graph_t& gout):g(gout){}
		const Graph_t& g;
	};

	template<typename Graph_t>
	typename Graph_t::_wt wsum(const Graph_t& g, vint& lv){
		typename Graph_t::_wt wv=std::accumulate(lv.begin(), lv.end(), 0.0, accum_w<Graph_t>(g));
		return wv;
	}

	template<typename Graph_t>
	typename Graph_t::_wt wsum(const Graph_t& g){
		typename Graph_t::_wt wv=std::accumulate(g.get_weights().begin(), 
															g.get_weights().end(), 0.0, plus<typename Graph_t::_wt>());
		return wv;
	}	

	template<typename Graph_t>
	typename Graph_t::_wt wsum(const Graph_t& g, int v, typename Graph_t::_bbt& bbref){
	/////////////////////
	//RETURNS wsum of neighborhood of v in bref + the weight of v
	//TODO@ test (23/3/18)
		typename Graph_t::_wt wv=g.get_w(v);
		bbref.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int u=bbref.next_bit();
			if(u==EMPTY_ELEM) return wv;
			if(g.is_edge(u, v))
				wv+=g.get_w(u);
		}
		return wv;
	}

	template<typename Graph_t>
	typename Graph_t::_wt wsum(Graph_t& g, int v){
	/////////////////////
	//RETURNS wsum of neighborhood of v in the graph
	//TODO@ test (5/4/18)
		typename Graph_t::_wt wv=g.get_w(v);
		typename Graph_t::_bbt& bbn=g.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int u=bbn.next_bit();
			if(u==EMPTY_ELEM) return wv;
			wv+=g.get_w(u);
		}
		return wv;
	}
	
	template<typename Graph_t>
	int sort_w(const Graph_t& g, vint& lv, bool min_sort=true){
		com::has_smaller_val<int, typename Graph_t::_wt> my_struct_smaller (g.get_weights()) ;
		com::has_greater_val<int, typename Graph_t::_wt> my_struct_greater (g.get_weights()) ;

		//sorting according to ldeg
		if(min_sort){
			std::sort(lv.begin(), lv.end(), my_struct_smaller );			
		}else{
			std::sort(lv.begin(), lv.end(), my_struct_greater );	
		}
		return 0;
	}

	template<typename Graph_t>
	int sort_w(const Graph_t& g, int* lv, int size, bool min_sort=true){
		com::has_smaller_val<int, typename Graph_t::_wt> my_struct_smaller (g.get_weights()) ;
		com::has_greater_val<int, typename Graph_t::_wt> my_struct_greater (g.get_weights()) ;

		//sorting according to ldeg
		if(min_sort){
			std::sort(lv, lv+size, my_struct_smaller );			
		}else{
			std::sort(lv, lv+size, my_struct_greater );	
		}
		return 0;
	}

	template<typename Graph_t>
	int sort_wd(const Graph_t& g, vint& lv, bool min_sort=true){
		com::has_smaller_val_prod<int, typename Graph_t::_wt> my_struct_smaller_comb (g.get_weights()) ;
		com::has_greater_val_prod<int, typename Graph_t::_wt> my_struct_greater_comb (g.get_weights()) ;

		//sorting according to ldeg
		if(min_sort){
			std::sort(lv.begin(), lv.end(), my_struct_smaller_comb );			
		}else{
			std::sort(lv.begin(), lv.end(), my_struct_greater_comb );	
		}
		return 0;
	}

	
	template<typename Graph_t>
	int sort_wd(const Graph_t& g, int* lv, int size, bool min_sort=true){
		com::has_smaller_val_prod<int, typename Graph_t::_wt> my_struct_smaller_comb (g.get_weights()) ;
		com::has_greater_val_prod<int, typename Graph_t::_wt> my_struct_greater_comb (g.get_weights()) ;

		//sorting according to ldeg
		if(min_sort){
			std::sort(lv, lv+size, my_struct_smaller_comb );			
		}else{
			std::sort(lv, lv+size, my_struct_greater_comb );	
		}
		return 0;
	}

		template<typename Graph_t>
	int sort_wddiff(const Graph_t& g, vint& lv, bool min_sort=true){
		com::has_smaller_val_diff<int, typename Graph_t::_wt> my_struct_smaller_diff (g.get_weights()) ;
		com::has_greater_val_diff<int, typename Graph_t::_wt> my_struct_greater_diff (g.get_weights()) ;

		//sorting according to ldeg
		if(min_sort){
			std::sort(lv.begin(), lv.end(), my_struct_smaller_diff );			
		}else{
			std::sort(lv.begin(), lv.end(), my_struct_greater_diff );	
		}
		return 0;
	}

	
	template<typename Graph_t>
	int sort_wddiff(const Graph_t& g, int* lv, int size, bool min_sort=true){
		com::has_smaller_val_diff<int, typename Graph_t::_wt> my_struct_smaller_diff (g.get_weights()) ;
		com::has_greater_val_diff<int, typename Graph_t::_wt> my_struct_greater_diff (g.get_weights()) ;

		//sorting according to ldeg
		if(min_sort){
			std::sort(lv, lv+size, my_struct_smaller_diff );			
		}else{
			std::sort(lv, lv+size, my_struct_greater_diff );	
		}
		return 0;
	}


	/*struct smaller_than_wd{
		smaller_than_wd(Graph_t* g_out):g(g_out){NV=g_out->number_of_vertices();}
		bool smaller_than_wd()(int a, int v){
			return g->get_wv(a)*(abs(a-NV)) <  g->get_wv(b)*(abs(b-NV));
		}

		int NV;
		Graph_t* g;
	};*/


//////////////
//edge-weighted graphs 
// date_first_commit@: 8/8/2018

	/*template<typename Graph_t>
	struct accum_we{
		typename Graph_t::_wt operator ()( double accum, int v) const{return (accum + lw[v]);}
		accum_we(double* lw):lw(lw){}
		const double* lw;
	};*/

	template<class Graph_t, class _wt>
	int ew_shift_2_highest_index(const Graph_t& g, const int* lv, _wt* lw, int size_lv, double wper=1.0){
	/////////////////////////
	// 
	// last_update@: 8/8/2018
	// param@lv: subproblem nodes
	// param@size: number of nodes of subproblem lv
	// param@lw: array of weights of nodes- weights will be initialied to 0. Edge weight shifted to the 
	//			 endpoint node with highest index (WRITE)
	//	param@wper: [0,1] ratio of the weight that is shifted towards highest index (DEFAULT VALUE 1-100% )
	//
	// RETURNS: 0 if OK, -1 if ERROR
	//
	// comments@: TEST-Does not work for integer weights! (25/11/2021)

		//init weights
		for(int i=0; i<size_lv; i++){
			lw[lv[i]]=0.0;
		}
		
		//main loop, enumerating all edges in the subproblem
		for(int i=0; i<size_lv-1; i++){
			for(int j=i+1; j<size_lv; j++){
				if(g.is_edge(lv[i],lv[j])){
					if(lv[i]<lv[j]){
						lw[lv[j]]+=wper*g.get_we(lv[i], lv[j]);
						lw[lv[i]]+=(1-wper)*g.get_we(lv[i], lv[j]);
					}else{
						lw[lv[i]]+=wper*g.get_we(lv[i], lv[j]);
						lw[lv[j]]+=(1-wper)*g.get_we(lv[i], lv[j]);
					}
				}
			}
		}

		return 0;
	}


	template<typename Graph_t>
	typename Graph_t::_wt wesum(const Graph_t& g, bool only_we=false){
		double res=0.0;
		const int NV=g.number_of_vertices();

		if (only_we) { 
			for (int i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (g.get_we(i, j) != Graph_t::NOWT)
						res += g.get_we(i, j);				//no edge-checking!
				}
			}
		}
		else {		//all weights, including vertex weights
			for (int i = 0; i < NV; i++) {
				for (int j = i; j < NV; j++) {
					if (g.get_we(i, j) != Graph_t::NOWT)
						res += g.get_we(i, j);				//no edge-checking!
				}
			}
		}
		
		return res;
	}

	template<typename Graph_t>
	typename Graph_t::_wt wesum(const Graph_t& g, vint& lv, bool only_we = false){
		double res=0.0;
		const int NV=lv.size();
		if (only_we) {
			for (int i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (g.get_we(i, j) != Graph_t::NOWT)
						res += g.get_we(lv[i], lv[j]);			//no edge-checking!
				}
			}
		}
		else {//all weights, including vertex weights

			for (int i = 0; i < NV; i++) {
				for (int j = i; j < NV; j++) {
					if (g.get_we(i, j) != Graph_t::NOWT)
						res += g.get_we(lv[i], lv[j]);			//no edge-checking!
				}
			}
		}
				
		return res;
	}











}

#endif