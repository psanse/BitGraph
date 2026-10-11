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
		 * @brief Sets all existing edge weights to the same value.
		 *
		 * Assigns @p weight to every existing edge. Entries corresponding to
		 * non-edges are set to `NO_WEIGHT`.
		 *
		 * @param weight Weight assigned to every existing edge.
		 *
		 * @pre @p weight must be different from `NO_WEIGHT`.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if
		 *          @p weight is `NO_WEIGHT`.
		 */
		void set_weights(weight_t weight = DEFAULT_WEIGHT);
		
		/**
		 * @brief Replaces the edge-weight matrix.
		 *
		 * Sets the edge weights from @p edge_weights. The matrix must be square,
		 * match the number of graph vertices, and be consistent with the current
		 * graph topology: existing edges must have a weight different from
		 * `NO_WEIGHT`, while non-edges must have weight `NO_WEIGHT`.
		 *
		 * @param edge_weights New edge-weight matrix.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if
		 *          @p edge_weights is invalid or inconsistent with the graph.
		 */
		void set_weights(const weight_matrix_t& edge_weights);

		/**
		 * @brief Assigns Pullan-style modulus-based weights to all existing edges.
		 *
		 * Sets each edge weight according to a cyclic rule based on the sum of
		 * its endpoint indices. For 0-based vertex indices, the generated weights
		 * lie in the range `[2, modulus]`:
		 *
		 * `weight(v,w) = 2 + ((v + w) % (modulus - 1))`.
		 *
		 * Non-edges retain weight `NO_WEIGHT`.
		 *
		 * @param modulus Upper bound of the generated edge-weight range.
		 *
		 * @pre @p modulus must be at least 2.
		 *
		 * @note This weighting scheme follows the convention used in the Pullan
		 *       benchmark generators for weighted graph instances.
		 * @note Although the parameter is named @p modulus, the actual modulus
		 *       used in the formula is `modulus - 1`; @p modulus denotes the
		 *       maximum generated weight.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if
		 *          @p modulus is less than 2.
		 */
		void set_modulus_weights(int modulus = DEFAULT_WEIGHT_MODULUS);

		/**
		 * @brief Returns the underlying graph.
		 *
		 * Provides read-only access to the graph topology.
		 *
		 * @return Const reference to the underlying graph.
		 */
		const graph_t& graph() const noexcept {
			return graph_;
		}
	
		/**
		 * @brief Returns mutable access to the underlying graph.
		 *
		 * This accessor is intended for internal use by algorithms that require
		 * direct access to the wrapped graph.
		 *
		 * @return Reference to the underlying graph.
		 *
		 * @note Mutable access is not part of the public API contract.
		 *       Modifying the graph directly may invalidate invariants maintained
		 *       by the weighted-graph wrapper.
		 */
		graph_t& graph() { return graph_; }
		
		/**
		 * @brief Returns the weight of an edge.
		 *
		 * @param v Source vertex.
		 * @param w Target vertex.
		 * @return Weight associated with the edge (@p v, @p w).
		 *
		 * @note No bounds checking is performed.
		 */
		weight_t weight(vertex_t v, vertex_t w)	const noexcept{
			return edge_weights_[static_cast<size_t>(v)]
								[static_cast<size_t>(w)];
		}

		/**
		 * @brief Returns the complete edge-weight matrix.
		 *
		 * Provides read-only access to the stored edge weights.
		 *
		 * @return Const reference to the edge-weight matrix.
		 */
		const weight_matrix_t& weights() const noexcept {
			return edge_weights_;
		}
				
		void set_name(std::string name) {
			graph_.set_name(std::move(name));
		}
		const std::string& name() const noexcept {
			return graph_.name(); 
		}
		void set_path(std::string path) {
			graph_.set_path(std::move(path));
		}
		const std::string& path() const noexcept {
			return graph_.path();
		}
		
		/////////////////////////
		// Basic graph operations
		// Mostly convenience wrappers around the underlying graph API.

		/**
		* @brief number of vertices of the graph
		* @returns: number of vertices (int type)
		* @details: internal use
		**/
		int num_vertices() const {
			return graph_.num_vertices();
		}

		/**
		* @brief number of vertices of the graph - consumer code
		* @returns: number of vertices (std::size_t type)
		* @details: for consumer code
		**/
		std::size_t size() const {
			return graph_.size();
		}

		std::size_t num_edges(bool lazy = true) {
			return graph_.num_edges(lazy);
		}

		/**
		 * @brief Adds an edge with the specified weight.
		 *
		 * Adds the edge (@p v, @p w) to the graph and assigns @p weight to it.
		 * If the edge already exists, its weight is replaced by @p weight.
		 *
		 * @param v Source vertex.
		 * @param w Target vertex.
		 * @param weight Weight assigned to the edge.
		 */
		virtual	void add_edge(
			vertex_t v, 
			vertex_t w, 
			weight_t weight = DEFAULT_WEIGHT);
	
		/**
		 * @brief Disabled maximum edge-weight query.
		 *
		 * This overload is intentionally unavailable for edge-weighted graphs.
		 *
		 * @param v First endpoint of the edge.
		 * @param w Second endpoint of the edge.
		 *
		 * @return This function is deleted and cannot be called.
		 */
		weight_t maximum_weight(vertex_t& v, vertex_t& w) const = delete;

		/////////////
		// Boolean properties

		/**
		 * @brief Tests whether two vertices are adjacent.
		 *
		 * @param v First vertex.
		 * @param w Second vertex.
		 * @return `true` if `(v,w)` is an edge; otherwise, `false`.
		 */
		bool is_edge(vertex_t v, vertex_t w) const {
			return graph_.is_edge(v, w);
		}

		/**
		 * @brief Checks whether the graph is unit edge-weighted.
		 *
		 * An edge-weighted graph is unit-weighted if every existing edge has weight
		 * equal to `DEFAULT_WEIGHT`. Non-edges are ignored.
		 *
		 * @return `true` if all existing edges have weight `DEFAULT_WEIGHT`;
		 *         otherwise, `false`.
		 *
		 * @note An edgeless graph is considered unit-weighted.
		 */
		bool is_unit_weighted() const noexcept;


		/**
		 * @brief Checks whether the edge-weighted graph is internally consistent.
		 *
		 * Verifies that the edge-weight matrix matches the graph topology and
		 * satisfies the class invariants. In particular, the matrix dimensions
		 * must match the number of vertices, existing edges must have a weight
		 * different from `NO_WEIGHT`, and non-edges must have weight `NO_WEIGHT`.
		 *
		 * @return `true` if the graph and edge-weight matrix are consistent;
		 *         otherwise, `false`.
		 */
		bool is_consistent() const noexcept;

		////////////////////////
		// Edge weight operations

		/**
		 * @brief Applies a transformation to all existing edge weights.
		 *
		 * Replaces each existing edge weight `w` with `f(w)`. Entries corresponding
		 * to non-edges remain equal to `NO_WEIGHT`.
		 *
		 * @tparam Func Unary callable type accepting a `weight_t` and returning
		 *              a value convertible to `weight_t`.
		 * @param f Transformation applied to each existing edge weight.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if the
		 *          transformation produces `NO_WEIGHT` for an existing edge.
		 */
		template<class Func>
		void transform_weights(Func f);
		
		////////////////////////
		// other operations		
	
		/**
		 * @brief Generates a random edge set with fixed cardinality and assigns a common weight.
		 *
		 * Replaces the current edge set with a uniformly random subset of
		 * `M = round(p * C(NV, 2))` undirected edges, where `NV` is the number of
		 * vertices. All generated edges are assigned @p weight, while non-edges
		 * are assigned `NO_WEIGHT`.
		 *
		 * @param p Target edge density in the range `[0, 1]`.
		 * @param weight Weight assigned to every generated edge.
		 *
		 * @note This implements a fixed-edge-count random graph model of the
		 *       `G(n,m)` type, with
		 *       `m = round(p * C(n,2))`, rather than the independent-edge
		 *       Erdos-Renyi `G(n,p)` model.
		 *
		 * @warning Follows the validation policy of the underlying graph generator
		 *          and `set_weights()`.
		 */
		void gen_random_edges(double p, weight_t weight = DEFAULT_WEIGHT);

		////////////
		// I/O 

		/**
		 * @brief Writes the edge-weighted graph to an output stream in DIMACS format.
		 *
		 * Writes the DIMACS header followed by one record for each existing edge.
		 * Each edge record contains the 1-based endpoint identifiers and the
		 * corresponding edge weight.
		 *
		 * @param os Output stream.
		 * @return Reference to @p os.
		 *
		 * @note This generic implementation writes directed edges. For undirected
		 *       graphs, a specialization should avoid writing each edge twice.
		 */
		virtual	std::ostream& write_dimacs(
			std::ostream& os = std::cout) const;


		/**
		* @brief Reads weighted directed graph from file in DIMACS format
		*
		*			I. vertex weights are read in lines n <v> <w>, if missing, weights are set to NO_WEIGHT
		*
		*			II. if the file has edges with no edge weights, weights are set to NO_WEIGHT
		*
		* @param filename name of the file
		* @returns 0 if success, -1 if error
		* 
		* TODO - Check (12/10/2026)
		**/
		int read_dimacs(const std::string& filename);
		
		virtual std::ostream& print_data(
			bool lazy = true,
			std::ostream& os = std::cout,
			bool trailing_newline = true);

		/**
		* @brief prints the edge (v, w) in line format: [v]-(val)->[w], one edge per line
		**/
		std::ostream& print_edge(
			vertex_t v, 
			vertex_t w, 
			std::ostream& os = std::cout, 
			bool trailing_newline = true) const;

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
		 * @brief Validates an edge-weight matrix against the current graph topology.
		 *
		 * Checks that @p edge_weights is square, matches the number of graph
		 * vertices, and is consistent with the current graph topology. Existing
		 * edges must have a weight different from `NO_WEIGHT`, while non-edges
		 * must have weight `NO_WEIGHT`.
		 *
		 * @param edge_weights Edge-weight matrix to validate.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if
		 *          @p edge_weights is invalid or inconsistent with the graph.
		 */
		void validate_weight_matrix(
			const weight_matrix_t& edge_weights) const;


		/**
		 * @brief Validates the current edge-weight matrix.
		 *
		 * Checks that the stored edge-weight matrix is square, matches the number
		 * of graph vertices, and is consistent with the current graph topology.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if the
		 *          current edge-weight matrix is invalid or inconsistent with the graph.
		 */
		void validate_weight_matrix() const {
			validate_weight_matrix(edge_weights_);
		}
			
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