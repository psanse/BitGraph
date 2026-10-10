/**
 * @file simple_graph_vw.h
 * @brief Vertex-weighted graph classes built on simple graph types.
 *
 * Defines `Base_Graph_W` and `Graph_W`, which provide vertex-weighted
 * graph functionality on top of the underlying graph representation.
 *
 * @author pss
 * @details Created 16/01/2019, last updated 09/10/2026.
 *
 * This code is part of the BITGRAPH C++ library.
 */

#ifndef BITGRAPH_GRAPH_SIMPLE_GRAPH_VERTEX_WEIGHTED_H
#define BITGRAPH_GRAPH_SIMPLE_GRAPH_VERTEX_WEIGHTED_H


#include "graph_types.h"
#include <iostream>
#include <vector>
#include <utility>

namespace bitgraph {
	
	/**
	 * @brief Base implementation for vertex-weighted graph classes.
	 *
	 * Provides the common functionality required by `Graph_W`, while keeping
	 * the underlying graph type and weight type generic.
	 *
	 * This base class exists primarily to enable specialization of operations
	 * with respect to the underlying graph type.
	 *
	 * @tparam GraphT Underlying graph type.
	 * @tparam WeightT Vertex-weight type.
	 *
	 * @note `Graph_W` is the intended user-facing graph type.
	 */

	template<class GraphT, class WeightT>
	class Base_Graph_W {
			
	public:
		
		using graph_type = GraphT;							// graph type	
		using graph_t = graph_type;
		using bitset_type = typename GraphT::bitset_type;

		using vertex_bitset_t = bitset_type;
		using VertexBitset = vertex_bitset_t;				// backward compatibility
		
		using vertex_set_t = bitgraph::vertex_set_t;

		using weight_t = WeightT;
		using Weight = weight_t;							// backward compatibility

		using weights_t = std::vector<weight_t>;			

		
		static constexpr weight_t NO_WEIGHT{ -1 };			// valid weights are non-negative
		static constexpr weight_t ZERO_WEIGHT{ 0 };
		static constexpr weight_t DEFAULT_WEIGHT{ 1 };

		// file extensions for weighted graphs (for I/O operations)
		// see read_dimacs functions (check)
		enum class weight_file_extension { 
			w = 0,
			d,
			www,
			none
		};		
		
		// present for backward compatibility with existing code
		enum { Wext = 0, Dext, WWWext, NOext };				

		///////////////////////
		// construction / destruction
		
		Base_Graph_W() {};																		
		explicit Base_Graph_W(weights_t& lw)
			: vertex_weights_(lw) 
		{
			graph_.reset(lw.size()); 
		}	
	
		Base_Graph_W(graph_t& graph, weights_t& weights)
			:graph_(graph), vertex_weights_(weights) 
		{ 
			assert(vertex_weights_.size() == graph_.size());
		}									

		Base_Graph_W(graph_t& graph)
			:graph_(graph), 
			vertex_weights_(graph.size(), 
			DEFAULT_WEIGHT )
		{}							

		explicit Base_Graph_W(int N, weight_t weight = DEFAULT_WEIGHT)
		{ 
			reset(N, weight);
		}		


		/**
		* @brief Reads weighted graph from ASCII file in DIMACS format
		*
		*		 If unable to read weights generates modulus weights [Pullman 2008]
		*
		*		 TODO: add support for other formats
		**/
		Base_Graph_W(const std::string& filename);

		//copy constructor, move constructor, copy operator =, move operator =
		Base_Graph_W(const Base_Graph_W& g) = default;
		Base_Graph_W(Base_Graph_W&& g) noexcept = default;
		Base_Graph_W& operator = (const Base_Graph_W& g) = default;
		Base_Graph_W& operator = (Base_Graph_W&& g)	 noexcept = default;

		//destructor
		virtual	~Base_Graph_W() = default;

		/////////////
		// setters and getters

		void set_weight(vertex_t v, weight_t value) noexcept{ 
			vertex_weights_[v] = value;
		}
		void set_weight(weight_t value = DEFAULT_WEIGHT) {
			vertex_weights_.assign(
				static_cast<std::size_t>(graph_.num_vertices()),
				value);
		}

		/**
		 * @brief Sets the weights of all vertices.
		 *
		 * @param weights Vertex weights. The vector size must equal the number
		 *                of vertices in the graph.
		 *
		 * @warning Calls `std::terminate()` if the number of weights does not
		 *          match the number of vertices.
		 */
		void set_weight(const std::vector<weight_t>& weights);

