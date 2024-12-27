/*
* graph_fast_sort.h: header for GraphFastRootSort class which sorts graphs by different criteria
* @created 12/03/15
* @tag1 changed nodes_ stack to vector (18/03/19)
* @last_update 19/12/24
* @dev pss
* 
* @TODO CHECK how the static order affects the degen order
*/

#ifndef __GRAPH_FAST_SORT_H__
#define __GRAPH_FAST_SORT_H__

#include <algorithm>
#include <iostream>
#include <vector>
#include "utils/logger.h"
#include "utils/common.h"						//sort functors
#include "decode.h"
#include "bitscan/bbtypes.h"					//for EMPTY_ELEM constant	
//#include "filter_graph_sort_type.h"			//limits template Graph_t to undirected types

using namespace std;
using vint = std::vector<int>;

///////////////////////////
//
// GraphFastRootSort class
// (Graph_t restricted to ugraph and sparse_ugraph)
//
////////////////////////////

template <class Graph_t>
class GraphFastRootSort{
	
public:
	using basic_type = Graph_t;											//graph type
	using type = typename GraphFastRootSort< Graph_t>;					//own type		
	using bb_type = typename Graph_t::_bbt;								//bitboard type

	enum class sort_print_t	{PRINT_DEGREE=0, PRINT_SUPPORT, PRINT_NODES};
	enum class sort_alg_t	{MIN_DEGEN = 0, MAX_DEGEN, MIN_DEGEN_COMPO, MAX_DEGEN_COMPO, MAX, MIN, MAX_WITH_SUPPORT, MIN_WITH_SUPPORT, NONE };
	enum class sort_place_t	{FIRST_TO_LAST = 0, LAST_TO_FIRST=1 };
	enum class sort_type_t	{NEW_TO_OLD = 0, OLD_TO_NEW = 1 };
	
	////////////////////////
	//static methods / utilities

	/*
	* @brief Computes the degree of the vertices of a graph 
	* @param g input graph G=(V, E)
	* @param deg output vector of size |V| (v[i] = deg(vi))
	*/
	static int compute_deg(const Graph_t& g, vint& deg);		
	
	//sorting subgraphs (experimental)
	static int SORT_SUBGRAPH_NON_INC_DEG(Graph_t& g, vint& lhs, vint& rhs, bool ftl= true);				
	static int SORT_SUBGRAPH_NON_DEC_DEG(Graph_t& g, vint& lhs, vint& rhs, bool ltf = true);			
	

	///////////////
	// drivers - the real public interface
	///////////////
	/*
	* @brief Computes a new ordering 
	* @param alg sorting algorithm
	* @param ltf last to first ordering if TRUE
	* @param o2n old to new ordering	if TRUE
	* @return new ordering in [OLD]->[NEW] format
	*/
	virtual vint new_order(int alg, bool ltf = true, bool o2n = true);

	/*
	* @brief Creates an isomorphism for a given ordering
	* @param gn output isomorphic graph
	* @param new_order given ordering in [OLD]->[NEW] format
	* @param d ptr to decode object to store the ordering
	* @comments only for simple undirected graphs with no weights
	* @return 0 if successful
	*/
	int reorder(const vint& new_order, Graph_t& gn, Decode* d = nullptr);

	////////////////////////
	//construction / allocation
	explicit GraphFastRootSort(Graph_t& gout) :
		g_(gout),  
		NV_(g_.number_of_vertices()),
		nb_neigh_(NULL), deg_neigh_(NULL) 
	{
		nb_neigh_.assign(NV_, 0);
		deg_neigh_.assign(NV_, 0);
		node_active_state_.init(NV_); 
	}
	~GraphFastRootSort() = default;
	GraphFastRootSort(const GraphFastRootSort&) = delete;
	GraphFastRootSort& operator=(const GraphFastRootSort&) = delete;
	GraphFastRootSort(GraphFastRootSort&&) = delete;
	GraphFastRootSort& operator=(GraphFastRootSort&&) = delete;

	const vint& get_degree() const { return nb_neigh_; }
	const vint& get_support() const { return deg_neigh_; }
	const Graph_t& get_graph() const { return g_; }
	std::size_t number_of_vertices() const { return NV_; }

	/////////////////////////
	// useful operations

	/*
	* @brief Sets trivial ordering [1..NV] in @nodes_,
	*		 a starting point for all sorting primitives	
	*		
	*/
	void set_ordering();	

	/*
	* @brief Sets an ordering in [OLD]->[NEW] format in @nodes_.
	*		 This will be the given ordering in composite orderings
	*/
	void set_ordering(vint& nodes) { nodes_ = nodes; }

	/*
	* @brief Computes the degree of each vertex
	*/ 
	const vint& compute_deg_root();											

