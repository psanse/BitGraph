 /**
  * @file simple_graph.h
  *
  * @brief Generic graph container for simple (unweighted, sparse and non-sparse) graphs.
  *
  * This header defines the primary template `Graph<BitsetT>`, which represents
  * a simple graph using a bitset-based adjacency structure.
  *
  * The class provides:
  *  - Core graph storage and basic operations
  *  - Graph construction, inspection, and modification
  *  - Import/export support for common graph formats (DIMACS, Matrix Market,
  *    edge lists, MMX)
  *  - Utility services such as logging and timing for graph operations
  *
  * @details
  * `Graph<BitsetT>` is the fundamental graph abstraction used throughout the
  * BitGraph library. The template parameter `BitsetT` specifies the underlying
  * adjacency representation and is currently restricted to:
  *  - `BBScan`   (dense bitset representation)
  *  - `BBScanSp` (sparse bitset representation)
  *
  * Higher-level graph abstractions (e.g. undirected graphs, weighted graphs,
  * and facade graph types) are built on top of this class.
  *
  * @note
  * This header is a core dependency of most BitGraph components and should be
  * included before facade or algorithm-level headers.
  *
  * @author Pablo San Segundo (pss)
  *
  * @date Created: 17/06/2010
  * @date Last update: 20/09/2026
  */

#ifndef BITSCAN_GRAPH_SIMPLE_GRAPH_H
#define BITSCAN_GRAPH_SIMPLE_GRAPH_H

#include "bitscan/bitscan.h"	
#include "utils/logger.h"
#include "utils/precise_timer.h"
#include "graph_types.h"
#include "graph/formats/detail/dimacs_format.h"
#include "graph/formats/detail/edges_format.h"
#include "graph/formats/detail/matrix_market_format.h"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>  

namespace bitgraph {
	
	// forward declaration
	namespace detail { 
		class GraphConversion;
	}

	//////////////////
	//
	// Generic class Graph<BitsetT>
	// 
	// (BitsetT is a bitset currently limited to the BBScan and BBScanSP types)
	// 
	//////////////////

	template<class BitsetT = BBScan>
	class Graph {

		// filter BBScan and BBScanSP types
		static_assert(std::is_same<BBScan, BitsetT>::value ||
							std::is_same<BBScanSp, BitsetT>::value, "is not a valid Graph type");

		friend class detail::GraphConversion;

	public:
						
		using bitset_type = BitsetT;				
		using vertex_set_type = bitset_type;

		// aliases for backward compatibility
		using vertex_bitset_t = vertex_set_type;
		using VertexBitset = vertex_bitset_t;		
		
		/////////////			
		//construction / destruction

		  /** @brief Constructs an empty graph. */
		Graph() noexcept;													// creates empty graph

		/**
	    * @brief Constructs a graph containing @p NV isolated vertices
	    *		(n = NV and m = 0)
	    *
	    * @note This constructor follows a fail-fast policy and terminates the program
		*       if graph initialization fails.
	    */
		explicit Graph(std::size_t NV) noexcept;										
			
		/**
		* @brief Constructs a graph by reading it from @p filename.
		*
		* @note This constructor follows a fail-fast policy and terminates the program
		*       if graph cannot be parsed.
		*/
		explicit Graph(std::string filename) noexcept;								

		/**
		 * @brief Constructs a graph from an old-style adjacency matrix.
		 *
		 * Every nonzero entry `adjacency[v][w]` represents the directed edge `(v,w)`.
		 * For an undirected graph, the input matrix is expected to be symmetric.
		 *
		 * @param NV Number of rows and columns in @p adjacency.
		 * @param adjacency Input adjacency matrix.
		 * @param filename Optional graph instance name or source filename.
		 *
		 * @pre If @p NV is greater than zero, @p adjacency and each of its
		 *      rows must be valid.
		 *
		 * @note This constructor follows a fail-fast policy and terminates the program
		 *       if graph initialization fails.
		 */
		Graph(std::size_t NV,const int * const adj[], std::string filename = "") noexcept;		

