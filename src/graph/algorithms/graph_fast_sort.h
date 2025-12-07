/**
* @file graph_fast_sort.h
* @brief header for GraphFastRootSort class which sorts graphs by different criteria
* @details: changed nodes_ stack to vector (18/03/19)
*			- now working with subgraphs (30/11/2025)
* @date: created 12/03/15,  last_update 27/01/25
* @author pss
**/

#ifndef __GRAPH_FAST_SORT_H__
#define __GRAPH_FAST_SORT_H__
							

#include "graph/simple_ugraph.h"				//to limit template Graph_t to undirected types - types must be known

#include "utils/logger.h"
#include "utils/common.h"						
#include "decode.h"
#include "bitscan/bbtypes.h"					//for EMPTY_ELEM constant	
#include "bitscan/bbobject.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include <numeric>								//std::iota


//alias
using vint = std::vector<int>;


namespace bitgraph {
	namespace _impl {

		///////////////////////////
		//
		// GraphFastRootSort class
		// (Graph_t should be restricted to ugraph and sparse_ugraph types)
		//
		////////////////////////////

		template <class Graph_t>
		class GraphFastRootSort {
			//restrict to ugraph and sparse_ugraph types
			static_assert(std::is_same<bitgraph::Ugraph<BBScan>, Graph_t>::value ||
				std::is_same<bitgraph::Ugraph<BBScanSp>, Graph_t>::value, "is not a valid GraphFastRootSort type");

		public:
			using basic_type = Graph_t;											//graph type
			using _gt = basic_type;												//alias for backward compatibility		
			using type = GraphFastRootSort< Graph_t>;							//own type		
			using bb_type = typename Graph_t::_bbt;								//bitset type
			
			//non structured enums are used for easy conversion to int when needed
			enum { PRINT_DEGREE = 0, PRINT_SUPPORT, PRINT_NODES };
			enum { MIN_DEGEN = 0, MAX_DEGEN, MIN_DEGEN_COMPO, MAX_DEGEN_COMPO, MAX, MIN, MAX_WITH_SUPPORT, MIN_WITH_SUPPORT, NONE };
			enum { FIRST_TO_LAST = 0, LAST_TO_FIRST };
			enum { NEW_TO_OLD = 0, OLD_TO_NEW };								//backward compatibility - possibly to be removed

			////////////////////////
			//static methods 

			/**
			* @brief Computes the degree of the vertices of a graph
			* @param g input graph G=(V, E)
			* @param deg output vector of size |V| (v[i] = deg(vi))
			**/
			static int compute_deg(const Graph_t& g, vint& deg);


			///////////////
			// drivers - the real public interface

			/**
			* @brief Computes a new ordering
			* @param alg sorting algorithm
			* @param ltf last to first ordering if TRUE
			* @param o2n old to new ordering	if TRUE
			* @return new ordering in [OLD]->[NEW] format
			**/
			virtual vint new_order(int alg, bool ltf = true, bool o2n = true);

			/**
			* @brief Computes a new ordering for the subgraph @bbsg. Only the vertices in @bbsg are reordered.
			* @param alg: sorting algorithm
			* @param bbsg: bitset encoding the subgraph vertices
			* @param ltf: last to first ordering if TRUE
			* @param o2n: old to new ordering	if TRUE
			* @result: new ordering in [OLD]->[NEW] format
			* @details: to be implemented according to the following steps:
			*			1) create the induced subgraph of size |bbsg|
			*			2) create a new ordering for the subgraph based on existing primitives
			*			3) map the ordering back to the original graph
			* 
			**/
			vint new_order(int alg, bb_type& bbsg, bool ltf = true, bool o2n = true);
	
			/**
			* @brief Creates an isomorphism for a given ordering
			* @param gn output isomorphic graph
			* @param new_order in [OLD]->[NEW]  format
			* @param d ptr to decode object to store the ordering
			* @comments only for simple undirected graphs with no weights
			* @return 0 if successful
			**/
			int reorder(const vint& new_order, Graph_t& gn, Decode* d = nullptr);
			static Graph_t reorder(const Graph_t& g, const vint& new_order, Decode* d = nullptr);

			////////////////////////
			//construction/destructions

