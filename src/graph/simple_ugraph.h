/**
 * @file simple_ugraph.h
 * @brief Definition of Ugraph<BitsetT>, a simple undirected graph class.
 *
 * @details Implements simple undirected graphs (without self-loops)
 *          using bitset-based adjacency matrices.
 *          Supports dense and sparse bitset representations.
 *
 * @author pss
 * @date 17/06/2010
 * @details last_update 13/10/2026	
 *
 * @copyright Part of the BITGRAPH C++ library.
 *
 * @todo Implement minimum-degree functions (03/01/2025).
 */

#ifndef BITGRAPH_GRAPH_SIMPLE_UGRAPH_H
#define BITGRAPH_GRAPH_SIMPLE_UGRAPH_H

#include "simple_graph.h"

namespace bitgraph {
	
	/**
	 * @brief Undirected graph represented by a bitset-based adjacency matrix.
	 *
	 * @tparam BitsetT Bitset type used to store vertex neighborhoods.
	 *         Supported types are BBScan (dense) and BBScanSp (sparse).
	 *
	 * @details Inherits common graph functionality from Graph<BitsetT>
	 *          and specializes operations to preserve undirected graph semantics.
	 *          Each edge (u,v) is represented symmetrically in the adjacency matrix.
	 *
	 * @note Vertices are indexed from 0 to NV_-1.
	 * @note Self-loops are not supported.
	 */
	template<class BitsetT = BBScan>
	class Ugraph : public Graph<BitsetT> {

		friend class detail::GraphConversion;

	public:

		// type aliases			
		using base_t = Graph<BitsetT>;
		using BaseT = base_t;					// alias for backward compatibility

		using bitset_type = typename base_t::bitset_type;
		using vertex_bitset_t = bitset_type;								// alias for semantic type
		using VertexBitset = vertex_bitset_t;								// alias for backward compatibility

		/**
		 * @brief Makes the base-class `remove_edges` overloads visible.
		 */
		using base_t::remove_edges;
		using base_t::create_subgraph;

		////////////
		// Named factories to create simple graphs

		/**
		 * @brief Creates a triangle graph.
		 *
		 * Returns the complete undirected graph on three vertices, with vertex
		 * set `{0, 1, 2}` and edges `(0,1)`, `(0,2)`, and `(1,2)`.
		 *
		 * @return A triangle graph with three vertices and three edges.
		 */
		static Ugraph make_triangle();

		/**
		 * @brief Creates a complete undirected graph.
		 *
		 * Returns the complete graph on @p NV vertices. Every pair of distinct
		 * vertices is adjacent and self-loops are excluded.
		 *
		 * @param NV Number of vertices.
		 * @return Complete undirected graph with @p NV vertices.
		 */
		static Ugraph make_complete_graph(std::size_t NV);

		/**
		 * @brief Creates a simple cycle graph.
		 *
		 * Returns the undirected cycle graph on @p NV vertices. Consecutive
		 * vertices are adjacent, and the last vertex is connected to vertex 0.
		 *
		 * @param NV Number of vertices.
		 * @return Cycle graph with @p NV vertices and @p NV edges.
		 *
		 * @pre @p NV must be at least 3.
		 * @warning Follows the graph initialization error policy if @p NV < 3.
		 */
		static Ugraph make_cycle(std::size_t NV);

		/**
		 * @brief Creates a star graph.
		 *
		 * Returns the undirected star graph on @p NV vertices, with vertex 0
		 * as the center and vertices `1, ..., NV-1` as leaves.
		 *
		 * @param NV Number of vertices.
		 * @return Star graph with @p NV vertices and @p NV - 1 edges.
		 *
		 * @pre @p NV must be at least 1.
		 * @warning Follows the graph initialization error policy if @p NV == 0.
		 */
		static Ugraph make_star(std::size_t NV);
					
		
		/////////////			
		//construction (cannot all be inherited) / destruction 	
		

		/**
		 * @brief Constructs an empty undirected graph.
		 */
		Ugraph() noexcept = default;										
		
		/**
		 * @brief Constructs an undirected graph with isolated vertices.
		 *
		 * @param NV Number of vertices. The resulting graph contains no
		 *        edges.
		 *
		 * @note Construction follows the graph fail-fast policy.
		 */
		explicit Ugraph(std::size_t NV) noexcept
			: base_t(NV)
		{}					
		
