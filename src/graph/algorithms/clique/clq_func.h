//clq_func.h: header for algorithms / data types related to cliques / isets

//date of creation: 10/07/19
//last update: 28/11/19
//dev@pss

#ifndef __CLIQUE_FUNC_H__
#define __CLIQUE_FUNC_H__

#include <iostream>
#include <algorithm>
//#include <numeric>							/* accumulate */

#include "utils/logger.h"
#include "utils/common.h"
//#include "../../copt_config_logs.h"
//#include "../clique_types.h"


//#undef NDEBUG to enable run-time assertions
#include <cassert>		


//////////////////
////switches
//#define my_assert_qfunc							//(OFF in Release mode!)
////
//////////////////


using namespace std;

namespace qfunc{
	
	//clique and independent set heuristics

	/**
	* @brief Heuristic that determines the size of the maximal independent set formed by the first consecutive vertices
	*		 in the (half-open) range [@begin, @end[ of the set @lv
	* @param g input graph
	* @param lv: input set of vertices (C-style array)
	* @param begin, end: positions of first and one-after-last the last vertex in @lv to be considered
	* @returns: size (number of vertices) of the independent set
	* @details: runs in O(n^2) worst-case time, where n = end - begin
	* @TODO - builds and iset but only returns its size
	**/
	template<class Graph_t>
	int find_iset(Graph_t& g, int lv[], unsigned int begin, unsigned int end) {

		////////////////////////
		assert(begin <= end);
		///////////////////////

		std::vector<int> iset;
		iset.push_back(lv[begin]);

		//main loop
		for (auto i = begin + 1; i < end; ++i) {
			int w = lv[i];

			//determines if w can be added to the independent set
			for (const auto& v : iset) {
				////////////////////////////////////////////////
				if (g.is_edge(v, w)) { return iset.size(); }
				///////////////////////////////////////////////
			}

			//adds the vertex to the independent set
			iset.push_back(w);

		}

		return iset.size();
	}

	/**
	* @brief Heuristic that determines the size of the maximal independent set that can enlarge the singleton set {@v}
	*        considering the first consecutive vertices in the (half open)range [@begin, @end[ of the set @lv
	* @param g input graph
	* @param v: input vertex that determines the singleton independent set {v} to be enlarged
	* @param lv: input set of vertices (C-style array)
	* @param begin, end: positions of first and one-after-last the last vertex in @lv to be considered
	* @returns: size of the enlarged independent set (excluding @v)
	* @details: runs in O(n^2) worst-case time, where n = end - begin
	**/
	template<class Graph_t>
	int find_iset(Graph_t& g, int v, int lv[], unsigned int begin, unsigned int end) {
		
		////////////////////////
		assert(begin <= end);
		///////////////////////

		int nV = 0;

		//main loop
		for (auto i = begin; i < end; ++i) {

			//candidate vertex to enlarge the independent set
			int w = lv[i];

			//check that w is non-adjacent to v
			if (g.neighbors(v).is_bit(w)) {return nV;}
			

			//check that w is non-adjacent to all the previous vertices in @lv 
			for (auto j = i - 1; j >= begin; --j) {

				///////////////////////////////////////////////////////////
				if (g.neighbors(w).is_bit(lv[j])) {return nV;}
				///////////////////////////////////////////////////////////
			}

			nV++;
		}
		
		return nV;		
	}

	/**
	* @brief Heuristic that enlarges the clique in @clq with vertices in the half-open range [@begin, @end[ of the set @lv.
	*		 Vertices are taken in order from @lv and fixed to @clq if adjacent to all vertices in @clq.
	* @param g input graph
	* @param lv: input set of vertices (C-style array)
	* @param begin, end: pointers to first and one after the last vertex in @lv to be considered
	* @param clq: input clique to be enlarged (if possible)
	* @returns: size (number of vertices) added to @clq
	**/
	template<class Graph_t>
	int find_clq(Graph_t& g, std::vector<int>& clq, int lv[], unsigned int begin, unsigned int end) {

		////////////////////////
		assert(begin <= end);
		///////////////////////

		bool clq_found = false;
		int nV = 0;
		
		//main loop that iterates over all the vertices in the range
		for (auto i = begin; i < end; ++i) {
			clq_found = true;
			int w = lv[i];
			
			//checks the vertex is adjacent to all vertices in @clq	
			for (const auto& v : clq) {
				if (!g.is_edge(v, w)) {
					clq_found = false;
					break;
				}
			}
			
			//adds the vertex to the clique if previous condition is met
			if (clq_found) {
				clq.push_back(lv[i]);
				nV++;
			}
		}

		return nV;
	}

