/**
* @file graph_func.h
* @brief main header for namespace gfunc providing general purpose functions for graphs
* @details: created 07/3/17, update 12/3/17, last_update 28/02/25  
* @author pss
*
* TODO CHECK edgeW::ew_shift_2_highest_index function  (09/01/25)
**/

#ifndef __GRAPH_FUNC_H__
#define __GRAPH_FUNC_H__

#include "simple_ugraph.h"
#include "simple_graph_w.h"				// must be after ugraph include
#include "simple_graph_ew.h"				// must beafter ugraph include
#include "utils/common.h"
#include "utils/logger.h"
#include <algorithm>
#include <iostream>

//aliases for graph types (see graph.h)
using vint = std::vector<int>;
using graph = Graph<bitarray>;
using ugraph = Ugraph<bitarray>;	
using ugraph_w = Graph_W<ugraph, double>;
using ugraph_wi = Graph_W<ugraph, int>;
using ugraph_ew = Graph_EW<ugraph, double>;
using ugraph_ewi = Graph_EW<ugraph, int>;


///////////////////////
// namespace gfunc 
// (stateless functions for GRAPH lib)

namespace gfunc{
	namespace sort {
		/*
		* @brief Sorts a set of vertices (lv) according to their degree wrt to a reference set (lref) of vertices.
		*
		*		  No restrictions between lv and lref (can have non-empty intersection)
		*
		* @param g: a simple graph
		* @param lv: list of vertices to be sorted
		* @param lref: list of reference vertices
		* @param min_sort: sorting criteria
		* @returns sorted list of vertices lv
		*/
		template<class Graph_t>
		vint& sort_deg(const Graph_t& g, vint& lv, const vint& lref, bool min_sort = true) {

			int deg = 0;

			//determine the degree of each vertex in lv wrt to ref
			vint ldeg(g.number_of_vertices(), 0);
			for (auto v : lv) {
				deg = 0;
				for (auto w : lref) {
					if (g.is_edge(v, w)) {
						deg++;
					}
				}
				ldeg[v] = deg;
			}

			//I/O
			//com::stl::print_collection(ldeg); 

			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv.begin(), lv.end(), com::has_smaller_val<int, vector<int>>(ldeg));
			}
			else {
				std::sort(lv.begin(), lv.end(), com::has_greater_val<int, vector<int>>(ldeg));
			}

