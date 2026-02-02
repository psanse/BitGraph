/**
 * @file simple_graph_ew.h
 *
 * @brief Generic base and implementation templates for edge-weighted graphs.
 *
 * This header defines the generic classes `Base_Graph_EW` and `Graph_EW`,
 * which provide support for **edge-weighted graphs** where **self-loops are
 * interpreted as vertex weights**.
 *
 * @details
 * The weighting model implemented in this file is intentionally restricted:
 *  - Edge weights are associated with edges {u, v}, u ≠ v
 *  - Self-loops (u, u) are interpreted as **vertex weights**
 *
 * Therefore, this implementation does **not** represent the most general case
 * of graphs with independent vertex and edge weights. Nevertheless, it is
 * sufficient for many applications where vertex weights can be encoded as
 * self-loops (e.g., maximum clique variants, train scheduling models, etc.).
 *
 * The class `Base_Graph_EW` provides common infrastructure shared across
 * different graph types and enables specialization of behavior according to
 * the underlying graph representation.
 *
 * The user-facing class is `Graph_EW<GraphT, WeightT>`, which extends
 * `Base_Graph_EW` with a public interface for edge-weighted graph operations.
 *
 * @note
 * This header contains **generic implementation code** and must not depend on
 * facade graph types. Facade-specific bindings (e.g., for `ugraph`) are defined
 * in higher-level headers.
 *
 * @par Historical notes
 *  - created: 16/01/2019
 *  - Milestone application: Train scheduling (10/11/2021)
 *  - Last update: 01/02/2026
 *
 * @author
 * Pablo San Segundo (pss)
 *
 * @copyright
 * This file is part of the GRAPH 1.0 C++ library.
 */

#ifndef __SIMPLE_GRAPH_EDGE_WEIGHTED_H__
#define __SIMPLE_GRAPH_EDGE_WEIGHTED_H__

#include "utils/common.h"
#include "graph_basic.h"
#include "simple_ugraph.h"
#include <iostream>
#include <vector>

namespace bitgraph {
	
	///////////////////////
	//
	// class Base_Graph_EW 
	// (used to be able to specialize functions according to the type of graph only)
	//
	// Note: user class is Graph_EW (see below) 
	// 
	// @todo there are some common parts with Base_Graph_W for weights in the vertices (12/12/2025)
	

	template<class GraphT, class WeightT>
	class Base_Graph_EW {
	public:
				
		using graph_type = GraphT;								// graph type
		using bitset_type = typename GraphT::bitset_type;		// bitset type used by graph type 
		using VertexBitset = bitset_type;						// alias for semantic type
		using Weight = WeightT;								
		
		using mat_t = vector<vector<Weight>>;					// type for a matrix of weights
		
		//enum to distinguish between vertex and edge weights
		enum { VERTEX, EDGE, BOTH };

		////////////////////////////
		//constants / globals

		static constexpr Weight NO_WEIGHT { -1 };					//possibly change sentinel to std::numeric_limits<WeightT>::max() ?		
		static constexpr Weight ZERO_WEIGHT{ 0 };
		static constexpr Weight DEFAULT_WEIGHT{ 1 };				//default weight value for weights (1.0)	

		//////////////////////////
		//constructors / destructor

		Base_Graph_EW() {};										//no memory allocation
		Base_Graph_EW(graph_type& g, mat_t& lwe) : g_(g), we_(lwe) {}										//creates graph from a non-weighted graph and a set of weights

		/**
		* @brief creates a graph with |V|= n and val weights as base line.
		* @param n number of vertices
		* @param val weight value
		* @param edge_weighted if TRUE creates an edge-weighted graph
		*			(vertex weights are set to NO_WEIGHT, instead of val)
		* @details: base line for all weights is set to val, i.e. vertex weights are
		*			set to val, but edge-weights will be overwritten when edges are added
		**/
		Base_Graph_EW(int n, Weight val = ZERO_WEIGHT, bool edge_weighted = false);						//creates empty graph with |V|= n and val weights	

		/**
		* @brief reads graph from file. If ewights are not found it generated them
		*		 automatically based on the Pullman 2008 formula
		* @details currently only reads dimacs format (with or without weights)
		**/
		explicit Base_Graph_EW(std::string filename);															//read weighted ASCII file or generate weights using formula- CHECK! (21/11/2021))

		//copy constructor, move constructor, copy operator =, move operator =
		Base_Graph_EW(const Base_Graph_EW& g) = default;
		Base_Graph_EW(Base_Graph_EW&& g) noexcept = default;
		Base_Graph_EW& operator = (const Base_Graph_EW& g) = default;
		Base_Graph_EW& operator = (Base_Graph_EW&& g)		noexcept = default;

