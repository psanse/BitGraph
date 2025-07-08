/**
* @file clq_func.h
* @brief  header for graph algorithms / data types related to cliques / isets
* @details: imported from COpt repo (created 10/07/19, last update: 28/11/19)
* @details: imported 09/05/2025, last update 20/05/2025
* @details: C++17 if contexpr instructions have been commented - add when C++17 compatibility is acceptable (currently only C++14)
* @dev: pss
**/

#ifndef __CLIQUE_FUNC_H__
#define __CLIQUE_FUNC_H__

#include <iostream>
#include "utils/logger.h"
#include "utils/common.h"

//#undef NDEBUG to enable run-time assertions
#include <cassert>		

namespace gfunc {
	namespace iset {

		using namespace bitgraph;
		using bbo = bitgraph::BBObject;

		///////////////////
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
		inline
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
		inline
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
				if (g.neighbors(v).is_bit(w)) { return nV; }


				//check that w is non-adjacent to all the previous vertices in @lv 
				for (auto j = i - 1; j >= begin; --j) {

					///////////////////////////////////////////////////////////
					if (g.neighbors(w).is_bit(lv[j])) { return nV; }
					///////////////////////////////////////////////////////////
				}

				nV++;
			}

			return nV;
		}
	}

	namespace clq {
		/**
		* @brief Clique heuristic that enlarges the clique in @clq with vertices in the half-open range [@begin, @end[ of the set @lv.
		*		 Vertices are taken in order from @lv and fixed to @clq if adjacent to all vertices in @clq.
		* @param g input graph
		* @param lv: input set of vertices (C-style array)
		* @param begin, end: pointers to first and one after the last vertex in @lv to be considered
		* @param clq: input clique to be enlarged (if possible)
		* @returns: size (number of vertices) added to @clq
		**/
		template<class Graph_t>
		inline
			int find_clique(Graph_t& g, std::vector<int>& clq, int lv[], unsigned int begin, unsigned int end) {

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

		/**
		* @brief Clique heuristic that builds a clique with the vertices in @bbsg.
		*		 Vertices are taken in order from @bbsg and fixed to @clq if possible.
		* @param g input graph
		* @param clq: output clique
		* @param bbsg: (bit)set of vertices
		* @param template Reverse: TRUE vertices are taken from bbsg from last to first, FALSE from first to last
		* @returns: number of vertices in clq
		* @TODO: possibly return the clique, add if constexpr when C++17 is accepted
		**/
		template<class Graph_t, bool Reverse = false>
		inline
			int find_clique(const Graph_t& g, std::vector<int>& clq, typename Graph_t::_bbt& bbsg) {

			typename Graph_t::_bbt bb(bbsg);
			clq.clear();

			//main loop - destructive scan of bb
			if /*constexpr*/ (Reverse) {
				bb.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE_REVERSE);
			}
			else { bb.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE); }

			int v =bbo::noBit;
			while (true) {

				if /*constexpr*/ (Reverse) { v = bb.prev_bit(); }
				else { v = bb.next_bit(); }

				/////////////////////////////
				if (v == bbo::noBit) break;
				///////////////////////////////

				//v fixed in the clique
				clq.push_back(v);

				//optimization of bb &= g.get_neighbors(w);	
				if /*constexpr*/ (Reverse) {
					for (auto nBB = WDIV(v); nBB >= 0; --nBB) {
						bb.block(nBB) &= g.neighbors(v).block(nBB);
					}
				}
				else {
					for (auto nBB = WDIV(v); nBB < g.number_of_blocks(); ++nBB) {
						bb.block(nBB) &= g.neighbors(v).block(nBB);
					}
				}
			}

			return clq.size();
		}

		/**
		* @brief Clique heuristic that builds a clique with the vertices in @bbsg.
		*		 Each time the vertex with maximum degree that can enlarge the clique is selected
		* @param g input graph
		* @param clq: output clique
		* @param bbsg: (bit)set of vertices
		* @param template Reverse: TRUE vertices are taken from bbsg from last to first, FALSE from first to last
		* @returns: number of vertices in clq
		* @details: created in 13/10/2019 (Paris), optimized in 06/05/2025
		* @TODO: possibly return the clique, add if constexpr when C++17 is accepted
		**/
		template<class Graph_t, bool Reverse = false>
		inline
			int find_clique_max_deg(const Graph_t& g, std::vector<int>& clq, const typename Graph_t::_bbt& bbsg) {

			typename Graph_t::_bbt bbsgC{ bbsg };
			clq.clear();

			int pcmax, pc;
			int vmax, v;
			do {

				/////////////////
				//finds the vertex with max deg in the current neighborhood
				pcmax = 0;

				if /*constexpr*/ (Reverse) { bbsgC.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE_REVERSE); }
				else { bbsgC.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE); }
				while (true) {
					if /*constexpr*/ (Reverse) { v = bbsgC.prev_bit(); }
					else { v = bbsgC.next_bit(); }

					////////////////////////////
					if (v == bbo::noBit) break;
					////////////////////////////

					pc = 0;
					const typename Graph_t::_bbt& bbn = g.neighbors(v);
					for (auto nBB = 0; nBB < g.number_of_blocks(); ++nBB) {
						pc += bitgraph::bblock::popc64(bbsgC.block(nBB) & bbn.block(nBB));
					}

					//store vertex if more neighbors
					if (pcmax < pc) {
						pcmax = pc;
						vmax = v;
					}
				}

				////////////////////////
				//actions depending on pcmax
				if (pcmax > 0) {

					//add vmax to the clique 
					clq.push_back(vmax);

					//removes non-adjacent vertices to vmax from bbsgC
					bbsgC &= g.neighbors(vmax);
				}
				else {

					////////////////////////
					assert(pcmax == 0);
					////////////////////////

					// pcmax == 0, empty neighborhood, bbsg is an iset
					// add first (could be any) vertex from bbsg to the clq  -  loop ends
					int w = bbsgC.lsb();

					if (w !=bbo::noBit) { clq.push_back(w); }
				}

				//if pcmax ==0 at least one vertex can enlarge clq, 
				//so continue iterating, else STOP
			} while (pcmax > 0);

			return clq.size();
		}

		/**
		* @brief: Clique heuristic that computes a clique from a pool of cliques with vertices in @bbsg.
		*		  Specifically, the pool are all the possible |@bbsg| cliques formed with a first vertex from @bbsg
		*		  enlarged by the remaining vertices taken in order.
		*		  The procedure chooses the LARGEST clique from the pool.
		* @param g input graph
		* @param clq: output clique
		* @param bbsg: (bit)set of vertices
		* @returns: number of vertices in clq
		* @details: imported from copt repo, optimized in 08/05/2025
		**/
		template<class Graph_t>
		inline
			int find_clique_from_pool(const Graph_t& g, std::vector<int>& clq, typename Graph_t::_bbt& bbsg) {

			typename Graph_t::_bbt bb(g.size());
			clq.clear();

			//main loop - seed vertex for a clique 
			vint clq_curr;
			for (int v = 0; v < g.size(); ++v) {

				clq_curr.clear();

				//fix vertex in the current clique
				clq_curr.push_back(v);

				////////////
				//determine clique with the first consecutive vertices in the neighboorhood of v

				//initialize bb with the neighborhood of v in bbsg in [v, end)
				bitgraph::AND<true>(v, g.size() - 1, g.neighbors(v), bbsg, bb);

				//main loop
				int w =bbo::noBit;
				bb.init_scan(bitgraph::BBObject::DESTRUCTIVE);
				while ((w = bb.next_bit_del()) !=bbo::noBit) {

					clq_curr.push_back(w);

					//optimization of bb &= g.get_neighbors(w);	
					for (auto nBB = WDIV(w); nBB < g.number_of_blocks(); ++nBB) {
						bb.block(nBB) &= g.neighbors(v).block(nBB);
					}

				}
				///////////

				//I/O
				//com::stl::print_collection(clq_curr, std::cout, true);

				//update clique if smaller than current clique
				if (clq.size() < clq_curr.size()) { clq = clq_curr; }
			}

			return clq.size();
		}
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

		/////////////////
		//boolean operations
	namespace iset {
		/**
		* @brief Determines if the set of vertices @bb is an independent set
		* @param g: input graph
		* @param bb: input set of vertices (bitset)
		* @returns TRUE if the set is an independent set (empty sets are NOT isets)
		* @TODO: not optimized for sparse graph types
		**/
		template<class Graph_t>
		inline
			bool is_iset(Graph_t& g, typename Graph_t::_bbt& bb) {
						
			int retVal = bb.init_scan(bbo::NON_DESTRUCTIVE);

			///////////////////////
			assert(retVal != -1);
			///////////////////////
						
			int v =bbo::noBit;
			while ( (v = bb.next_bit()) !=bbo::noBit ) {

				//check neighborhood of v
				const auto& bbnv = g.neighbors(v);
				for (auto nBB = 0; nBB < bb.number_of_blocks(); ++nBB) {				

					//////////////////////////////////////////
					if ( bb.block(nBB) & bbnv.block(nBB) ) {
						return false;
					}
					/////////////////////////////////////////
				}

			}//end bitscan

			//returns false if the set is empty, true otherwise
			return (bb.is_empty()) ? false : true;

		}

		/**
		* @brief Determines if the set of vertices @bb is an independent set
		* @param g: input graph
		* @param lv: input set of vertices
		* @returns TRUE if the set is an independent set (empty sets are NOT isets)
		* @details: created 6/10/2017, last updated 06/05/2025
		* @TODO: not optimized for sparse graph types
		**/
		template<class Graph_t>
		inline
			bool is_iset(Graph_t& g, std::vector<int>& lv) {

			auto nV = lv.size();
			if (nV == 0) return false;					////empty set is not an independent set

			for (auto i = 0; i < nV - 1; ++i) {
				for (auto j = i + 1; j < nV; ++j) {

					if (g.is_edge(lv[i], lv[j])) {
						return false;
					}
				}
			}

			return true;
		}

		/**
		* @brief Determines if {v} is not adjacent to any vertex in @lv.
		* @param g: input graph
		* @param lv: input set of vertices
		* @param v: input vertex
		* @returns TRUE if the set {lv + v} is an independent set or if {lv} is empty.
		* @details: created 5/02/2018, last updated 06/05/2025
		* @comment: if {lv} is an iset, then {lv + v} is also an iset
		**/
		template<class Graph_t>
		inline
			bool is_iset(Graph_t& g, std::vector<int>& lv, int v) {

			if (lv.empty()) return true;

			for (const auto& w : lv) {
				if (g.is_edge(v, w)) {
					return false;
				}
			}

			return true;
		}
	}

	namespace clq {
		/**
		* @brief Determines if the set of vertices @bb forms a clique
		* @param g: input graph
		* @param bb: input set of vertices (bitset)
		* @returns TRUE if the set is a clique (empty sets are NOT cliques)
		* @TODO: not optimized for sparse graph types
		**/
		template<class Graph_t>
		inline
			bool is_clique(Graph_t& g, typename Graph_t::_bbt& bb) {

			int retVal = bb.init_scan(bbo::NON_DESTRUCTIVE);

			///////////////////////
			assert(retVal != -1);
			///////////////////////
			
			int v =bbo::noBit;
			while ((v = bb.next_bit()) !=bbo::noBit) {

				//check neighborhood of v
				const auto& bbnv = g.neighbors(v);
				for (auto nBB = 0; nBB < bb.number_of_blocks(); ++nBB) {

					//////////////////////////////////////////
					if (!(bb.block(nBB) & bbnv.block(nBB))) {
						return false;
					}
					/////////////////////////////////////////
				}

			}//end bitscan

			//returns false if the set is empty, true otherwise
			return (bb.is_empty()) ? false : true;

		}

		/**
		* @brief Determines if the set of vertices @lv is a clique
		* @param g: input graph
		* @param lv: input set of vertices
		* @returns TRUE if the set is a clique (empty sets are NOT cliques)
		* @details: created 6/10/2017, last updated 06/05/2025
		* @TODO: not optimized for sparse graph types
		**/
		template<class Graph_t>
		inline
			bool is_clique(Graph_t& g, std::vector<int>& lv) {

			auto nV = lv.size();
			if (nV == 0) return false;					//an empty set is not a clique

			for (auto i = 0; i < nV - 1; ++i) {
				for (auto j = i + 1; j < nV; ++j) {

					if (!g.is_edge(lv[i], lv[j])) {
						return false;
					}
				}
			}

			return true;
		}

		/**
		* @brief Determines if the set of vertices @lv is a clique
		* @param g: input graph
		* @param lv: input set of vertices
		* @returns TRUE if the set is a clique (empty sets are NOT cliques)
		* @details: created 6/10/2017, last updated 06/05/2025
		* @TODO: not optimized for sparse graph types
		**/
		template<class Graph_t>
		inline
			bool is_clique(Graph_t& g, int lv[], std::size_t size) {

			if (size == 0) return false;					//an empty set is not a clique

			for (auto i = 0; i < size - 1; ++i) {
				for (auto j = i + 1; j < size; ++j) {

					if (!g.is_edge(lv[i], lv[j])) {
						return false;
					}
				}
			}

			return true;
		}

		/**
		* @brief Determines if {v} is adjacent to any vertex in @lv.
		* @param g: input graph
		* @param lv: input set of vertices
		* @param v: input vertex
		* @returns TRUE if the set {lv + v} is an independent set or if {lv} is empty.
		* @details: created 5/02/2018, last updated 06/05/2025
		* @comment: if {lv} is a clique, then {lv + v} is also a clique
		**/
		template<class Graph_t>
		inline
			bool is_clique(Graph_t& g, std::vector<int>& lv, int v) {

			if (lv.empty()) return true;

			for (const auto& w : lv) {
				if (!g.is_edge(v, w)) {
					return false;
				}
			}

			return true;
		}
	}

		//////////////////////////////////
		// VERY CLIQUE SPECIFIC - PLACE IT IN COPT
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

	//////////////////////
	//maximum clique bounds

	namespace clq {
		/**
		* @brief Clique heuristic that computes a LOWER bound for the maximum clique in @g[@bbsg].
		*		 Equivalent to find_clique(..., @clq, @bbsg) but does not store the clique.
		* @param g input graph
		* @param bbsg: (bit)set of vertices
		* @param template Reverse: TRUE vertices are taken from bbsg from last to first, FALSE from first to last
		* @returns: number of vertices in clq
		* @TODO: possibly return the clique
		**/
		template<class Graph_t, bool Reverse = false>
		inline
			int find_clique_lb(const Graph_t& g, typename Graph_t::_bbt& bbsg) {

			typename Graph_t::_bbt bb(bbsg);
			int lb = 0;

			//main loop - destructive scan of bb
			if /*constexpr*/ (Reverse) {
				bb.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE_REVERSE);
			}
			else { bb.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE); }

			int v =bbo::noBit;
			while (true) {

				if /*constexpr*/ (Reverse) { v = bb.prev_bit(); }
				else { v = bb.next_bit(); }

				/////////////////////////////
				if (v ==bbo::noBit) break;
				///////////////////////////////

				//v fixed in the clique
				lb++;

				//optimization of bb &= g.get_neighbors(w);	
				if constexpr (Reverse) {
					for (auto nBB = WDIV(v); nBB >= 0; --nBB) {
						bb.block(nBB) &= g.neighbors(v).block(nBB);
					}
				}
				else {
					for (auto nBB = WDIV(v); nBB < g.number_of_blocks(); ++nBB) {
						bb.block(nBB) &= g.neighbors(v).block(nBB);
					}
				}
			}

			return lb;
		}

		/**
		* @brief Clique heuristic that computes a LOWER bound for the size maximum clique in @g.
		*		 Equivalent to find_clique_lb(..., @bsg) with subset @bbsg equal to V(@g)
		* @param g input graph
		* @param template Reverse: TRUE vertices are taken from bbsg from last to first, FALSE from first to last
		* @returns: a lower bound for the size of the maximum clique in g.
		**/
		template<class Graph_t, bool Reverse = false>
		inline
			int find_clique_lb(const Graph_t& g) {

			typename Graph_t::_bbt bb((int)g.size(), true);
			int lb = 0;

			//main loop - destructive scan of bb
			if /*constexpr*/ (Reverse) {
				bb.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE_REVERSE);
			}
			else { bb.init_scan(bitgraph::BBObject::NON_DESTRUCTIVE); }

			int v =bbo::noBit;
			while (true) {

				if /*constexpr*/ (Reverse) { v = bb.prev_bit(); }
				else { v = bb.next_bit(); }

				/////////////////////////////
				if (v ==bbo::noBit) break;
				///////////////////////////////

				//v fixed in the clique
				lb++;

				//optimization of bb &= g.get_neighbors(w);	
				if constexpr (Reverse) {
					for (auto nBB = WDIV(v); nBB >= 0; --nBB) {
						bb.block(nBB) &= g.neighbors(v).block(nBB);
					}
				}
				else {
					for (auto nBB = WDIV(v); nBB < g.number_of_blocks(); ++nBB) {
						bb.block(nBB) &= g.neighbors(v).block(nBB);
					}
				}
			}

			return lb;
		}

		///////////////////////
		// Skeleton of alternative implementation of find_clique_lb...
		// Each time the bitset is scanned to find the first vertex that can enlarge
		// the clique (early exit when it cannot)

		//template<class Graph_t>
		//int find_clique_lb_ALT(const Graph_t& g, typename Graph_t::_bbt& bbsg) {

		//	//////////////////////////////////////
		//	if (bbsg.is_empty()) { return 0; }		/* empty set */
		//	/////////////////////////////////////

		//	int v = bbsg.lsb();
		//	std::vector<int> clq;
		//	clq.push_back(first_v);

		//	//scans from v + 1 onwards to find the next vertex in the clique
		//	bbsg.init_scan(v, bbo::NON_DESTRUCTIVE);
		//	while ((v = bbsg.next_bit()) != bbo::noBit) {

		//		//check if v can enlarge the current clique in clq
		//		bool is_adj = true;
		//		for (const auto& w : clq) {
		//			if (!g.is_edge(v, w)) {
		//				is_adj = false;
		//				break;
		//			}
		//		}

		//		/////////////////////////////
		//		if (!is_adj) { break; }
		//		/////////////////////////////

		//		clq.push_back(v);

		//	}//end while

		//	return clq.size();
		//}

		/////////////////////
		//max clique upper bound (SEQ vertex coloring)

		/**
		* @brief: greedy sequential iset coloring of the vertices in @bbsg
		* @param g: input graph
		* @param bbsg: (bit)set of vertices to be colored
		* @param ub: (optional) C-array of size g.size() with the resulting coloring
		* @returns: size of the coloring, i.e., number of different colors used
		* @details: efficiency is in O(n^3) in the worst-case, but fast in practice as it
		*		    is reduced by WORD_SIZE constant
		* @TODO : UNIT TEST
		* @TODO : two functions, one for the color bound, the other for the coloring
		**/
		template<class Graph_t>
		inline
			int SEQ(const Graph_t& g, const typename Graph_t::_bbt& bbsg, int* ub = nullptr) {

			int pc = bbsg.size();
			if (pc == 0) { return 0; }			//early exit - bitset bbsg is empty	

			int col = 1, v =bbo::noBit, nBB =bbo::noBit;

			//main loop - greedy coloring	
			typename Graph_t::_bbt bb_unsel(bbsg);
			typename Graph_t::_bbt bb_sel(g.size());
			while (true) {

				//load bb_sel with remaining vertices to be colored
				bb_sel = bb_unsel;

				//build a new iset with vertices from bb_sel
				bb_sel.init_scan(bitgraph::BBObject::DESTRUCTIVE);
				while ((v = bb_sel.next_bit_del()) !=bbo::noBit) {

					//update coloring
					if (ub) { ub[v] = col; }

					//remove colored vertex from bbsg
					bb_unsel.erase_bit(v);

					/////////////////////////////////
					if ((--pc) == 0) { return col; }				//early exit - all vertices colored
					/////////////////////////////////

					//removes neighbors of v
					bb_sel.erase_block(WDIV(v), -1,  g.neighbors(v));
				}

				//open a new color
				++col;
			}

			return col;		//should not reach here
		}
	}


		//////////////////////////////////
		// Incremental bounds VERY CLIQUE SPECIFIC - PLACE IT IN COPT

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


			////////////////////////
			//Others - outgoing / incoming edges

		//	template<class Graph_t>
		//	inline
		//	int number_outgoing_edges(Graph_t& g, std::vector<int> clq) {
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
	
}//end of namespace gfunc


#endif