		/**
		 * @brief Sets the weights of all vertices by moving a weight vector.
		 *
		 * @param weights Vertex weights. The vector size must equal the number
		 *                of vertices in the graph.
		 *
		 * @warning Calls `std::terminate()` if the number of weights does not
		 *          match the number of vertices.
		 */
		void set_weight(std::vector<weight_t>&& weights);

		/**
		 * @brief Assigns vertex weights using the Pullan modulus scheme.
		 *
		 * For a 0-based internal vertex index `v`, the weight is
		 *
		 * \f[
		 *   w(v) = ((v + 1) \bmod modulus) + 1.
		 * \f]
		 *
		 * This corresponds to the DIMACS convention of Pullan (2008),
		 * where vertices are numbered from 1 and vertex `i` receives
		 * weight `i mod modulus + 1`.
		 *
		 * @param modulus Modulus used to generate the vertex weights.
		 *
		 * @warning Calls `std::terminate()` if `modulus <= 0`.
		 *
		 * @note For 0-based internal indexing, the generated weight sequence
		 *       starts at 2. Pullan (2008) uses `modulus = 200` for the
		 *       DIMACS-VW instances.
		 */
		void set_modulus_weights(int modulus = bitgraph::DEFAULT_WEIGHT_MODULUS);

		const graph_t& graph() const {
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
		graph_t& graph() noexcept {
			return graph_;
		}

		weight_t weight(vertex_t v) const noexcept{ 
			return vertex_weights_[v];
		}

		const std::vector<weight_t>& weight() const noexcept { 
			return vertex_weights_;
		}
		
		void set_name(std::string name) {
			graph_.set_name(name);
		}

		const std::string& name() const noexcept {
			return graph_.name();
		}

		void set_path(std::string path) {
			graph_.set_path(path);
		}

		const std::string& path() const noexcept {
			return graph_.path();
		}

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
		 * @brief Finds a vertex of maximum weight.
		 *
		 * @param v Output vertex attaining the maximum weight.
		 * @return Weight of vertex `v`.
		 *
		 * @warning Calls `std::terminate()` if the graph has no vertex weights.
		 */
		weight_t maximum_weight(vertex_t& v) const;

		const vertex_bitset_t& neighbors(vertex_t v) const { 
			return graph_.neighbors(v); 
		}
		
		//////////////////////////
		// memory allocation 

		/**
		 * @brief Reinitializes the graph with a given number of isolated vertices.
		 *
		 * Existing graph contents are discarded. All vertices are assigned the
		 * specified weight.
		 *
		 * @param NV Number of vertices in the new graph.
		 * @param weight Initial weight assigned to every vertex.
		 * @param name Optional graph instance name.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if
		 *          the graph or its vertex weights cannot be initialized.
		 */
		void reset(
			std::size_t NV,
			weight_t weight = DEFAULT_WEIGHT,
			string name = "") noexcept;


		/////////////////////////
		// Basic graph operations
		// Convenience wrappers around the underlying graph API.

		/**
		 * @brief Adds an edge between two vertices.
		 *
		 * @param v First endpoint.
		 * @param w Second endpoint.
		 *
		 * @note Self-loops are not allowed.
		 */
		void add_edge(vertex_t v, vertex_t w) { 
			graph_.add_edge(v, w);
		}

		/**
		 * @brief Returns the graph density.
		 *
		 * @param lazy If `true`, uses the lazy density computation provided by
		 *             the underlying graph.
		 * @return Graph density.
		 */
		double density(bool lazy = true) const {
			return graph_.density(lazy);
		}

		/**
		 * @brief Generates random edges with probability @p p.
		 *
		 * This operation modifies only the graph topology; vertex weights are
		 * not affected.
		 *
		 * @param p Probability of generating each edge.
		 */
		void gen_random_edges(double p) {
			graph_.gen_random_edges(p);
		}

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
		 * @brief Checks whether the graph is unit-weighted.
		 *
		 * A graph is unit-weighted if every vertex has weight equal to
		 * `DEFAULT_WEIGHT` (1).
		 *
		 * @return `true` if all vertex weights are equal to 1; otherwise, `false`.
		 *
		 * @note An empty graph is considered unit-weighted.
		 * @note A unit-weighted graph is equivalent to an unweighted graph
		 *       from the perspective of vertex-weighted optimization.
		 */ 
		bool is_unit_weighted() const noexcept;


		///////////////////////////
		// Vertex weight operations

		/**
		 * @brief Applies a transformation to all vertex weights except `NO_WEIGHT`.
		 *
		 * For each valid weight `w`, the operation performs `w = f(w)`.
		 * Entries equal to `NO_WEIGHT` are left unchanged.
		 *
		 * @tparam Func Callable accepting a `weight_t` and returning a value
		 *              assignable to `weight_t`.
		 * @param f Transformation function.
		 */
		template<class Func>
		void transform_weights(Func f);
				

		////////////////////////
		// other operations

		/**
		 * @brief Creates the complement of the vertex-weighted graph.
		 *
		 * The resulting graph has the complementary topology and preserves the
		 * vertex weights, name, and path of the original graph.
		 *
		 * @param g Output graph containing the complement.
		 *
		 * @note Prefer the value-returning `create_complement()` overload in new code.
		 *       This output-parameter overload is retained for internal use and
		 *       backward compatibility.
		 */
		void create_complement(Base_Graph_W& g) const;

		/**
		 * @brief Creates and returns the complement of the vertex-weighted graph.
		 *
		 * The resulting graph has the complementary topology and preserves the
		 * vertex weights, name, and path of the original graph.
		 *
		 * @return The complement graph.
		 * 
		 * @note This overload should be implemented in the `Graph_W` facade
		 *       to return the user-facing type and avoid slicing in polymorphic use.
		 */
		Base_Graph_W create_complement() const;
		

		////////////
		// I/O
	
		/**
		 * @brief Writes the vertex-weighted graph to an output stream in DIMACS format.
		 *
		 * Vertex weights are written as `n <v> <weight>` records and edges using
		 * DIMACS edge records. Vertex identifiers are 1-based. Self-loops are ignored.
		 *
		 * @param os Output stream.
		 * @return Reference to @p os.
		 */
		virtual ostream& write_dimacs(std::ostream& os = std::cout) const;
			
		/**
		 * @brief Reads a vertex-weighted graph from a DIMACS file.
		 *
		 * Vertex weights may be contained in the DIMACS file itself or read from
		 * an optional separate weight file.
		 *
		 * @param filename Name of the DIMACS graph file.
		 * @param type Format of the optional separate weight file.
		 * @return 0 on success; `-1` if the input cannot be read or has
		 *         an invalid format.
		 *
		 * @note On failure, the graph is reset to an empty state.
		 *
		 * @deprecated Use the overload taking `weight_file_extension`.
		 */
		[[deprecated("Use the typed read_dimacs overload with weight_file_extension")]]
		int read_dimacs(string filename, int type) {
			return read_dimacs(
				filename,
				static_cast<weight_file_extension>(type)) ? 0 : -1;
		}

		/**
		 * @brief Reads a vertex-weighted graph from a DIMACS file.
		 *
		 * Vertex weights may be contained in the DIMACS file itself or read from
		 * an optional separate weight file.
		 *
		 * @param filename Name of the DIMACS graph file.
		 * @param type Format of the optional separate weight file.
		 * @return `true` on success; `false` if the input cannot be read or has
		 *         an invalid format.
		 *
		 * @note On failure, the graph is reset to an empty state.
		 */
		bool read_dimacs(const std::string& filename,
			weight_file_extension type = weight_file_extension::none);

		/**
		* @brief Reads weights from an external file (only weights)
		*
		*		 Format: numbers ordered and separated (line, space)
		*				  i.e. {5 6 7 ...} -> w(1)=5, w(2)=6, w(3)=7,...
		*
		*		 Weights not assigned in the file are set to 0.0
		*
		* @param filename name of the file
		* @returns true if success, false if error (empty vector of weights)
		**/
		bool read_weights(const std::string& filename);

		/**
		 * @brief Prints graph information to an output stream.
		 *
		 * Prints the underlying graph data and appends a tag identifying the graph
		 * as vertex-weighted.
		 *
		 * @param lazy Passed to the underlying graph `print_data()` operation.
		 * @param os Output stream.
		 * @param trailing_newline If `true`, appends a newline after the output.
		 * @return Reference to the output stream.
		 */
		std::ostream& print_data(
			bool lazy = true,
			std::ostream& os = std::cout, 
			bool trailing_newline = true) const;

		/**
		 * @brief Prints the graph edges to an output stream.
		 *
		 * Forwards the operation to the underlying graph.
		 *
		 * @param os Output stream.
		 * @param trailing_newline If `true`, appends a newline after the output.
		 * @return Reference to the output stream.
		 */
		std::ostream& print_edges(
			std::ostream& os = std::cout,
			bool trailing_newline = false) 
		{
			return graph_.print_edges(os, trailing_newline);
		}

		/**
		 * @brief Prints vertex weights to an output stream.
		 *
		 * By default, each vertex weight is printed in the form `[v:(weight)]`.
		 * If @p show_vertices is `false`, only the collection of weights is printed,
		 * preserving vertex order.
		 *
		 * @param os Output stream.
		 * @param show_vertices If `true`, prints vertex identifiers together with
		 *                      their weights; otherwise, prints only the weights.
		 * @return Reference to @p o.
		 */
		std::ostream& print_weights(
			std::ostream& os = std::cout, 
			bool show_vertices = true)	const;

		/**
		 * @brief Prints the weights of the vertices contained in a bitset.
		 *
		 * Each set bit identifies a vertex whose weight is written to the output
		 * stream.
		 *
		 * @param vertices Bitset containing the vertices to print.
		 * @param os Output stream.
		 * @return Reference to @p o.
		 *
		 * @note @p vertices is non-const because scanning updates the internal
		 *       scan cursor of the bitset, even though the set of bits itself is
		 *       not modified.
		 */
		std::ostream& print_weights(
			vertex_bitset_t& vertices, 
			std::ostream& os = std::cout) const;

		/**
		* @brief prints the weights of the vertices in the stack vertices
		* @param vertices: a set of vertices with a stack interface
		**/
		std::ostream& print_weights(
			const utils::FixedStack<int>& vertices,
			std::ostream& os = std::cout) const;

		/**
		* @brief prints the weights of the vertices in the FixedStack lv
		*		 given a mapping of the vertices
		* @param mapping: input mapping of the vertices with at least the same size as
		*				  the FixedStack lv
		* @param vertices: a set of vertices with a FixedStack interface
		**/
		std::ostream& print_weights(
			const utils::FixedStack<int>& vertices,
			const VertexMapping& mapping,
			std::ostream& os = std::cout)	const;

		/**
		 * @brief Prints the weights of the vertices contained in a vertex set.
		 *
		 * @param vertices Vertex set whose weights are printed.
		 * @param os Output stream.
		 * @return Reference to @p os.
		 *
		 * @note Supports C-style arrays through `vertex_set_t`.
		 */
		std::ostream& print_weights(
			vertex_set_t& vertices, 
			std::ostream& os = std::cout) const;

		/**
		 * @brief Prints the weights of the vertices contained in a C-style array.
		 *
		 * @param vertices Pointer to the array of vertex identifiers.
		 * @param n Number of vertices in the array.
		 * @param os Output stream.
		 * @return Reference to @p os.
		 *
		 * @note Retained for backward compatibility with code using C-style arrays.
		 */
		std::ostream& print_weights(
			const vertex_t* vertices, 
			int n, 
			std::ostream& os = std::cout) const;

		/////////////////////////////////////
		// data members

	protected:
		
		/**
		* @brief Returns mutable access to the vertex-weight vector.
		*
		* @return Reference to the internal vertex-weight vector.
		*
		* @note Intended for derived classes and internal algorithms.
		*/
		std::vector<weight_t>& weight() noexcept {
			return vertex_weights_;
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
		 * @brief Resets the graph and vertex weights without explicitly deallocating storage.
		 *
		 * Clears the underlying graph state and the vertex-weight vector.
		 *
		 * @note Intended for derived classes and internal use.
		 * @note To release owned storage, destroy the object or assign a freshly
		 *       constructed instance as appropriate.
		 */
		void reset() { 
			graph_.clear(); 
			vertex_weights_.clear(); 
		}

		/**
		 * @brief Underlying graph.
		 */
		graph_t graph_;								
		
		/**
		 * @brief Vertex-weight vector.
		 *
		 * Entry `vertex_weights_[v]` stores the weight associated with vertex `v`.
		 */
		std::vector<weight_t> vertex_weights_;						
	};

}//end namespace bitgraph

namespace bitgraph {

	/**
	 * @brief User-facing vertex-weighted graph class.
	 *
	 * Provides the public vertex-weighted graph type built on top of
	 * `Base_Graph_W`. This facade can be specialized for specific underlying
	 * graph types while reusing the common weighted-graph implementation
	 * provided by the base class.
	 *
	 * @tparam GraphT Underlying graph type.
	 * @tparam WeightT Vertex-weight type.
	 */
	template<class GraphT, class WeightT>
	class Graph_W : public Base_Graph_W <GraphT, WeightT> {};
}

/////////////////////////////////////////////
// Necessary implementations in header file	

#include "detail/simple_graph_vw_imp.h"


#endif // BITGRAPH_GRAPH_SIMPLE_GRAPH_VERTEX_WEIGHTED_H
