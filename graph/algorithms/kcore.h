/**
 * @file kcore.h
 * @brief declaration of the KCore class which manages coreness of vertices in a graph
 *
 * @created 6/6/14
 * @last_update 27/01/25
 * @author pss
 *
 * TODO - mention reference paper to determine KCore
 * TODO - using pointer to external bitset as set of vertices of the induced subgraph. Change to an owned bitset or vector of vertices (13/01/25)
 *		  Add different types to define the set of vertices that induces the subgraph (13/01/25)
 **/

#ifndef __KCORE_H__
#define __KCORE_H__

#include "utils/logger.h"
#include "simple_ugraph.h"
#include "algorithms/graph_fast_sort.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <map> 


 //undef NDEBUG to avoid assertions
#include <cassert>

/////////////////////////
//SWAP-MACRO: places vertex u as last vertex in the bin with one less degree. Updates bin but not degree of u
#define SWAP_BIN(u)	{ int du = deg_[(u)]; int pu = pos_[(u)]; int pw = bin_[du]; int w = ver_[pw];		\
						if( (u) != w) { pos_[(u)] = pw; pos_[w] = pu; ver_[pu] = w; ver_[pw] = (u);}	\
						bin_[du]++;																			}

//int du=deg_[u];
//int pu=pos_[u];
//int pw=bin_[du];
//int w= ver_[pw];
//if(u!=w){			//else there is no need to move the vertex
//	pos_[u]=pw;
//	pos_[w]=pu;		//SWAP_BIN (u->w) with first vertex with same degree
//	ver_[pu]=w;
//	ver_[pw]=u;
//}	
// bin_[du]++;  

//////////////////////////

//useful alias
using sparse_ugraph = Ugraph<sparse_bitarray>;			//simple sparse undirected graph
		
using vint = std::vector<int>;
using vint_it = std::vector<int>::iterator;
using vint_cit = std::vector<int>::const_iterator;

using map_t = std::map<int, int>;	
using map_it = std::map<int, int>::iterator;


///////////////////
//
// KCore class
//
// Manages coreness of vertices in a graph
// Managaes core of a graph
//
////////////////////

template<class Graph_t>
class KCore{
public:	
	friend ostream& operator<< (std::ostream& o, const KCore& kc){ kc.print_kcore(o); return o;}

	enum print_t { DEG, BIN, VER, POS };

	//alias types
	using basic_type = Graph_t;						//a graph from GRAPH, typically sparse_ugraph
	using type = KCore;

	//alias types for backward compatibility
	using _bbt = typename basic_type::_bbt;			//bitset type
	using _gt = basic_type;							//graph type

	//////////////
	//globals

	/*
	* @brief maximum core of a graph in O(|V|*|V|), by determining
	*		the width of a graph according to a minimum degenerate 
	*		ordering.
	*	
	*		I. Use for middle and small graphs (not for sparse graphs)
	* 
	* @returns maximum core of the graph
	* 
	* TO NOTE: it is not possible an early exit (13/01/25)
	*/
	static int find_kcore				(Graph_t& g);				
	
//////////////////////////////
//construction / destruction
public:

	//constructors
	KCore								(Graph_t& g);
	KCore								(Graph_t& g, _bbt subg);
	KCore								(Graph_t& g, vint subg);
		
	//copy and move semantics disallowed
	KCore								(const KCore& kc)				= delete;	
	KCore&  operator =					(const KCore& kc)				= delete;
	KCore								(KCore&& kc)	noexcept		= delete;
	KCore& operator =					(KCore&& kc)	noexcept		= delete;

	//destructor - default
	virtual ~KCore						()								= default;
	 
////////////////
//setters and getters 

	/*
	* @brief Maximum core number of the graph
	*		 (must be called afer kcore())
	* @returns Maximum core number of the graph or -1 if error
	*/
	int max_core_number					()								const;																	
	
	/*
	* @brief Core number of a given vertex
	*		 (must be called afer kcore())
	*/
	int coreness						(std::size_t v)					const { return deg_[v]; }
	
	/*
	* @brief Size of the kcore for a given k >=0 , number of vertices with core number k
	*		 (must be called afer kcore())
	*/
	int core_size						(std::size_t k)					const;
	
	/*
	* @brief Returns the k-core set of vertices for a given k >=0 
	*		 (must be called afer kcore()).
	* 
	*		 I. k = 0: all vertices in V
	*		II. k = 1: vertices in the 1-core etc...
	*/
	vint core_set						(std::size_t k)					const;

	/*
	* @brief Coreness of all vertices
	*		 (must be called afer kcore()).
	*/
	const vint& coreness_numbers		()								const	{ return deg_;}
			
	/*
	* @brief Arrangement of the vertices according to non-decreasing kcore number
	*		 (must be called afer kcore()).
	*/
	const vint& kcore_ordering			()								const	{ return ver_;}


	const _gt& get_graph				()								const { return g_; }
   const _bbt& get_subgraph				()								const { return subg_; }

