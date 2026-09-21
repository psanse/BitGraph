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
#include "utils/prec_timer.h"
#include "graph_types.h"


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

		//set of (named) factories to create simple graphs

		static Ugraph make_triangle();

		/**
		 * @brief Creates a complete undirected graph.
		 *
		 * Constructs the complete graph \f$K_n\f$, in which every pair of distinct
		 * vertices is connected by an edge. The graph contains no self-loops.
		 *
		 * @param NV Number of vertices in the complete graph.
		 * @return Complete graph containing @p NV vertices and
		 *         \f$NV(NV-1)/2\f$ edges.
		 *
		 * @note The empty graph \f$K_0\f$ and the single-vertex graph \f$K_1\f$ are
		 *       valid results.
		 * @note Construction follows the graph fail-fast policy and terminates if
		 *       graph allocation fails.
		 */
		static Ugraph make_clique(std::size_t NV);

		/**
		 * @brief Creates a simple undirected cycle graph.
		 *
		 * Constructs the cycle graph \f$C_n\f$, where every vertex has degree two.
		 * Consecutive vertices are adjacent, and the last vertex is connected to
		 * vertex zero.
		 *
		 * @param NV Number of vertices in the cycle. It must be at least
		 *        three.
		 * @return Cycle graph containing @p NV vertices and
		 *         @p NV edges.
		 *
		 * @note Construction follows the graph fail-fast policy and terminates if
		 *       @p NV is smaller than three or graph allocation fails.
		 */
		static Ugraph make_cycle(std::size_t NV);

		/**
		 * @brief Creates an undirected star graph centered at vertex zero.
		 *
		 * Constructs the star graph \f$K_{1,n-1}\f$. Vertex `0` is the center and is
		 * adjacent to every other vertex; no edges exist between the remaining
		 * vertices.
		 *
		 * @param	NV Total number of vertices. It must be at least one.
		 * @return Star graph containing @p NV vertices and
		 *         `NV	 - 1` edges.
		 *
		 * @note Construction follows the graph fail-fast policy and terminates if
		 *       @p NV is zero or allocation fails.
		 */
		static Ugraph make_star(std::size_t NV);

		// @todo other named factories... (03/01/2025)
			
		// type aliases

		using BaseT = Graph<BitsetT>;		//parent type

		using bitset_type = typename BaseT::bitset_type;
		using vertex_bitset_t = bitset_type;								// alias for semantic type
		using VertexBitset = vertex_bitset_t;								// alias for backward compatibility

		/////////////			
		//construction (cannot all be inherited) / destruction 	
		

		/** @brief Constructs an empty graph with no vertices. */
		Ugraph() noexcept = default;										
		
		/**
		 * @brief Constructs an undirected graph with isolated vertices.
		 *
		 * @param NV Number of vertices. The resulting graph contains no
		 *        edges.
		 *
		 * @note Construction follows the graph fail-fast policy.
		 */
		explicit Ugraph(std::size_t NV) noexcept: BaseT(NV) {}					
		
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
			: BaseT() { this->reset(std::move(filename)); }	

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
		 * @param adjacency Input adjacency matrix. A null pointer is permitted only
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
		Ugraph(std::size_t NV, const int * const adj[], string name) noexcept;			

		// @todo: copy constructor, move constructor, copy operator =, move operator = (1/1/2025)

		/** @brief Destroys the graph. */
		~Ugraph() override = default;
	

		/////////////
		// setters and getters
		
		/**
		* @brief Counts the number of edges	(includes self loops)
		* @param lazy if TRUE (reads value @NE_)
		*			  if FALSE counts and updates @NE_
		**/
		std::size_t num_edges(bool lazy = true)	 override;

		/**
		* @brief Counts the number of edges	in an induced subgraph by a set of vertices
		**/
		std::size_t num_edges(const BitsetT&) 	const override;

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
		double density(bool lazy = true) override;
	
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
		double density(const BitsetT& vertices) const override;

		/**
		 * @brief Computes the complement of this undirected graph.
		 *
		 * For every pair of distinct vertices `{v,w}`, @p ugcomp contains the edge
		 * `{v,w}` exactly when this graph does not contain it. Self-loops are
		 * excluded.
		 *
		 * The output graph is reset to the same number of vertices as this graph
		 * before construction. Any previous contents of @p ugcomp are discarded.
		 * Only the strict upper triangle of the adjacency matrix is inspected, and
		 * each complement edge is inserted once through Ugraph::add_edge().
		 *
		 * @param ugcomp Output graph that receives the complement.
		 *
		 * @pre @p ugcomp must be a different object from this graph.
		 *
		 * @note Edge-count metadata is maintained by add_edge().
		 * @note This function follows the graph fail-fast policy and terminates if
		 *       the input and output objects are the same or output allocation fails.
		 */
		void create_complement(Ugraph& ugcomp) const;


		/////////////
		// degree-related operations
		// 
		// TODO implement bitstring conversions according to the templates properly (3/4/18) - CHECK (02/01/2025)
	public:

		/**
		* @brief Computes the number of neighbors of v (deg(v))	*
		**/
		int degree(vertex_t v) const { return this->adj_[v].count(); }

		/**
		*  @brief number of neighbors of v in a set of vertices
		*
		*		  (specialized for sparse graphs)
		*
		*  @param bbn input non-sparse (bit) set of vertices
		**/
		int degree(vertex_t v, const Bitset& bbn) const;

		/**
		*  @brief number of neighbors of v in a sparse encoded set of vertices
		*
		*		  (sparse graphs ONLY )
		*
		*  @param bbn input sparse (bit) set of vertices
		*
		* TODO - currently not implemented, only for sparse graphs (03/03/2025)
		**/
		int degree(int v, const BitsetSp& bbs) const;

		/**
		*  @brief truncated number of neighbors of v in a set of vertices
		*
		*		  (specialized for sparse graphs)
		*
		*  @param bbn input (bit) set of vertices
		*  @returns neighbors of v if <= UB, otherwise UB
		**/
		int degree(int v, int UB, const Bitset& bbn) const;  //truncated degree (14/2/2016)

		/**
		*  @brief truncated number of neighbors of v in a sparse enconded set of vertices
		*
		*		  (ONLY for sparse graphs)
		*
		*  @param bbn input sparse (bit) set of vertices
		*  @returns neighbors of v if <= UB, otherwise UB
		*
		* TODO - currently not implemented, only for sparse graphs (03/03/2025)
		**/
		int degree(int v, int UB, const BitsetSp& bbs) const;	//truncated degree  (14/2/2016)

		/**
		*  @brief number of neighbors of v in a vertex set with higher index than v
		*
		*		  (applied as pivotal strategy for clique enumeration)
		*
		* @param v: input vertex
		* @param bbn: input (bit) set of vertices
		*
		**/
		int degree_up(int v, const Bitset& bbn)	const;  //TODO: test (27/4/2016)

		/**
		* @brief number of neighbors of v that come after v
		*
		* @param v: input vertex
		**/
		int degree_up(int v) const;

		/**
		*  @brief returns the maximum degree of the graph,
		*         i.e., the maximum degree of any of its vertices
		**/
		int max_graph_degree() const;

		/**
		*  @brief returns the maximum degree of an induced subgraph
		*  @param sg input (bit) set of vertices of the subgraph
		**/
		template<class VertexSet>
		int max_subgraph_degree(VertexSet& sg) const;


		/**
		* @brief number of edges with a single endpoint in a vertex from @sg
		* @param sg input (bit) set of vertices
		**/
		template<class VertexSet>
		int outgoing_degree(VertexSet& sg) const;

		/**
		* @brief number of edges with a single endpoint in a vertex from @sg
		* @param sg input (vector) set of vertices
		**/
		int outgoing_degree(std::vector<int> sg) const;

		//////////////	
		// Modifiers
	public:

		/**
		* @brief Adds bidirectional edge {v, w}
		*		 a) no self-loops are added (currently no feedback)
		*		 b) keeps track of the number of edges
		* @param v endpoint
		* @param w endpoint
		**/
		void add_edge(int v, int w)	override;

		/**
		* @brief Removes bidirectional edge {v, w}
		*		 a) if self_loop (v = w), graph remains unchanged
		*		 b) keeps track of the number of edges
		* @param v endpoint
		* @param w endpoint
		**/
		void remove_edge(int v, int w) override;

		/**
		* @brief generates undirected edges with probability p, exactly.
		* @param p probability of an edge
		* @details:
		* - uses a different more precise model than legacy code which operated edge by edge
		**/
		void gen_random_edges(double p)	override;

		//////////////	
		// Induced subgraphs

		//TODO	Graph& create_subgraph	(std::size_t first_k, Graph& g) const  override;

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






























