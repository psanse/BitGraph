/*
 * kcore.h file for the classes Base_Graph_W and Graph_W for weighted graphs
 *
 * @creation_date 6/6/14
 * @last_update 11/01/25
 * @dev pss
 *
 * //TODO - mention reference paper to determine KCore
 */

#ifndef __KCORE_H__
#define __KCORE_H__

#include <vector>
#include <iostream>
#include <algorithm>
#include <map> 
#include "utils/logger.h"
#include "graph/simple_sparse_ugraph.h"

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
//	pos_[w]=pu;	//SWAP_BIN (u->w) with first vertex with same degree
//	ver_[pu]=w;
//	ver_[pw]=u;
//}	
// bin_[du]++;  

//////////////////////////

using namespace std;									//TODO - remove from header file

//useful alias
typedef Ugraph<sparse_bitarray> sparse_ugraph;			//simple sparse undirected graph

typedef map<int,int>			map_t;
typedef map<int,int>::iterator	map_it;

using vint = vector<int>;
using vint_it = vector<int>::iterator;
using vint_cit = vector<int>::const_iterator;

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

	friend ostream& operator<< (std::ostream& o, const KCore& kc){ kc.print_kcore(o); return o;}
	
	enum print_t { DEG, BIN, VER, POS };

public:
	using basic_type = Graph_t;						//a graph from GRAPH, typically sparse_ugraph
	using type = KCore;

	//alias for backward compatibility
	using _bbt = typename basic_type::_bbt;		//bitset type
	using _gt = basic_type;						//graph type

///////////////
//construction
public:

	KCore	(Graph_t& g, _bbt* bbset = nullptr);
	
	//TODO - destructor, copies, moves...
	 
////////////////
//setters and getters

	/*
	* @brief Maximum core number of the graph
	*		 (must be called afer kcore())
	* @returns Maximum core number of the graph or -1 if error
	*/
	int get_max_kcore					()								const;																	
	
	/*
	* @brief Core number of a given vertex
	*		 (must be called afer kcore())
	*/
	int coreness						(std::size_t v)					const { return deg_[v]; }
	
	/*
	* @brief Size of tke kcore for a given k >=0, number of vertices with core number k
	*		 (must be called afer kcore())
	*/
	int get_kcore_size					(std::size_t k)					const;
	
	/*
	* @brief Set of vertices with kcore number equal or greater than a given k >=0, the k-core set of vertices
	*		 (must be called afer kcore()).
	* 
	*		 I. K=0: all vertices in V
	*		II. K=1: vertices in the 1-core etc...
	*/
	vint get_kcore_set					(std::size_t k)					const;

	/*
	* @brief Coreness of all vertices
	*		 (must be called afer kcore()).
	*/
	const vint& get_kcore_numbers		()								const	{ return deg_;}
		
	/*
	* @brief Sorting of the vertices according to non-decreasing kcore number
	*		 (must be called afer kcore()).
	*/
	const vint& get_kcore_ordering		()								const	{ return ver_;}
	
	/*
	* @brief Extension to operate on induced subgraphs. If psg = nullptr kcore is 
	*		 computed on the full graph.
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int set_subgraph					(_bbt * psg = nullptr);

//////////////
// Main operations

/*
	* @brief Computes maximum core and coreness (core algorithm). 
	*		 After its execution: 
	*	
	*		 I. @ver_ contains the vertices in non-decreasing kcore order (new-to-old format)
	*		II. @deg_ contains the coreness of each vertex
	*		III.The maximum core number of the graph is @deg_[last_vertex]
	*/
	void find_kcore						();										


	int find_kcore_UB					(int UB);								//new kcore (vertices with kcore=UB (or nearest real degree > UB) are placed last in ver_)
	
	/*
	* @brief width(v) = maximum number of neighbours of v that come AFTER v given the ordering in @vder_ 
	*		 Computes the maximum width value of the vertices in @vder_ 
	* 
	*		 I. The real width is provided by rev = TRUE (related to vertices that PRECEDE v)
	*		 II.Heavy computationally - use only for tests
	* 
	* @param rev TRUE: computes width in reverse order (last-to-first) 
	* @return maximum width of the graph / subgraph
	*/
	int minimum_width							(bool rev = false);						//computes width of the graph using ver_ list 
	
	/*
	* @brief Width of the graph / subgraph as the degree of the last vertex 
	*		 in the non-decreasing kcore ordering in @ver_ (CHECK)
	* 
	* @return  width of the graph / subgraph		
	*/
	int minimum_width_fast						();						
	
	//clique heuristics
	vint find_heur_clique				(int num_iter = EMPTY_ELEM);			//greedy clique heuristic based on KCore ordering