	//////////////////////////////////
	// VERY CLIQUE SPECIFIC - PLACE IT IN COPT
	//// enlarges @clq with vertices in lv, starting from @begin
	//// can detect cliques by swapping one of the vertices in @quasi
	////
	//// RETURN val:number of vertices added to clq (0 or 1)
	////
	//// COMMENTS: format issues-MUST BE! @quasi[VERTEX]:=-1 or VERTEX

	//template<class Graph_t>
	//int find_clq(Graph_t& g, vint& clq, int* quasi, int begin, int* lv, int end) {
	//			
	//	int num_added = 0, counter = 0, iqv = -1;	

	//	//main loop - iterates over each vertex in the range
	//	for (int i = (end - 1); i >= begin ; i--){						//reverse direction! perhaps better for brock? (26/01/2021)
	//	//for (int i = begin; i < end; i++) {	
	//	// 	
	//		counter = 0;

	//		for (auto j = 0; j < clq.size(); ++j) {
	//			if (!g.is_edge(lv[i], clq[j])) { 
	//				counter++;				
	//				if (quasi[clq[j]] == -1 || lv[i] == quasi[clq[j]] || counter >= 2) { 
	//					counter = 2;  
	//					break; 
	//				}
	//				else { iqv = j;  };					
	//			}
	//		}
	//		
	//		if (counter == 0) {	
	//			LOG_ERROR("warning!- improving clique with previous dolls, find_clq(...)" << lv[i]);
	//			cin.get();
	//			clq.push_back(lv[i]);
	//			num_added++;
	//			break;						//quasi info may not be valid	
	//		}else if (counter == 1) {
	//			int qswap = quasi[clq[iqv]];
	//			if (  g.is_edge(lv[i], qswap) ) {	//quasi-clique check
	//				
	//				clq.push_back(lv[i]);
	//				clq[iqv] = qswap;			//swap vertex in @clq with the new quasi vertex 					
	//				num_added++;
	//															
	//				break;						//EXIT-quasi clique info is not valid any more
	//			}
	//			else {
	//				//LOG_INFO("ENCONTRADO SWAP SIN MEJORA: "<< lv[i]);
	//			}
	//			
	//		}			
	//	}

