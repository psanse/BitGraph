/**
 * @file simple_graph_ew.h
 *
 * @brief Generic base and implementation templates for edge-weighted graphs.
 *
 * This header defines the generic classes `Base_Graph_EW` and `Graph_EW`,
 * which provide support for edge-weighted graphs.
 *
 * @details
 * Edge weights are stored in a weight matrix synchronized with the topology
 * of the underlying graph. Existing edges have a weight different from
 * `NO_WEIGHT`, while entries corresponding to non-edges are set to
 * `NO_WEIGHT`.
 *
 * Self-loops are not supported; consequently, diagonal entries of the
 * edge-weight matrix are always `NO_WEIGHT`.
 *
 * `Base_Graph_EW` provides the common implementation independently of the
 * concrete graph representation and serves as the base for graph-specific
 * specializations.
 *
 * The user-facing class is `Graph_EW<GraphT, WeightT>`, which extends
 * `Base_Graph_EW` with graph-specific behavior where required.
 *
 * @note This header contains generic implementation code and must not depend
 *       on facade graph types. Facade-specific bindings and specializations
 *       (e.g., for `ugraph`) are defined in higher-level headers.
 *
 * @par Historical notes
 * - Created: 16/01/2019
 * - Milestone application: Train scheduling (10/11/2021)
 * - Last update: 12/10/2026
 *
 * @author Pablo San Segundo (pss)
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
	
	/**
	 * @brief Generic base implementation for edge-weighted graphs.
	 *
	 * Associates an edge-weight matrix with an underlying graph of type
	 * @p GraphT. Existing edges have a weight different from `NO_WEIGHT`,
	 * while entries corresponding to non-edges are set to `NO_WEIGHT`.
	 *
	 * This class provides the common implementation of edge-weighted graph
	 * operations independently of the concrete graph type. Graph-specific
	 * behavior can be provided by specializations of `Graph_EW`.
	 *
	 * The user-facing class is `Graph_EW`; `Base_Graph_EW` is intended primarily
	 * as an implementation base and specialization layer.
	 *
	 * @tparam GraphT Underlying graph type.
	 * @tparam WeightT Type used to represent edge weights.
	 *
	 * @invariant The edge-weight matrix is square and its dimensions match the
	 *            number of graph vertices.
	 * @invariant An entry `(v,w)` is different from `NO_WEIGHT` if and only if
	 *            `(v,w)` is an existing edge.
	 * @invariant Self-loops are not allowed and diagonal entries are `NO_WEIGHT`.
	 *
	 * @todo Review common functionality shared with the vertex-weighted graph
	 *       implementation (`Base_Graph_VW`).
	 */
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
					
		
		enum class edge_print_format {
			edge_list,
			edge_matrix
		};

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
			validate_weight_matrix_base();
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
		 * @brief Constructs an edge-weighted graph from a DIMACS file.
		 *
		 * @param[in] filename Path to the input DIMACS file.
		 *
		 * @details Reads a graph in DIMACS format, with or without edge weights.
		 *          If weights are not provided, they are generated automatically
		 *          using the Pullman (2008) formula.
		 *
		 * @note Fails fast if the file cannot be read or contains invalid data.
		 */
		explicit Base_Graph_EW(const std::string& filename);														

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
			std::string name = "") noexcept;


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
			weight_t weight);

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
		virtual void set_weights(weight_t weight = DEFAULT_WEIGHT);
		
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
		virtual void set_modulus_weights(
			int modulus = DEFAULT_WEIGHT_MODULUS);

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

		std::size_t num_edges(bool lazy = true) const {
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
		* @brief Reads an edge-weighted graph from a file in DIMACS format
		*
		*			I.   edges are read from lines `e <v> <w> [<weight>]` (1-based vertices);
		*			     if no edge in the file has a weight, weights are generated with
		*			     set_modulus_weights() (Pullan 2008); otherwise an edge with no
		*			     weight is assigned DEFAULT_WEIGHT
		*
		*			II.  vertex-weight lines (`n`/`v`) are ignored: vertex weights are not
		*			     supported and the diagonal of the weight matrix stays NO_WEIGHT
		*
		*			III. edges are inserted through the (virtual) add_edge(), so graph-specific
		*			     behaviour (e.g. symmetric weights in undirected graphs) is preserved
		*
		* The input is rejected if the header is invalid, the number of edges differs
		* from the header, a vertex is out of range, an edge is a self-loop, an edge
		* weight is unreadable or equals NO_WEIGHT, or an unknown record tag is found.
		*
		* @param filename name of the file
		* @returns 0 if success, -1 if error
		*
		* @note On failure the graph is reset to an empty state.
		* @note When called from a constructor of this class, add_edge() is not
		*       virtual: derived classes with different add_edge() semantics must
		*       call read_dimacs() from their own constructors.
		**/
		int read_dimacs(const std::string& filename);
		
		/**
		 * @brief Prints graph information to an output stream.
		 *
		 * Prints the underlying graph data and appends a tag identifying the graph
		 * as edge-weighted.
		 *
		 * @param lazy Passed to the underlying graph `print_data()` operation.
		 * @param os Output stream.
		 * @param trailing_newline If `true`, appends a newline after the output.
		 * @return Reference to the output stream.
		 */
		virtual std::ostream& print_data(
			bool lazy = true,
			std::ostream& os = std::cout,
			bool trailing_newline = true) const;

		/**
		 * @brief Prints an edge and its weight.
		 *
		 * If (@p v, @p w) is an existing edge, prints it in the form
		 * `[v]--(weight)-->[w]`. Otherwise, prints
		 * `[v]--(NO_EDGE)-->[w]`.
		 *
		 * @param v Source vertex.
		 * @param w Target vertex.
		 * @param os Output stream.
		 * @param trailing_newline If `true`, appends a newline after the output.
		 * @return Reference to @p os.
		 */
		virtual std::ostream& print_edge(
			vertex_t v, 
			vertex_t w, 
			std::ostream& os = std::cout, 
			bool trailing_newline = true) const;

		/**
		 * @brief Prints the edges of the graph and their weights.
		 *
		 * Edges are written using the format specified by @p format. In
		 * `edge_list` mode, each existing edge is printed individually. In
		 * `edge_matrix` mode, the complete edge-weight matrix is printed, using
		 * `NO_WEIGHT` entries to represent non-edges.
		 *
		 * @param os Output stream.
		 * @param format Edge-printing format.
		 * @param trailing_newline If `true`, appends an additional newline after
		 *                         the complete output.
		 *
		 * @return Reference to @p os.
		 */
		virtual std::ostream& print_edges(
			std::ostream& os = std::cout,
			edge_print_format format = edge_print_format::edge_list,
			bool trailing_newline = false) const;

		/**
		 * @brief Prints the edges induced by a set of vertices.
		 *
		 * Prints all existing directed edges whose endpoints both belong to
		 * @p vertices, together with their corresponding edge weights.
		 *
		 * @param vertices Set of vertices defining the induced subgraph.
		 * @param os Output stream.
		 *
		 * @return Reference to @p os.
		 */
		std::ostream& print_edges(
			vertex_set_t& vertices,
			std::ostream& o = std::cout) const;
					

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
		virtual void validate_weight_matrix_base(
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
		void validate_weight_matrix_base() const {
			validate_weight_matrix_base(edge_weights_);
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
		* @brief Returns mutable access to the edge-weight matrix.
		*
		* @return Reference to the internal edge-weight matrix.
		*
		* @note Intended for derived classes and internal algorithms.
		*/
		weight_matrix_t& mutable_weights() noexcept {
			return edge_weights_;
		}

		/**
		* @brief Returns mutable access to the neighborhood of a vertex.
		*
		* @param v Vertex whose neighborhood is requested.
		* @return Reference to the neighborhood bitset of `v`.
		*
		* @note Intended for derived classes and internal algorithms.
		*/
		vertex_bitset_t& neighbors(vertex_t v) noexcept {
			return graph_.neighbors(v);
		}
						
	
		/**
		 * @brief Resets the graph to an empty state.
		 *
		 * Removes all vertices, edges, and edge weights.
		 */
		void reset() {
			graph_.reset();
			edge_weights_.clear();
		}

	private:
		/**
		 * @brief Reports a fatal graph initialization error.
		 *
		 * @param[in] message Description of the error.
		 *
		 * @note Terminates the program without returning.
		 */
		[[noreturn]]
		void graph_initialization_error(const std::string& message)
		{
			std::cerr << "[BITGRAPH ERROR] " << message << '\n';
			std::terminate();
		}
		
		/////////////////
		// data members

	protected:
		/**
		 * @brief Underlying graph.
		 */
		graph_t graph_;	

		/**
		 * @brief Edge-weight matrix.
		 *
		 * Entry `edge_weights_[v][w]` stores the weight associated with the edge
		 * (v, w). Non-edges are represented by `NO_WEIGHT`.
		 */
		weight_matrix_t edge_weights_;																			

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