	/*
	* @brief sets a new induced subgraph.
	* 
	*		 I. allocates @ver_ only
	*		 
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int reset_subgraph					(_bbt psg);

//////////////
// Main operations
public:
/*
	* @brief Computes maximum core and coreness (core algorithm). 
	*		 After its execution: 
	*	
	*		 I. @ver_ contains the vertices in non-decreasing kcore order (new-to-old format)
	*		II. @deg_ contains the coreness of each vertex
	*		III.The maximum core number of the graph is @deg_[last_vertex]
	* 
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int find_kcore						(bool is_subg = false);										

	//CHECK and  refactor
	int find_kcore_UB					(int UB);								//new kcore (vertices with kcore=UB (or nearest real degree > UB) are placed last in ver_)
	
	/*
	* @brief Computes the maximum number of neighbors that come AFTER any vertex in the ordering in @ver_ (first-to-last)
	*		 ( Computes the minimum width of the graph when rev := FALSE  (@ver_ first-to-last). )			
	* 
	*		 I. The width of a graph G is w if the vertices can be arranged such that the maximum number of neighbours of any vertex
	*				PRECEDING that vertex is w.	*
	*		II. A non-increasing kcore ordering (i.e. vertices in the largest kcore first) is a miminum width ordering.
	*		III.The minimum width of a graph  = the maximum kcore of a graph 
	*		IV.Heavy computationally - use only for tests
	* 
	* @param rev if FALSE computes mimimum width, TRUE a width but not minimum
	* @return minimum width or just a width of the graph @g_
	*/
	int minimum_width					(bool rev = false);						//computes width of the graph using ver_ list 
	
	//////////////////////
	//clique heuristics - to refactor
	vint find_heur_clique				(int num_iter = EMPTY_ELEM);			//greedy clique heuristic based on KCore ordering
inline	vint find_heur_clique_opt		(int num_iter = EMPTY_ELEM);			//only available for sparse graphs
vint find_heur_clique_sparse			(int num_iter = EMPTY_ELEM);			//only available for sparse graphs	

////////////////
//Experimental - to refactor 
	int make_kcore_filter				(map_t& filter, bool reverse=true);		//applied to clique, probably remove

///////////////
//I/O

	/*
	* @brief streams k-core information
	* 
	* @param add_real_deg if TRUE prints also the real degree of vertices
	* @param o output stream
	*/
	std::ostream& print_kcore			(bool add_real_deg = false, bool is_subg = false, std::ostream& o = std::cout)	const;

//////////////
// private interface
private:
		
	/*
	* @brief Inits deg_ and bins_ data structures (graph or induced subgraph depending on @subg_)
	* 
	* @param subg: if TRUE, the subgraph induced by the bitset of vertices (@subg_) is considered
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int init_kcore						(bool is_subg = false);

	/*
	* @brief Inits bin_ data structure for graph / subgraph
	*		 (bin[deg] = number of vertices with degree deg)
	*				 
	*      I. @deg_ MUST already be set
	* 
	* @param subg: if TRUE, the subgraph induced by the bitset of vertices (@subg_) is considered
	* @returns 0 if success, -1 if memory allocation fails
	* 
	* @created 13/3/16
	* @last_update 12/01/25
	*/
	int init_bin						(bool is_subg = false);
		
	/*
	* @brief Bin sorts vertices in non-decreasing degree in @ver_ (new-to-old format)
	*		  (init_kcore() must be called first)
	* 
	*		 I. Involves @bin_ @deg_ and @ver_	
	*		II. Updates @pos_ to call later the kcore() algorithm 	
	* 
	* @param subg: if TRUE, the subgraph induced by the bitset of vertices (@subg_) is considered
	*/
	void bin_sort						(bool is_subg = false);
	
	//experimental
	void bin_sort						(vint& lv, bool rev);					//bin sort according to vertex set lv (rev TRUE: vertices taken in reverse order)
	
	//I/O
	std::ostream& print					(print_t = VER, std::ostream& o = std::cout);
							
///////////
// data members
private:

	Graph_t& g_;																//the one and only graph G=(V, E)			
	const int NV_;																//size of graph |V| - for convenience
	_bbt subg_;																	//reference induced subgraph to study coreness 
	
	//data structures
	vint deg_;																	//coreness of vertices																
	vint bin_;																	//bins [deg[v]] for bin sort sorting algorithm
	vint ver_;																	//vertices in non-decreasing kcore order (mapping in new-to-old format)
	vint pos_;																	//position of vertices in ver_ (old-to-new format)
};

//////////////////////////////////////////////////////
// IMPLEMENTATION - in header for generic code