		//move and copy semantics allowed
		Graph(const Graph& g) = default;
		Graph& operator = (const Graph& g) = default;
		Graph(Graph&& g) noexcept = default;
		Graph& operator = (Graph&& g) noexcept = default;

		virtual	~Graph() = default;

		/////////////
		// setters and getters

		/**
		 * @brief Sets the graph instance name and extracts its directory path.
		 *
		 * If @p name_or_path contains a directory separator, the final component is
		 * stored as the instance name and the preceding portion, including the final
		 * separator, is stored as the path. Both Unix (`/`) and Windows (`\`)
		 * separators are recognized.
		 *
		 * If no separator is present, the complete argument becomes the instance name
		 * and the stored path is cleared.
		 *
		 * @param name_or_path Instance name or complete file path.
		 */
		void set_name(std::string name_or_path);

		/**
		 * @brief Returns the graph instance name without its directory path.
		 *
		 * @return Constant reference to the stored instance name.
		 *
		 * @note The returned reference remains valid until the graph name is modified
		 *       or the graph object is destroyed.
		 */
		const std::string& name() const noexcept { return name_; }

		/**
		 * @brief Sets the directory path associated with the graph instance.
		 *
		 * The path is stored exactly as supplied; no normalization is performed and
		 * no trailing directory separator is added automatically.
		 *
		 * @param pathName Directory path to store.
		 */
		void set_path(std::string path_name) { path_.swap(path_name); }

		/**
		 * @brief Returns the directory path associated with the graph instance.
		 *
		 * The stored path includes its final directory separator when it was extracted
		 * from a complete filename.
		 *
		 * @return Constant reference to the stored directory path.
		 */
		const std::string& path() const noexcept { return path_; }

		/**
		* @brief number of vertices of the graph. Alias to num_vertices()
		**/
		std::size_t size() const  noexcept { return static_cast<std::size_t>(NV_); }

		/**
		* @brief number of vertices of the graph as signed integer
		* @return: number of vertices as a signed integer
		* @details: aliases order() and num_vertices()
		**/
		int num_vertices() const  noexcept { return NV_; }		

		/**
		* @brief number of bitblocks in each bitset (row) of the adjacency matriz
		*		 In the case of sparse graphs, the value is the maximum capacity
		*		 of all bitsets.
		**/
		int num_blocks() const noexcept { return NBB_; }
			

		/**
		 * @brief Returns the number of edges in the graph.
		 *
		 * @param lazy If `true`, returns a previously computed nonzero value when
		 *             available. If `false`, always recomputes the value from the
		 *             adjacency matrix.
		 * @return Number of adjacency entries.
		 *
		 * @note A zero cached value is treated as an uninitialized value. Therefore,
		 *       edgeless graphs are recomputed on every lazy call.
		 * @note Operations that modify the adjacency matrix must update or invalidate
		 *       NE_ to prevent a stale (wrong) nonzero result.
		 */
		virtual	std::size_t num_edges(bool lazy = true) const;

		/**
		* @brief Counts the number of edges	in an induced subgraph by a set of vertices
		* @param set input bitset of vertices that induces the subgraph
		* @returns number of edges
		**/
		virtual std::size_t num_edges(const BitsetT& set) const;
		
		/**
		 * @brief Returns the bitset-based adjacency matrix.
		 *
		 * Each element of the returned vector represents one adjacency row. Row
		 * `v` contains the outgoing neighbors of vertex `v`. For an undirected graph,
		 * the adjacency matrix is symmetric.
		 *
		 * @return Constant reference to the adjacency matrix.
		 *
		 * @note The reference remains valid until an operation reallocates or replaces
		 *       the adjacency matrix, such as reset().
		*/
		const vector<vertex_bitset_t>& adjacency_matrix() const { return adj_; }

		/**
		 * @brief Returns the neighbors of a vertex.
		 *
		 * For a directed graph, the returned bitset contains the outgoing neighbors
		 * of @p vertex.
		 *
		 * @param vertex Vertex whose neighborhood is requested.
		 * @return Constant reference to the vertex-neighborhood bitset.
		 *
		 * @pre `0 <= vertex < num_vertices()`.
		 */
		const vertex_bitset_t& neighbors(vertex_t v) const;