inline	vint find_heur_clique_opt		(int num_iter=EMPTY_ELEM);				//only available for sparse graphs
vint find_heur_clique_sparse			(int num_iter = EMPTY_ELEM);			//only available for sparse graphs	
	
	//experimental 
	int make_kcore_filter				(map_t& filter, bool reverse=true);		//applied to clique, probably remove

	//I/O
	void print_kcore					(bool real_deg=false, std::ostream& o = std::cout)		const;

//////////////
// private interface
private:

	//k-core init steps
	void init_kcore						();										//inits degrees and bins
	void init_bin						();										//inits just the bin
	void bin_sort						();										//default bin sort
	void bin_sort						(vint& lv, bool rev);					//bin sort according to vertex set lv (rev TRUE: vertices taken in reverse order)
	
	//I/O
	void print							(print_t = VER, std::ostream& o = std::cout);
							
///////////
// data members
private:

	Graph_t& g_;																		//the one and only graph G=(V, E)			
	const int NV_;																//size of graph |V| - for convenience, possibly remove
	_bbt* psg_;																	//to manage kcore in subgraphs (default nullptr)	
	
	//algo data structures
	vint deg_;																	//coreness of vertices																
	vint bin_;																	//bins of differente degrees, implements bin sort algorithm
	vint ver_;																	//vertices in non-decreasing kcore order (new to old format)
	vint pos_;
};

//////////////////////////////////////////////////////
// IMPLEMENTATION - in header for generic code

template<class Graph_t>
KCore<Graph_t>::KCore(Graph_t& g, _bbt* bbset): g_(g), NV_(g.number_of_vertices()), deg_(NV_), pos_(NV_){
	//*** check empty bbset
	//*** check id bbset may hold NV_/64 bitblocks
	set_subgraph(bbset);
}

template<class Graph_t>
int KCore<Graph_t>::set_subgraph(_bbt* psg) {

	psg_ = psg;

	try {
		if (psg_ != nullptr) {
			ver_.assign (psg_ -> popcn64(), EMPTY_ELEM);		//nullptr - operates on the subgraph induced by the bitset of vertices (*psg)
		}
		else {
			ver_.assign (NV_, EMPTY_ELEM);						//nullptr - operates on the full graph
		}
	}
	catch (std::bad_alloc& ba) {
		LOGG_ERROR("bad_alloc exception - KCore<T>::set_subgraph", ba.what());
		return -1;
	}
}

template<class Graph_t>
void KCore<Graph_t>::find_kcore(){

	//inits data structures
	init_kcore();
	bin_sort();
	
	auto u = EMPTY_ELEM;
	auto v = EMPTY_ELEM;

	if(psg_ == nullptr){

		//kcore computation for the full graph
		for(auto v : ver_){		

			//iterates over N(v)
			_bbt& neigh = g_.get_neighbors(v); 
			if (neigh.init_scan(bbo::NON_DESTRUCTIVE) != EMPTY_ELEM) {			//CHECK MUST BE - for sparse_bitarrays
								
				while ((u = neigh.next_bit()) != EMPTY_ELEM) {

					if (deg_[u] > deg_[v]) {
						SWAP_BIN(u);			//swap bin movement for v (also sorted in ver_)
						--deg_[u];				//decrease degree of swapped vertex
					}
				}

			}			

		}//vertex iteration

	}else{

		//kcore computation for the induced subgraph by the (bit)set of vertices in psg_
		_bbt neigh(NV_);

		//sorts by degree and computes degeneracy
		for (auto v : ver_) {

			//computes neightbors of v in the subgraph
			AND(g_.get_neighbors(v), *psg_, neigh);
			
			//iterates over the neighbors of v in the subgraph
			if(neigh.init_scan(bbo::NON_DESTRUCTIVE) != EMPTY_ELEM){			//CHECK MUST BE - for sparse_bitarrays
				
				while ((u = neigh.next_bit()) != EMPTY_ELEM) {

					if (deg_[u] > deg_[v]) {
						SWAP_BIN(u);			//swap bin movement for v (also sorted in ver_)
						--deg_[u];				//decrease degree of swapped vertex
					}
				}

			}//endif

		}//vertex iteration
	}
}	