		//destructor
		virtual	~Base_Graph_EW() = default;

		/////////////
		// setters and getters	

		/**
		* @brief getter for edge-weights
		* @param v, Weight: edge
		* @details: no check for vertex existence
		**/
		Weight weight(int v, int w)	const { return we_[v][w]; }

		/**
		* @brief getter for vertex-weights
		* @param v: vertex
		* @details: no check for vertex existence
		**/
		Weight weight(int v) const { return we_[v][v]; }

		/*
		*  @brief getter for the specific subset of vertex-weights
		*/
		std::vector<Weight> vertex_weights() const;

		/**
		*  @brief getter for the all the weights (vertex and edge-weights)
		**/
		mat_t& weights() { return we_; }
		const mat_t& weights() const { return we_; }

		/**
		*  @brief getter for the graph (no weight information just edges)
		**/
		GraphT& graph() { return g_; }
		const GraphT& graph() const { return g_; }

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

		/**
		* @brief neighbor set of vertex @v
		* @param v input vertex
		**/
		const VertexBitset& neighbors(int v) const { return g_.neighbors(v); }
		VertexBitset& neighbors(int v) { return g_.neighbors(v); }

		void set_name(std::string str) { g_.set_name(str); }
		string name() const { return g_.name(); }
		void set_path(std::string path_name) { g_.set_path(path_name); }
		string path() const { return g_.path(); }

		double density(bool lazy = true) { return g_.density(lazy); }

		//////////////////////////
		// memory allocation

	protected:
		/**
		* @brief clearing without deallocation - internal use only
		**/
		void reset() { g_.reset(); we_.clear(); }

	public:
		/**
		* @brief resets to a graph with |V|= n with weight val in everey vertex and edge
		* @param n number of vertices
		* @param name name of the instance
		* @details: fast-fail policy - exits if failure
		**/
		template<bool EdgeWeightedGraph = false>
		void reset(std::size_t n, Weight val = ZERO_WEIGHT, string name = "");					//before: int init (int n, Weight val = NO_WEIGHT, bool reset_name = true);

		/////////////////////////
		// basic operations

			/**
			* @brief Adds an edge (v, w) with weight val
			*	      (if the edge exists, sets val)
			**/
		virtual	void add_edge(int v, int w, Weight val = ZERO_WEIGHT);

		/**
		*  @brief sets vertex-weight
		*  @details: vertex-weights are stored as self-loop edge-weights
		**/
		void set_weight(int v, Weight val) { we_[v][v] = val; }

		/**
		*  @brief sets edge-weight to an EXISTNG given directed edge (v, w)
		*		  (in a non-edge it can only set weight to NO_WEIGHT)
		*  @param v input vertex
		*  @param Weight input vertex
		*  @param val input weight value

		*  @details: asserts it REALLY is an edge (and not a self-loop)
		*  @details: in a non-edge it can only set weight to NO_WEIGHT
		**/
		virtual	void set_weight(int v, int w, Weight val);

		/**
		*  @brief sets edge weights in @lw  if there is a corresponding edge.
		*		  Vertex weights are not modified.
		*  @param lw matrix of edge weights
		*  @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
		*						  (aditional cleaning)
		*  @details: asserts  @lw.size() == |V|
		**/
		template<bool EraseNonEdges = false>
		void set_weight(mat_t& lw, bool edges_only = true);

		/**
		*  @brief sets all vertex-weights (self-loop edge weights) to the same weight @val
		**/
		void set_vertex_weight(Weight val = ZERO_WEIGHT);

		/**
		*  @brief sets all edge-weights to val IF there is a corresponding edge.
		*		  Vertex weights are not modified.
		*  @param lw matrix of edge weights
		*  @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
		*						  (aditional cleaning)
		**/
		template<bool EraseNonEdges = false>
		void set_edge_weight(Weight val = ZERO_WEIGHT);

		/**
		* @brief sets new edge-weights based on modulus operation [Pullan 2008, MODULUS = 200]
		*
		*			I. we(v, w) = 1 + ((v + w) % MODULUS)	(1-based index)
		*
		*			II. vertex-weigths are not changed
		* @param template EraseNonEdges:  if TRUE, non-edges are set to NO_WEIGHT
		* @param MODULUS: modulus value [Pullan 2008, DEFAULT_WEIGHT_MODULUS = 200]
		**/
		template <bool EraseNonEdges = false>
		void set_modulus_edge_weight(int MODULUS = DEFAULT_WEIGHT_MODULUS);


		////////////////
		// Conversions 

		/**
		* @brief sets all vertex weights to NO_WEIGHT
		* @param erase_non_edges if TRUE, sets non-edges to NO_WEIGHT
		**/
		void make_edge_weighted(bool erase_non_edges = false);