			explicit GraphFastRootSort(Graph_t& gout) :
				g_(gout),
				NV_(g_.num_vertices())
			{
				nb_neigh_.assign(NV_, 0);
				deg_neigh_.assign(NV_, 0);
				node_active_state_.reset(NV_);
			}

			//move and copy semantics not allowed
			GraphFastRootSort(const GraphFastRootSort&) = delete;
			GraphFastRootSort& operator=	(const GraphFastRootSort&) = delete;
			GraphFastRootSort(GraphFastRootSort&&) = delete;
			GraphFastRootSort& operator=	(GraphFastRootSort&&) = delete;

			//destructor
			~GraphFastRootSort() = default;

			////////////////////////
			//setters / getters

			const vint& degree() const { return nb_neigh_; }
			const vint& support() const { return deg_neigh_; }
			const Graph_t& graph() const { return g_; }
			std::size_t num_vertices() const { return NV_; }

			///////////////////////
			// allocation
		protected:

			/**
			* @brief Restores context for NV_ vertices
			**/
			int reset();

		public:
			/////////////////////////
			// main operations - sorting, etc.

			/**
			* @brief Sets trivial ordering [1..NV] in @nodes_,
			*		 a starting point for all sorting primitives
			**/
			void set_ordering();

			/*
			* @brief Sets an ordering in [OLD]->[NEW] format in @nodes_.
			*		 This will be the given ordering in composite orderings
			*/
			void set_ordering(vint& nodes) { nodes_ = nodes; }

			/**
			* @brief Computes the degree of each vertex
			**/
			const vint& compute_deg_root();

			/**
			* @brief Computes support for all vertices (sum of the number of neighbors)
			* @comments May include the same vertex twice
			**/
			const vint& compute_support_root();

			/**
			* @brief Computes a non_increasing_degree (non-degenerate) ordering
			* @param rev reverse ordering if TRUE
			* @important requires prior computation of deg
			* @return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_non_increasing_deg(bool rev);

			/**
			* @brief Computes a non-decreasing degree (non-degenerate) ordering
			* @param rev reverse ordering if TRUE
			* @important requires prior computation of deg
			* @return ouptut ordering in [NEW]->[OLD] format
			**/
			const vint& sort_non_decreasing_deg(bool rev);

			/*
			* @brief Computes a non-increasing degree (non-degenerate) ordering with tiebreak by supprt
			* @param rev reverse ordering if TRUE
			* @important requires prior computation of deg and support
			* @return output ordering in[NEW]->[OLD] format
			*/
			const vint& sort_non_increasing_deg_with_support_tb(bool rev);

			/**
			* @brief Computes a non-decreasing degree (non-degenerate) ordering with tiebreak by supprt
			* @param rev reverse ordering if TRUE
			* @important requires prior computation of deg and support
			* @return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_non_decreasing_deg_with_support_tb(bool rev);

			/**
			* @brief Degenerate non-decreasing degree ordering
			* @comments deg info is not restored after the call
			* @return output ordering in [NEW]->[OLD] format
			* TODO - optimize
			**/
			const vint& sort_degen_non_decreasing_deg(bool rev);
			const vint& sort_degen_non_increasing_deg(bool rev);

			//Expermimental alternative implementation - CHECK efficiency
			//Does not required cached degree info of vertices in @nb_neigh_
			const vint& sort_degen_non_decreasing_deg_B(bool rev);