template<class Graph_t>
int KCore<Graph_t>::find_kcore_UB (int UB_out){
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
	init_kcore();
	bin_sort();

	//Check subgraph is empty
	if(psg_ != nullptr){
		LOG_INFO("Subgraph variant not implemented yet - KCore<Graph_t>::kcore_UB");
		return -1;
	}
	
	//check that UB_out is not the maximum graph degree
	//i.e. deg = 1 , bin has size 2 (0 and 1)
	if(bin_.size() <= UB_out + 1){																		
		LOG_INFO("UB is not worse than maximum graph degree: vertices left as is - KCore<Graph_t>::kcore_UB");
		return UB_out;						
	}

	//variables / update UB to the nearest existing degree
	auto u = EMPTY_ELEM, v = EMPTY_ELEM;
	auto UB = UB_out;
	int w = ver_[bin_[UB]];
	if(deg_[w] != UB){ UB = deg_[w];}
		
	//main loop
	int deg = UB; int p_iter; int p_newUB; 
	bool first_time_newLB = true;
	while(deg >= 1){
		p_iter=bin_[deg];
		do{
			//classical kcore loop for vertices with degree deg
			v=ver_[p_iter];
			/*cout<<"current vertex v: "<<v<<endl;*/
			first_time_newLB=true;
			p_newUB=EMPTY_ELEM;

			//loop over neighbors of v with degree greater than UB
			if(g_.get_neighbors(v).init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
				while(1){
					u=g_.get_neighbors(v).next_bit();
					if(u==EMPTY_ELEM) break;

					int dv=deg_[v];
					if(deg_[u]>UB){
						SWAP_BIN(u);			
						//update degree: if vertex u has deg LB+1, instead of updating to LB it takes the degree of v
						if(deg_[u]==UB+1){
							deg_[u]=dv;	
							if(first_time_newLB && dv!=UB){
								first_time_newLB=false;
								p_newUB=pos_[u];		//new position of u
							}
						}else deg_[u]--;
					}
				}
			}//endif

			///////////////////////////////////////////////////////
			//extra loop for new vertices with degree LB caused by v

			if(p_newUB!=EMPTY_ELEM){
				while(p_newUB!=bin_[UB+1]){

					//classical kcore loop for new vertices with degree LB
					v=ver_[p_newUB];

					//loop over neighbors of v with degree greater than UB
					if(g_.get_neighbors(v).init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
						while(1){
							u=g_.get_neighbors(v).next_bit();
							if(u==EMPTY_ELEM) break;

							int dv=deg_[v];
							if(deg_[u]>UB){
								SWAP_BIN(u);			//swaps u to the last position of vertices with one less degree	(does not update degree)
								//update degree
								(deg_[u]==UB+1)? deg_[u]=dv : deg_[u]--;
							}
						}
					}//endif

					p_newUB++;	//next vertex in UB
				}//next vertex with degree UB
			}//end if filter condition

			// end of extra loop
			//////////////////////////////////////////////////

			p_iter++;	//next vertex v
		}while(p_iter!=bin_[deg+1]);

		//find next legal deg below current deg
		const int DEG=deg;
		do{deg--;}
		while(bin_[DEG]==bin_[deg] && deg>0);
	}//end of main loop
	

	//new bin sort over ver_ in reverse order for final sorting
	init_bin();
	vint lv(ver_);				//requires a copy
	bin_sort(lv,true);			//reverse order
	
	return UB;
}	