			return lv;
		}

		/*
		* @brief Sorts a set of vertices (lv) according to their degree wrt to a reference set (bbref) of vertices.
		*
		*		  No restrictions between lv and bbref (can have non-empty intersection)
		*
		* @param g: a simple graph
		* @param lv: list of vertices to be sorted
		* @param bbref: list (bitset) of reference vertices
		* @param min_sort: sorting criteria
		* @returns sorted list of vertices lv
		*/
		template<class Graph_t>
		vint& sort_deg(const Graph_t& g, vint& lv, typename  Graph_t::_bbt& bbref, bool min_sort = true) {

			int deg = 0;

			//determine the degree of each vertex in lv wrt to ref
			vint ldeg(g.number_of_vertices(), 0);
			for (auto v : lv) {
				ldeg[v] = g.degree(v, bbref);
			}

			//I/O
			//com::stl::print_collection(ldeg); 

			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv.begin(), lv.end(), com::has_smaller_val<int, vector<int>>(ldeg));
			}
			else {
				std::sort(lv.begin(), lv.end(), com::has_greater_val<int, vector<int>>(ldeg));
			}

			return lv;
		}
	} //namespace sort


	/*
	* @brief List of neighbors of v in the set of vertices bbref ()
	* @param g: a simple undirected graph
	* @param lv_n: output list of vertices
	* @param bbref: list (bitset) of reference vertices
	* @returns number of vertices in lv_n
	* @created: 03/09/18
	* @last_update: 09/01/25
	*/
	template<class Graph_t>
	int neighbors(const Graph_t& g, int v,  typename  Graph_t::_bbt& bbref, vector<int>& lv){
	
		lv.clear();
		lv.reserve(g.number_of_vertices());		//allocates maximum possible size

		auto MAXNBB = g.number_of_blocks();
		for(decltype(MAXNBB) nBB = 0; nBB < MAXNBB; ++nBB){

			//block NBB of the intersection of N(v) and bbref
			BITBOARD bb = g.neighbors(v).block(nBB) & bbref.block(nBB);		
			
			//decodes vertices from the bitblock and appends to vertex list
			int offset = WMUL(nBB);
			while(bb != 0){
				int v64 =bblock::lsb(bb);

				///////////////////////////////
				lv.emplace_back(v64 + offset);
				///////////////////////////////

				bb ^= Tables::mask[v64];				//removes v64 from the bitset (XOR bitmask)
			}
		}		
		
		return lv.size();
	}

	/*
	* @brief List of neighbors of v in the set of vertices bbref () that come after v
	* @param g: a simple undirected graph 
	* @param lv_n: output list of vertices
	* @param bbref: list (bitset) of reference vertices
	* @returns number of vertices in lv_n
	* @created: 03/09/18
	* @last_update: 09/01/25
	*/
	template<class Graph_t>
	int neighbors_after (const Graph_t& g, int v,  typename  Graph_t::_bbt& bbref, vector<int>& lv){
		
		lv.clear();
		lv.reserve(g.number_of_vertices());		//allocates maximum possible size

		auto MAXNBB = g.number_of_blocks();

		//////////////////////////////////////////
		//first nBB - trimming and decoding

		//block index of v
		int nBBv = WDIV(v);				

		//first block of the intersection of N(v) and bbref from v		
		BITBOARD bb = g.neighbors(v).block(nBBv) & bbref.block(nBBv);

		//trims preceding vertices
		bb &= Tables::mask_high[WMOD(v)];							

		//decodes block of v (appends to lv)	
		while (bb != 0) {
			int v64 = bblock::lsb(bb);			

			///////////////////////////////
			lv.emplace_back(v64 + nBBv);
			///////////////////////////////

			bb ^= Tables::mask[v64];								//removes v64 from the bitset (XOR bitmask)
		}

		//////////////////////////////////////////
		//remaining blocks
		
		for (decltype(MAXNBB) nBB = nBBv + 1; nBB < MAXNBB; ++nBB) {

			//block NBB of the intersection of N(v) and bbref
			BITBOARD bb = g.neighbors(v).block(nBB) & bbref.block(nBB);

			//decodes vertices from the bitblock and appends to vertex list
			int offset = WMUL(nBB);
			while (bb != 0) {
				int v64 = bblock::lsb(bb);
				
				///////////////////////////////
				lv.emplace_back(v64 + offset);
				///////////////////////////////

				bb ^= Tables::mask[v64];				//removes v64 from the bitset (XOR bitmask)
			}
		}

		return lv.size();
	}
		
	/*
	* @brief creates a complete graph of a given size N
	* @param g: a simple graph
	* @param N: number of vertices
	* @returns 0 if success, -1 if error
	*/
	template<class Graph_t>
	int create_complete(Graph_t& g, std::size_t N) {

		if (g.reset(N) == -1) { return -1; }

		for (auto i = 0; i < N; ++i) {
			for (auto j = 0; j < N; j++) {
				g.add_edge(i, j);
			}
		}
		return 0;
	}

	/*
	* @brief creates a complete undirected graph (clique) of a given size N
	*		 (specialization for undirected graphs)
	* 
	* @param g: a simple graph
	* @param N: number of vertices
	* @returns 0 if success, -1 if error
	*/
	inline
	int create_complete(ugraph& g, int N) {

		if (g.reset(N) == -1) { return -1; }
		
		for (auto i = 0; i < N-1; ++i) {
			for (auto j = i + 1; j < N; ++j) {
				g.add_edge(i, j);
			}
		}
		return 0;
	}

	/*
	* @brief Determines if an induced subgraph has no edges 
	*		 If this is not the case provides the first edge found
	*
	* @param g: a simple graph
	* @param bbsg: bitset of vertices that induces the subgraph
	* @param edge: output first-found edge (pair of vertices) in the induced subgraph
	* @returns TRUE if the induced subgraph has no edges, FALSE otherwise
	*/
	template<class Graph_t>
	bool is_edgeFree_subgraph(Graph_t& g, typename Graph_t::_bbt& bbsg, vint& edge) {
		
		//cleans output edge
		edge.clear();

		//decodes subgraph to vector
		vint lv;
		bbsg.to_vector(lv);

		//singleton input bitset - induced subgraph has no edges
		auto NV = lv.size();
		if (NV <= 1) { return true; }

		//searches for an edge in the induced subgraph
		for (auto i = 0; i < NV - 1; i++) {
			for (auto j = i + 1; j < NV; j++) {

				if (g.is_edge(lv[i], lv[j])) {
					edge.emplace_back(lv[i]);
					edge.emplace_back(lv[j]);
					return false;
				}

			}
		}

		return true;
	}

	/*
	* @brief Determines if an induced subgraph has no triangles
	*		 If this is not the case provides the first triangle found
	*
	* @param g: a simple graph
	* @param bbsg: bitset of vertices that induces the subgraph
	* @param edge: output first-found triangle (triplet of vertices) in the induced subgraph
	* @returns TRUE if the induced subgraph has no edges, FALSE otherwise
	*/
	template<class Graph_t>
	bool is_triangleFree_subgraph(Graph_t& g, typename Graph_t::_bbt& bbsg, vint& triangle) {
	
		//cleans output 
		triangle.clear();

		//decodes subgraph to vector
		vint lv;
		bbsg.to_vector(lv);

		//input bitset of size 2 - induced subgraph has no triangles
		auto NV = lv.size();
		if (NV <= 2) { return true; }
				
		//searches for an edge in the induced subgraph
		for (auto i = 0; i < NV - 2; ++i) {
			for (auto j = i + 1; j < NV - 1; ++j) {

				if (g.is_edge(lv[i], lv[j])) {		//determines an edge

					for (auto k = j + 1; k < NV; ++k) {
						
						if (g.is_edge(lv[j], lv[k]) && g.is_edge(lv[i], lv[k])) {		//determines a triangle with edge (i,j)
							triangle.emplace_back(lv[i]);
							triangle.emplace_back(lv[j]);
							triangle.emplace_back(lv[k]);
							return false;
						}
					}

				}
			}
		}
		return true;
	}
				