		/**
		 * @brief Returns modifiable access to the neighbors of a vertex.
		 *
		 * The cached edge count is invalidated because the caller may modify the
		 * returned adjacency row.
		 *
		 * @param vertex Vertex whose neighborhood is requested.
		 * @return Modifiable reference to the vertex-neighborhood bitset.
		 *
		 * @pre `0 <= vertex < num_vertices()`.
		 *
		 * @warning For an undirected graph, the caller is responsible for preserving
		 *          adjacency-matrix symmetry and the no-self-loop invariant.
		 */
		vertex_bitset_t& neighbors(vertex_t v);
		

		//////////////////////////
		// memory allocation 
	public:
		
		/**
		 * @brief Reinitializes the graph with @p NV isolated vertices.
		 *
		 * Existing graph contents are discarded. The resulting graph contains no
		 * edges, and every adjacency bitset is initialized with all bits cleared.
		 *
		 * @param NV Number of vertices in the new graph.
		 * @param name Optional graph instance name or path.
		 *
		 * @note This function follows a fail-fast policy and terminates the program
		 *       if the requested graph cannot be represented or allocated.
		 * @date created 31/12/24
		 * @date last_update 20/09/26
		 */
		void reset(std::size_t NV, std::string name = "") noexcept;

		/**
		* @brief sets graph from file in dimacs/MTX/Edges formats (in this order)
		* @param filename file
		* @note This function follows a fail-fast policy and terminates the program
		*       if the requested graph cannot be represented or allocated.
		**/
		void reset(std::string filename) noexcept;

		/**
		* @brief resets to default values (does not deallocate memory)
		* @details: to deallocate memory -  g = graph()
		* @details: in general, should not be called directly
		**/
		void reset() noexcept;

		/**
		 * @brief Requests removal of unused storage from each adjacency bitset.
		 *
		 * The graph's vertices, edges, and cached edge count remain unchanged.
		 * The amount of storage released depends on the bitset implementation and
		 * is not guaranteed.
		 */
		void shrink_to_fit();

		/**
		 * @brief Reduces the graph to its first @p new_size vertices.
		 *
		 * Keeps vertices in `[0, new_size)` and removes all other vertices and their
		 * incident edges. The remaining adjacency rows are cleared beyond the new
		 * vertex range, and the cached edge count is invalidated.
		 *
		 * @param new_size Number of vertices to retain. It must be smaller than the
		 *        current number of vertices.
		 * @return `0` on success; `-1` if @p new_size is invalid, in which case the
		 *         graph remains unchanged.
		 *
		 * @note Whether adjacency bit blocks are physically deallocated depends on
		 *       the bitset implementation.
		 */
		int shrink_to_fit(std::size_t new_size);

		//////////////	
		// Basic operations	
	public:

		/**
		 * @brief Computes the density of the graph.
		 *
		 * For a directed graph without self-loops, density is defined as
		 * \f[
		 *     d = \frac{|E|}{|V|(|V|-1)}.
		 * \f]
		 *
		 * @param lazy If `true`, num_edges() may use a previously cached edge count.
		 *             If `false`, the edge count is recomputed from the adjacency
		 *             matrix.
		 * @return Graph density in the interval `[0,1]`. Returns `0.0` when the graph
		 *         contains fewer than two vertices.
		 *
		 * @note Derived graph classes may override this function to use a different
		 *       maximum-edge formula. For example, an undirected simple graph uses
		 *       \f$|V|(|V|-1)/2\f$.
		 */
		virtual	double density(bool lazy = true) const;

		/**
		 * @brief Computes the density of the subgraph induced by @p vertices.
		 *
		 * The density is defined as the number of directed edges in the induced
		 * subgraph divided by the maximum possible number of directed edges:
		 * \f[
		 *     d = \frac{|E(S)|}{|S|(|S|-1)},
		 * \f]
		 * where \f$S\f$ is the selected vertex set. Self-loops are not considered.
		 *
		 * @param vertices Bitset containing the vertices that induce the subgraph.
		 * @return Induced-subgraph density in the interval `[0,1]`. Returns `0.0`
		 *         when fewer than two vertices are selected.
		 *
		 * @pre Every set bit in @p vertices must identify a valid graph vertex.
		 */
		virtual double density(const BitsetT& vertices) const;
	