template<class Graph_t>
void KCore<Graph_t>::init_kcore(){

	int max_gdeg=0, v=EMPTY_ELEM;

	if(psg_==nullptr){
		//kcore of the whole graph
		for(int v=0; v<NV_; v++){
			deg_[v]=g_.degree(v);
			if(max_gdeg<deg_[v])
					max_gdeg=deg_[v];
		}

		bin_.assign(max_gdeg+1, 0);
		for(int v=0; v<NV_; v++){
			bin_[deg_[v]]+=1;					//note that isolani should not be in the list
		}

	}else{
		//kcore of bbset of vertices
		psg_->init_scan(bbo::NON_DESTRUCTIVE);	//bbset cannot be empty, no empty check condition
		while(true){
			v=psg_->next_bit();
			if(v==EMPTY_ELEM) break;

			//compute degree in the set
			deg_[v]=g_.degree(v, *psg_);
			if(max_gdeg<deg_[v])
				max_gdeg=deg_[v];
		}

		bin_.assign(max_gdeg+1, 0);
		psg_->init_scan(bbo::NON_DESTRUCTIVE);
		v=EMPTY_ELEM;
		while(true){
			v=psg_->next_bit();
			if(v==EMPTY_ELEM) break;

			bin_[deg_[v]]+=1;	
		}
	}
}

template<class Graph_t>
void KCore<Graph_t>::init_bin(){
//////////////////
// Initialization of bin only (compared with init_kcore())
// Assumes degrees in deg_ are set appropiately
//
// last_update: 13/3/16
//
// REMARKS: Only for the full graph at present
//
// APPLICATIONS: 1-kcore_UB

	int max_gdeg=0, v=EMPTY_ELEM;

	if(psg_==nullptr){
		//kcore of the whole graph
		for(int v=0; v<NV_; v++){
			//deg_[v]=g_.degree(v);			//degrees already computed
			if(max_gdeg<deg_[v])
					max_gdeg=deg_[v];
		}

		bin_.assign(max_gdeg+1, 0);
		for(int v=0; v<NV_; v++){
			bin_[deg_[v]]+=1;					//note that isolani should not be in the list
		}

	}else{
		LOG_ERROR("Kcore::init_kcore_UB() - not valid for subgrahs");
	}
}

template<class Graph_t>
void KCore<Graph_t>::bin_sort(){
////////////////
// sorts vertices in bbset by non decreasing degree in linear time
//
// REMARKS: init_k_core has to be called first

	//init bin_ to point at the correct position in vertex array according to size
	int start=0, num=0;
	for(int d=0; d<bin_.size(); d++){		
		num=bin_[d];					
		bin_[d]=start;
		start+=num;
	}

	if(psg_==nullptr){
		//bin_sort of all the graph
		for(int v=0; v<ver_.size(); v++){
			pos_[v]=bin_[deg_[v]];			//stores the new position of v (pos_ is not strictly needed for bin_sort, but it is for kcore)
			ver_[pos_[v]]=v;		
			bin_[deg_[v]]++;
		}
	}else{
		//bin_sort of subgraph
		psg_->init_scan(bbo::NON_DESTRUCTIVE);
		int v=EMPTY_ELEM;
		while(true){
			v=psg_->next_bit();
			if(v==EMPTY_ELEM) break;

			pos_[v]=bin_[deg_[v]];			
			ver_[pos_[v]]=v;		
			bin_[deg_[v]]++;
		}
	}

	//corrects bin indexes after ordering (after update will point to the first element of their type)
	//after correction: bin[0]=0 always, bin[1]=0 if there are no isolani etc.
	for(int d=bin_.size()-1; d>=1; d--){
		bin_[d]=bin_[d-1];
	}
	bin_[0]=0;
}

template<class Graph_t>
void KCore<Graph_t>::bin_sort(vint& lv, bool rev){
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
	int start=0, num=0;
	for(int d=0; d<bin_.size(); d++){		
		num=bin_[d];					
		bin_[d]=start;
		start+=num;
	}
		
	if(rev){
		for(vint::reverse_iterator it=lv.rbegin(); it!=lv.rend(); it++){
			int v=*it;
			pos_[v]=bin_[deg_[v]];			
			ver_[pos_[v]]=v;		
			bin_[deg_[v]]++;
		}
	}else{
		for(vint::iterator it=lv.begin(); it!=lv.end(); it++){
			int v=*it;
			pos_[v]=bin_[deg_[v]];			
			ver_[pos_[v]]=v;		
			bin_[deg_[v]]++;
		}
		
	}
		
	//corrects bin indexes after ordering (after update will point to the first element of their type)
	//after correction: bin[0]=0 always, bin[1]=0 if there are no isolani etc.
	for(int d=bin_.size()-1; d>=1; d--){
		bin_[d]=bin_[d-1];
	}
	bin_[0]=0;
}