		/**
		 * @brief Constructs an undirected graph by reading it from a file.
		 *
		 * The supported input formats are tried according to the policy implemented
		 * by reset(std::string).
		 *
		 * @param filename Input graph filename.
		 *
		 * @note Construction follows the graph fail-fast policy and terminates if the
		 *       file cannot be read in a supported format.
		 */
		explicit Ugraph(std::string filename) noexcept
			: base_t() 
		{ 
			this->reset(std::move(filename));
		}	

		/**
		 * @brief Constructs an undirected graph from an adjacency matrix.
		 *
		 * The constructor reads the strict upper triangle of @p adj_matrix. Every
		 * nonzero entry `adj_matrix[v][w]`, with `v < w`, creates the undirected edge
		 * `{v,w}`. The symmetric adjacency entries are generated internally by
		 * add_edge().
		 *
		 * Diagonal entries are ignored, so the resulting graph contains no
		 * self-loops. Entries in the lower triangle are not inspected.
		 *
		 * @param NV Number of rows and columns in @p adj_matrix.
		 * @param adj_matrix Input adjacency matrix. A null pointer is permitted only
		 *        when @p NV is zero.
		 * @param name Optional graph instance name or source filename.
		 *
		 * @pre If @p NV is greater than zero, @p adj_matrix and each of its
		 *      rows must point to valid arrays containing at least @p NV
		 *      integer entries.
		 *
		 * @note Every nonzero upper-triangle entry is interpreted as an edge.
		 * @note Construction follows the graph fail-fast policy and terminates if the
		 *       graph cannot be represented or allocated.
		 */
		[[deprecated("Raw adjacency-matrix construction is deprecated.")]]
		Ugraph(
			std::size_t NV,
			const int * const adj_matrix[],
			std::string name = "") noexcept;

		// Copy and move semantics are allowed.
		Ugraph(const Ugraph&) = default;
		Ugraph& operator=(const Ugraph&) = default;

		Ugraph(Ugraph&&) noexcept = default;
		Ugraph& operator=(Ugraph&&) noexcept = default;
			
		~Ugraph() override = default;
	
		/////////////
		// setters and getters
		
		/**
		 * @brief Returns the number of edges of the graph.
		 *
		 * Each undirected edge is counted once. Self-loops are not supported.
		 *
		 * @param lazy If `true`, a valid cached edge count may be used.
		 *             If `false`, the count is recomputed from the adjacency
		 *             matrix and the cache is updated.
		 * @return Number of edges.
		 */
		std::size_t num_edges(bool lazy = true)	const override;

		/**
		 * @brief Counts the number of edges in the subgraph induced by a vertex set.
		 *
		 * Counts all edges whose endpoints both belong to @p vertices.
		 *
		 * @param vertices Bitset containing the vertices that induce the subgraph.
		 * @return Number of edges in the induced subgraph.
		 */
		std::size_t num_edges(const vertex_bitset_t& vertices) 	const override;

		/////////////
		// Basic operations

		/**
		 * @brief Computes the density of the undirected graph.
		 *
		 * For an undirected simple graph without self-loops, density is defined as
		 * \f[
		 *     d = \frac{|E|}{|V|(|V|-1)/2}.
		 * \f]
		 *
		 * Each undirected edge is counted once.
		 *
		 * @param lazy If `true`, num_edges() may use a previously cached edge count.
		 *             If `false`, the edge count is recomputed from the adjacency
		 *             matrix.
		 * @return Graph density in the interval `[0,1]`. Returns `0.0` when the graph
		 *         contains fewer than two vertices.
		 */
		double density(bool lazy = true) const override;
	
		/**
		 * @brief Computes the density of the undirected subgraph induced by
		 *        @p vertices.
		 *
		 * For the selected vertex set \f$S\f$, density is defined as
		 * \f[
		 *     d = \frac{|E(S)|}{|S|(|S|-1)/2},
		 * \f]
		 * where \f$E(S)\f$ is the set of edges whose endpoints both belong to
		 * \f$S\f$. Each undirected edge is counted once, and self-loops are excluded.
		 *
		 * @param vertices Bitset containing the vertices that induce the subgraph.
		 * @return Induced-subgraph density in the interval `[0,1]`. Returns `0.0`
		 *         when fewer than two vertices are selected.
		 *
		 * @pre Every set bit in @p vertices must identify a valid graph vertex.
		 */
		double density(const vertex_bitset_t& vertices) const override;