	/*
	* @brief Computes support for all vertices (sum of the number of neighbors)
	* @comments May include the same vertex twice	 
	*/
	const vint& compute_support_root();									
		
	
	/*
	* @brief Computes a degree non-degenerate ordering in @nodes_, format [NEW]->[OLD]
	* @param rev reverse ordering if TRUE
	* @comment1 requires prior computation of support and deg
	* @comment2	initially sets @nodes_ to 1...NV
	* @return New ordering in [OLD]->[NEW] format
	*/
	const vint&  sort_non_increasing_deg(bool rev=false);						
	const vint&  sort_non_decreasing_deg(bool rev=false);
	const vint&  sort_non_increasing_deg_with_support_tb(bool rev=false);
	const vint&  sort_non_decreasing_deg_with_support_tb(bool rev=false);
		
	/*
	* @brief Degenerate degree ordering
	* @comments deg info is not restored after the call
	* @return New ordering in [OLD]->[NEW] format
	*/
	const vint&  sort_degen_non_decreasing_degree(bool rev=false);				
	const vint&  sort_degen_non_increasing_degree(bool rev=false);				
	
	/*
	*@brief Composite (min) degenerate ordering on a prior given ordering in @nodes_ 
	*@param rev reverse ordering if TRUE
	*@comments the vertex ordering has to be in @nodes prior to the call 
	*@TODO - create unique function with bool param 
	*@return New ordering in [OLD]->[NEW] format
	*/
	const vint& sort_degen_composite_non_decreasing_degree( bool rev = false);		
	const vint& sort_degen_composite_non_increasing_degree( bool rev = false);	
	
	/////////////////
	//TODO - substitute original primitives for n=0. n can be positive or negative (09/12/2020)
	// int sort_non_increasing_deg (vint& rhs, vint& lhs,  bool rev = false);
		  
    //int  sort_non_increasing_deg(int n, bool rev = false);
	//int  sort_non_decreasing_deg(int n, bool rev = false);
	//int  sort_non_increasing_deg_with_support_tb(int n, bool rev = false);
	//int  sort_non_decreasing_deg_with_support_tb(int n, bool rev = false);

	////////////////////////
	// I/O
	ostream& print(int type, ostream& o) const;

///////////////////////
// internal operations
protected:

	/*
	* @brief Restores context for NV_ vertices
	*/
	int reset();
	
		
/////////////////////////////////////////////
// data members	
protected:

	Graph_t& g_;											//ideally CONST but some operations like get_neighbors are non-const (TODO!)
	std::size_t NV_;

	vint nb_neigh_;
	vint deg_neigh_;
	bb_type node_active_state_;								//bitset for active vertices: 1bit-active, 0bit-passive. Used in degenerate orderings	
	vint nodes_;											//stores the ordering
};
/////////////////////////////////////////////////////////////////////////////////////////////