////////////////////
//namespace for vertex-weighted graphs 
// 
//@created: 9/1/17
//@last_update: 9/1/25

	namespace vertexW {

		/*
		* @brief sum of weights of the vertices in a (bit)set (bbref)	
		*/
		template<typename Graph_t>
		typename Graph_t::_wt wsum(const Graph_t& g, typename Graph_t::_bbt& bbref) {

			typename Graph_t::_wt total_weight = 0;

			//bitscanning configuration
			bbref.init_scan(bbo::NON_DESTRUCTIVE);

			//bitscan to sum weights
			int v;
			while ( (v = bbref.next_bit() ) != EMPTY_ELEM) {
				total_weight += g.weight(v);
			}
					
			return total_weight;
		}

		/*
		* @brief sum of weights of the vertices in a set (lv)
		*/
		template<typename Graph_t>
		typename Graph_t::_wt wsum(const Graph_t& g, vint& lv) {

			typename Graph_t::_wt total_weight = 0;

			//iterate and add
			for (int v : lv) {
				total_weight += g.weight(v);
			}

			return total_weight;
		}

		/*
		* @brief sum of weights of the vertices in a graph
		*/
		template<typename Graph_t>
		typename Graph_t::_wt wsum(const Graph_t& g) {

			typename Graph_t::_wt total_weight = 0;

			// Obtiene el vector de pesos de los vértices
			const auto& weights = g.weights();

			// Recorre todos los pesos y los suma
			for (const auto& weight : weights) {
				total_weight += weight;
			}

			return total_weight;
		}


		/*
		* @brief sum of weights the neighborhood of a vertex (v) in a (bit)set of vertices (bbref)
		*		 plus the weight of vertex v		
		*/
		template<typename Graph_t>
		typename Graph_t::_wt wsum(const Graph_t& g, int v, typename Graph_t::_bbt& bbref) {
		
			auto total_weight = g.weight(v);

			//bitscanning configuration
			bbref.init_scan(bbo::NON_DESTRUCTIVE);

			//bitscan to sum weights
			int w;
			while ((w = bbref.next_bit()) != EMPTY_ELEM) {
				if (g.is_edge(v, w)) {
					total_weight += g.weight(w);
				}
			}
					
			return total_weight;
		}

		/*
		* @brief sum of weights the neighborhood of a vertex (v) 
		*		 plus the weight of vertex v
		*/
		template<typename Graph_t>
		typename Graph_t::_wt wsum(Graph_t& g, int v) {
			
			auto total_weight = g.weight(v);
			const auto& bbn = g.neighbors(v);

			//bitscanning configuration
			if (bbn.init_scan(bbo::NON_DESTRUCTIVE) != -1) {
				int u = BBObject::noBit;
				while ((u = bbn.next_bit()) != BBObject::noBit) {
					total_weight += g.weight(u);
				}
			}

			return total_weight;
		}

		/*
		* @brief sorts a set of vertices according to non-increasing or non-decreasing weights 
		* @param g: a simple graph
		* @param lv: std::vector of vertices to be sorted
		* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
		* @returns sorted set of vertices
		*/
		template<typename Graph_t>
		vint& sort_w(const Graph_t& g, vint& lv, bool min_sort = true) {

			//sorting criteria
			const auto& weights = g.weight();

			//sorting lambdas
			auto compare_smaller = [&weights](int a, int b) {
				return weights[a] < weights[b];
			};

			auto compare_greater = [&weights](int a, int b) {
				return weights[a] > weights[b];
				};

			//sorts vertices according to the given criteria
			if (min_sort) {
				std::sort(lv.begin(), lv.end(), compare_smaller);
			}
			else {
				std::sort(lv.begin(), lv.end(), compare_greater);
			}
					
			return lv;
		}

		/*
		* @brief sorts a C-style set of vertices according to non-increasing or non-decreasing weights
		* @param g: a simple graph
		* @param lv: pointer to an array of vertices to be sorted
		* @param: size: number of vertices in the array
		* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
		* @returns pointer to the sorted array
		*/
		template<typename Graph_t>
		int* sort_w(const Graph_t& g, int *lv, std::size_t size, bool min_sort = true) {
			
			//sorting criteria
			const auto& weights = g.weight();

			//sorting lambdas
			auto compare_smaller = [&weights](int a, int b) {
				return weights[a] < weights[b];
			};

			auto compare_greater = [&weights](int a, int b) {
				return weights[a] > weights[b];
			};

			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv, lv + size, compare_smaller);
			}
			else {
				std::sort(lv, lv + size, compare_greater);
			}
			return lv;
		}

		/*
		* @brief sorts a set of vertices according to weight x degree
		* @param g: a simple graph
		* @param lv: set of vertices
		* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
		* @returns sorted set of vertices
		*/
		template<typename Graph_t>
		vint& sort_wdProd(const Graph_t& g, vint& lv, bool min_sort = true) {

			//weights as part of the sorting criteria
			const auto& weights = g.weights();

			//sorting lambdas - degree * weight criteria
			auto compare_smaller = [&g, &weights](int a, int b) {
				return (weights[a] * g.degree(a)) < (weights[b] * g.degree(b));
			};

			auto compare_greater = [&g, &weights](int a, int b) {
				return (weights[a] * g.degree(a)) > (weights[b] * g.degree(b));
			};

			//sorting according to lambdas
			if (min_sort) {
				std::sort(lv.begin(), lv.end(), compare_smaller);
			}
			else {
				std::sort(lv.begin(), lv.end(), compare_greater);
			}
			return lv;
		}


		/*
		* @brief sorts a C-style set of vertices according to weight x degree
		* @param g: a simple graph
		* @param lv: set of vertices
		* @param size: number of vertices in the array
		* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
		* @returns pointer to the sorted array
		*/
		template<typename Graph_t>
		int* sort_wdProd(const Graph_t& g, int* lv, int size, bool min_sort = true) {

			//weights as part of the sorting criteria
			const auto& weights = g.weights();

			//sorting lambdas - degree * weight criteria
			auto compare_smaller = [&g, &weights](int a, int b) {
				return (weights[a] * g.degree(a)) < (weights[b] * g.degree(b));
				};

			auto compare_greater = [&g, &weights](int a, int b) {
				return (weights[a] * g.degree(a)) > (weights[b] * g.degree(b));
				};
						
			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv, lv + size, compare_smaller);
			}
			else {
				std::sort(lv, lv + size, compare_greater);
			}

			return lv;
		}

		/*
		* @brief sorts a set of vertices according to weight - degree
		* @param g: a simple graph
		* @param lv: set of vertices
		* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
		* @returns sorted set of vertices
		*/
		template<typename Graph_t>
		vint& sort_wdDif(const Graph_t& g, vint& lv, bool min_sort = true) {

			//weights as part of the sorting criteria
			const auto& weights = g.weights();
		
			//sorting lambdas - weight - degree criteria
			auto compare_smaller = [&g, &weights](int a, int b) {
				return (weights[a] - g.degree(a)) < (weights[b] - g.degree(b));
			};

			auto compare_greater = [&g, &weights](int a, int b) {
				return (weights[a] - g.degree(a)) > (weights[b] - g.degree(b));
			};

			/*com::has_smaller_val_diff<int, typename vector<Graph_t::_wt>> my_struct_smaller_diff(g.weights());
			com::has_greater_val_diff<int, typename vector<Graph_t::_wt>> my_struct_greater_diff(g.weights());*/

			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv.begin(), lv.end(), compare_smaller);
			}
			else {
				std::sort(lv.begin(), lv.end(), compare_greater);
			}
			return lv;
		}

		/*
		* @brief sorts a C-style set of vertices according to weight - degree
		* @param g: a simple graph
		* @param lv: set of vertices
		* @param size: number of vertices in the array
		* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
		* @returns pointer to the sorted array
		*/
		template<typename Graph_t>
		int* sort_wdDif(const Graph_t& g, int* lv, int size, bool min_sort = true) {

			//weights as part of the sorting criteria
			const auto& weights = g.weights();

			//sorting lambdas - weight - degree criteria
			auto compare_smaller = [&g, &weights](int a, int b) {
				return (weights[a] - g.degree(a)) < (weights[b] - g.degree(b));
			};

			auto compare_greater = [&g, &weights](int a, int b) {
				return (weights[a] - g.degree(a)) > (weights[b] - g.degree(b));
			};

			//sorting according to ldeg
			if (min_sort) {
				std::sort(lv, lv + size, compare_smaller);
			}
			else {
				std::sort(lv, lv + size, compare_greater);
			}
			return lv;
		}
	

	} //namespace vertexW