		/**
		 * @brief Creates the complement of the current undirected graph.
		 *
		 * Replaces @p ugcomp with the complement of the current graph. The output
		 * graph has the same number of vertices, and two distinct vertices are
		 * adjacent in @p ugcomp if and only if they are not adjacent in the
		 * current graph.
		 *
		 * Self-loops are not created.
		 *
		 * @param ugcomp Destination graph receiving the complement.
		 *
		 * @pre @p ugcomp must be different from the current graph.
		 * @note The name of @p ugcomp is cleared.
		 * @warning Follows the graph initialization error policy if the input and
		 *          output graphs are the same object.
		 */
		void create_complement(Ugraph& ugcomp) const;

		/**
		 * @brief Creates and returns the complement of the undirected graph.
		 *
		 * Constructs the complement of the current graph and returns it by value.
		 * The returned graph has the same number of vertices as the current graph,
		 * while its edge set is replaced by the complement edge set.
		 *
		 * @return Complement of the current undirected graph.
		 *
		 * @note The returned graph has an empty name.
		 */
		Ugraph create_complement() const {
			Ugraph ugcomp(this->num_vertices());
			this->create_complement(ugcomp);
			return ugcomp;
		}

		/////////////
		// degree-related operations

		/**
		 * @brief Returns the degree of a vertex.
		 *
		 * @param v Vertex identifier.
		 * @return Number of neighbors of @p v.
		 *
		 * @pre @p v must be a valid vertex.
		 */
		degree_t degree(vertex_t v) const {
			assert(v >= 0 && v < this->NV_);
			return this->adj_[static_cast<size_t>(v)].count();
		}

		/**
		 * @brief Returns the degree of a vertex within an induced subgraph.
		 *
		 * Counts the neighbors of @p v that belong to @p vertices.
		 *
		 * @param v Vertex identifier.
		 * @param vertices Bitset defining the induced subgraph.
		 * @return Number of neighbors of @p v contained in @p vertices.
		 *
		 * @pre @p v must be a valid vertex.
		 */
		degree_t degree(vertex_t v,
			const vertex_bitset_t& vertices) const;
		
		/**
		 * @brief Returns the degree of a vertex truncated at an upper bound.
		 *
		 * Counts the neighbors of @p v that belong to @p vertices. If the actual
		 * degree exceeds @p upper_bound, the function returns @p upper_bound.
		 *
		 * @param v Vertex identifier.
		 * @param upper_bound Maximum value returned.
		 * @param vertices Bitset containing the vertices of interest.
		 * @return Degree of @p v within @p vertices, truncated at @p upper_bound.
		 *
		 * @note This overload is specialized for sparse graph processing.
		 */
		degree_t degree(
			vertex_t v, 
			degree_t upper_bound,
			const vertex_bitset_t& vertices) const; 
				
		/**
		 * @brief Returns the number of higher-indexed neighbors of a vertex.
		 *
		 * Counts the neighbors of @p v that belong to @p vertices and whose
		 * vertex identifier is strictly greater than @p v.
		 *
		 * This operation is useful, for example, in pivot-based clique
		 * enumeration strategies.
		 *
		 * @param v Vertex identifier.
		 * @param vertices Bitset containing the vertices of interest.
		 * @return Number of neighbors of @p v in @p vertices with index greater
		 *         than @p v.
		 *
		 * @pre @p v must be a valid vertex.
		 */
		degree_t degree_up(vertex_t v,
			const vertex_bitset_t& vertices) const;  

		/**
		 * @brief Returns the number of higher-indexed neighbors of a vertex.
		 *
		 * Counts the neighbors of @p v whose vertex identifier is strictly
		 * greater than @p v.
		 *
		 * This operation is useful, for example, in pivot-based clique
		 * enumeration strategies.
		 *
		 * @param v Vertex identifier.
		 * @return Number of neighbors of @p v with index greater than @p v.
		 *
		 * @pre @p v must be a valid vertex.
		 */
		degree_t degree_up(vertex_t v) const;

		/**
		*  @brief returns the maximum degree of the graph,
		*         i.e., the maximum degree of any of its vertices
		**/
		degree_t max_degree() const;