template<class Graph_t>
int KCore<Graph_t>::minimum_width (bool rev){
///////////////////
// Width of the ordering in ver_ using real degrees (rev=TRUE reverse direction)
// HEAVY COMPUTATIONALLY - use only for checking purposes	

	int max_width = EMPTY_ELEM;
	int	width = EMPTY_ELEM;
	_bbt bb_unsel(NV_); 
	_bbt bb_sel(NV_);

	if(rev){

		for(auto it = ver_.rbegin(); it != ver_.rend(); ++it){

			//computes subgraph population (TODO: optimize)
			bb_unsel = g_.get_neighbors(*it);
			bb_unsel.erase_bit(bb_sel);
			
			///////////////////////////////
			width = bb_unsel.popcn64();
			if (max_width < width) { max_width = width; }
			////////////////////////////////

			bb_sel.set_bit(*it);
		}

	}else{

		for(auto it = ver_.begin(); it != ver_.end(); ++it){

			///computes subgraph population (TODO: optimize)
			bb_unsel = g_.get_neighbors(*it);
			bb_unsel.erase_bit(bb_sel);
			
			///////////////////////////////
			width = bb_unsel.popcn64();
			if (max_width < width) { max_width = width; }
			///////////////////////////////

			bb_sel.set_bit(*it);
		}
	}

	return max_width;
}

template<class Graph_t>
int KCore<Graph_t>::minimum_width_fast() {
	
	//assert DEBUG
	if (lv.empty()) {
		LOG_ERROR("degree information not available - KCore<Graph_t>::width_fast");
		return -1;
	}
	
	auto v = ver_.back();
	auto count = 0;

	//counts the number of neighbours of v in vdeg_
	for (auto it = ver_.rbegin() + 1; it != ver_(); ++it) {
		if (g_.is_edge(*it, v)) {
			count++;						
		}
	}

	return count;	
}


template<class Graph_t>
int KCore<Graph_t>::get_max_kcore() const{

	//assert
	if (ver_.empty()) {
		return -1;
	}

	return deg_[ver_.back()];
}

template<class Graph_t>
vint KCore<Graph_t>::get_kcore_set (std::size_t k) const {

	vint res;
	
	for(auto v = 0; v < deg_.size(); ++v){
		if (deg_[v] >= k) {
			res.emplace_back(v);
		}
	}
		
	return res;
}

template<class Graph_t>
int KCore<Graph_t>::get_kcore_size (std::size_t k) const	{

	auto count = 0;
	
	for(auto v = 0; v < deg_.size(); ++v){
		if (deg_[v] == k) {
			count++;
		}
	}
		
	return count;
}

template<class Graph_t>
int KCore<Graph_t>::make_kcore_filter (map_t& filter, bool reverse) {
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
	int next_kcore_pos=0, current_k=EMPTY_ELEM;

	if(reverse){
		current_k=deg_[ver_.back()];
		for(int i=ver_.size()-2; i>=0; --i){
			++next_kcore_pos;
			if(deg_[ver_[i]]!=current_k){
				//adds to map kcore values and intermediate values
				int diff=current_k-deg_[ver_[i]];
				int ck=current_k;
				
				do{
					filter.insert(pair<int, int>(ck--,next_kcore_pos));
					diff --;
				}while(diff>=1);

				//filter.insert(pair<int, int>(current_k,next_kcore_pos));
				current_k=deg_[ver_[i]];
			}
		}
	}else{
		
		current_k=deg_[ver_.front()];
		for(int i=1; i<ver_.size(); ++i){
			++next_kcore_pos;
			if(deg_[ver_[i]]!=current_k){	
				//adds to map kcore values and intermediate values
				int diff=deg_[ver_[i]]-current_k;
				int ck=current_k;
				
				do{
					filter.insert(pair<int, int>(ck--,next_kcore_pos));
					diff --;
				}while(diff>=1);

				//filter.insert(pair<int, int>(current_k,next_kcore_pos));
				current_k=deg_[ver_[i]];
			}
		}
	}

return filter.size();
}

