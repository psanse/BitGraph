 /**
   * @file simple_graph_w.h
   * @brief classes Base_Graph_W and Graph_W for simple weighted graphs 
   *
   * @created 16/01/19
   * @last_update 06/01/25
   * @author pss
   *
   * This code is part of the GRAPH 1.0 C++ library
   *
   **/

#ifndef __SIMPLE_GRAPH_WEIGHTED_H__
#define __SIMPLE_GRAPH_WEIGHTED_H__

#include "utils/common.h"
#include "graph_basic.h"
#include "simple_ugraph.h"
#include <iostream>
#include <vector>
#include <algorithm>

namespace bitgraph {
	
	///////////////////////
	//
	// Base_Graph_W class 
	// 
	// (used to be able to specialize functions
	// according to the type of graph only)
	//
	// User type class is Graph_W 
	//
	////////////////////////

	template<class GraphT, class WeightT>
	class Base_Graph_W {

	public:
		enum { Wext = 0, Dext, WWWext, NOext };				// file extensions for weights (used in function read_dimacs - @todo CHECK if necessary, add scope (28/01/2026))	

		using graph_type = GraphT;							// graph type	
		using bitset_type = typename GraphT::bitset_type;	// bitset type used by graph type 
		using VertexBitset = bitset_type;					// alias for semantic type
		using Weight = WeightT;
		
		//constants - globals
		static constexpr Weight NO_WEIGHT{ -1 };				// possibly change to a sentinel std::numeric_limits<WeightT>::max() ?
		static constexpr Weight ZERO_WEIGHT{ 0 };
		static constexpr Weight DEFAULT_WEIGHT{ 1 };			// default weight value for weights (1.0)	

		//constructors
		Base_Graph_W() {};																					//No memory allocation
		explicit Base_Graph_W(std::vector<Weight>& lw) : w_(lw) { g_.reset(lw.size()); }							//creates empty graph with |V|= n with vertex weights
		Base_Graph_W(graph_type& g, vector<Weight>& lw) :g_(g), w_(lw) { assert(w_.size() == g_.size()); }				//creates graph with vertex weights	
		Base_Graph_W(graph_type& g) :g_(g), w_(g.size(), DEFAULT_WEIGHT) {}										//creates graph with DEFAULT_WEIGHTs
		explicit Base_Graph_W(int N, Weight val = DEFAULT_WEIGHT) { reset(N, val); }								//creates empty graph with |V|= N and weight value val

		/**
		* @brief Reads weighted graph from ASCII file in DIMACS format
		*
		*		 If unable to read weights generates modulus weights [Pullman 2008]
		*
		*		 TODO: add support for other formats
		**/
		explicit Base_Graph_W(std::string filename);

		//copy constructor, move constructor, copy operator =, move operator =
		Base_Graph_W(const Base_Graph_W& g) = default;
		Base_Graph_W(Base_Graph_W&& g) noexcept = default;
		Base_Graph_W& operator = (const Base_Graph_W& g) = default;
		Base_Graph_W& operator = (Base_Graph_W&& g)	 noexcept = default;

		//destructor
		virtual	~Base_Graph_W() = default;

		/////////////
		// setters and getters

		void set_weight(int v, Weight val) { w_.at(v) = val; }
		void set_weight(Weight val = DEFAULT_WEIGHT) { w_.assign(g_.num_vertices(), val); }

		/*
		* @brief sets vertex weights to all vertices
		* @param lw vector of weights of size |V|
		* @returns 0 if success, -1 if error
		*/
		int	 set_weight(std::vector<Weight>& lw);

		/**
		* @brief generates weights based on modulus operation [Pullan 2008, MODE = 200]
		*
		*			w(v) = (v + 1) % MODE	(v 1-based index)
		*
		*			MODE = 1 -> w(v) = 1	(unweighted graph)
		**/
		int set_modulus_weight(int MODE = DEFAULT_WEIGHT_MODULUS);


		graph_type& graph() { return g_; }
		const graph_type& graph() const { return g_; }