		/**
		 * @brief Returns the maximum degree in an induced subgraph.
		 *
		 * Computes the maximum degree among the vertices contained in
		 * @p vertices, considering only neighbors that also belong to
		 * @p vertices.
		 *
		 * The bitset contents are not modified, but its internal scan cursor
		 * is used during the computation.
		 *
		 * @param vertices Bitset containing the vertices that induce the subgraph.
		 * @return Maximum degree in the induced subgraph. Returns 0 if
		 *         @p vertices is empty.
		 */
		degree_t max_subgraph_degree(
			vertex_bitset_t& vertices) const;

		/**
		 * @brief Returns the number of edges leaving a vertex set.
		 *
		 * Counts the edges with exactly one endpoint in @p vertices and the
		 * other endpoint outside the set.
		 *
		 * The bitset contents are not modified, but its internal scan cursor
		 * is used during the computation.
		 *
		 * @param vertices Bitset containing the vertices of the set.
		 * @return Number of edges between @p vertices and its complement.
		 */
		degree_t outgoing_degree(
			vertex_bitset_t& vertices) const;

		/**
		 * @brief Returns the number of edges leaving a vertex set.
		 *
		 * Counts the edges with exactly one endpoint in @p vertices and the
		 * other endpoint outside the set.
		 *
		 * @param vertices Set of vertices.
		 * @return Number of edges between @p vertices and its complement.
		 */
		degree_t outgoing_degree(
			const vertex_set_t& vertices) const;
			
	public:

		//////////////	
		// Modifiers

		/**
		* @brief Adds bidirectional edge {v, w}
		*		 a) no self-loops are added (currently no feedback)
		*		 b) keeps track of the number of edges
		* @param v endpoint
		* @param w endpoint
		**/
		void add_edge(vertex_t v, vertex_t w)	override;

		/**
		 * @brief Removes an undirected edge.
		 *
		 * Removes the edge between @p v and @p w if it exists, updating both
		 * symmetric adjacency entries and the stored number of edges.
		 *
		 * If @p v == @p w or the edge does not exist, the graph remains unchanged.
		 *
		 * @param v First endpoint of the edge.
		 * @param w Second endpoint of the edge.
		 */
		void remove_edge(vertex_t v, vertex_t w) override;

		/**
		 * @brief Removes all edges incident to a vertex.
		 *
		 * Removes every edge having @p v as an endpoint. Since the graph is
		 * undirected, the corresponding symmetric adjacency entries are also
		 * removed.
		 *
		 * @param v Vertex whose incident edges are removed.
		 *
		 * @pre @p v must be a valid vertex.
		 */
		void remove_edges(vertex_t v) override;

		/**
		 * @brief Generates a uniformly random graph with approximately density @p p.
		 *
		 * Selects exactly `round(p * C(n,2))` edges uniformly at random from all
		 * possible undirected edges.
		 *
		 * @note This corresponds to a fixed-edge-count random graph model similar
		 *       to G(n,m), rather than the independent-edge G(n,p) model.
		 */
		void gen_random_edges(double p)	override;

		//////////////	
		// Induced subgraphs
		
		/**
		 * @brief Creates the subgraph induced by a given vertex set.
		 *
		 * @param[out] graph Output undirected graph, overwritten by the induced subgraph.
		 * @param[in] vertices Vertices of the original graph to retain.
		 *
		 * @pre All vertices must belong to the original graph.
		 * @pre The output graph must differ from the source graph.
		 *
		 * @note Vertices are renumbered consecutively according to
		 *       the scanning order of the vertex set.
		 * @note An empty vertex set produces an empty graph.
		 */
		void create_subgraph(Ugraph& graph, 
			const vertex_set_t& vertices) const;

		/**
		 * @brief Creates the subgraph induced by a vertex set.
		 *
		 * @param[in] vertices Vertices of the original graph to retain.
		 * @return The induced undirected subgraph.
		 *
		 * @pre All vertices in the set must belong to the original graph.
		 *
		 * @note Vertices are renumbered consecutively according to
		 *       the iteration order of the vertex set.
		 */
		Ugraph create_subgraph(const vertex_set_t& vertices) const {
			Ugraph induced;
			this->create_subgraph(induced, vertices);
			return induced;
		}

		/**
		 * @brief Creates the subgraph induced by the neighbors of a vertex.
		 *
		 * @param[out] graph Output undirected graph, overwritten by the induced subgraph.
		 * @param[in] v Vertex whose neighborhood defines the subgraph.
		 *
		 * @pre v must be a valid vertex of the original graph.
		 * @pre The output graph must differ from the source graph.
		 *
		 * @note The vertex v itself is not included in the induced subgraph.
		 * @note An isolated vertex produces an empty graph.
		 */
		void create_subgraph(Ugraph& graph, vertex_t v) const;
		