template<class Graph_t>
vint KCore<Graph_t>::find_heur_clique(int num_iter){
///////////////////////
// A more efficient and clean implementation of the greedy clique heuristic based on Kcore
//
// COMMENTS: If num_iter==EMPTY_ELEM (default value) it will complete all possible iterations
	
	int max_size=1, iter=1; 
	vint curr_clique, largest_clique, neighbors;
	//main loop
	for(int i= ver_.size()-1; i>=0; i--){
		
		//CUT at root level
		int v=ver_[i];
		if(deg_[v]<max_size){
			break;					
		}

		//determines neighbor set in degeneracy order
		neighbors.clear();
		for(int j=i-1; j>=0; j--){
			if(deg_[ver_[j]]>=max_size && g_.get_neighbors(v).is_bit(ver_[j])){
				neighbors.push_back(ver_[j]);									//vertices are placed in neighbor in degeneracy order (I)
			}
		}

		//determines largest greedy clique in neighbor set
		curr_clique.clear();
		for(int n=0; n<neighbors.size(); n++){	//vertices selected in degeneracy order (I)							
			bool good_vertex=true;
			for(int l=0; l<curr_clique.size(); l++){
				if( !g_.get_neighbors(curr_clique[l]).is_bit(neighbors[n]) ){
					good_vertex=false;
					break;
				}
			}
			if(good_vertex)
					curr_clique.push_back(neighbors[n]);
		}

		//adds initial vertex
		curr_clique.push_back(v);

		//update size
		if(max_size<curr_clique.size()){
			 largest_clique=curr_clique;
			 max_size=curr_clique.size();

			 //I/O
			 LOGG_DEBUG(iter, "." , "lb:" , max_size, " seed:" , ver_[i]);
		}


		//evaluation of number of iterations (exit condition received parameter)
		if(++iter>=num_iter && num_iter!=EMPTY_ELEM )	break;
	}

	if(num_iter!=EMPTY_ELEM)
		LOG_PRINT("kc["<<"it:"<<iter<<","<<" lb:"<<max_size<<"]");
	else LOG_PRINT("kc[lb:"<<max_size<<"]");

	return largest_clique;
}

template<>
inline
vint KCore<sparse_ugraph>::find_heur_clique_opt(int num_iter){
///////////////////////
// Optimized version of find_clique for speed
// date:30/12/2014
//
// COMMENTS: At the moment, only available for sparse_graph type
	
	int max_size=1, iter=1, from=EMPTY_ELEM; 
	vint curr_clique, largest_clique;
	sparse_bitarray bbneigh(NV_);
	
	//main loop
	for(int i=ver_.size()-1; i>0; i--){
		
		//CUT at root level
		if(deg_[ver_[i]]<max_size){
			break;					
		}

		//determines neighbor set
		curr_clique.clear();
		bbneigh=g_.get_neighbors(ver_[i]);
		
		//iterates over all vertices to pick them in degeneracy ordering
		for(int j=i-1; j>=0; j--){
			if(deg_[ver_[j]]<max_size) break;
			if(bbneigh.is_bit(ver_[j])){				//adjacent
				curr_clique.push_back(ver_[j]);	
							
				bbneigh&=g_.get_neighbors(ver_[j]);
			}
		}

		//adds initial vertex
		curr_clique.push_back(ver_[i]);

		//update size
		if(max_size<curr_clique.size()){
			 largest_clique=curr_clique;
			 max_size=largest_clique.size();

			 //I/O
			 LOGG_DEBUG(iter, ".", "lb:", max_size, " seed: [" , i, ":" , ver_[i], "]");
		}

		//evaluation of number of iterations 
		if( ++iter>=num_iter && num_iter!=EMPTY_ELEM)	break;
	}

	if(num_iter!=EMPTY_ELEM)
		LOG_INFO("kc[","it:", iter, "," , " lb:", max_size, "]");
	else LOG_INFO("kc[lb:", max_size, "]");

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
	remove_kcore(vector<int>& pos, vector<int>& ver, vector<int>& deg, int max_clique):pos(pos), ver(ver), deg(deg), max_kcore(max_clique){}
	bool operator()(int data) const{
		return(deg[ver[pos[data]]]<max_kcore);
	}
	vector<int>& pos;
	vector<int>& ver;
	vector<int>& deg;
	int max_kcore;
};