			/**
			*@brief Composite non-decreasing degree degenerate ordering based on a prior given ordering
			*@param rev reverse ordering if TRUE
			*@comments the vertex ordering has to be set (with set_ordering(...)) prior to the call
			*@return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_degen_composite_non_decreasing_deg(bool rev);


			/**
			*@brief Composite non-increasing degree degenerate ordering based on a prior given ordering
			*@param rev reverse ordering if TRUE
			*@comments the vertex ordering has to be set (with set_ordering(...)) prior to the call
			*@return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_degen_composite_non_increasing_deg(bool rev);

			/////////////////
			// Subgrah ordering 
			// 
			// TODO - add further primitives for composites, etc...

			/**
			*@brief sorts the first k vertices by non-increasing degree (non-degenerate)
			*@param first_k  first k < |V|  vertices to sort ([0..k-1])
			*@param rev reverse ordering if TRUE
			*@return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_non_increasing_deg(int first_k, bool rev);

			/**
			*@brief sorts [first, last] consecutive vertices by non-increasing degree (non-degenerate)
			*@param first  first vertex to sort (0-based index) - in  [0, |V|-1]
			*@param last  last vertex to sort	(0-based index)	- in  [0, |V|-1], > first
			*@param rev reverse ordering if TRUE
			*@return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_non_increasing_deg(int first, int last, bool rev);

			/**
			*@brief sorts the first k vertices by non-decreasing degree (non-degenerate)
			*@param first_k  first k < |V|  vertices to sort ([0..k-1])
			*@param rev reverse ordering if TRUE
			*@return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_non_decreasing_deg(int first_k, bool rev);

			/**
			*@brief sorts [first, last] consecutive vertices by non-decreasing degree (non-degenerate)
			*@param first  first vertex to sort (0-based index) - in  [0, |V|-1]
			*@param last  last vertex to sort	(0-based index)	- in  [0, |V|-1], > first
			*@param rev reverse ordering if TRUE
			*@return output ordering in [NEW]->[OLD] format
			**/
			const vint& sort_non_decreasing_deg(int first, int last, bool rev);

			//TODO - add tiebreak support for subgraph ordering 
			//int  sort_non_increasing_deg_with_support_tb(int n, bool rev = false);
			//int  sort_non_decreasing_deg_with_support_tb(int n, bool rev = false);

		////////////////////////
		// I/O
			ostream& print(int type, ostream& o, bool eofl = true) const;


			/////////////////////////////////////////////
			// data members	
		protected:

			Graph_t& g_;											//ideally CONST but some operations like neighbors are non-const (TODO!)
			int NV_;												//number of vertices cached - g_.num_vertices()  

			vint nb_neigh_;											//stores the degree of the vertices		
			vint deg_neigh_;										//stores the support of the vertices (degree of neighbors)
			bb_type node_active_state_;								//bitset for active vertices: 1bit-active, 0bit-passive. Used in degenerate orderings	
			vint nodes_;											//stores the ordering

		};//end of GraphFastRootSort class

	}//end of namespace _impl

	using _impl::GraphFastRootSort;	

}//end of namespace bitgraph

////////////////////////////////////////////////////////////
// Necessary header implementations for generic code

namespace bitgraph {

	template<class Graph_t>
	inline
		vint GraphFastRootSort<Graph_t>::new_order(int alg, bool ltf, bool o2n)
	{
		nodes_.clear();

		switch (alg) {
		case NONE:								//trivial case- with exit condition!
			nodes_.reserve(NV_);
			for (auto i = 0; i < NV_; i++) {
				nodes_.emplace_back(i);
			}

			///////////////////////
			LOG_WARNING("NONE alg. sorting petition detected, returning trivial isomorphism- GraphFastRootSort<Graph_t>::new_order()");
			return nodes_;
			///////////////////////

			break;
		case MIN_DEGEN:
			set_ordering();
			compute_deg_root();
			sort_degen_non_decreasing_deg(ltf);			//checked with framework - (20/12/19 - what does this mean?)
			break;
		case MIN_DEGEN_COMPO:
			compute_deg_root();
			compute_support_root();
			sort_non_decreasing_deg_with_support_tb(false /* MUST BE*/);
			sort_degen_composite_non_decreasing_deg(ltf);
			break;
		case MAX_DEGEN:
			set_ordering();
			compute_deg_root();
			sort_degen_non_increasing_deg(ltf);
			break;
		case MAX_DEGEN_COMPO:
			compute_deg_root();
			compute_support_root();
			sort_non_increasing_deg_with_support_tb(false /* MUST BE*/);
			sort_degen_composite_non_increasing_deg(ltf);
			break;
		case MAX:
			compute_deg_root();
			sort_non_increasing_deg(ltf);
			break;
		case MIN:
			compute_deg_root();
			sort_non_decreasing_deg(ltf);
			break;
		case MAX_WITH_SUPPORT:
			compute_deg_root();
			compute_support_root();
			sort_non_increasing_deg_with_support_tb(ltf);
			break;
		case MIN_WITH_SUPPORT:
			compute_deg_root();
			compute_support_root();
			sort_non_decreasing_deg_with_support_tb(ltf);
			break;
		default:
			LOGG_ERROR("unknown sorting algorithm : ", alg, "- GraphFastRootSort<Graph_t>::new_order");
			LOG_ERROR("exiting...");
			exit(-1);
		}

		//conversion [NEW] to [OLD] if required
		//note: sorting produced by sorting primitives is always in [OLD] to [NEW] format
		if (o2n) { Decode::reverse_in_place(nodes_); }
		return nodes_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_deg(bool rev) {

		//initialization
		node_active_state_.set_bit(0, NV_ - 1);					//all vertices active, pending to be ordered
		nodes_.clear();
		nodes_.reserve(NV_);

		int max_deg = NV_;
		int v = BBObject::noBit;
		do {
			max_deg = NV_;

			//selects an active vertex with minimum degree
			for (int j = 0; j < NV_; j++) {
				if (node_active_state_.is_bit(j) && nb_neigh_[j] < max_deg) {
					max_deg = nb_neigh_[j];
					v = j;
				}
			}

			//////////////////////////////////
			nodes_.emplace_back(v);
			if (nodes_.size() == NV_) { break; }			//exit condition

			node_active_state_.erase_bit(v);
			///////////////////////////////////

			//update degree info of the remaining active vertices
			bb_type& bbn = g_.neighbors(v);

			if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
				int w = BBObject::noBit;
				while ((w = bbn.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						nb_neigh_[w]--;
					}
				}
			}

		} while (true);


		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}