		/**
		 * @brief Creates the subgraph induced by the neighbors of a vertex.
		 *
		 * @param[in] v Vertex whose neighborhood defines the subgraph.
		 * @return The induced undirected subgraph.
		 *
		 * @pre v must be a valid vertex of the original graph.
		 *
		 * @note The vertex v itself is not included in the induced subgraph.
		 * @note An isolated vertex produces an empty graph.
		 */
		Ugraph create_subgraph(vertex_t v) const {
			Ugraph induced;
			this->create_subgraph(induced, v);
			return induced;
		}

		/**
		 * @brief Creates the subgraph induced by the first k vertices.
		 *
		 * @param[in] first_k Number of vertices to retain.
		 * @return The induced undirected graph.
		 *
		 * @pre 0 <= first_k <= NV_.
		 *
		 * @note Vertex indices are preserved.
		 * @note A value of zero produces an empty graph.
		 */
		Ugraph create_subgraph_first_k(int first_k) const {
			Ugraph induced;
			base_t::create_subgraph_first_k(first_k, induced);
			return induced;
		}

		////////////////////////////
		// Basic read/write operations
		// Read operations are inherited from Graph<BitsetT>.
		// Write operations are specialized to preserve undirected graph semantics.
		
		/**
		 * @brief Writes the undirected graph in DIMACS edge format.
		 *
		 * @param[out] os Output stream.
		 *
		 * @note Vertices are numbered from 1 in the output.
		 * @note Each undirected edge is written exactly once.
		 * @note The edge count is recomputed before writing.
		 * @note Self-loops are not supported.
		 */
		void write_dimacs(std::ostream& os) override;

		/**
		 * @brief Writes the undirected graph as an edge list.
		 *
		 * @param[out] os Output stream.
		 *
		 * @note Vertices are numbered from 1 in the output.
		 * @note Each undirected edge is written exactly once.
		 * @note Comment lines start with '%'.
		 * @note Self-loops are not written.
		 */
		void write_EDGES(std::ostream& os) override;

		/**
		 * @brief Writes the undirected graph in Matrix Market coordinate format.
		 *
		 * @param[out] os Output stream.
		 *
		 * @note Uses the Matrix Market pattern symmetric format.
		 * @note Vertices are numbered from 1 in the output.
		 * @note Each undirected edge is written exactly once.
		 * @note The edge count is recomputed before writing.
		 * @note Self-loops are not supported.
		 */
		void write_mtx(std::ostream& os);

		/////////////////	
		//	I/O basic operations
	
		/**
		 * @brief Prints the degree of each vertex to an output stream.
		 *
		 * @param[out] os Output stream (std::cout by default).
		 * @return Reference to the output stream.
		 *
		 * @note Does not modify the graph.
		 */
		std::ostream& print_degrees(
			std::ostream & = std::cout)	const;

		/**
		 * @brief Prints the edges of the undirected graph to an output stream.
		 *
		 * @param[out] os Output stream (std::cout by default).
		 * @param[in] trailing_newline Whether to append a newline.
		 * @return Reference to the output stream.
		 *
		 * @note Each undirected edge is printed exactly once.
		 * @note Does not modify the graph.
		 */
		std::ostream& print_edges(std::ostream & = std::cout,
			bool trailing_newline = false) override;

		/**
		 * @brief Prints the edges associated with a given vertex set.
		 *
		 * @param[in] vertices Bitset specifying the vertices to consider.
		 * @param[out] os Output stream.
		 * @return Reference to the output stream.
		 *
		 * @note Each undirected edge is printed exactly once.
		 * @note The vertex bitset may have its scan state modified.
		 */
		std::ostream& print_edges(vertex_bitset_t& vertices,
			std::ostream&);

		/**
		 * @brief Prints the adjacency matrix of the graph.
		 *
		 * @param[out] os Output stream (std::cout by default).
		 * @return Reference to the output stream.
		 *
		 * @note Does not modify the graph.
		 */
		std::ostream& print_adjacency_matrix(
			std::ostream & = std::cout) const;
				
	};

}//end namespace bitgraph

/////////////////////////////////////////////////////////////
// Necessary implementation of template methods in header file

#include "graph/detail/simple_ugraph_imp.h"

#endif // BITGRAPH_GRAPH_SIMPLE_UGRAPH_H