	//	return num_added;
	//}
	//	

//	template<class Graph_t> 
//	bool is_iset(Graph_t& g, typename Graph_t::_bbt& bb){
//	/////////////////
//	// TRUE if subgraph bb is and independent set
//	//
//	// COMMENTS: /* Not optimized for all graph types */
//
//		bitarray neighbor(g.number_of_vertices());
//		if(bb.init_scan(bbo::NON_DESTRUCTIVE)==EMPTY_ELEM) return false;	//empty cliqe
//		while(true){
//			int v=bb.next_bit();
//			if (v==EMPTY_ELEM) break;
//
//			//check neighborhood						
//			AND(g.get_neighbors(v), bb, neighbor);
//			if(!neighbor.is_empty())
//					return false;
//			 
//		}
//		return true;
//	}
//
//	template<class Graph_t> 
//	bool is_iset_edge_based(Graph_t& g, vint& lv){
//	/////////////////////
//	// determines iset by looking at edges: simple approach
//	// with early check detection; specially suited when nodes
//	// come as lists not bitstrings (6/10/17)
//		int nElem = lv.size();
//		for(int i=0; i< nElem - 1; i++)
//			for(int j=i+1; j< nElem; j++){
//				if(g.is_edge(lv[i], lv[j])){
//					return false;
//				}
//			}
//
//		return true;
//	}
//
//	template<class Graph_t> 
//	bool is_iset_union(Graph_t& g, vint& lv, int v){
//	/////////////////////
//	// Assuming the nodes in lv form an iset, returns TRUE if
//	// if the set lv + {v} is also an iset 
//	// date: (5/2/18)
//	// Note: an empty lv set always returns TRUE
//
//		if(lv.empty()) return true;
//
//		for(int i=0; i<lv.size(); i++){
//			if(g.is_edge(lv[i], v)){
//				return false;
//			}
//		}
//
//		return true;
//	}
//
//	template<class Graph_t> 
//	bool is_clique(Graph_t& g, typename Graph_t::_bbt& bb){
//	/////////////////
//	// TRUE if subgraph bb is a clique
//	//
//	// COMMENTS: /* Not optimized for all graph types / not valid for sparse graphs */
//	//
//	// /* TODO-CHECK: SIZE OF BB=1
//
//		bitarray neighbor(g.number_of_vertices());
//		if(bb.init_scan(bbo::NON_DESTRUCTIVE)==EMPTY_ELEM) return false;	//empty cliqe
//		while(true){
//			int v=bb.next_bit();
//			if (v==EMPTY_ELEM) break;
//
//			//check neighborhood						
//			AND(g.get_neighbors(v), bb, neighbor);
//			neighbor.set_bit(v);
//			if(!(bb==neighbor)) 
//					return false; 
//		}
//		return true;
//	}
//	
//	//specialization
//	inline bool is_clique(sparse_ugraph& g, vint& lv) {
//		struct is_clique_not_implemented_with_sparse_ugraph {};
//		return false;
//	}
//
//	template<class Graph_t> 
//	bool is_clique(Graph_t& g, vint& lv){
//	/////////////////
//	// TRUE if subgraph lv is a clique
//	// 
//	// COMMENTS:
//	//  I. implemented with unrolling for early condition detection
//	//  II. the bad part is the initial construction of a bitstring from lv
//	//	/* Not optimized for all graph types */
//
//		if(lv.empty()) return false;
//		else if(lv.size()==1) return true;
//		typename Graph_t::_bbt ref(lv, g.number_of_vertices());
//	
//	/*	ref.print();
//		LOG_INFO(".........");*/
//
//		for(vint::iterator it=lv.begin(); it!=lv.end(); it++){
//			int v=*it;
//			typename Graph_t::_bbt& neigh=g.get_neighbors(v);			
//			ref.erase_bit(v);												//to compare only neighbors of v
//			
//			//check neighborhood-unroll
//			for(int nBB=0; nBB<neigh.number_of_bitblocks(); nBB++){
//				BITBOARD aux=ref.get_bitboard(nBB) & neigh.get_bitboard(nBB);
//				if (aux != ref.get_bitboard(nBB)) {
//#ifdef	print_clq_func_logs
//					LOG_INFO("v:" << v << "not in clique" << "-qfunc::is_clique()");
//#endif
//					return false;
//				}
//			}
//
//			ref.set_bit(v);													//resets context
//
//			//without unrolling
//			/*AND(g.get_neighbors(v), bb, neighbor);
//			neighbor.set_bit(v);
//			if(!(bb==neighbor)) 
//			return false; */
//
//		}
//		return true;
//	}
//		
//	template<class Graph_t> 
//	bool is_clique_edge_based(Graph_t& g, vint& lv){
//	/////////////////////
//	// determines clique by looking at edges: simple approach
//	// with early check detection; specially suited when nodes
//	// come as lists not bitstrings (6/10/17)
//	//
//	// COMMENTS: An empty clique is a clique
//		
//		int nElem = lv.size();
//		for(int i=0; i< nElem -1; i++)
//			for(int j=i+1; j< nElem; j++){
//				if(!g.is_edge(lv[i], lv[j])){
//					return false;
//				}
//			}
//
//		return true;
//	}
//	
//	template<class Graph_t> 
//	bool is_clique(Graph_t& g, int* lv, int size){
//	/////////////////
//	// TRUE if subgraph bb is a clique (C-ARRAY INTERFACE)
//	// 
//	// COMMENTS:
//	//  I. implemented with unrolling for early condition detection
//	//  II. the bad part is the initial construction of a bitstring from lv
//	//	/* Not optimized for all graph types */
//
//		if(size<=0) return false;
//		else if(size==1) return true;
//		typename Graph_t::_bbt ref(g.number_of_vertices());
//		for(int i=0; i<size; i++){
//			ref.set_bit(lv[i]);
//		}
//	
//		for(int i=0; i<size; i++){
//			int v=lv[i];
//			typename Graph_t::_bbt& neigh=g.get_neighbors(v);			
//			ref.erase_bit(v);												//to compare only neighbors of v
//			
//			//check neighborhood-unroll
//			for(int nBB=0; nBB<neigh.number_of_bitblocks(); nBB++){
//				BITBOARD aux=ref.get_bitboard(nBB)& neigh.get_bitboard(nBB);
//				if(aux!=ref.get_bitboard(nBB))
//					return false;
//			}
//
//			ref.set_bit(v);													//resets context
//
//			//without unrolling
//			/*AND(g.get_neighbors(v), bb, neighbor);
//			neighbor.set_bit(v);
//			if(!(bb==neighbor)) 
//			return false; */
//
//		}
//
//		return true;
//	}
//
//	template<class Graph_t>
//	int quasi_clq(Graph_t& g, vint& clq, int* quasi, typename Graph_t::_bbt& bbv) {
//		////////////////////////////////
//		// caches quasicliques in @quasi for consecutive vertices not in @clq (candidate quasi_vertices range [0-highest vertex index of clq[)
//		// quasi[VERTEX_IN_CLQ]:= qv (@clq - {VERTEX_IN_CLQ} + {qv} is a clique in @g of the same size)
//		// @bbv-set of vertices not considered in qv
//		//
//		// COMMENTS: quasi MUST be of size the number of vertices of @g
//		//
//		// RETURN val:number of quasi-cliques (-1 if a new clique is found)	
//
//		//ASSERT
//		if (clq.empty()) return 0;
//
//		typename Graph_t::_bbt bbclq(clq, g.number_of_vertices());
//		int num_added = 0, num_no_clq = 0, qv = -1, lastOfclq = bbclq.msbn64();
//
//		for (int v = 0; v < lastOfclq; v++) {									/* last vertex in clq excluded, since it MUST BE in @bbv*/
//			if (bbclq.is_bit(v) || !bbv.is_bit(v)) continue;					//filters vertices in @clq and in @bbv
//			num_no_clq = 0;
//			for (int j = 0; j < clq.size(); j++) {
//				if (!g.is_edge(v, clq[j])) {
//					num_no_clq++;
//					if (num_no_clq >= 2) { break; }
//					else { qv = clq[j]; }
//				}
//			}
//
//			if (num_no_clq == 0) { //Already a clique and HAS NOT BEEN FOUND!
//				clq.push_back(v);
//				num_added++;
//
//#ifdef my_assert_qfunc					
//				LOG_ERROR("clique found in preceding dolls-com::quasi_clq()");
//				LOG_ERROR("may only happen in QUASI-CLIQUE mode, and if a new solution of size > +1 has been found previously");
//				/*if (is_clique(g, clq)) {
//					LOG_ERROR("bizarre clique, exiting....-com::quasi_clq()");
//					LOG_ERROR("v that may be added:" << v);
//					stringstream sstr;
//					com::stl::print_collection<vint>(clq, sstr);
//					LOG_ERROR("clq with v included" << sstr.str());
//					exit(-1);
//				}
//				else {
//					LOG_ERROR("bizarre clique, exiting....-com::quasi_clq()");
//				}*/
//#endif
//				return -1;
//			}
//			else if (num_no_clq == 1) {
//				quasi[qv] = v;
//				num_added++;
//			}
//		}
//		return num_added;
//	}
//
//	template<class Graph_t>
//	int greedy_clique_LB (const Graph_t& g,  typename Graph_t::_bbt& bbsg){
//	//////////////////////
//	// first neighbor node at each iteration, starting from {1}		/* TO TEST, but looks ok */
//	// 
//		int lb=0;
//		typename Graph_t::_bbt bb(bbsg);
//		bb.init_scan(bbo::DESTRUCTIVE);
//		while(true){
//			int v=bb.next_bit_del();
//			if (v==EMPTY_ELEM) break;
//			lb++;
//			bb&=g.get_neighbors(v);		
//		}
//		return lb;
//	}
//
//	template<class Graph_t>
//	int greedy_clique_LB (const Graph_t& g,  typename Graph_t::_bbt& bbsg, vint& clq,  bool reverse=false){
//	//////////////////////
//	// first neighbor node at each iteration, starting from {1}		/* TO TEST, but looks ok */
//	//
//	// RETURNS set of nodes in clq
//
//
//		int lb = 0, v = EMPTY_ELEM;
//		typename Graph_t::_bbt bb(bbsg);
//		clq.clear();
//
//		if (reverse) { bb.init_scan(bbo::NON_DESTRUCTIVE_REVERSE); }
//		else { bb.init_scan(bbo::NON_DESTRUCTIVE); }
//		while(true){
//			if (reverse) { v = bb.previous_bit(); }
//			else { v = bb.next_bit(); }	
//
//			if (v==EMPTY_ELEM) break;
//			lb++;
//			bb&=g.get_neighbors(v);	
//			clq.push_back(v);
//		}
//		return lb;
//	}
//
//	template<class Graph_t>
//	int greedy_clique_LB_max_deg(const Graph_t& g, typename Graph_t::_bbt& bbsg, vint& clq, bool reverse=false) {
//		//////////////////////
//		// At each iteration takes the item with larger neighborhood
//		//
//		// RETURNS set of nodes in clq or O if bbsg is empty
//		//
//		// date@Paris_13_/10_/19
//		// TODO@OPTIMIZE- Gets stuck in practice for some graphs!!
//
//		const int N = g.number_of_vertices();
//		int lb = 0;
//		typename Graph_t::_bbt bb1(bbsg);
//		typename Graph_t::_bbt bb2(bb1);
//		clq.clear();
//
//	
//		int pcmax = 0, pc, vmax, v;
//		do {
//			//finds the best guy in the current neighborhood
//			pcmax = 0;
//			vmax = EMPTY_ELEM;
//			
//			if (reverse) {	bb2.init_scan(bbo::NON_DESTRUCTIVE_REVERSE);}
//			else {	bb2.init_scan(bbo::NON_DESTRUCTIVE);}
//			while (true) {
//				if (reverse) {	v = bb2.previous_bit();	}
//				else {	v = bb2.next_bit();	}
//
//				if (v == EMPTY_ELEM) break;							
//				pc = 0;
//				const typename Graph_t::_bbt& bbn = g.get_neighbors(v);
//				for (int nBB = 0; nBB <= WDIV(N); nBB++) {
//					pc += BitBoard::popc64(bb2.get_bitboard(nBB) & bbn.get_bitboard(nBB));
//				}
//				
//				if (pcmax < pc) {
//					pcmax = pc;
//					vmax = v;
//				}				
//			}
//			
//			if (pcmax > 0) {
//				clq.push_back(vmax);
//				bb1 &= g.get_neighbors(vmax);
//				bb2 = bb1;
//				lb++;
//			}
//			else {							 /* empty neighborhood, add first vertex in the set to the clq */
//				int w = bb2.lsbn64();
//				if (w != EMPTY_ELEM) {
//					clq.push_back(w);
//					lb++;
//				}
//				break;
//			}
//		} while (true);
//		return lb;
//	}
//
//	template<class Graph_t>
//	int greedy_clique_all_nodes_LB(const Graph_t& g, typename Graph_t::_bbt& bbsg, vint& clq) {
//		//////////////////////
//		// first neighbor node at each iteration, starting from {1}		/* TO TEST, but looks ok */
//		//
//		// RETURNS set of nodes in clq
//		//
//		///* TODO-include nb  of iterations as as parameter */
//
//		const int N = g.number_of_vertices();
//		int lb = 0;
//		int lb_max = 0;
//		vint clq_max;
//		typename Graph_t::_bbt bb(N);
//		for (int v = 0; v < N; v++) {
//			lb_max = 0;
//			clq_max.clear();
//			bb = bbsg;
//			bb &= g.get_neighbors(v);
//			clq_max.push_back(v);
//			bb.init_scan(bbo::DESTRUCTIVE);
//			while (true) {
//				int v = bb.next_bit_del();
//				if (v == EMPTY_ELEM) break;
//				lb_max++;
//				bb &= g.get_neighbors(v);
//				clq_max.push_back(v);
//			}
//			if (lb_max > lb) {
//				lb = lb_max;
//				clq = clq_max;
//			}
//		}
//		return lb;
//	}
//
//
//	template<class Graph_t>
//	int greedy_consec_clique_LB (const Graph_t& g,  typename Graph_t::_bbt& bbsg){
//	//////////////////////
//	// starting from first node in bbsg, determines consecutive nodes that form a clique
//	// 
//		int lb=0;
//		vint lv;
//		lv.push_back(bbsg.lsbn64());
//		if(lv.front()!=EMPTY_ELEM){
//			lb++;
//		}else return lb;					/* empty set */
//
//		bbsg.erase_bit(lv.front());
//		bbsg.init_scan(bbo::NON_DESTRUCTIVE);
//		while(true){
//			int v=bbsg.next_bit();
//			if (v==EMPTY_ELEM) break;
//			bool is_adj=true;
//			for(int i=0; i<lv.size(); i++){
//				if(!g.is_edge(lv[i],v)){
//					is_adj=false;
//					break;
//				}
//			}
//			if(!is_adj) break;
//			lv.push_back(v);
//			lb++;		
//		}
//		bbsg.set_bit(lv.front());
//		return lb;
//	}
//
//	template<class Graph_t>
//	int greedy_consec_clique_LB (const Graph_t& g, vint& clq){
//	//////////////////////
//	// for all the graph, determines consecutive nodes that form a clique
//	// 
//		int lb=1;
//		clq.clear();
//		clq.push_back(0);
//		const int NV=g.number_of_vertices();
//		
//		for(int v=1; v<NV; v++){
//
//			//check adjacency
//			bool is_adj=true;
//			for(int w=0; w<v; w++){
//				if(!g.is_edge(v,w)){
//					is_adj=false;
//					break;
//				}
//			}
//
//			if(!is_adj)  break;
//			clq.push_back(v);
//			lb++;		
//		}
//		return lb;
//	}
//
//	template<class Graph_t>
//	int greedy_consec_clique_LB (const Graph_t& g,  typename Graph_t::_bbt& bbsg, vint& clq ){
//	//////////////////////
//	// starting from first node in bbsg, determines consecutive nodes that form a clique
//	// 
//		int lb=0;
//		clq.clear();
//		clq.push_back(bbsg.lsbn64());
//		if(clq.front()!=EMPTY_ELEM){
//			lb++;		
//		}else{
//			clq.clear();
//			return 0;					/* empty set */
//		}
//
//		bbsg.erase_bit(clq.front());
//		bbsg.init_scan(bbo::NON_DESTRUCTIVE);
//		while(true){
//			int v=bbsg.next_bit();
//			if (v==EMPTY_ELEM) break;
//			bool is_adj=true;
//			for(int i=0; i<clq.size(); i++){
//				if(!g.is_edge(clq[i],v)){
//					is_adj=false;
//					break;
//				}
//			}
//			if(!is_adj) break;
//			clq.push_back(v);
//			lb++;		
//		}
//		bbsg.set_bit(clq.front());
//		return lb;
//	}
//	
//	template<class Graph_t>
//	inline
//		void incUB(const Graph_t& g, int ub[], size_t size) {
//		//computes incremental clique bound for nodes 1 to size for the given graph
//		ub[0] = 1;
//		int max = 0;
//		for (int v = 1; v < size; v++) {
//			max = 0;
//			for (int w = 0; w < v; w++) {
//				if (g.is_edge(v, w)) {
//					if (ub[w] > max) max = ub[w];
//				}
//			}
//			ub[v] = max + 1;
//		}
//	}
//
//	template<class Graph_t>
//	inline
//		int incUB(const Graph_t& g, typename Graph_t::_bbt& bbsg, int ub[]) {
//		// computes incremental clique bound for nodes in bbsg
//		// RETURNS -1 if bbsg is empty, 0 in any other case
//		// 
//		// COMMENTS
//		// Does not overwrite other values of ub[] 
//		//
//		//*EXPERIMENTAL*
//
//		vint nodes;
//		bbsg.to_vector(nodes);			/* conversion-seems the simplest  thing to do */
//		if (nodes.empty()) return -1;
//		ub[nodes.front()] = 1;
//
//		int max = 0;
//		for (int i = 1; i < nodes.size(); i++) {
//			max = 0;
//			for (int j = 0; j < i; j++) {
//				if (g.is_edge(nodes[i], nodes[j])) {
//					if (ub[nodes[j]] > max) max = ub[nodes[j]];
//				}
//			}
//			ub[nodes[i]] = max + 1;
//		}
//		return 0;
//	}
//	
//	/////////////////////
//	//SEQ
//	template<class Graph_t>
//	inline
//		int iset_seq(const Graph_t& g, typename Graph_t::_bbt& bbsg, int* ub = NULL) {
//		///////////////////////
//		//greedy iset seq coloring
//		//
//		// COMMENTS: bbsg will be erased!
//
//		int pc = bbsg.popcn64(), col = 1, v = EMPTY_ELEM, nBB = EMPTY_ELEM;
//		if (pc == 0) { return 0; }
//		typename Graph_t::_bbt m_sel(g.number_of_vertices());
//		while (true) {
//			m_sel = bbsg;
//			m_sel.init_scan(bbo::DESTRUCTIVE);
//			while (true) {
//				if ((v = m_sel.next_bit_del(nBB, bbsg)) == EMPTY_ELEM)
//					break;
//				if (ub) { ub[v] = col; }
//				if ((--pc) == 0) { return col; }
//
//				m_sel.erase_block(nBB, g.get_neighbors(v));
//			}
//			++col;
//		}
//
//		return col;		//should not reach here
//	}
//	   
//	template<class Graph_t>
//	inline
//		int number_outgoing_edges(Graph_t& g, std::vector<int> clq) {
//		///////////////////////
//		// RETURNS edges outgoing from every vertex of @clq
//	
//		int nb_edges = 0;
//		typename Graph_t::_bbt bbcov(clq, g.number_of_vertices());
//		for (int i = 0; i < clq.size(); i++) {
//			for (int NBB = 0; NBB < g.number_of_blocks(); NBB++) {
//				BITBOARD bb = g.get_neighbors(clq[i]).get_bitboard(NBB) &~bbcov.get_bitboard(NBB);
//				nb_edges += BitBoard::popc64(bb);
//			}
//		}		
//		return nb_edges;
//	}
//
//	template<class Graph_t>
//	inline
//		int max_number_outgoing_edges(Graph_t& g, std::vector<vector<int>>& part, int& pos) {
//		///////////////////////
//		// RETURNS index of element in @part with maximum number of outgoing edges (-1 if empty)
//
//		int max = 0, num = 0;
//		pos = -1;
//		for (int i = 0; i < part.size(); i++) {
//			num = number_outgoing_edges(g, part[i]);
//			if (max < num) { max = num;  pos = i; }
//		}
//		return max;
//	}
//
//	template<class Graph_t>
//	inline
//		int min_number_outgoing_edges(Graph_t& g, std::vector<vector<int>>& part, int& pos) {
//		///////////////////////
//		// RETURNS index of element in @part with minimum number of outgoing edges (-1 if empty)
//
//		int min = CLQ_MAXINT, num = 0;
//		pos = -1;
//		for (int i = 0; i < part.size(); i++) {
//			num = number_outgoing_edges(g, part[i]);
//			if (min > num) { min = num;  pos = i; }
//		}
//		return min;
//	}
}


#endif