template<class Graph_t>
inline
vint GraphFastRootSort<Graph_t>::new_order (int alg, bool ltf, bool o2n)
{
	nodes_.clear();

	switch (alg) {
	case sort_alg_t::NONE:								//trivial case- with exit condition!
 		nodes_.reserve(NV_);						
		for (int i = 0; i < NV_; i++) {
			nodes_.emplace_back(i);
		}
		
		///////////////////////
		LOG_WARNING("NONE alg. sorting petition detected, returning trivial isomorphism- GraphFastRootSort<Graph_t>::new_order()");
		return nodes_;
		///////////////////////

		break;
	case sort_alg_t::MIN_DEGEN:
		set_ordering();
		compute_deg_root();
		sort_degen_non_decreasing_degree(ltf);			//checked with framework - (20/12/19 - what does this mean?)
		break;
	case sort_alg_t::MIN_DEGEN_COMPO:
		compute_deg_root();
		compute_support_root();
		sort_non_decreasing_deg_with_support_tb(false /* MUST BE*/);
		sort_degen_composite_non_decreasing_degree(ltf);
		break;
	case sort_alg_t::MAX_DEGEN:
		set_ordering();
		compute_deg_root();
		sort_degen_non_increasing_degree(ltf);
		break;
	case sort_alg_t::MAX_DEGEN_COMPO:
		compute_deg_root();
		compute_support_root();
		sort_non_increasing_deg_with_support_tb(false /* MUST BE*/);
		sort_degen_composite_non_increasing_degree(ltf);
		break;
	case sort_alg_t::MAX:
		compute_deg_root();
		sort_non_increasing_deg(ltf);
		break;
	case sort_alg_t::MIN:
		compute_deg_root();
		sort_non_decreasing_deg(ltf);
		break;
	case sort_alg_t::MAX_WITH_SUPPORT:
		compute_deg_root();
		compute_support_root();
		sort_non_increasing_deg_with_support_tb(ltf);
		break;
	case sort_alg_t::MIN_WITH_SUPPORT:
		compute_deg_root();
		compute_support_root();
		sort_non_decreasing_deg_with_support_tb(ltf);
		break;
	default:
		LOG_ERROR("unknown sorting algorithm : ", alg, "- GraphFastRootSort<Graph_t>::new_order");
		LOG_ERROR("exiting...");
		exit(-1);
	}
		
	//conversion [NEW] to [OLD] if required
	if (!o2n) { Decode::reverse_in_place(nodes_); }            
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_degree(bool rev){
	node_active_state_.set_bit(0, NV_-1);					//all active, pending to be ordered
	int min_deg=NV_, v=EMPTY_ELEM;
	nodes_.clear();

	for(int i=0; i<NV_; i++){
		min_deg=NV_;
		for(int j=0; j<NV_; j++){
			if(node_active_state_.is_bit(j) && nb_neigh_[j] < min_deg){
				min_deg=nb_neigh_[j];
				v=j;
			}
		}
		nodes_.push_back(v);
		node_active_state_.erase_bit(v);
		bb_type& bbn=g_.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int w=bbn.next_bit();
			if(w==EMPTY_ELEM) break;
			if(node_active_state_.is_bit(w))
				nb_neigh_[w]--;
		}
	}//endFor

	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_degen_non_increasing_degree(bool rev){
	node_active_state_.set_bit(0, NV_-1);											//all active, pending to be ordered
	int max_deg=0, v=EMPTY_ELEM;
	nodes_.clear();

	for(int i=0; i<NV_; i++){
		max_deg=-1;
		for(int j=0; j<NV_; j++){
			if(node_active_state_.is_bit(j) && nb_neigh_[j] > max_deg){			 /* MUST BE >=  (20/12/24) TODO- CHECK*/
				max_deg=nb_neigh_[j];
				v=j;
			}
		}
		nodes_.push_back(v);
		node_active_state_.erase_bit(v);
		bb_type& bbn=g_.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int w=bbn.next_bit();
			if(w==EMPTY_ELEM) break;
			if(node_active_state_.is_bit(w))
				nb_neigh_[w]--;
		}
	}//endFor

	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline 
const vint& GraphFastRootSort<Graph_t>::sort_degen_composite_non_decreasing_degree(bool rev)
{
	node_active_state_.set_bit(0, NV_ - 1);			//all active, pending to be ordered
	int min_deg = NV_, v = EMPTY_ELEM;
	vint nodes_ori = nodes_;
	nodes_.clear();

	for (int i = 0; i < NV_; i++) {
		//finds vertex with minimum degree
		min_deg = NV_;
		for (int j = 0; j < NV_; j++) {
			int u = nodes_ori[j];
			if (node_active_state_.is_bit(u) && nb_neigh_[u] < min_deg) {
				min_deg = nb_neigh_[u];
				v = u;
			}
		}

		//updates context
		nodes_.emplace_back(v);
		node_active_state_.erase_bit(v);

		//updates neighborhood info in remaining vertices
		bb_type& bbn = g_.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while (true) {
			int w = bbn.next_bit();
			if (w == EMPTY_ELEM) break;
			if (node_active_state_.is_bit(w))
				nb_neigh_[w]--;
		}
	}

	if (rev) {
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline 
const vint& GraphFastRootSort<Graph_t>::sort_degen_composite_non_increasing_degree(bool rev)
{
	node_active_state_.set_bit(0, NV_ - 1);											//all active, pending to be ordered
	int max_deg = 0, v = EMPTY_ELEM;
	vint nodes_ori = nodes_;
	nodes_.clear();
		
	for (int i = 0; i < NV_; i++) {
		//finds vertex with maximum degree
		max_deg = -1;
		for (int j = 0; j < NV_; j++) {
			int u = nodes_ori[j];
			if (node_active_state_.is_bit(u) && nb_neigh_[u] > max_deg) {			 /* MUST BE >=  (20/12/24) TODO- CHECK*/
				max_deg = nb_neigh_[u];
				v = u;
			}
		}

		//updates context
		nodes_.emplace_back(v);
		node_active_state_.erase_bit(v);

		//updates neighborhood info in remaining vertices
		bb_type& bbn = g_.get_neighbors(v);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while (true) {
			int w = bbn.next_bit();
			if (w == EMPTY_ELEM) break;
			if (node_active_state_.is_bit(w))
				nb_neigh_[w]--;
		}
	}

	if (rev) {
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}


template<class Graph_t>
inline
void GraphFastRootSort<Graph_t>::set_ordering(){
	nodes_.clear();
	nodes_.reserve(NV_);
	for(int i=0; i<NV_; i++){
			nodes_.emplace_back(i);
	}
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg(bool rev){
	set_ordering();
	com::has_greater_val<int, vint> pred(nb_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);
	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}


template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(bool rev){
	set_ordering();
	com::has_smaller_val<int, vint> pred(nb_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);
	
	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg_with_support_tb(bool rev ){
	set_ordering();
	com::has_greater_val_with_tb<int, vint> pred(nb_neigh_, deg_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);

	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg_with_support_tb(bool rev){
	set_ordering();
	com::has_smaller_val_with_tb<int, vint> pred(nb_neigh_, deg_neigh_);
	std::stable_sort(nodes_.begin(),  nodes_.end(), pred);

	if(rev){
		std::reverse(nodes_.begin(), nodes_.end());
	}
	return nodes_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::compute_deg_root(){
	for(int elem=0; elem<NV_; elem++){		
		nb_neigh_[elem]=g_.get_neighbors(elem).popcn64();
	}
	return nb_neigh_;
}

template<class Graph_t>
inline
const vint& GraphFastRootSort<Graph_t>::compute_support_root()
{
	for(int elem=0; elem<NV_; elem++){
		deg_neigh_[elem]=0;
		bb_type& bbn=g_.get_neighbors(elem);
		bbn.init_scan(bbo::NON_DESTRUCTIVE);
		while(true){
			int w=bbn.next_bit();
			if(w==EMPTY_ELEM) break;
			deg_neigh_[elem]+=nb_neigh_[w];	
		}
	}

	return deg_neigh_;
}

template<class Graph_t>
inline
ostream& GraphFastRootSort<Graph_t>::print (int type, ostream& o) const
{	
	switch (type) {
	case sort_print_t::PRINT_DEGREE:
		for (auto elem : nb_neigh_) {
			o << elem << " ";
		}		
		break;
	case sort_print_t::PRINT_SUPPORT:
		for (auto elem : deg_neigh_) {
			o << elem << " ";
		}		
		break;
	case sort_print_t::PRINT_NODES:
		com::stl::print_collection<vint>(nodes_, o);		//formatted print with size in brackets
		break;
	default:
		LOG_ERROR("unknown print type- GraphFastRootSort<Graph_t>::print()");
		LOG_ERROR("exiting...");
		exit(-1);
	}

	o<<endl;
	return o;
}

template<class Graph_t>
inline int GraphFastRootSort<Graph_t>::compute_deg(const Graph_t & g, vint & deg){
	int NV_ = g_.number_of_vertices();	
	deg.assign(NV_, -1);
	for (int v = 0; v < NV_; v++) {
		deg[v] = g_.get_neighbors(v).popcn64();
	}
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::reset(){
	nb_neigh_.clear();
	nb_neigh_.resize(NV_);
	//nb_neigh_.assign(NV_, 0);

	deg_neigh_.clear();
	deg_neigh_.resize(NV_);
	//deg_neigh_.assign(NV_, 0);	
	
	node_active_state_.set_bit(0, NV_ - 1);		//all active, pending to be ordered
	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::reorder(const vint& new_order, Graph_t& gn, Decode* d) 
{
	std::size_t NV = g_.number_of_vertices();
	gn.init(NV);
	gn.set_name(g_.get_name(), false /* no path separation */ );	
	gn.set_path(g_.get_path());
	
	///generate isomorphism (only for undirected graphs) 
	for (int i = 0; i < NV - 1; i++) {
		for (int j = i + 1; j < NV; j++) {
			if (g_.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
				//////////////////////////////////////////////
				gn.add_edge(new_order[i], new_order[j]);			//maps the new edges according to the new given order
				//////////////////////////////////////////////
			}
		}
	}

///////////////
//stores decoding information [NEW]->[OLD]
	if (d != NULL) {
		vint aux(new_order);						//new_order is in format [OLD]->[NEW]
		Decode::reverse_in_place(aux);				//aux is in format [NEW]->[OLD]		
		d->insert_ordering(aux);
	}

	return 0;
}

template<class Graph_t>
inline
int GraphFastRootSort<Graph_t>::SORT_SUBGRAPH_NON_INC_DEG(Graph_t& g, vint& lhs, vint& rhs, bool ftl) {
///////////////////////////
// degenerate sorting  of  @lhs according to the degree of vertices wrt to @lhs and @rhs 
// degeneracy is only related to @lhs
	
	const int NV = g.number_of_vertices();
	vector<int> nb_neigh(NV, EMPTY_ELEM);
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
	com::has_smaller_val<int, vint> p(nb_neigh);

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
	std::size_t NV = g.number_of_vertices();
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
	/*com::stl::print_array(nb_neigh, nb_neigh+ g_.number_of_vertices());
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
		/*com::stl::print_array(nb_neigh, nb_neigh + g_.number_of_vertices());
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

/////////////////
//	
// namespace for GRAPH sort basic (enum) types
// (deprecated code - 22/12/24)
//
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





#endif