		/**
		* @brief sets all non-edges to weight NO_WEIGHT.
		**/
		void erase_non_edge_weights();

		/////////////////////////
		// boolean properties

		bool is_edge(int v, int w) const { return g_.is_edge(v, w); }

		/*
		* @brief consistency check
		*
		*		 I. edges with NO_WEGHT weight values
		*
		* @returns true if consistent, false otherwise
		*
		*  TODO... other checks
		*/
		bool is_consistent();

		////////////////////////
		//weight operations

		/**
		* @brief transforms weights (excluding NO_WEIGHT values) using functor f
		* @param f functor
		* @param type: EDGE (edge-weights), VERTEX (vertex-weights) or BOTH
		**/
		template<class Func>
		void transform_weights(Func f, int type = BOTH);

		/**
		* @brief specific transformation of weights (excluding NO_WEIGHT values)
		*		 from positive to negative, i.e.,  we(i, j) = - we(i, j)
		* @param type: EDGE, VERTEX or BOTH
		**/
		void complement_weights(int type = BOTH);

		////////////////////////
		// other operations

			/**
			* @brief created complement graph in @g
			* @param g output graph
			* @returns 0 if success, -1 if error
			**/
		int create_complement(Base_Graph_EW<GraphT, Weight>& g) const;

		/**
		* @brief generates random edges uniformly with probability p and weight val
		**/
		virtual void gen_random_edges(double, Weight val);

		////////////
		// I/O 

		virtual std::ostream& print_data(bool lazy = true, std::ostream& o = std::cout, bool endl = true);

		/**
		* @brief prints the edge (v, w) in line format: [v]-(val)->[w], one edge per line
		**/
		std::ostream& print_edge(int v, int w, std::ostream& o = std::cout, bool endl = true) const;

		/**
		* @brief streams the vertex @v in format [v:(val)]
		**/
		std::ostream& print_vertex(int v, std::ostream& o = std::cout, bool endl = true) const;

		/**
		* @brief streams non-empty (excluding NO_WEIGHT value) weight info
		*		 for all directed edges.
		**/
		std::ostream& print_weights(std::ostream& o = std::cout, bool line_format = true, int type = BOTH)	const;

		/**
		* @brief streams non-empty (excluding NO_WEIGHT value)  weights
		*		 of directed edges induced by the subgraph of vertices in lv
		**/
		std::ostream& print_weights(VertexList& lv, std::ostream& o = std::cout, int type = BOTH)	const;

		///////////
		// read/write operations

		/**
		* @brief Reads weighted directed graph from file in DIMACS format
		*
		*			I. vertex weights are read in lines n <v> <w>, if missing, weights are set to NO_WEIGHT
		*
		*			II. if the file has edges with no edge weights, weights are set to NO_WEIGHT
		*
		* @param filename name of the file
		* @returns 0 if success, -1 if error
		**/
		int read_dimacs(std::string filename);

		/**
		* @brief Writes directed graph to stream in dimacs format
		*
		*		 I. weights in self-loops are considered vertex weights
		*
		*		II. vertex weights with value NO_WEIGHT are not written to file
		*
		*		III.For every edge, a weight is written (including NO_WEIGHT)
		*
		* @param o output stream
		* @returns output stream
		**/
		virtual	std::ostream& write_dimacs(std::ostream& o);

		/////////////////////
		//private I/O

	protected:
		/**
		* @brief prints the edges of the graph in line format [v]-(val)->[w], one edge per line
		**/
		virtual std::ostream& print_edges(std::ostream& o = std::cout, bool eofl = true) const;

		/**
		* @brief prints edges in different formats
		* @param line_format: if TRUE, prints one edge per line, otherwise in matrix format
		**/
		virtual	std::ostream& print_edge_weights(std::ostream& o = std::cout, bool line_format = true) const;
		std::ostream& print_vertex_weights(std::ostream& o = std::cout) const;

		/**
		* @brief prints the edges of the graph induced by the vertices of lv in line format
		* @param lv: input set of vertices of the induced subgraph
		**/
		virtual	std::ostream& print_edge_weights(VertexList& lv, std::ostream& o = std::cout)	const;

		/**
		* @brief prints the vertices of the vertices of lv
		* @param lv: input set of vertices
		**/
		std::ostream& print_vertex_weights(VertexList& lv, std::ostream& o = std::cout) const;

		///////////////////
		//data members

	protected:
		GraphT g_;
		mat_t   we_;								//matrix of vertex and edge-weights 																

	}; //end of class Base_Graph_EW


}//end of namespace bitgraph


namespace bitgraph {

	////////////////////////
	//
	// Graph_EW class 
	// 
	// I.  main template class to specialize for different types of graphs
	// II. currently only defined for ugraph type
	//
	///////////////////////