template<class Graph_t>
inline int KCore<Graph_t>::find_kcore(Graph_t& g)
{
	//degenerate minimum degree ordering
	GraphFastRootSort<Graph_t> gfs(g);
	vint degen_order = gfs.new_order(GraphFastRootSort<Graph_t>::MIN_DEGEN, ::com::sort::LAST_TO_FIRST);		//MUST BE LAST-TO-FIRST!
	
	//I. count maximum of the neighbors of each vertex v that precedes v in the ordering
	//II.find the maximum count
	int max_width = 0;
	for (auto j = 1; j < degen_order.size(); ++j) {

		auto width = 0;
		for (auto i = j - 1; i >= 0; --i) {
			if (g.is_edge(degen_order[j], degen_order[i])) {
				++width;
			}
		}

		//update max_width
		if (max_width < width) {
			max_width = width;		
		}
		
		//TO NOTE: no early exit if width decreases in two subseqent vertices - 
		//the values are not guaranteed to increase monotonically and then
		//decrease monotonically after the maxmium core (as seen in tests with brock200_1.clq 13/01/25)

		//ON THE OTHER HAND, the maximum width value is the maximum core number 
	}

	return max_width;
}

template<class Graph_t>
inline KCore<Graph_t>::KCore(Graph_t& g) : g_(g), NV_(g.number_of_vertices()), deg_(NV_), pos_(NV_) {

	try {
		ver_.assign(NV_, EMPTY_ELEM);					
	}
	catch (std::bad_alloc& ba) {
		LOGG_ERROR("bad_alloc exception - KCore<T>::reset_subgraph", ba.what());
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

template<class Graph_t>
inline KCore<Graph_t>::KCore(Graph_t& g, _bbt subg): g_(g), NV_(g.number_of_vertices()), deg_(NV_), pos_(NV_) {

	subg_ = std::move(subg);

	try {
		ver_.assign(subg_.size(), EMPTY_ELEM);
	}
	catch (std::bad_alloc& ba) {
		LOGG_ERROR("bad_alloc exception - KCore<T>::reset_subgraph", ba.what());
		LOG_ERROR("Exiting...");
		std::exit(-1);
	}

}

template<class Graph_t>
inline KCore<Graph_t>::KCore(Graph_t& g, vint set) : g_(g), NV_(g.number_of_vertices()), deg_(NV_), pos_(NV_) {

	//builds a  bitset from a vector of vertices (population size NV_)
	subg_ = _bbt(NV_, set);

	try {
		ver_.assign(subg_.size(), EMPTY_ELEM);		
	}
	catch (std::bad_alloc& ba) {
		LOGG_ERROR("bad_alloc exception - KCore<T>::reset_subgraph", ba.what());
		LOG_ERROR("Exiting...");
		std::exit(-1);
	}

}

template<class Graph_t>
inline int KCore<Graph_t>::reset_subgraph(_bbt subg) {

	subg_ = std::move(subg);

	try {
		ver_.assign (subg_.size(), EMPTY_ELEM);		
	}
	catch (std::bad_alloc& ba) {
		LOGG_ERROR("bad_alloc exception - KCore<T>::reset_subgraph", ba.what());
		return -1;
	}

	return 0;
}



template<class Graph_t>
inline int KCore<Graph_t>::find_kcore(bool is_subg){

	//inits data structures
	if (init_kcore(is_subg) == -1) {
		LOG_ERROR("Error during memory allocation - KCore<Graph_t>::init_kcore");
		return -1;
	}
	bin_sort(is_subg);
	
	auto u = EMPTY_ELEM;
	auto v = EMPTY_ELEM;

	if(!is_subg){

		//kcore computation for the full graph
		for(auto& v : ver_){		

			//iterates over N(v)
			_bbt& neigh = g_.neighbors(v); 
			if (neigh.init_scan(bbo::NON_DESTRUCTIVE) != -1) {			//CHECK MUST BE - for sparse_bitarrays
								
				while ((u = neigh.next_bit()) != BBObject::noBit) {

					if (deg_[u] > deg_[v]) {
						SWAP_BIN(u);			//swap bin movement for v (also sorted in ver_)
						--deg_[u];				//decrease degree of swapped vertex
					}
				}

			}			

		}//vertex iteration

	}else{

		//kcore computation for the induced subgraph by the (bit)set of vertices in subg_
		_bbt neigh(NV_);

		//sorts by degree and computes degeneracy
		for (auto v : ver_) {

			//computes neightbors of v in the subgraph
			AND(g_.neighbors(v), subg_, neigh);
			
			//iterates over the neighbors of v in the subgraph
			if(neigh.init_scan(bbo::NON_DESTRUCTIVE) != -1){			//CHECK MUST BE - for sparse_bitarrays
				
				while ((u = neigh.next_bit()) != BBObject::noBit) {

					if (deg_[u] > deg_[v]) {
						SWAP_BIN(u);			//swap bin movement for v (also sorted in ver_)
						--deg_[u];				//decrease degree of swapped vertex
					}
				}

			}//endif

		}//vertex iteration
	}

	return 0;
}	

template<class Graph_t>
inline int KCore<Graph_t>::find_kcore_UB (int UB_out){
////////////
//  date of creation: 5/3/16
//  last update: 5/3/16
//	
//  Degeneracy ordering assuming a known upper bound for the kcore number (UB_out): 
//  Vertices with degree UB_out (or closest real degree > UB are placed last in ver_)
//  Note that this IS a degeneracy ordering assuming that UB_out is >= kcore(G)
//
//  Compared with kcore, here the way vertices are ordered is:
//  1-Start with vertices with kcore = UB 2-For each kcore set, remove vertices iteratively
//  3-Select any vertex with kcore = UB as a consequence of 2, and remove iteratively
//  4-Goto step 1
//
//  OUTPUT:   A degeneracy ordering in ver_ (reverse order).
//			  Note deg_ does not contain kcore number information now, but deg_[last_vertex in ver_] 
//			  should hold UB_out (possibly corrected, see RETURN VALUE).
//  RETURNS:  corrected UB(nearest UB greater than UB_out that exists)  or -1 is subgraph information exists,
//  
//  RELEVANCE: Would seem to be interesting for enumerating all maximal cliques, according
//			   to my new idea (to be proved). Essentially, this degeneracy ordering is linear
//           	--with width d (the kcore number)-- and places vertices oriented by non-decreasing degree
//				starting with degree UB. Note this is not the case in the O(m) kcore() algorithm which
//				places vertices in non-increasing kcore order.
//
//  DATA STRUCTURES: bin, ver, hash of vertex positions   (same as default kcore)
//
//  COMMENTS: Implemented ONLY for the full graph, not subgraphs

	//inits data structures
	init_kcore(false);
	bin_sort(false);
		
	
	//check that UB_out is not the maximum graph degree
	//i.e. deg = 1 , bin has size 2 (0 and 1)
	if(bin_.size() <= UB_out + 1){																		
		LOG_INFO("UB is not worse than maximum graph degree: vertices left as is - KCore<Graph_t>::kcore_UB");
		return UB_out;						
	}

	//variables / update UB to the nearest existing degree
	int u = BBObject::noBit;
	int v = BBObject::noBit;
	int UB = UB_out;
	int w = ver_[bin_[UB]];
	if(deg_[w] != UB){ UB = deg_[w];}
		
	//main loop
	int deg = UB;
	int p_iter; 
	int p_newUB; 
	bool first_time_newLB = true;
	while(deg >= 1){
		p_iter=bin_[deg];
		do{
			//classical kcore loop for vertices with degree deg
			v = ver_[p_iter];
			first_time_newLB = true;
			p_newUB = -1;

			//loop over neighbors of v with degree greater than UB
			if(g_.neighbors(v).init_scan(bbo::NON_DESTRUCTIVE) != -1){
				while((u = g_.neighbors(v).next_bit()) != BBObject::noBit){
									
					if(deg_[u] > UB){

						/////////////////
						SWAP_BIN(u);	
						/////////////////
						
						//update degree: if vertex u has deg LB+1, instead of updating to LB it takes the degree of v
						if(deg_[u] == UB + 1){
							deg_[u] = deg_[v];
							if(first_time_newLB && deg_[v] != UB){
								first_time_newLB = false;
								p_newUB = pos_[u];		//new position of u
							}
						}else deg_[u]--;
					}
				}
			}

			///////////////////////////////////////////////////////
			//extra loop for new vertices with degree LB caused by v

			if(p_newUB != BBObject::noBit){
				while(p_newUB != bin_[UB + 1]){

					//classical kcore loop for new vertices with degree LB
					v=ver_[p_newUB];

					//loop over neighbors of v with degree greater than UB
					if(g_.neighbors(v).init_scan(bbo::NON_DESTRUCTIVE) != -1){

						while((u = g_.neighbors(v).next_bit()) != BBObject::noBit){
							if(deg_[u]>UB){
								
								/////////////////
								SWAP_BIN(u);										//swaps u to the last position of vertices with one less degree	(does not update degree)
								/////////////////
								
								(deg_[u]==UB+1)? deg_[u]= deg_[v] : deg_[u]--;		//updates degree
							}
						}
					}//endif

					p_newUB++;	//next vertex in UB
				}
			}// end of extra loop 			

			p_iter++;	//next vertex v

		}while(p_iter!=bin_[deg+1]);

		//find next legal deg below current deg
		const int DEG = deg;
		do{
			deg--;
		} while(bin_[DEG] == bin_[deg] && deg > 0);
	}//end of main loop
	

	//new bin sort over ver_ in reverse order for final sorting
	init_bin();
	vint lv(ver_);				//requires a copy
	bin_sort(lv,true);			//reverse order
	
	return UB;
}	

template<class Graph_t>
inline int KCore<Graph_t>::init_kcore(bool is_subg){

	int max_deg = 0, v = EMPTY_ELEM;

	if(!is_subg){									//kcore of the whole graph
		
		//sets degrees and finds maximum degree of G
		for (auto v = 0; v < NV_; ++v){
			deg_[v] = g_.degree(v);
			if (max_deg < deg_[v]) {
				max_deg = deg_[v];
			}
		}

		//allocates bin space	 
		try {
			/////////////////////////////////
			bin_.assign(max_deg + 1, 0);
			/////////////////////////////////
		}
		catch (std::bad_alloc& ba) {
			LOGG_ERROR("bad_alloc exception - KCore<T>::init_kcore", ba.what());
			return -1;
		}

		//sets bins values
		for(auto v = 0; v < NV_; ++v ){
			bin_[deg_[v]] += 1;										//note that isolani should not be in the list
		}

	}else{

		//kcore for the subgraph induced by subg_
		//subg_ cannot be empty, so the assertion MUST hold
		assert(subg_.init_scan(bbo::NON_DESTRUCTIVE) != -1);					
		
		while ((v = subg_.next_bit()) != BBObject::noBit) {

			//sets degree values with endoints in the set
			deg_[v] = g_.degree (v, subg_);
			if (max_deg < deg_[v]) {
				max_deg = deg_[v];
			}
		}

		//allocates bin space for the induced subgraph	
		try {
			/////////////////////////////////
			bin_.assign(max_deg + 1, 0);
			/////////////////////////////////
		}
		catch (std::bad_alloc& ba) {
			LOGG_ERROR("bad_alloc exception - KCore<T>::init_kcore", ba.what());
			return -1;
		}

		//sets bins values for the induced subgraph
		//subg_ cannot be empty, so the assertion MUST hold
		assert(subg_.init_scan(bbo::NON_DESTRUCTIVE)!=-1);
		
		v = BBObject::noBit;
		while( (v = subg_.next_bit()) != BBObject::noBit ){			
			bin_[deg_[v]] += 1;	
		}
	}

	return 0;
}

template<class Graph_t>
inline int KCore<Graph_t>::init_bin(bool is_subg){
	
	//finds maximum degree of G
	int max_deg = EMPTY_ELEM;
	max_deg = *( std::max_element(deg_.begin(), deg_.end()) );

	//allocates bin space	 
	try {
		/////////////////////////////////
		bin_.assign(max_deg + 1, 0);
		/////////////////////////////////
	}
	catch (std::bad_alloc& ba) {
		LOGG_ERROR("bad_alloc exception - KCore<T>::init_kcore", ba.what());
		return -1;
	}

	//sets bins values as required
	int v = EMPTY_ELEM;
	if (!is_subg) {

		//full graph
		for (auto v = 0; v < NV_; ++v) {
			bin_[deg_[v]] += 1;									//note that isolani should not be in the list
		}

	}
	else {

		//induced subgraph
		//subg_ cannot be empty, assertion MUST HOLD
		assert(subg_.init_scan(bbo::NON_DESTRUCTIVE)!=-1);				

		v = BBObject::noBit;
		while ((v = subg_.next_bit()) != BBObject::noBit) {
			bin_[deg_[v]] += 1;
		}

	}

	return 0;
}

template<class Graph_t>
inline void KCore<Graph_t>::bin_sort(bool is_subg){


	//sets bin_ with the position in the new ordering (I): 
	//based on the number of vertices with that degree previously stored with init_bin()
	//Example  bin_ = {3, 4, 2, 1} -> bin_ = {0, 3, 7, 9}

	auto start = 0;
	for (auto& bval : bin_) {
		auto num = bval;
		bval = start;
		start += num;
	}	

	if(!is_subg){

		//bin_sort full graph
		for(auto v = 0; v < ver_.size(); ++v){
			pos_[v] = bin_[deg_[v]];					//stores the new position of v (pos_ is not strictly needed for bin_sort, but it is for kcore)
			ver_[pos_[v]] = v;							//places v in the new position in ver_ (mapping -  new-to-old format)			
			bin_[deg_[v]]++;
		}
	}else{

		//bin_sort subgraph induced by subg_
		//subg_ cannot be empty, assertion MUST hold
		assert(subg_.init_scan(bbo::NON_DESTRUCTIVE)!=-1);			

		auto v = BBObject::noBit;
		while( (v = subg_.next_bit())!= BBObject::noBit){
			pos_[v] = bin_[deg_[v]];			
			ver_[pos_[v]] = v;		
			bin_[deg_[v]]++;
		}
	}

	//restores the bin indexes with the position in the new ordering (I)
	//after fixing: bin[0] = 0 always, bin[1] = 0 if there are no isolani etc.
	//bin_ = {1, 4, 8, 10} -> bin_ = {0, 1, 4, 8}

	for(auto d = bin_.size() - 1; d >= 1 ; --d){
		bin_[d] = bin_[d - 1];
	}
	bin_[0] = 0;

}

template<class Graph_t>
inline void KCore<Graph_t>::bin_sort(vint& lv, bool rev){
////////////////
// sorts vertices in lv by non decreasing degree (deg_) in linear time (EXPERIMENTAL)
// date of creation: 7/3/16
// last update: 13/3/16
// 
// 1-vertex degrees must be previously stored in deg_ (by calling init_kcore(...) first)
// 2-lv contains the vertex order (rev=TRUE for reverse order)
//
// PARAMETERS
//  lv: vertex order for bin sort
//  rev: TRUE for reverse order from lv, FALSE direct order
//
// REMARKS: 
// 1- Current application: kcore_UB->here lv MUST BE the full set of vertices in G (not a subgraph)
// 2- Should work when lv is a subgraph in G (would be equivalent to subgraph bin_sort), but not TESTED

	//init bin_ to point at the correct position in vertex array according to size
	int start = 0;
	int num = 0;
	for(auto d = 0; d < bin_.size(); ++d){		
		num = bin_[d];					
		bin_[d] = start;
		start += num;
	}
		
	if(rev){
		for(vint::reverse_iterator it = lv.rbegin(); it != lv.rend(); ++it){
			int v=*it;
			pos_[v]=bin_[deg_[v]];			
			ver_[pos_[v]]=v;		
			bin_[deg_[v]]++;
		}
	}else{
		for(vint::iterator it = lv.begin(); it != lv.end(); ++it){
			int v=*it;
			pos_[v]=bin_[deg_[v]];			
			ver_[pos_[v]]=v;		
			bin_[deg_[v]]++;
		}
		
	}
		
	//corrects bin indexes after ordering (after update will point to the first element of their type)
	//after correction: bin[0]=0 always, bin[1]=0 if there are no isolani etc.
	for(auto d = bin_.size() - 1; d >= 1; --d){
		bin_[d] = bin_[d-1];
	}
	bin_[0] = 0;
}

template<class Graph_t>
inline int KCore<Graph_t>::minimum_width (bool rev){

	int maxNumNeigh = EMPTY_ELEM; 
	int	numNeigh = EMPTY_ELEM;
	_bbt bb_unsel(NV_); 
	_bbt bb_sel(NV_);

	if(rev){

		for(auto it = ver_.rbegin(); it != ver_.rend(); ++it){

			//computes subgraph population (TODO: optimize)
			bb_unsel = g_.neighbors(*it);
			bb_unsel.erase_bit(bb_sel);
			
			///////////////////////////////
			numNeigh = bb_unsel.size();
			if (maxNumNeigh < numNeigh) { maxNumNeigh = numNeigh; }
			////////////////////////////////

			bb_sel.set_bit(*it);
		}

	}else{

		for(auto it = ver_.begin(); it != ver_.end(); ++it){

			//computes subgraph population (TODO: optimize)
			bb_unsel = g_.neighbors(*it);
			bb_unsel.erase_bit(bb_sel);
			
			///////////////////////////////
			numNeigh = bb_unsel.size();
			if (maxNumNeigh < numNeigh) { maxNumNeigh = numNeigh; }
			///////////////////////////////

			bb_sel.set_bit(*it);
		}
	}

	return maxNumNeigh;
}



template<class Graph_t>
inline int KCore<Graph_t>::max_core_number() const{

	//assert
	if (ver_.empty()) {
		return -1;
	}

	return deg_[ver_.back()];
}

template<class Graph_t>
vint KCore<Graph_t>::core_set (std::size_t k) const {

	vint res;
	
	for(auto v = 0; v < deg_.size(); ++v){
		if (deg_[v] >= k) {
			res.emplace_back(v);
		}
	}
		
	return res;
}

template<class Graph_t>
inline int KCore<Graph_t>::core_size (std::size_t k) const	{

	auto count = 0;
	
	for(auto v = 0; v < deg_.size(); ++v){
		if (deg_[v] == k) {
			count++;
		}
	}
		
	return count;
}

template<class Graph_t>
inline int KCore<Graph_t>::make_kcore_filter (map_t& filter, bool reverse) {
///////////////////////////////
// maps kcore number to the starting vertex of the next kcore partition (filter[kc(v)]->first vertex w,  kc(w)=kc(v)+1)
// Example [v,kc(v)], reverse=TRUE, kcore dec:[1,1][2,1][3,2][4,1] then filter[2]=1 the single element in filter
// Example [v,kc(v)], reverse=FALSE, kcore dec:[1,1][2,1][3,2][4,1] then filter[1]=4 the single element in filter
//
// PARAMS:
// 1-filter: the mapping
// 2-reverse: if TRUE assumes kcore partitions are in increasing order from first to last
//            if FALSE assumes kcore partitions are in decreasing order from first to last
//
// RETURNS: size of filter
// REMARKS: kcore-decomposition must exist (prior call to k-core())
	
	//***assert existence of kcore decomposition
	
	filter.clear();
	int next_kcore_pos = 0;
	int current_k = EMPTY_ELEM;

	if(reverse){
		current_k = deg_[ver_.back()];
		for(auto i = ver_.size() - 2; i >= 0; --i){
			++next_kcore_pos;
			if(deg_[ver_[i]] != current_k){
				//adds to map kcore values and intermediate values
				int diff = current_k - deg_[ver_[i]];
				int ck = current_k;
				
				do{
					filter.insert(pair<int, int>(ck--,next_kcore_pos));
					--diff;
				}while(diff >= 1);

				//filter.insert(pair<int, int>(current_k,next_kcore_pos));
				current_k = deg_[ver_[i]];
			}
		}
	}else{
		
		current_k = deg_[ver_.front()];
		for(auto i = 1; i < ver_.size(); ++i){
			++next_kcore_pos;
			if(deg_[ver_[i]] != current_k){	
				//adds to map kcore values and intermediate values
				int diff = deg_[ver_[i]] - current_k;
				int ck = current_k;
				
				do{
					filter.insert(pair<int, int>(ck--,next_kcore_pos));
					--diff;
				}while(diff >= 1);

				//filter.insert(pair<int, int>(current_k,next_kcore_pos));
				current_k = deg_[ver_[i]];
			}
		}
	}

	return filter.size();
}

template<class Graph_t>
inline vint KCore<Graph_t>::find_heur_clique(int num_iter){
///////////////////////
// A more efficient and clean implementation of the greedy clique heuristic based on Kcore
//
// COMMENTS: If num_iter==EMPTY_ELEM (default value) it will complete all possible iterations
	
	int max_size = 1;
	int iter = 1;
	vint curr_clique;
	vint largest_clique;
	vint neighbors;

	//main loop
	for(auto i = ver_.size() - 1; i >= 0; --i){
		
		//CUT at root level
		int v = ver_[i];
		if(deg_[v] < max_size){
			break;					
		}

		//determines neighbor set in degeneracy order
		neighbors.clear();
		for(int j = i - 1; j >= 0; --j){
			if(deg_[ver_[j]] >= max_size && g_.neighbors(v).is_bit(ver_[j])){
				neighbors.push_back(ver_[j]);									//vertices are placed in neighbor in degeneracy order (I)
			}
		}

		//determines largest greedy clique in neighbor set
		curr_clique.clear();
		for(int n = 0; n < neighbors.size(); ++n){	//vertices selected in degeneracy order (I)							
			bool good_vertex = true;
			for(auto l = 0; l < curr_clique.size(); ++l){
				if( !g_.neighbors(curr_clique[l]).is_bit(neighbors[n]) ){
					good_vertex = false;
					break;
				}
			}
			if (good_vertex) {
				curr_clique.push_back(neighbors[n]);
			}
		}

		//adds initial vertex
		curr_clique.push_back(v);

		//update size
		if(max_size < curr_clique.size()){
			 largest_clique = curr_clique;
			 max_size = curr_clique.size();

			 //I/O
			 LOGG_DEBUG(iter, "." , "lb:" , max_size, " seed:" , ver_[i]);
		}


		//evaluation of number of iterations (exit condition received parameter)
		if(++iter >= num_iter && num_iter != EMPTY_ELEM )	break;
	}

	if (num_iter != EMPTY_ELEM) {
		LOGG_PRINT("kc[" , "it:" , iter , "," , " lb:" , max_size , "]");
	}
	else LOGG_PRINT("kc[lb:" , max_size , "]");

	return largest_clique;
}

template<>
inline vint KCore<sparse_ugraph>::find_heur_clique_opt(int num_iter){
///////////////////////
// Optimized version of find_clique for speed
// date:30/12/2014
//
// COMMENTS: At the moment, only available for sparse_graph type
	
	auto max_size = 1;
	auto iter = 1;
	auto from = EMPTY_ELEM;

	vint curr_clique;
	vint largest_clique;
	sparse_bitarray bbneigh(NV_);
	
	//main loop
	for(auto i = ver_.size() - 1; i > 0; --i){
		
		//CUT at root level
		if(deg_[ver_[i]]<max_size){
			break;					
		}

		//determines neighbor set
		curr_clique.clear();
		bbneigh=g_.neighbors(ver_[i]);
		
		//iterates over all vertices to pick them in degeneracy ordering
		for(auto j = i - 1; j >= 0; --j){

			if (deg_[ver_[j]] < max_size) { break; }
			
			if(bbneigh.is_bit(ver_[j])){				//adjacent
				curr_clique.push_back(ver_[j]);								
				bbneigh &= g_.neighbors(ver_[j]);
			}
		}

		//adds initial vertex
		curr_clique.push_back(ver_[i]);

		//update size
		if(max_size < curr_clique.size()){
			 largest_clique = curr_clique;
			 max_size = largest_clique.size();

			 //I/O
			 LOGG_DEBUG(iter, ".", "lb:", max_size, " seed: [" , i, ":" , ver_[i], "]");
		}

		//evaluation of number of iterations 
		if (++iter >= num_iter && num_iter != EMPTY_ELEM) { break; }
	}

	if (num_iter != EMPTY_ELEM) {
		LOGG_INFO("kc[", "it:", iter, ",", " lb:", max_size, "]");
	}
	else { LOGG_INFO("kc[lb:", max_size, "]"); }

	return largest_clique;
}

struct less_kcore{
	less_kcore(vector<int>& pos, vector<int>& ver,  vector<int>& deg ):pos(pos), ver(ver), deg(deg){}
	bool operator()(int lhs, int rhs) const{
		return(deg[ver[pos[lhs]]]>deg[ver[pos[lhs]]]);
	}
	vector<int>& pos;
	vector<int>& ver;
	vector<int>& deg;
};

struct remove_kcore{

	//constructor
	remove_kcore(vector<int>& pos, vector<int>& ver, vector<int>& deg, int max_clique):
		pos(pos), ver(ver), deg(deg), max_core_number(max_clique)
	{}
	
	//functor operator
	bool operator()(int data) const{
		return(deg[ver[pos[data]]]<max_core_number);
	}

	//data members
	vector<int>& pos;
	vector<int>& ver;
	vector<int>& deg;
	int max_core_number;
};

template<>
inline vint KCore<sparse_ugraph>::find_heur_clique_sparse(int num_iter){
///////////////////////
// Optimized version of find_clique for speed in large saprse graphs
// date:30/12/2014
//
// COMMENTS: (only available for sparse_graph type)
	
	auto max_size = 1;
	auto iter = 1;
	auto from = EMPTY_ELEM;

	vint curr_clique;
	vint largest_clique;
	vint candidates;
	sparse_bitarray bbneigh(NV_);
	
	//main loop
	for(auto i = ver_.size() - 1; i > 0; --i){
		
		//CUT at root level
		if(deg_[ver_[i]] < max_size){ break; }

		//determines neighbor set
		curr_clique.clear();
		bbneigh = g_.neighbors(ver_[i]);
		bbneigh.to_vector(candidates);

		//sort degeneracy
		sort(candidates.begin(), candidates.end(), less_kcore(pos_, ver_, deg_));		

		candidates.erase(remove_if(candidates.begin(), candidates.end(), remove_kcore(pos_, ver_,deg_, max_size)), candidates.end());
		
		
		//iterates over all vertices to pick them in degeneracy ordering
		for(auto n = 0; n < candidates.size(); ++n){
			
			if(bbneigh.is_bit(candidates[n])){				//adjacent
				
				curr_clique.push_back(candidates[n]);	
				bbneigh&=g_.neighbors(candidates[n]);
			}
		}

		//adds initial vertex
		curr_clique.push_back(ver_[i]);

		//update size
		if(max_size < curr_clique.size()){
			 largest_clique = curr_clique;
			 max_size = largest_clique.size();

			 //I/O
			 LOGG_DEBUG(iter, ".", "lb:" , max_size , " seed: [" , i , ":", ver_[i], "]");
		}

		//evaluation of number of iterations 
		if (++iter >= num_iter && num_iter != EMPTY_ELEM) { break; }
	}

	//log
	if (num_iter != EMPTY_ELEM) {
		LOGG_INFO("kc[", "it:", iter, ",", " lb:", max_size, "]");
	}
	else { LOGG_INFO("kc[lb:", max_size, "]"); }
	
	return largest_clique;
}

///////////////////////////
//
// INPUT/OUTPUT
//
///////////////////////

template<class Graph_t>
inline std::ostream& KCore<Graph_t>::print(print_t type, ostream& o){
	switch(type){
	case DEG:
		for (auto i = 0; i < deg_.size(); ++i) {
			o << deg_[i] << " ";
		}
		o << endl;
		break;
	case BIN:
		for (auto i = 0; i < bin_.size(); ++i) {
			o << bin_[i] << " ";
		}
		o<<endl;
		break;
	case VER:
		for (auto i = 0; i < ver_.size(); ++i) {
			o << ver_[i] << " ";
		}
		o << endl;
		break;
	case POS:
		for (auto i = 0; i < pos_.size(); ++i) {
			o << pos_[i] << " ";
		}
		o << endl;
		break;
	default:
		;
	}

	return o;
}

template<class Graph_t>
inline std::ostream& KCore<Graph_t>::print_kcore (bool real_deg, bool subgraph,  ostream& o)	const{

	if(!subgraph){

		//whole graph
		for(auto it = ver_.begin(); it != ver_.end(); ++it){
			o << "[" << *it << ":" << deg_[*it];
			if (real_deg) {
				o << ":" << g_.degree(*it) << "] ";
			}
			else {
				o << "] "; 
			}
		}
	}else{	

		//subgraph
		auto v = BBObject::noBit;	

		//should not be empty - sparse graphs will produce an error
		if (subg_.init_scan(bbo::NON_DESTRUCTIVE) != -1) {

			while ((v = subg_.next_bit()) != BBObject::noBit) {
				o << "[" << v << "," << deg_[v];
				if (real_deg) {
					o << ":" << g_.degree(v) << "] ";
				}
				else {
					o << "] ";
				}
			}
		}
		
	}

	return o;
}


#endif