		/**
		 * @brief Computes the proportion of nonempty adjacency bit blocks.
		 *
		 * For a dense graph representation, block density is defined as the number
		 * of adjacency blocks containing at least one set bit divided by the total
		 * number of blocks in the adjacency matrix:
		 * \f[
		 *     d_B =
		 *     \frac{\text{number of nonempty blocks}}
		 *          {|V| \cdot \text{blocks per adjacency row}}.
		 * \f]
		 *
		 * @return Block density in the interval `[0,1]`. Returns `0.0` when the graph
		 *         contains no adjacency blocks.
		 *
		 * @note Sparse graph representations may provide a specialized implementation.
		 */
		double block_density() const;
		
		 /**
		  * @brief Computes the global storage density of the sparse adjacency matrix.
		  *
		  * The density is the number of allocated sparse bit blocks divided by the
		  * number of blocks required by the equivalent dense adjacency matrix:
		  * \f[
		  *     d_B =
		  *     \frac{\text{allocated blocks}}
		  *          {|V| \cdot \text{blocks per adjacency row}}.
		  * \f]
		  *
		  * @return Sparse-storage density in the interval `[0,1]`. Returns `0.0` when
		  *         the graph contains no possible adjacency blocks.
		  */

		double block_density_sparse() const = delete;
			
		/**
		 * @brief Computes the average bit occupancy of the sparse adjacency rows.
		 *
		 * For each vertex with allocated adjacency blocks, the row occupancy is
		 * defined as
		 * \f[
		 *     d_v =
		 *     \frac{\text{number of set bits in row }v}
		 *          {\text{allocated blocks in row }v \cdot \mathrm{WORD\_SIZE}}.
		 * \f]
		 *
		 * The function returns the arithmetic mean of these row occupancies over all
		 * graph vertices:
		 * \f[
		 *     \bar{d} = \frac{1}{|V|}\sum_{v\in V} d_v.
		 * \f]
		 *
		 * Adjacency rows without allocated blocks contribute zero to the average.
		 *
		 * @return Average sparse-row bit occupancy in the interval `[0,1]`. Returns
		 *         `0.0` when the graph contains no vertices.
		 *
		 * @note This operation is defined only for Graph<BBScanSp>.
		 */
		double average_block_density_sparse() const = delete;


		/**
		 * @brief Returns the out-degree of a vertex.
		 *
		 * Counts the edges directed from @p v to other vertices. In an undirected
		 * graph, this is the ordinary degree of @p v.
		 *
		 * @param v Vertex whose out-degree is requested.
		 * @return Number of outgoing edges from @p v.
		 *
		 * @pre `0 <= v < num_vertices()`.
		 * 
		 * @note The implementation includes self-loops in the count.
		 */
		int degree_out(vertex_t v) const;


		/**
		 * @brief Returns the in-degree of a vertex.
		 *
		 * Counts edges directed toward @p v. In an undirected graph, this equals
		 * the ordinary degree of @p v.
		 *
		 * @param v Vertex whose in-degree is requested.
		 * @return Number of incoming edges to @p v.
		 *
		 * @pre `0 <= v < num_vertices()`.
		 *
		 * @note The implementation includes self-loops in the count.
		 */
		int degree_in(vertex_t v) const;

		/**
		 * @brief Computes the complement of this directed graph.
		 *
		 * For every ordered pair of distinct vertices `(v,w)`, the output graph
		 * contains the edge `(v,w)` exactly when this graph does not contain it.
		 * Self-loops are excluded.
		 *
		 * The output graph is reset and reallocated as necessary before the complement
		 * is constructed. Its previous contents are discarded.
		 *
		 * @param gcomp Output graph that receives the complement.
		 *
		 * @pre @p gcomp must be a different object from this graph.
		 *
		 * @note The input and output graphs have the same number of vertices.
		 * @note Edge-count metadata is maintained through add_edge().
		 * @note This function follows the graph fail-fast policy and terminates if
		 *       allocation of the output graph fails.
		 */
		void create_complement(Graph& gcomp) const;