template<>
inline
vint KCore<sparse_ugraph>::find_heur_clique_sparse(int num_iter){
///////////////////////
// Optimized version of find_clique for speed in large saprse graphs
// date:30/12/2014
//
// COMMENTS: (only available for sparse_graph type)
	
	int max_size=1, iter=1, from=EMPTY_ELEM; 
	vint curr_clique, largest_clique, candidates;
	sparse_bitarray bbneigh(NV_);
	
	//main loop
	for(int i=ver_.size()-1; i>0; i--){
		
		//CUT at root level
		if(deg_[ver_[i]]<max_size){
			break;					
		}

		//determines neighbor set
		curr_clique.clear();
		bbneigh=g_.get_neighbors(ver_[i]);
		bbneigh.to_vector(candidates);
		sort(candidates.begin(), candidates.end(), less_kcore(pos_, ver_, deg_));		//sort degeneracy
		candidates.erase(remove_if(candidates.begin(), candidates.end(),remove_kcore(pos_, ver_,deg_, max_size)), candidates.end());
		
		
		//iterates over all vertices to pick them in degeneracy ordering
		for(int n=0; n<candidates.size(); n++){
			if(bbneigh.is_bit(candidates[n])){				//adjacent
				curr_clique.push_back(candidates[n]);	
							
				bbneigh&=g_.get_neighbors(candidates[n]);
			}
		}

		//adds initial vertex
		curr_clique.push_back(ver_[i]);

		//update size
		if(max_size<curr_clique.size()){
			 largest_clique=curr_clique;
			 max_size=largest_clique.size();

			 //I/O
			 LOGG_DEBUG(iter, ".", "lb:" , max_size , " seed: [" , i , ":", ver_[i], "]");
		}

		//evaluation of number of iterations 
		if( ++iter>=num_iter && num_iter!=EMPTY_ELEM)	break;
	}

	if(num_iter!=EMPTY_ELEM)
		LOG_INFO("kc[", "it:", iter, "," , " lb:" , max_size , "]");
	else LOG_INFO("kc[lb:" , max_size, "]");
	return largest_clique;
}

///////////////////////////
//
// INPUT/OUTPUT
//
///////////////////////


template<class Graph_t>
void KCore<Graph_t>::print(print_t type, ostream& o){
	switch(type){
	case DEG:
		for(int i=0; i<deg_.size();i++)
			o<<deg_[i]<<" ";
		o<<endl;
		break;
	case BIN:
		for(int i=0; i<bin_.size();i++)
			o<<bin_[i]<<" ";
	    o<<endl;
		break;
	case VER:
		for(int i=0; i<ver_.size();i++)
			o<<ver_[i]<<" ";
		o<<endl;
		break;
	case POS:
		for(int i=0; i<pos_.size();i++)
			o<<pos_[i]<<" ";
		o<<endl;
		break;
	default:
		;
	}
}

template<class Graph_t>
void KCore<Graph_t>::print_kcore(bool real_deg, ostream& o)	const{
	if(psg_==nullptr){	//whole graph
		for(vint::const_iterator it=ver_.begin(); it!=ver_.end(); it++){
			cout<<"["<<*it<<":"<<deg_[*it];
			if(real_deg){
				cout<<":"<<g_.degree(*it)<<"] ";
			}else  cout<<"] ";
		}

		/*for(vint_cit it=deg_.begin(); it!=deg_.end(); ++it){
			o<<"["<<it-deg_.begin()<<","<<*it<<"] ";
		}*/
	}else{				//subgraph
		int v=EMPTY_ELEM;
		if(m_subg->init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
			while(true){
				v=m_subg->next_bit();
				if(v==EMPTY_ELEM) break;
				o<<"["<<v<<","<<deg_[v];
				if(real_deg){
					cout<<":"<<g_.degree(v)<<"] ";
				}else  cout<<"] ";
			}
		}
	}
}


#endif