//////////////
//namespace for edge-weighted graphs 
// 
//@created:8/8/18
//@last_update: 9/1/25
 
	namespace edgeW {

		/*template<typename Graph_t>
		struct accum_we{
			typename Graph_t::_wt operator ()( double accum, int v) const{return (accum + lw[v]);}
			accum_we(double* lw):lw(lw){}
			const double* lw;
		};*/
		
		template<typename Graph_t>
		typename Graph_t::_wt wesum(const Graph_t& g, bool only_we = false)		 { LOG_ERROR("not implemented yet -  edgeW::wesum"); return 0; }
		
		//alias
		template<class W>
		using UEW = Graph_EW<ugraph, W>;

		/*
		* @brief Determines the sum of the edge-weights of a given undirected graph
		* @param only_we: flag to consider only edge-weights (true) or also vertex-weights we_(v, v) (false)
		* @returns sum of the weights
		* 
		* TODO - simplified code (09/01/25)
		*/
		template<typename W>
		typename UEW<W>::_bbt wesum(const UEW<W>& g, bool only_we = false) {
		
			double total_weight = 0.0;
			const auto NV = g.number_of_vertices();
						
			for (auto i = 0; i < NV - 1; ++i) {
				for (auto j = (only_we ? i + 1 : i); j < NV; j++) {

					if (g.get_we(i, j) != UEW<W>::NOWT) {
						total_weight += g.get_we(i, j);				//no edge-checking!
					}
				}
			}
			
			return total_weight;
		}

		/*
		* @brief Determines the sum of the edge-weights of an induced subgraph
		* 
		*		I. Does not add default empty weigth value in case of inconsisten graphs		
		* 
		* @param lv: list of vertices in the induced subgraph
		* @param only_we: flag to consider only edge-weights (true) or also vertex-weights we_(v, v) (false)
		* @returns sum of the weights
		* 
		*/
		template<typename Graph_t>
		typename Graph_t::_wt wesum(const Graph_t& g, vint& lv, bool only_we = false) {
			
			typename Graph_t::_wt total_weight = 0.0;
			const auto NV = lv.size();
			
			for (auto i = 0; i < NV - 1; i++) {
				for (auto j = (only_we ? i + 1 : i); j < NV; ++j) {

					if (g.get_we(i, j) != Graph_t::NOWT) {				//Checks graph consistency - perhaps remove or do it in DEBUG mode only
						total_weight += g.get_we(lv[i], lv[j]);			//no edge-checking!
					}
				}
			}
			
			return total_weight;
		}

		template<class Graph_t, class _wt>
		int ew_shift_2_highest_index(const Graph_t& g, const int* lv, _wt* lw, int size_lv, double wper = 1.0) {
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
			for (int i = 0; i < size_lv; i++) {
				lw[lv[i]] = 0.0;
			}

			//main loop, enumerating all edges in the subproblem
			for (int i = 0; i < size_lv - 1; i++) {
				for (int j = i + 1; j < size_lv; j++) {
					if (g.is_edge(lv[i], lv[j])) {
						if (lv[i] < lv[j]) {
							lw[lv[j]] += wper * g.get_we(lv[i], lv[j]);
							lw[lv[i]] += (1 - wper) * g.get_we(lv[i], lv[j]);
						}
						else {
							lw[lv[i]] += wper * g.get_we(lv[i], lv[j]);
							lw[lv[j]] += (1 - wper) * g.get_we(lv[i], lv[j]);
						}
					}
				}
			}

			return 0;
		}



	} //namespace edgeW

} //namespace

#endif