		Weight weight(int v) const { return w_[v]; }
		const vector<Weight>& weight() const { return w_; }
		vector<Weight>& weight() { return w_; }

		/*
		* @brief Determines weight and vertex of maximum weight
		* @param v ouptut vertex of maximum weight
		* @returns weight of vertex v
		*/
		Weight maximum_weight(int& v)	const;

		const VertexBitset& neighbors(int v) const { return g_.neighbors(v); }
		VertexBitset& neighbors(int v) { return g_.neighbors(v); }

		void set_name(std::string str) { g_.set_name(str); }
		std::string name() const { return g_.name(); }
		void set_path(std::string path_name) { g_.set_path(path_name); }
		std::string path() const { return g_.path(); }

		/**
		* @brief number of vertices of the graph
		* @returns: number of vertices (int type)
		* @details: internal use
		**/
		int num_vertices() const { return g_.num_vertices(); }

		/**
		* @brief number of vertices of the graph - consumer code
		* @returns: number of vertices (std::size_t type)
		* @details: for consumer code
		**/
		std::size_t size() const { return g_.size(); }

		std::size_t num_edges(bool lazy = true) { return g_.num_edges(lazy); }

		//////////////////////////
		// memory allocation 

		/**
		* @brief resets to empty graph given name and number of vertices
		* @param n number of vertices
		* @param name name of the instance
		* @details: fast-fail policy - exits if error
		**/
		void reset(std::size_t n, Weight val = DEFAULT_WEIGHT, string name = "");

	protected:
		/**
		* @brief resets to default values (does not deallocate memory)
		* @details: to deallocate do g = Base_Graph_W<xx>()
		* @detials: should not be called directly in the general case
		**/
		void reset() { g_.reset(); w_.clear(); }

		/////////////////////////
		//basic graph operations for simplicity
		//(calls directly graph services)

	public:
		/**
		* @brief adds edge (no self-loops allowed)
		**/
		void add_edge(int v, int w) { g_.add_edge(v, w); }

		double density(bool lazy = true) { return g_.density(lazy); }

		/**
		* @brief generates edges with probability p
		* @details: graph operation, no weights involved since
		*			edges are unweighted
		**/
		void gen_random_edges(double p) { g_.gen_random_edges(p); }


		/////////////
		// boolean properties

		bool is_edge(int v, int w) const { return g_.is_edge(v, w); }

		/**
		* @brief checks if the graph is unit-weighted
		* @returns true if all weights are 1.0, otherwise false
		* @details: a unit-weighted graph is equivalent to an unweighted graph
		*		    from a theoretical perspective
		**/
		bool is_unit_weighted();


		///////////////////////////
		//weight operations

			/**
			* @brief transforms (vertex) weights (excluding NO_WEIGHT values) using functor f
			* @param f functor
			**/
		template<class Func>
		void transform_weights(Func f);

		/**
		* @brief specific transformation of weights (excluding NO_WEIGHT values)
		*		 from positive to negative, i.e.,  w(i) = - w(i)
		**/
		void complement_weights();


		////////////////////////
		// other operations

			/*
			* @brief Complement graph (currently name info of original graph is lost)
			* @param g output graph
			* @returns 0 if success, -1 if error
			*/
		int create_complement(Base_Graph_W& g) const;

		////////////
		// I/O
	public:

		/**
		* @brief Writes directed graph to stream in dimacs format
		*
		*		 (self-loops are not considered)
		**/
		virtual ostream& write_dimacs(std::ostream& o = std::cout);

		/**
		* @brief Reads weighted undirected graph from file in DIMACS format
		* @param filename name of the file
		* @param type extension of additional filename for separate weights (Wext, Dext, WWWext, NOext)
		*		 default NOext - no additional file
		* @returns 0 if success, -1 if error
		**/
		int read_dimacs(string filename, int type = NOext);

		/**
		* @brief Reads weights from an external file (only weights)
		*
		*		 Format: numbers ordered and separated (line, space)
		*				  i.e. {5 6 7 ...} -> w(1)=5, w(2)=6, w(3)=7,...
		*
		*		 Weights not assigned in the file are set to 0.0
		*
		* @param filename name of the file
		* @returns 0 if success, -1 if error (empty vector of weights)
		**/
		int read_weights(string filename);