		return nodes_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_degen_non_increasing_deg(bool rev) {

		//initialization
		node_active_state_.set_bit(0, NV_ - 1);										//all vertices active, pending to be ordered
		nodes_.clear();
		nodes_.reserve(NV_);

		//main loop
		int max_deg = 0, v = BBObject::noBit;
		do {
			//finds vertex with maximum degree
			max_deg = -1;
			for (auto j = 0; j < NV_; j++) {
				if (node_active_state_.is_bit(j) && nb_neigh_[j] > max_deg) {
					max_deg = nb_neigh_[j];
					v = j;
				}
			}

			//////////////////////////////////
			nodes_.emplace_back(v);
			if (nodes_.size() == NV_) { break; }			//exit condition

			node_active_state_.erase_bit(v);
			//////////////////////////////////

			//updates neighborhood info in remaining vertices
			bb_type& bbn = g_.neighbors(v);
			if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
				int w = BBObject::noBit;
				while ((w = bbn.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						nb_neigh_[w]--;
					}
				}
			}

		} while (true);

		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}
		return nodes_;
	}

	template<class Graph_t>
	inline const vint& GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_deg_B(bool rev)
	{

		int min_deg = NV_, deg = 0;
		node_active_state_.set_bit(0, NV_ - 1);					//all active, pending to be ordered
		nodes_.clear();

		//main loop
		do {

			min_deg = NV_;
			int w = BBObject::noBit;
			int v = BBObject::noBit;

			//selects an active vertex with minimum degree
			if (node_active_state_.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
				LOG_ERROR("init scan failed -  GraphFastRootSort<Graph_t>::sort_degen_non_decreasing_deg_B");
				LOG_ERROR("exting");
				std::exit(EXIT_FAILURE);
			}

			while ((w = node_active_state_.next_bit()) != BBObject::noBit) {
				deg = g_.degree(w, node_active_state_);
				if (min_deg > deg) {											// >= is possible
					min_deg = deg;
					v = w;
				}
			}

			//////////////////////////////////
			node_active_state_.erase_bit(v);
			nodes_.emplace_back(v);
			//////////////////////////////////

		} while (nodes_.size() < NV_);

		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}

		return nodes_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_degen_composite_non_decreasing_deg(bool rev)
	{
		node_active_state_.set_bit(0, NV_ - 1);			//all active, pending to be ordered
		int min_deg = NV_, v = EMPTY_ELEM;
		vint nodes_ori = nodes_;
		nodes_.clear();

		for (auto i = 0; i < NV_; i++) {

			//finds vertex with minimum degree with TB according to the given ordering in nodes_ori 
			min_deg = NV_;
			for (auto j = 0; j < NV_; j++) {
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
			bb_type& bbn = g_.neighbors(v);
			if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
				int w = BBObject::noBit;
				while ((w = bbn.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						nb_neigh_[w]--;
					}
				}
			}
		}

		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}
		return nodes_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_degen_composite_non_increasing_deg(bool rev)
	{
		node_active_state_.set_bit(0, NV_ - 1);											//all active, pending to be ordered
		int max_deg = 0, v = EMPTY_ELEM;
		vint nodes_ori = nodes_;
		nodes_.clear();

		for (auto i = 0; i < NV_; i++) {
			//finds vertex with maximum degree
			max_deg = -1;
			for (auto j = 0; j < NV_; j++) {
				int u = nodes_ori[j];
				if (node_active_state_.is_bit(u) && nb_neigh_[u] > max_deg) {
					max_deg = nb_neigh_[u];
					v = u;
				}
			}

			//updates context
			nodes_.emplace_back(v);
			node_active_state_.erase_bit(v);

			//updates neighborhood info in remaining vertices
			bb_type& bbn = g_.neighbors(v);
			if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
				int w = BBObject::noBit;
				while ((w = bbn.next_bit()) != BBObject::noBit) {
					if (node_active_state_.is_bit(w)) {
						nb_neigh_[w]--;
					}
				}
			}
		}

		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}
		return nodes_;
	}

	template<class Graph_t>
	inline const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg(int first_k, bool rev)
	{

		vint kord;
		_sort::fill_vertices(kord, first_k);

		//////////////////////////////////////////////////////
		has_greater_val<int, vint> pred(nb_neigh_);
		//////////////////////////////////////////////////////

		std::stable_sort(kord.begin(), kord.end(), pred);
		if (rev) {
			std::reverse(kord.begin(), kord.end());
		}

		//generates the ordering in @nodes_ - first k vertices at the beginning
		nodes_ = kord;
		nodes_.reserve(NV_);
		for (int v = first_k; v < NV_; v++) {
			nodes_.emplace_back(v);
		}
		return nodes_;
	}

	template<class Graph_t>
	inline const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg(int first, int last, bool rev) {

		vint kord;
		kord.reserve(last - first + 1);
		for (int i = first; i <= last; i++) {
			kord.emplace_back(i);
		}

		//////////////////////////////////////////////////////
		has_greater_val<int, vint> pred(nb_neigh_);
		//////////////////////////////////////////////////////

		std::stable_sort(kord.begin(), kord.end(), pred);
		if (rev) {
			std::reverse(kord.begin(), kord.end());
		}

		//generates the ordering - first k vertices at the beginning
		set_ordering();				//trivial ordering in @nodes_
		int index = first;
		for (auto v : kord) {
			nodes_[index++] = v;	//substitute in nodes_ the sorted vertices
		}

		return nodes_;
	}

	template<class Graph_t>
	inline const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(int first_k, bool rev) {
		vint kord;
		_sort::fill_vertices(kord, first_k);

		//////////////////////////////////////////////////////
		has_smaller_val<int, vint> pred(nb_neigh_);
		//////////////////////////////////////////////////////

		std::stable_sort(kord.begin(), kord.end(), pred);
		if (rev) {
			std::reverse(kord.begin(), kord.end());
		}

		//generates the ordering - first k vertices at the beginning
		nodes_ = kord;
		nodes_.reserve(NV_);
		for (int v = first_k; v < NV_; v++) {
			nodes_.emplace_back(v);
		}

		return nodes_;
	}

	template<class Graph_t>
	inline const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(int first, int last, bool rev) {

		vint kord;
		kord.reserve(last - first + 1);
		for (int i = first; i <= last; i++) {
			kord.emplace_back(i);
		}

		//////////////////////////////////////////////////////
		has_smaller_val<int, vint> pred(nb_neigh_);
		//////////////////////////////////////////////////////

		std::stable_sort(kord.begin(), kord.end(), pred);
		if (rev) {
			std::reverse(kord.begin(), kord.end());
		}

		//generates the ordering - first k vertices at the beginning
		set_ordering();				//trivial ordering in @nodes_
		int index = first;
		for (auto v : kord) {
			nodes_[index++] = v;	//substitute in @nodes_ the sorted vertices
		}

		return nodes_;
	}


	template<class Graph_t>
	inline
		void GraphFastRootSort<Graph_t>::set_ordering() {
		nodes_.clear();
		nodes_.reserve(NV_);
		for (int i = 0; i < NV_; i++) {
			nodes_.emplace_back(i);
		}
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg(bool rev) {
		set_ordering();
		has_greater_val<int, vint> pred(nb_neigh_);
		std::stable_sort(nodes_.begin(), nodes_.end(), pred);
		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}
		return nodes_;
	}


	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg(bool rev) {
		set_ordering();
		has_smaller_val<int, vint> pred(nb_neigh_);
		std::stable_sort(nodes_.begin(), nodes_.end(), pred);

		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}
		return nodes_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_non_increasing_deg_with_support_tb(bool rev) {
		set_ordering();
		has_greater_val_with_tb<int, vint> pred(nb_neigh_, deg_neigh_);
		std::stable_sort(nodes_.begin(), nodes_.end(), pred);

		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}
		return nodes_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::sort_non_decreasing_deg_with_support_tb(bool rev) {
		set_ordering();
		has_smaller_val_with_tb<int, vint> pred(nb_neigh_, deg_neigh_);
		std::stable_sort(nodes_.begin(), nodes_.end(), pred);

		if (rev) {
			std::reverse(nodes_.begin(), nodes_.end());
		}
		return nodes_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::compute_deg_root() {

		for (int elem = 0; elem < NV_; ++elem) {
			nb_neigh_[elem] = g_.neighbors(elem).count();
		}

		return nb_neigh_;
	}

	template<class Graph_t>
	inline
		const vint& GraphFastRootSort<Graph_t>::compute_support_root()
	{
		for (int elem = 0; elem < NV_; ++elem) {
			deg_neigh_[elem] = 0;
			bb_type& bbn = g_.neighbors(elem);
			if (bbn.init_scan(BBObject::NON_DESTRUCTIVE) != -1) {
				int w = BBObject::noBit;
				while ((w = bbn.next_bit()) != EMPTY_ELEM) {
					deg_neigh_[elem] += nb_neigh_[w];
				}
			}
		}

		return deg_neigh_;
	}

	template<class Graph_t>
	inline
		ostream& GraphFastRootSort<Graph_t>::print(int type, ostream& o, bool eofl) const
	{
		switch (type) {
		case PRINT_DEGREE:
			bitgraph::_stl::print_collection(nb_neigh_, o, eofl);			
			break;
		case PRINT_SUPPORT:
			bitgraph::_stl::print_collection(deg_neigh_, o, eofl);			
			break;
		case PRINT_NODES:
			bitgraph::_stl::print_collection(nodes_, o, eofl);
			break;
		default:
			LOG_ERROR("unknown print type- GraphFastRootSort<Graph_t>::print()");
			LOG_ERROR("exiting...");
			exit(-1);
		}

		if (eofl) { o << endl; }
		return o;
	}

	template<class Graph_t>
	inline int GraphFastRootSort<Graph_t>::compute_deg(const Graph_t& g, vint& deg) {
		auto NV = g.num_vertices();
		deg.assign(NV, -1);
		for (auto v = 0; v < NV; v++) {
			deg[v] = g.neighbors(v).size();
		}
		return 0;
	}

	template<class Graph_t>
	inline
		int GraphFastRootSort<Graph_t>::reset() {

		nb_neigh_.clear();
		nb_neigh_.resize(NV_);

		deg_neigh_.clear();
		deg_neigh_.resize(NV_);

		node_active_state_.set_bit(0, NV_ - 1);		//all active, pending to be ordered
		return 0;
	}

	template<class Graph_t>
	inline 
	vint GraphFastRootSort<Graph_t>::new_order(int alg, bb_type& bbsg, bool ltf, bool o2n)
	{			
		//convert bbsg to vector
		vint lv;
		bbsg.extract(lv);

		//////////////////////////////////////////////////////////////////////////////////////////
		assert(!lv.empty() && "empty subgraph detected- GraphFastRootSort<Graph_t>::new_order()");
		/////////////////////////////////////////////////////////////////////////////////////////
				

		//create the induced subgraph of size |bbsg|
		Graph_t sg;

		////////////////////////////////////
		this->g_.create_subgraph(sg, lv);	
		////////////////////////////////////

		//create a new ordering for the subgraph based on existing primitives
		GraphFastRootSort<Graph_t> sort(sg);
		vint ord_sg = sort.new_order(alg, ltf, false /* n2o format*/);

		//map the ordering @ord back to the original graph
		std::vector<int> ord(NV_);
		std::iota(ord.begin(), ord.end(), 0);
		
		//build reverse mapping from sg to the original graph g
		vint sg_to_g = ord;
		int v = bbo::noBit;
		int index_in_sg = 0;
		bbsg.init_scan(bbo::NON_DESTRUCTIVE);
		while ((v = bbsg.next_bit()) != bbo::noBit) {
			sg_to_g[index_in_sg++] =v;
		}
		
		//mapping of ord_sg to ord ([NEW]->[OLD] format)
		v = bbo::noBit;
		index_in_sg = 0;
		bbsg.init_scan(bbo::NON_DESTRUCTIVE);
		while((v = bbsg.next_bit()) != bbo::noBit) {
			int new_index_in_sg = ord_sg[index_in_sg++];
			ord[v] = sg_to_g[new_index_in_sg];
		}

		
		////////////////
		// check

#ifndef NDEBUG

		assert(ord.size() == NV_ && "ERROR: ord.size() != N - GraphFastRootSort<Graph_t>::new_order");

		//verify vertices outside bbsg have not been reordered
		for (int v = 0; v < ord.size(); ++v) {
			if (!bbsg.is_bit(v)) {
				////////////////////////////////////////////////////////////////////////////////////////////////////////
				assert(ord[v] == v && "ERROR: vertex outside bbsg reordered - GraphFastRootSort<Graph_t>::new_order");
				/////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}

#endif
		//reverse to [OLD]->[NEW] if required
		if (o2n) {
			Decode::reverse_in_place(ord);
		}

		return ord;		
	}

	template<class Graph_t>
	inline
		int GraphFastRootSort<Graph_t>::reorder(const vint& new_order, Graph_t& gn, Decode* d)
	{
		std::size_t NV = g_.num_vertices();
		gn.reset(NV);
		gn.set_name(g_.name());
		gn.set_path(g_.path());

		///generate isomorphism (only for undirected graphs) 
		for (auto i = 0u; i < NV - 1; i++) {
			for (auto j = i + 1; j < NV; j++) {
				if (g_.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
					//////////////////////////////////////////////
					gn.add_edge(new_order[i], new_order[j]);			//maps the new edges according to the new given order
					//////////////////////////////////////////////
				}
			}
		}

		///////////////
		//stores decoding information [NEW]->[OLD]
		if (d != nullptr) {
			vint aux(new_order);						//new_order is in format [OLD]->[NEW]
			Decode::reverse_in_place(aux);				//aux is in format [NEW]->[OLD]		
			d->add_ordering(aux);
		}

		return 0;
	}


	template<class Graph_t>
	inline Graph_t bitgraph::_impl::GraphFastRootSort<Graph_t>::reorder(const Graph_t& g, const vint& o2n, Decode* d)
	{
		Graph_t gres;

		std::size_t NV = g.num_vertices();
		gres.reset(NV);
		gres.set_name(g.name());
		gres.set_path(g.path());

		///generate isomorphism (only for undirected graphs) 
		for (auto i = 0u; i < NV - 1; i++) {
			for (auto j = i + 1; j < NV; j++) {
				if (g.is_edge(i, j)) {									//in O(log) for sparse graphs, should be specialized for that case
					//////////////////////////////////////////////
					gres.add_edge(o2n[i], o2n[j]);			//maps the new edges according to the new given order
					//////////////////////////////////////////////
				}
			}
		}

		///////////////
		//stores decoding information [NEW]->[OLD]
		if (d != nullptr) {		
			auto aux = Decode::reverse(o2n);				//o2n is in format [OLD]->[NEW], aux is in format [NEW]->[OLD]
			d->add_ordering(aux);
		}

		return gres;	
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

}//end of namespace bitgraph	





#endif

