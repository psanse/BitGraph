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
#include "graph_types.h"
#include "formats/dimacs_format.h"
#include "formats/mmio.h"
#include "formats/edges_format.h"
#include "formats/mmx_format.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

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
						
		using bitset_type = BitsetT;				// basic type (a type of bitset)
		using vertex_bitset_t = bitset_type;
		using VertexBitset = vertex_bitset_t;		// alias for backward compatibility
		
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
		Graph(std::size_t NV, int* adj[], std::string filename = "") noexcept;		

		//move and copy semantics allowed
		Graph(const Graph& g) = default;
		Graph& operator = (const Graph& g) = default;
		Graph(Graph&& g) noexcept = default;
		Graph& operator = (Graph&& g) noexcept = default;

		virtual	~Graph() = default;

		/////////////
		// setters and getters

		/**
		* @brief Sets instance name.
		* @param instance name of instance
		* @details: Separates path and instance name internally (if applicable)
		**/
		void set_name(std::string instance);
		std::string name() const noexcept { return name_; }

		void set_path(std::string path_name) { path_ = std::move(path_name); }
		std::string path() const noexcept { return path_; }

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
		virtual	std::size_t num_edges(bool lazy = true);

		/**
		* @brief Counts the number of edges	in an induced subgraph by a set of vertices
		* @param set input bitset of vertices that induces the subgraph
		* @returns number of edges
		**/
		virtual std::size_t num_edges(const BitsetT& set) const;
		
		const vector<vertex_bitset_t>& adjacency_matrix() const { return adj_; }
		const vertex_bitset_t& neighbors(vertex_t v) const {
			assert(v >= 0 && v < NV_);
			return adj_[v];
		}
		vertex_bitset_t& neighbors(vertex_t v) {
			assert(v >= 0 && v < NV_);
			return adj_[v];
		}

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
		* @brief reduces the capacity to the size of the bitsets that make
		*		 the adjacency matrix.
		* @details: calls the STL shrink_to_fit function for each bitset
		*			of the adjacency matrix
		**/
		void shrink_to_fit();

		/**
		* @brief reduces the graph to the first N vertices (deallocation takes place)
		*
		*		(currently only for sparse graphs)
		*
		* @param N: number of vertices of the new graph, which must be strictly less
		*		    than the current number of vertices NV_
		* @returns 0 if success, otherwise -1 (graph unchanged)
		**/
		int shrink_to_fit(std::size_t N);

		//////////////	
		// Basic operations	
	public:
		/**
		* @brief density of the directed graph
		* @param lazy reads NE_ cached value if TRUE
		**/

		virtual	double density(bool lazy = true);

		/**
		* @brief density of the subgraph induced by a set of vertices
		* @param set input (bit) set of vertices
		**/
		template <class U = vertex_bitset_t>
		double density(const U& set);

		/**
		* @brief number of non-empty bit blocks / total number of bit blocks
		*
				 Specialized for sparse graphs
		*		 (in the case of sparse graphs, density is expected to be 1.0)
		*
		**/
		double block_density() const;

		/**
		* @brief number of allocated blocks / total possible number of blocks
		*
		*		 I.ONLY for sparse graphs
		**/
		double block_density_sparse() const;

		/**
		* @brief average measure of block density (averages the density of each sparse bitset)
		*
		*		 I.ONLY for sparse graphs
		**/
		double average_block_density_sparse() const;

		/**
		* @brief number of outgoing edges from v
		* @param v input vertex
		**/
		int degree_out(vertex_t v) const {
			assert(v >= 0 && v < NV_);
			return adj_[v].count();
		}

		/**
		* @brief number edges incident to v
		* @param v input vertex
		**/
		int degree_in(vertex_t v) const;

		/**
		* @brief Computes complement graph
		* @param g: output complement graph
		* @return 0 if success, -1 if error
		**/
		int create_complement(Graph& g) const;

		//////////////	
		// Modifiers

	public:
		/**
		* @brief adds edge {v -> w} to the graph,
		*		 no self loops allowed
		* @param v: outgoing endpoint
		* @param w: ingoing endpoint
		*
		**/
		virtual void add_edge(vertex_t v, vertex_t w);

		/**
		* @brief removes edge {v -> w} from the graph
		* @param v outgoing endpoint
		* @param w ingoing endpoint
		**/
		virtual void remove_edge(vertex_t v, vertex_t w);

		/**
		* @brief removes edges with endpoint in v (outgoing/ingoing)
		* @param v input vertex
		**/
		void remove_edges(vertex_t v);

		/**
		* @brief removes all edges
		**/
		void remove_edges();


		/**
		* @brief makes all edges bidirected (conversion to undirected graph)
		**/
		void make_bidirected();

		//random generation

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
		// deleted - CHECK	
		virtual void remove_vertices(const Bitset& set) = delete;	//commented out implementation - EXPERIMENTAL

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
		* @brief determines if two graphs have the same adjacency matrices
		* @param lhs left hand side graph
		* @param rhs right hand side graph
		* @returns TRUE if lhs.adj_ == rhs.adj_
		**/
		template <class U>
		friend bool operator ==	 (const Graph<U>& lhs, const Graph<U>& rhs);
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

		/**
		* @brief writes timestamp for dimacs format
		**/
		std::ostream& timestamp_dimacs(std::ostream& o = std::cout) const;

		/**
		* @brief writes graph name for dimacs format
		**/
		std::ostream& name_dimacs(std::ostream& o = std::cout) const;

		/**
		* @brief writes graph header for dimacs format
		**/
		std::ostream& header_dimacs(std::ostream& o = std::cout, bool lazy = true);

		/*
		* @brief writes directed graph in edge list format
		* @param o output stream
		*/
		virtual	void  write_EDGES(std::ostream& o);

		////////////
		// I/O operations

		/**
		* @brief prints basic data of the graph to the output stream (n, m and density)
		* @param lazy if TRUE, reads the number of edges from the cached value @NE_ to compute density
		* @details Uses the Template Method Pattern (num_edges will be overriden in derived classes)
		* @details Density can be a heavy operation to compute, since it requires the number of edges.
		*		   If  @lazy is TRUE the number of edges is read from the cached value @NE_
		**/
		ostream& print_data(bool lazy = true, std::ostream & = std::cout, bool eofl = true);

		/**
		* @brief Adjacency matrix to the output stream, in a readable 0-1 format
		**/
		ostream& print_adj(std::ostream & = std::cout, bool eofl = true) const;

		/**
		* @brief streams edges of the graph to the output stream in format [v]-->[w]
		**/
		virtual ostream& print_edges(std::ostream & = std::cout, bool eofl = false);

		/*
		* @brief streams edges of the subgraph induced by a set of vertices to output stream
		* @param bbsg input (bit) set of vertices
		* @param o output stream
		*/
		template <class U = vertex_bitset_t>
		ostream& print_edges(U& bbsg, ostream& o = std::cout)	const;


		//////////////////
		// handlers
	protected:
		/**
		 * @brief Reports a graph initialization failure and terminates the program.
		 *
		 * @param message Description of the failure.
		 */
		[[noreturn]]
		inline void graph_initialization_error(const char* message) noexcept
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
		* @brief Cached number of edges in the graph.
		*
		* This value may be recomputed by num_edges() when lazy evaluation is
		* disabled or the edge count is no longer valid.
		*/
		std::size_t NE_;

		/**
		 * @brief Indicates whether NE_ contains the current number of edges.
		 *
		 * If false, num_edges() must recompute the edge count from the adjacency
		 * matrix before returning or caching it.
		 */
		bool edge_count_valid_ = false;

		/**
		* @brief Number of bit blocks required for each adjacency row.
		*
		* For sparse bitset representations, this is the maximum possible number
		* of blocks rather than the number currently allocated in every row.
		*/
		int NBB_ = 0;								

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


#endif // BITSCAN_GRAPH_SIMPLE_GRAPH_H
