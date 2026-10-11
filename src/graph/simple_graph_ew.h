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

#ifndef BITGRAPH_GRAPH_SIMPLE_GRAPH_EDGE_WEIGHTED_H
#define BITGRAPH_GRAPH_SIMPLE_GRAPH_EDGE_WEIGHTED_H

#include "graph_traits.h"
#include "weighted_constants.h"
#include "graph_unweighted.h"
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
	class Base_Graph_EW : public bitgraph::weight_constants<WeightT> {
		
	public:

		using graph_type = GraphT;
		using graph_t = graph_type;
		using bitset_type = typename GraphT::bitset_type;
		
		using vertex_bitset_t = bitset_type;
		using VertexBitset = vertex_bitset_t;		// Backward compatibility.
		
		using vertex_set_t = bitgraph::vertex_set_t;

		using weight_t = WeightT;
		using weights_t = std::vector<weight_t>;
		using weight_matrix_t = std::vector<std::vector<weight_t>>;
		
		
		using Weight = weight_t;				// backward compatibility
		using mat_t = weight_matrix_t;			// backward compatibility
										
		// constants		
		using constants_t = weight_constants<WeightT>;

		using constants_t::NO_WEIGHT;
		using constants_t::ZERO_WEIGHT;
		using constants_t::DEFAULT_WEIGHT;
					
				
		// enum for backward compatibility with previous versions
		// to remove - this class is only for edge-weighted graphs, so vertex weights are not relevant
		enum { VERTEX, EDGE, BOTH };


		///////////////////////
		// construction / destruction

		Base_Graph_EW() = default;
		
		/**
		 * @brief Constructs an edge-weighted graph from an existing graph and an
		 *        edge-weight matrix.
		 *
		 * Copies the topology of @p graph and the supplied edge weights.
		 *
		 * @param graph Graph whose topology is copied.
		 * @param edge_weights Edge-weight matrix.
		 *
		 * @warning Calls `std::terminate()` if the edge-weight matrix is invalid
		 *          or inconsistent with the graph topology.
		 */
		Base_Graph_EW(
			const graph_t& graph,
			const weight_matrix_t& edge_weights)
			: graph_(graph),
			edge_weights_(edge_weights)		
		{
			validate_weight_matrix();
		}

		/**
		 * @brief Constructs an edge-weighted graph from an existing graph.
		 *
		 * Copies the topology of @p graph and initializes the edge-weight matrix.
		 * Existing edges are assigned `DEFAULT_WEIGHT`, while non-edges are assigned
		 * `NO_WEIGHT`.
		 *
		 * @param graph Graph whose topology is copied.
		 */
		explicit Base_Graph_EW(const graph_t& graph)
			: graph_(graph)
		{
			initialize_edge_weights(DEFAULT_WEIGHT);
		}

		explicit Base_Graph_EW(std::size_t NV)
		{
			reset(NV);
		}

		/**
		* @brief reads graph from file. If ewights are not found it generated them
		*		 automatically based on the Pullman 2008 formula
		* @details currently only reads dimacs format (with or without weights)
		**/
		explicit Base_Graph_EW(const std::string& filename);															//read weighted ASCII file or generate weights using formula- CHECK! (21/11/2021))

		//copy constructor, move constructor, copy operator =, move operator =
		Base_Graph_EW(const Base_Graph_EW& g) = default;
		Base_Graph_EW(Base_Graph_EW&& g) noexcept = default;
		Base_Graph_EW& operator = (const Base_Graph_EW& g) = default;
		Base_Graph_EW& operator = (Base_Graph_EW&& g) noexcept = default;

		//destructor
		virtual	~Base_Graph_EW() = default;

		//////////////////////////
		// memory allocation (API)

		/**
		 * @brief Resets the graph to an empty edge-weighted graph.
		 *
		 * Reinitializes the graph with @p NV isolated vertices and initializes
		 * the entire edge-weight matrix to `NO_WEIGHT`. Any previous graph
		 * topology and edge weights are discarded.
		 *
		 * @param NV Number of vertices.
		 * @param name Optional graph instance name.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if the
		 *          graph or its edge-weight matrix cannot be reinitialized.
		 */
		void reset(std::size_t NV,
			string name = "") noexcept;


		/////////////
		// setters and getters	


		/**
		 * @brief Sets the weight of an existing directed edge.
		 *
		 * Assigns @p weight to the directed edge (@p v, @p w).
		 *
		 * @param v Source vertex.
		 * @param w Target vertex.
		 * @param weight New edge weight.
		 *
		 * @pre (@p v, @p w) must be an existing edge.
		 * @pre @p v and @p w must be different vertices.
		 * @pre @p weight must be different from `NO_WEIGHT`.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if any
		 *          precondition is violated.
		 */
		virtual	void set_weight(
			vertex_t v, 
			vertex_t w,
			weight_t val);


		/**
		*  @brief sets all edge-weights to val IF there is a corresponding edge.
		*		  Vertex weights are not modified.
		*  @param lw matrix of edge weights
		*  @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
		*						  (aditional cleaning)
		**/
		template<bool EraseNonEdges = false>
		void set_weights(Weight val = DEFAULT_WEIGHT);

		/**
		*  @brief sets edge weights in @lw  if there is a corresponding edge.
		*		  Vertex weights are not modified.
		*  @param edge_weights matrix of edge weights
		*  @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
		*						  (aditional cleaning)
		*  @details: asserts  @lw.size() == |V|
		**/
		template<bool EraseNonEdges = false>
		void set_weights(weight_matrix_t& edge_weights, bool edges_only = true);
		
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
		void set_modulus_edge_weights(int MODULUS = DEFAULT_WEIGHT_MODULUS);

		/**
		* @brief getter for edge-weights
		* @param v, Weight: edge
		* @details: no check for vertex existence
		**/
		Weight weight(int v, int w)	const { return edge_weights_[v][w]; }

		/**
		* @brief getter for vertex-weights
		* @param v: vertex
		* @details: no check for vertex existence
		**/
		Weight weight(int v) const { return edge_weights_[v][v]; }

		/*
		*  @brief getter for the specific subset of vertex-weights
		*/
		std::vector<Weight> vertex_weights() const;

		/**
		*  @brief getter for the all the weights (vertex and edge-weights)
		**/
		mat_t& weights() { return edge_weights_; }
		const mat_t& weights() const { return edge_weights_; }

		/**
		*  @brief getter for the graph (no weight information just edges)
		**/
		GraphT& graph() { return graph_; }
		const GraphT& graph() const { return graph_; }

		/**
		* @brief number of vertices of the graph
		* @returns: number of vertices (int type)
		* @details: internal use
		**/
		int num_vertices() const { return graph_.num_vertices(); }

		/**
		* @brief number of vertices of the graph - consumer code
		* @returns: number of vertices (std::size_t type)
		* @details: for consumer code
		**/
		std::size_t size() const { return graph_.size(); }

		std::size_t num_edges(bool lazy = true) { return graph_.num_edges(lazy); }

		/**
		* @brief neighbor set of vertex @v
		* @param v input vertex
		**/
		const vertex_bitset_t& neighbors(int v) const { return graph_.neighbors(v); }
		vertex_bitset_t& neighbors(int v) { return graph_.neighbors(v); }

		void set_name(std::string str) { graph_.set_name(str); }
		string name() const { return graph_.name(); }
		void set_path(std::string path_name) { graph_.set_path(path_name); }
		string path() const { return graph_.path(); }

		double density(bool lazy = true) { return graph_.density(lazy); }

			

		

		/////////////////////////
		// basic operations

			/**
			* @brief Adds an edge (v, w) with weight val
			*	      (if the edge exists, sets val)
			**/
		virtual	void add_edge(int v, int w, Weight val = ZERO_WEIGHT);

		


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

		bool is_edge(int v, int w) const { return graph_.is_edge(v, w); }

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

		
		////////////////////////
		// other operations

			/**
			* @brief created complement graph in @g
			* @param g output graph
			* @returns 0 if success, -1 if error
			**/
		void create_complement(Base_Graph_EW<graph_type, Weight>& g) const;

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
		std::ostream& print_weights(Vertices& lv, std::ostream& o = std::cout, int type = BOTH)	const;

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
		virtual	std::ostream& print_edge_weights(Vertices& lv, std::ostream& o = std::cout)	const;

		/**
		* @brief prints the vertices of the vertices of lv
		* @param lv: input set of vertices
		**/
		std::ostream& print_vertex_weights(Vertices& lv, std::ostream& o = std::cout) const;

	protected:

		/**
		 * @brief Validates the consistency of the edge-weight matrix.
		 *
		 * Checks that the edge-weight matrix is square, matches the number of graph
		 * vertices, and is consistent with the graph topology. Existing edges must
		 * have a weight different from `NO_WEIGHT`, while non-edges must have
		 * weight `NO_WEIGHT`.
		 *
		 * Self-loops and self-loop weights are not allowed.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if the
		 *          edge-weight matrix is invalid or inconsistent with the graph.
		 */
		void validate_weight_matrix() const;

		/**
		 * @brief Initializes the edge-weight matrix from the current graph topology.
		 *
		 * Resizes the edge-weight matrix to match the number of graph vertices.
		 * Existing edges are assigned @p weight, while non-edges are assigned
		 * `NO_WEIGHT`.
		 *
		 * @param weight Weight assigned to existing edges.
		 */
		void initialize_edge_weights(
			weight_t weight = DEFAULT_WEIGHT);
	
		/**
		 * @brief Resets the graph to an empty state.
		 *
		 * Removes all vertices, edges, and edge weights.
		 */
		void reset() {
			graph_.reset();
			edge_weights_.clear();
		}
		


		/////////////////
		// data members

		graph_t graph_;							
		weight_matrix_t edge_weights_;			// matrix of (only) edge-weights 																

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
// Necessary implementations in header file

#include "simple_graph_ew_imp.h"


#endif  // BITGRAPH_GRAPH_SIMPLE_GRAPH_EDGE_WEIGHTED_H