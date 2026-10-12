/**
 * @file simple_ugraph.h 
 * @brief class Ugraph for simple undirected graphs	(no self loops)
 *
 * @created 17/6/10
 * @last_update 21/09/26
 * @author pss
 *
 * This code is part of the GRAPH 1.0 C++ library 
 *
 * @todo implement min_degree functions (03/01/2025)
 **/

#ifndef BITGRAPH_GRAPH_SIMPLE_UGRAPH_H
#define BITGRAPH_GRAPH_SIMPLE_UGRAPH_H

#include "simple_graph.h"

namespace bitgraph {
	
	//////////////////
	//
	// Generic class Ugraph<BitsetT>
	// 
	// (BitsetT is limited to bitarray and sparse_bitarray types)
	// 
	//////////////////

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
		 * The constructor reads the strict upper triangle of @p adjacency. Every
		 * nonzero entry `adjacency[v][w]`, with `v < w`, creates the undirected edge
		 * `{v,w}`. The symmetric adjacency entries are generated internally by
		 * add_edge().
		 *
		 * Diagonal entries are ignored, so the resulting graph contains no
		 * self-loops. Entries in the lower triangle are not inspected.
		 *
		 * @param numVertices Number of rows and columns in @p adjacency.
		 * @param adj_matrix Input adjacency matrix. A null pointer is permitted only
		 *        when @p numVertices is zero.
		 * @param name Optional graph instance name or source filename.
		 *
		 * @pre If @p numVertices is greater than zero, @p adjacency and each of its
		 *      rows must point to valid arrays containing at least @p numVertices
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
		* @brief Counts the number of edges	(includes self loops)
		* @param lazy if TRUE (reads value @NE_)
		*			  if FALSE counts and updates @NE_
		**/
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
		*  @brief Computes the subgraph induced by a set of vertices
		*  @param lv input set of vertices (std::vector<int>)
		*  @returns 0 if success, -1 if error
		**/
		int create_subgraph(Ugraph& g, Vertices& lv) const;

		/**
		*  @brief Computes the subgraph induced by the neighborhood of a vertex (29/08/21)
		*
		*		 Calls create_subgraph(Ugraph& g, Vertices& lv)
		*
		*  @param v input vertex which determines the neighborhood
		*  @returns 0 if success, -1 if error
		**/
		int create_subgraph(Ugraph& g, int v) const;


		////////////
		// Write basic operations
		// Note: Read operations are inherited from the base class Graph (using the Template Pattern)
	public:
		/**
		* @brief writes undirected graph in dimacs format
		* @param o output stream
		**/
		void write_dimacs(std::ostream& filename) override;

		/**
		* @brief writes undirected graph in edge list format
		* @param o output stream
		**/
		void write_EDGES(std::ostream& filename) override;

		/**
		* @brief writes undirected graph in MMX (Matrix Exchange) format
		* @param o output stream
		**/
		void write_mtx(std::ostream& filename);				//MTX format only for Ugraph? (03/01/2025)

		/////////////////	
		//	I/O basic operations
	public:

		std::ostream& print_degrees(std::ostream & = std::cout)	const;
		std::ostream& print_edges(std::ostream & = std::cout, bool eofl = false) override;

		std::ostream& print_edges(vertex_bitset_t& bbsg, std::ostream&);
		std::ostream& print_adjacency_matrix(std::ostream & = std::cout) const;

		//////////////	
		// deleted methods legacy - CHECK	
		virtual void remove_vertices(const Bitset& bbn) override = delete;				//commented out implementation - EXPERIMENTAL

		/**
		*  @brief enlarges the graph with a new vertex (provided its neighborhood)
		*		  TODO - code removed, BUGGY (should not be called , unit tests DISABLED)
		**/
		int add_vertex(bitset_type* neigh = nullptr) = delete;
	};

}//end namespace bitgraph

/////////////////////////////////////////////////////////////
// Necessary implementation of template methods in header file

#include "graph/detail/simple_ugraph_imp.h"

#endif // BITGRAPH_GRAPH_SIMPLE_UGRAPH_H






