		std::ostream& print_data(bool lazy = true, std::ostream& o = std::cout, bool endl = true);
		std::ostream& print_edges(std::ostream& o = std::cout, bool eofl = false) { g_.print_edges(o, eofl); return o; }

		/**
		* @brief streams vertex-weights in the format [v:(val)]
		* @param o output stream
		* @param show_vert: if FALSE does not show vertex info but the collection of weights
		*					in order
		**/
		std::ostream& print_weights(std::ostream& o = std::cout, bool show_vert = true)	const;

		/**
		* @brief streams vertex-weights in the subset of vertices  bbsg
		**/
		std::ostream& print_weights(VertexBitset& bbsg, std::ostream& o = std::cout) const;

		/**
		* @brief prints the weights of the vertices in the stack lv
		* @param lv: a set of vertices with a stack interface
		**/
		std::ostream& print_weights(com::FixedStack<int>& lv, ostream& o = std::cout) const;

		/**
		* @brief prints the weights of the vertices in the FixedStack lv
		*		 given a mapping of the vertices
		* @param mapping: input mapping of the vertices with at least the same size as
		*				  the FixedStack lv
		* @param lv: a set of vertices with a FixedStack interface
		**/
		std::ostream& print_weights(com::FixedStack<int>& lv, const VertexMapping& mapping, std::ostream& o = std::cout)	const;

		/**
		* @brief prints the weights of the vertices in lv
		* @supports C-arrays
		**/
		std::ostream& print_weights(VertexList& lv, std::ostream& o = std::cout) const;
		std::ostream& print_weights(int* lv, int n, std::ostream& o = std::cout) const;

		/////////////////////////////////////
		// data members

	protected:
		graph_type g_;								//graph
		vector<Weight> w_;						//vector of weights 
	};

}//end namespace bitgraph

namespace bitgraph {

	///////////////////////
	//
	// Graph_W class 
	// (main template class to specialize for different types of graphs) 
	//
	///////////////////////

	template<class GraphT, class WeightT>
	class Graph_W : public Base_Graph_W <GraphT, WeightT> {};
}

////////////////////////////////////
// 
// Specialization for undirected graphs
//
////////////////////////////////////

namespace bitgraph {

	template<class WeightT>
	class Graph_W<ugraph, WeightT> : public Base_Graph_W<ugraph, WeightT> {
	public:
			
		using BaseT = Base_Graph_W<ugraph, WeightT>;					//parent type
		using graph_type = typename BaseT::graph_type;
		using bitset_type = typename BaseT::bitset_type;
		using Weight = typename BaseT::Weight;

		using BaseT::NO_WEIGHT;
		using BaseT::ZERO_WEIGHT;
		using BaseT::DEFAULT_WEIGHT;
		
		//alias types for backward compatibility
		//using _wt = weight_type;											
		using VertexBitset = bitset_type;

		//constructors (inherited from Base class)
		using BaseT::Base_Graph_W;

		/////////////
		//useful interface-specific for undirected weighted graphs
		int max_graph_degree() const { return this->g_.max_graph_degree(); }
		int degree(int v) const { return this->g_.degree(v); }
		int degree(int v, const Bitset& bbn) const { return this->g_.degree(v, bbn); }

		///////////
		//I/O operations

		/*
		* @brief Writes undirected graph to stream in dimacs format
		*
		*		 (self-loops are not considered)
		*/
		ostream& write_dimacs(ostream& o = std::cout) override;
	};
	
}//end namespace bitgraph


/////////////////////////////////////////////
//
// Necessary implementations in header file	

namespace bitgraph {

	template<class GraphT, class WeightT>
	template<class Func>
	inline
		void Base_Graph_W<GraphT, WeightT>::transform_weights(Func f)
	{
		auto NV = num_vertices();

		///////////////////////////////////////////////////////
		std::transform(w_.begin(), w_.end(), w_.begin(), f);
		///////////////////////////////////////////////////////

	}

}//end namespace bitgraph



#endif