		//////////////	
		// Modifiers

	public:
		/**
		 * @brief Adds a directed edge from @p v to @p w.
		 *
		 * If the edge already exists, the graph is unchanged. Self-loops are ignored.
		 * The cached edge count is incremented only when a new edge is inserted.
		 *
		 * @param v Source vertex.
		 * @param w Destination vertex.
		 *
		 * @pre `0 <= v < num_vertices()` and `0 <= w < num_vertices()`.
		 *
		 * @note If the edge-count cache was previously invalid, it remains invalid;
		 *       num_edges() recomputes the count before returning it.
		 */
		virtual void add_edge(vertex_t v, vertex_t w);

		/**
		 * @brief Removes the directed edge from @p v to @p w.
		 *
		 * If the edge is absent, the graph is unchanged. Self-loops are not stored
		 * and therefore have no effect.
		 *
		 * @param v Source vertex.
		 * @param w Destination vertex.
		 *
		 * @pre `0 <= v < num_vertices()` and `0 <= w < num_vertices()`.
		 *
		 * @note If the edge-count cache is valid, it is decremented when an edge is
		 *       removed. An invalid cache remains invalid until num_edges()
		 *       recomputes it.
		 */
		virtual void remove_edge(vertex_t v, vertex_t w);

		/**
		 * @brief Removes all edges incident to a vertex.
		 *
		 * Removes every outgoing edge from @p v and every incoming edge to @p v.
		 * Any self-loop at @p v is also removed.
		 *
		 * @param v Vertex whose incident edges are removed.
		 * @pre `0 <= v < num_vertices()`.
		 *
		 * @note The cached edge count is invalidated and will be recomputed when
		 *       next requested.
		 */
		void remove_edges(vertex_t v);

		/**
		 * @brief Removes every edge from the graph.
		 *
		 * The vertices and adjacency storage are retained. After this call, the
		 * graph contains no edges and its cached edge count is zero.
		 */
		void remove_edges();


		/**
		 * @brief Adds the reverse edge for every existing directed edge.
		 *
		 * After this call, distinct vertices @p i and @p j are adjacent in both
		 * directions whenever either directed edge existed before the call.
		 * Existing edges are retained; self-loops are unchanged.
		 *
		 * @note The edge count remains valid because add_edge() maintains it.
		 */
		void make_bidirected();

		///////////////
		// random generation

		/**
		* @brief generates directed edges with probability p.
		*
		*		 I. (v, v) not allowed.
		*
		* @param v input endpoint
		* @param w input endpoint
		* @returns 0 is success, -1 if error
		**/
		virtual void gen_random_edges(double p);

		/**
		* @brief generates edge (v, w) with probability p.
		*
		*		 I. (v, v) not allowed.
		*		 II. Valid for directed/undirected graphs (TMP design)
		*
		* @param v input endpoint
		* @param w input endpoint
		* @returns 0 is success, -1 if error
		**/
		int gen_random_edge(vertex_t v, vertex_t w, double p);

		//////////////	
		// Induced subgraphs

		/**
		* @brief computes the induced subgraph by the first k vertices in the current graph
		* @param first_k first k vertices to be included in the new graph
		* @param g output new induced subgraph
		* @returns the new induced subgraph (if the operation fails, g remains unchanged)
		**/
		virtual	Graph& create_subgraph(int first_k, Graph& g) const;

		/**
		* @brief creates the subgraph induced by the vertices NOT in the input set
		* @param set input set of vertices
		* @param g ouptut induced subgraph
		**/
		void remove_vertices(const Bitset& set, Graph& g);

		//////////////	
		// deleted - 
		virtual void remove_vertices(const Bitset& set) = delete;	// TODO in the future?

		/////////////
		// Boolean properties
	public:

		/**
		 * @brief Tests whether the graph contains the edge from @p v to @p w.
		 *
		 * For directed graphs, the order of the vertices is significant and the
		 * function tests the edge `(v,w)`. For undirected graphs, it tests whether
		 * @p v and @p w are adjacent.
		 *
		 * @param v Source vertex. It must be a valid vertex index.
		 * @param w Destination vertex. An invalid index is treated as a non-adjacent
		 *          vertex.
		 * @return `true` if the edge `(v,w)` exists; otherwise, `false`.
		 *
		 * @pre `0 <= v < num_vertices()`.
		 *
		 * @note The validity of @p v is checked only by an assertion. The explicit
		 *       run-time check on @p w protects accesses involving sparse or reduced
		 *       graph representations.
		 */
		virtual	bool is_edge(vertex_t v, vertex_t w) const;

		/**
		* @brief returns TRUE if (v, v) edges are present
		**/
		bool is_self_loop() const;

		////////////////
		//Comparisons
	public:

		/**
		 * @brief Compares two graphs for structural equality.
		 *
		 * Graphs are equal when they have the same number of vertices and identical
		 * adjacency matrices. Names, paths, and cached edge counts are ignored.
		 *
		 * @tparam U Bitset type used by both graphs.
		 * @param lhs First graph.
		 * @param rhs Second graph.
		 * @return `true` if the graphs are structurally equal; otherwise, `false`.
		 */
		template <class U>
		friend bool operator ==	 (const Graph<U>& lhs, const Graph<U>& rhs);

		/**
		 * @brief Compares two graphs for structural inequality.
		 *
		 * @tparam U Bitset type used by both graphs.
		 * @param lhs First graph.
		 * @param rhs Second graph.
		 * @return `true` if the vertex counts or adjacency matrices differ;
		 *         otherwise, `false`.
		 */
		template <class U>
		friend bool operator !=	 (const Graph<U>& lhs, const Graph<U>& rhs);

		////////////
		// Read / write basic operations

	public:
		/**
		* @brief reads a simple directed unweighted graph in DIMACS format
		* @returns 0 if correct, -1 in case of error (error code non-throwing interface)
		* @details: iterates on the parsed number of edges
		*
		* TODO: iterate on number of lines 'e' and check if it is consistent
		*		with the parsed number of edges
		**/
		int read_dimacs(const std::string& filename) noexcept;

		/**
		* @brief reads a graph matrix exchange format (at the moment only MCPS)
		* @returns 0 if correct, -1 in case of error (error code non-throwing interface)
		**/
		int read_mtx(const std::string& filename) noexcept;

		/**
		* @brief reads a graph in list of edges format
		* @returns 0 if correct, -1 in case of error (error code non-throwing interface)
		**/
		int read_EDGES(const std::string& filename) noexcept;

		/**
		* @brief reads 0-1 adjacency matrix (rows) with a first line indicating |V|
		* @details: (error code non-throwing interface)
		**/
		int read_01(const std::string& filename) noexcept;

		/*
		* @brief writes directed graph in dimacs format
		*
		*		(specialized for sparse graphs)
		*
		* @param o output stream
		*/
		virtual	void  write_dimacs(std::ostream& o);

		/*
		* @brief writes directed graph in edge list format
		* @param o output stream
		*/
		virtual	void  write_EDGES(std::ostream& o);

		/** @name DIMACS output primitives
		 * These functions write individual parts of a DIMACS file. They are intended
		 * for graph format implementations and advanced callers.
		 * @{
		 */

		/**
		* @brief writes timestamp for dimacs format
		**/
		std::ostream& timestamp_dimacs(std::ostream& o = std::cout) const;

		/**
		* @brief writes graph name for dimacs format
		**/
		std::ostream& name_dimacs(std::ostream& o = std::cout) const;

		/**
		 * @brief Writes the DIMACS problem line for this graph.
		 *
		 * Writes `p edge n m`, where `n` is the number of vertices and `m` is the
		 * number of edges.
		 *
		 * @param out Output stream.
		 * @param lazy If `true`, num_edges() may use a valid cached edge count.
		 *             If `false`, the edge count is recomputed.
		 * @return Reference to @p out.
		 */
		std::ostream& header_dimacs(
			std::ostream& o = std::cout,
			bool lazy = true) const;
		