	template <class GraphT, class WeightT>
	class Graph_EW : public Base_Graph_EW<GraphT, WeightT> {};
}

//////////////////////////////////////////////////////////////////
//  
// Necessary implementations in header file

namespace bitgraph {

	template <class GraphT, class WeightT>
	template <bool EraseNonEdges>
	inline
		void Base_Graph_EW< GraphT, WeightT>::set_weight(mat_t& lw, bool edges_only) {

		auto NV = num_vertices();

		/////////////////////////
		assert(lw.size() == NV);
		/////////////////////////

		/*if (lw.size() != NV) {
			LOG_ERROR("bizarre matrix of weights-Base_Graph_EW<GraphT,WeightT >::set_edge_weight(mat_t...)");
			return -1;
		}*/

		//set to empty wv and non-edges
		for (int v = 0; v < NV; ++v) {
			for (int w = 0; w < NV; ++w) {

				if (edges_only && v == w) continue;			//skips vertex weights if required

				if (g_.is_edge(v, w)) {
					we_[v][w] = lw[v][w];
				}
				else {
					//cleans non-edge weights if required
					if (EraseNonEdges) { we_[v][w] = NO_WEIGHT; }
				}
			}
		}
	}

	template<class GraphT, class WeightT>
	template<class Func>
	inline
		void Base_Graph_EW<GraphT, WeightT>::transform_weights(Func f, int type)
	{
		auto NV = num_vertices();

		switch (type) {
			//edge-weights
		case EDGE:
			for (auto i = 0; i < NV - 1; ++i) {
				for (auto j = i + 1; j < NV; ++j) {
					if (we_[i][j] != NO_WEIGHT) {
						we_[i][j] = f(we_[i][j]);
					}
					if (we_[j][i] != NO_WEIGHT) {
						we_[j][i] = f(we_[j][i]);
					}
				}
			}
			break;
			//vertex-weights
		case VERTEX:
			for (auto i = 0; i < NV; ++i) {
				if (we_[i][i] != NO_WEIGHT) {
					we_[i][i] = f(we_[i][i]);
				}
			}
			break;
		case BOTH:
			//vertex and edge-weights
			for (auto i = 0; i < NV; ++i) {
				for (auto j = 0; j < NV; ++j) {
					if (we_[i][j] != NO_WEIGHT) {
						we_[i][j] = f(we_[i][j]);
					}
				}
			}
			break;
		default:
			//should not happen	
			LOG_ERROR("unknown type -  Base_Graph_EW<GraphT, WeightT>::transform_weights");
			LOG_ERROR("exiting");
			std::exit(EXIT_FAILURE);
		}
	}

	template <class GraphT, class WeightT>
	template<bool EraseNonEdges>
	inline
		void Base_Graph_EW< GraphT, WeightT>::set_edge_weight(Weight val) {

		auto NV = num_vertices();

		//set to empty weight the non-edges
		for (int v = 0; v < NV; v++) {
			for (int w = 0; w < NV; w++) {
				if (g_.is_edge(v, w)) {
					we_[v][w] = val;
				}
				else {
					if (EraseNonEdges) { we_[v][w] = NO_WEIGHT; }
				}
			}
		}
	}

	template<class GraphT, class WeightT>
	template <bool EraseNonEdges>
	inline
		void Base_Graph_EW<GraphT, WeightT>::set_modulus_edge_weight(int MODULUS) {

		int NV = num_vertices();

		for (int v = 0; v < NV; ++v) {
			for (int w = 0; w < NV; ++w) {

				if (g_.is_edge(v, w)) {

					///////////////////////////////////////
					set_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
					///////////////////////////////////////
				}
				else {
					if (EraseNonEdges) {
						set_edge_weight(v, w, NO_WEIGHT);
					}
				}
			}
		}
	}

	
	template<class GraphT, class WeightT>
	template<bool EdgeWeightedGraph>
	inline
		void Base_Graph_EW<GraphT, WeightT>::reset(std::size_t NV, Weight val, string name)
	{
		///////////////
		g_.reset(NV);
		////////////////
		
		try {
			we_.assign(NV, vector<WeightT>(NV, val));
		}
		catch (...) {
			LOG_ERROR("bad weight assignment - Base_Graph_EW<GraphT, Weight>::reset");
			LOG_ERROR("exiting");
			std::exit(EXIT_FAILURE);
		}

		//set vertex weights to NO_WEIGHT if required
		if (EdgeWeightedGraph) {
			for (auto v = 0u; v < NV; ++v) {
				we_[v][v] = NO_WEIGHT;
			}
		}

		g_.set_name(name);
	
	}

}//end of namespace bitgraph



#endif