		/** @} */

		////////////
		// I/O 

		/**
		 * @brief Writes the graph's vertex count, edge count, and density.
		 *
		 * The edge count and density are obtained through num_edges() and density(),
		 * so derived graph types use their corresponding implementations.
		 *
		 * @param lazy If `true`, a valid cached edge count may be used. If `false`,
		 *             the edge count is recomputed from the adjacency matrix.
		 * @param out Output stream.
		 * @param trailing_new_line If `true`, appends a newline after the data.
		 * @return Reference to @p out.
		 *
		 * @note Recomputing the edge count may be costly for large graphs.
		 */
		ostream& print_data(
			bool lazy = true,
			std::ostream & = std::cout, 
			bool trailing_new_line = true) const;

		/**
		 * @brief Writes the adjacency matrix in a readable 0–1 format.
		 *
		 * Each row represents a vertex, and each column indicates whether an edge
		 * from that vertex to the corresponding vertex exists.
		 *
		 * @param out Output stream.
		 * @param trailing_new_line If `true`, appends a newline after the matrix.
		 * @return Reference to @p out.
		 */
		ostream& print_adj(
			std::ostream & = std::cout, 
			bool trailing_new_line = true) const;

		/**
		 * @brief Writes the graph's edges in `[v]-->[w]` format.
		 *
		 * For a directed graph, each edge is written from its source vertex to its
		 * destination vertex. Derived graph types may override this function to
		 * control how their edges are listed.
		 *
		 * @param out Output stream.
		 * @param trailing_new_line If `true`, appends a newline after the final edge.
		 * @return Reference to @p out.
		 */
		virtual ostream& print_edges(
			std::ostream & = std::cout, 
			bool trailing_new_line = false);

		/*
		* @brief streams edges of the subgraph induced by a set of vertices to output stream
		* @param bbsg input (bit) set of vertices
		* @param o output stream
		*/
		//template <class U = vertex_bitset_t>
		ostream& print_edges(
			const vertex_bitset_t& bbsg,
			ostream& o = std::cout)	const;


		//////////////////
		// handlers
	protected:
		/**
		 * @brief Reports a graph initialization failure and terminates the program.
		 *
		 * @param message Description of the failure.
		 */
		[[noreturn]]
		static void graph_initialization_error(const char* message) noexcept
		{
			LOG_ERROR(message);
			LOG_ERROR("Terminating the program.");
			std::terminate();
		}

		//////////////////////////
		// data members
	protected:

		/**
		* @brief Adjacency matrix represented as one vertex bitset per row.
		*
		* Row `v` contains the outgoing neighbors of vertex `v`. For undirected
		* graphs, the matrix is symmetric.
		*/
		std::vector<vertex_bitset_t> adj_;		

		/**
		* @brief Number of vertices in the graph.
		*/
		int NV_ = 0;

		/**
		* @brief Number of bit blocks required for each adjacency row.
		*
		* For sparse bitset representations, this is the maximum possible number
		* of blocks rather than the number currently allocated in every row.
		*/
		int NBB_ = 0;

		/**
		* @brief Cached number of edges in the graph.
		*
		* This value may be recomputed by num_edges() when lazy evaluation is
		* disabled or the edge count is no longer valid.
		*/
		mutable std::size_t NE_;
			
		
		/**
		 * @brief Indicates whether NE_ contains the current number of edges.
		 *
		 * If false, num_edges() must recompute the edge count from the adjacency
		 * matrix before returning or caching it.
		 */
		mutable bool edge_count_valid_ = false;								

		/**
		 * @brief Graph instance name without its directory path.
		 */
		std::string name_;	

		/**
		 * @brief Directory path associated with the graph instance.
		 */
		std::string path_;						
	};	
		
	
}//end namespace bitgraph

//////////////////////////////////////////
// Necessary implementation of template methods in header file

#include "detail/simple_graph_imp.h"
#include "detail/simple_sparse_graph_imp.h"			// specialized implementation for sparse graphs


#endif // BITSCAN_GRAPH_SIMPLE_GRAPH_H
