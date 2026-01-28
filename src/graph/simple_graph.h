 /**
   * @file simple_graph.h
   * @brief class Graph for simple graphs 
   *
   * @details: created 17/6/10, last_update 06/01/25
   * @author pss
   *
   * This code is part of the GRAPH 1.0 C++ library
   * 
   * TODO use SFINAE to filter types (10/01/2025)
   *
   **/

#ifndef __SIMPLE_GRAPH_H__
#define __SIMPLE_GRAPH_H__

#include "bitscan/bitscan.h"					//dependency to BITSCAN lib				
#include "formats/dimacs_format.h"
#include "formats/mmio.h"
#include "formats/edges_format.h"
#include "formats/mmx_format.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <vector>

namespace bitgraph {
					

	//////////////////
	//
	// Generic class Graph<BitSetT>
	// 
	// (BitSetT is a bitset currently limited to the BBScan and BBScanSP types)
	// 
	//////////////////

	template<class BitSetT = BBScan>
	class Graph {

		//filter out invalid types
		static_assert(std::is_same<BBScan, BitSetT>::value ||
			std::is_same<BBScanSp, BitSetT>::value, "is not a valid Graph type");

		friend class GraphConversion;

	public:

		using index_t = int;

		using Self = Graph<BitSetT>;				//own type
		using bitset_type = BitSetT;				//basic type (a type of bitset)
		using VertexBitset = bitset_type;			//alias for semantic type
	

		/////////////			
		//construction / destruction
		Graph() noexcept;													//creates empty graph
		explicit Graph(std::size_t NV);										//creates graph with n=|V| and m=0 	
		explicit Graph(std::string filename);								//creates graph from file		
		Graph(std::size_t n, int* adj[], std::string filename = "");		//old-style adjacency matrix

		//move and copy semantics allowed
		Graph(const Graph& g) = default;
		Graph& operator =					(const Graph& g) = default;
		Graph(Graph&& g)			noexcept = default;
		Graph& operator =					(Graph&& g)			noexcept = default;

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
		std::size_t size() const  noexcept { 
			assert(NV_ >= 0);
			return static_cast<std::size_t>(NV_); 
		}

		/**
		* @brief number of vertices of the graph as signed integer
		* @return: number of vertices as a signed integer
		* @details: aliases order() and num_vertices()
		**/
		index_t num_vertices() const  noexcept { return NV_; }
		index_t order() const  noexcept { return NV_; }


		/**
		* @brief number of bitblocks in each bitset (row) of the adjacency matriz
		*		 In the case of sparse graphs, the value is the maximum capacity
		*		 of all bitsets.
		**/
		index_t num_blocks() const noexcept { return NBB_; }

		/*
		* @brief Counts the number of edges	(includes self loops)		
		* @param lazy if TRUE (reads value @NE_)
		*			  if FALSE counts and updates @NE_
		* @returns number of edges
		* @details: it can be a heavy operation for massive graphs
		*/
		virtual	std::size_t num_edges(bool lazy = true);

		/**
		* @brief Counts the number of edges	in an induced subgraph by a set of vertices
		* @param set input bitset of vertices that induces the subgraph
		* @returns number of edges
		**/
		virtual	std::size_t num_edges(const BitSetT& set)	const;

		const vector<BitSetT>& adjacency_matrix()		const { return adj_; }
		const BitSetT& neighbors(int v)					const { return adj_[v]; }
		BitSetT& neighbors(int v) { return adj_[v]; }

		//////////////////////////
		// memory allocation 
	public:

		/**
		* @brief resets to empty graph given name and number of vertices
		* @param NV number of vertices
		* @param name name of the instance
		* @details: fast-fail policy - exits if failure
		* @date: created 31/12/24, last_update 07/12/25
		**/
		void reset(std::size_t NV, string name = "") noexcept;

		/**
		* @brief sets graph from file in dimacs/MTX/Edges formats (in this order)
		* @param filename file
		* @details: fast-fail policy - exits if failure
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
		template <class U = BitSetT>
		double density(const U& set);

		/**
		* @brief number of non-empty bit blocks / total number of bit blocks
		*
				 Specialized for sparse graphs
		*		 (in the case of sparse graphs, density is expected to be 1.0)
		*
		**/
		double block_density()						const;

		/**
		* @brief number of allocated blocks / total possible number of blocks
		*
		*		 I.ONLY for sparse graphs
		**/
		double block_density_sparse()						const;

		/**
		* @brief average measure of block density (averages the density of each sparse bitset)
		*
		*		 I.ONLY for sparse graphs
		**/
		double average_block_density_sparse()						const;

		/**
		* @brief number of outgoing edges from v
		* @param v input vertex
		**/
		int degree_out(int v)					const { return adj_[v].count(); }

		/**
		* @brief number edges incident to v
		* @param v input vertex
		**/
		int degree_in(int v)					const;

		/**
		* @brief Computes complement graph
		* @param g: output complement graph
		* @return 0 if success, -1 if error
		**/
		int create_complement(Graph& g)					 const;

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
		virtual void add_edge(int v, int w);

		/**
		* @brief removes edge {v -> w} from the graph
		* @param v outgoing endpoint
		* @param w ingoing endpoint
		**/
		virtual void remove_edge(int v, int w);

		/**
		* @brief removes edges with endpoint in v (outgoing/ingoing)
		* @param v input vertex
		**/
		void remove_edges(int v);

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
		int gen_random_edge(int v, int w, double p);

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
		void remove_vertices(const BitSet& set, Graph& g);

		//////////////	
		// deleted - CHECK	
		virtual void remove_vertices(const BitSet& set) = delete;	//commented out implementation - EXPERIMENTAL

		/////////////
		// Boolean properties
	public:

		virtual	bool is_edge(int v, int w)			const { return(adj_[v].is_bit(w)); }

		/**
		* @brief returns TRUE if (v, v) edges are present
		**/
		bool is_self_loop()						const;

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
		// I/O basic operations

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
		template <class U = BitSetT>
		ostream& print_edges(U& bbsg, ostream& o = std::cout)	const;

		//////////////////////////
		// data members
	protected:
		std::vector<BitSetT> adj_;		//adjacency matrix 

		index_t NV_;					//number of vertices
		std::size_t NE_;				//number of edges (can be very large)
		index_t NBB_;					//number of bit blocks per row (in the case of sparse graphs this is a maximum value)

		//names
		std::string name_;				//name of instance, without path	
		std::string path_;				//path of instance
	};

	
}//end namespace bitgraph

//////////////////////////////////////////
// Necessary implementation of template methods in header file


namespace bitgraph {
	
	template <class U>
	bool operator== (const Graph<U>& lhs, const Graph<U>& rhs) {
		return lhs.adj_ == rhs.adj_;
	}

	template <class U>
	bool operator!= (const Graph<U>& lhs, const Graph<U>& rhs) {
		return lhs.adj_ != rhs.adj_;
	}

} // namespace bitgraph


namespace bitgraph {

	template<class BitSetT>
	template <class U>
	inline
		double Graph<BitSetT>::density(const U& bbN) {
		BITBOARD  edges = num_edges(bbN);
		if (edges == 0) { return 0.0; }

		BITBOARD  pc = bbN.popcn64();
		return edges / static_cast<double>(pc * (pc - 1) / 2);
	}

	template<class BitSetT>
	template<class U>
	inline
		std::ostream& Graph<BitSetT>::print_edges(U& bbsg, std::ostream& o) const {

		for (int i = 0; i < NV_ - 1; i++) {

			if (!bbsg.is_bit(i)) continue;

			for (int j = i + 1; j < NV_; j++) {

				if (!bbsg.is_bit(j)) continue;

				if (is_edge(i, j)) {
					o << "[" << i << "]" << "-->" << "[" << j << "]" << endl;
				}
				if (is_edge(j, i)) {
					o << "[" << j << "]" << "-->" << "[" << i << "]" << endl;
				}
			}
		}
		return o;
	}

	//////////////////////////////////////////
	//
	// class Graph<BitSetT> implementation

	template<class BitSetT>
	inline
		Graph<BitSetT>::Graph(void) noexcept :
		NV_(0), NE_(0), NBB_(0),
		name_(""), path_("")
	{
	}

	template<class BitSetT>
	inline
		Graph<BitSetT>::Graph(string filename) :
		NV_(0), NE_(0), NBB_(0),
		name_(""), path_("")
	{
		reset(filename);		
	}

	template<class BitSetT>
	inline
		Graph<BitSetT>::Graph(std::size_t NV) {
		name_.clear();
		path_.clear();
		reset(NV);
	}

	template <class BitSetT>
	inline
		Graph<BitSetT>::Graph(std::size_t NV, int* adj[], string filename) {
		
		///////////////
		reset(NV);
		//////////////
		
		set_name(filename);

		//add edges
		const auto nV = static_cast<int>(NV);
		for (auto i = 0; i < nV; i++) {
			for (int j = 0; j < nV; j++) {
				if (adj[i][j] == 1) {
					add_edge(i, j);
				}
			}
		}
	}


	template<class BitSetT>
	inline
		void Graph<BitSetT>::set_name(std::string name) {

		//update name
		size_t found = name.find_last_of("/\\");

		//update path and name
		if (found != string::npos) {
			name_ = name.substr(found + 1);
			path_ = name.substr(0, found + 1);  //includes slash
		}
		else {
			name_ = std::move(name);
			path_.clear();
		}
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::reset() noexcept {
		adj_.clear(), name_.clear(), path_.clear();
		NV_ = 0, NBB_ = 0, NE_ = 0;
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::reset(std::size_t NV, string name) noexcept {
		
		//check size - must fit in int type
		if (NV > std::numeric_limits<int>::max()) {
			LOGG_ERROR("Invalid graph size ", NV, " - Graph<BitSetT>::reset");
			LOG_ERROR("exiting... ");
			std::exit(EXIT_FAILURE);
		}

		//initialization
		NV_ = static_cast<int>(NV);
		NBB_ = INDEX_1TO1(NV_);
		NE_ = 0;

		try {
			//////////////////////////////
			adj_.assign(NV, BitSetT(NV));				//bitsets initialize to 0 - CHECK 		
			//////////////////////////////
		}
		catch (const std::bad_alloc& e) {
			LOG_ERROR("memory for graph not allocated - Graph<BitSetT>::reset");
			LOG_ERROR("%s", e.what());
			LOG_ERROR("exiting... ");
			std::exit(EXIT_FAILURE);
		}

		//update instance name
		this->set_name(std::move(name));

	}

	template<class BitSetT>
	inline
		Graph<BitSetT>& Graph<BitSetT>::create_subgraph(int first_k, Graph<BitSetT>& newg) const {

		//assert is size required is greater or equal current size
		if (first_k >= NV_ || first_k <= 0) {
			LOG_ERROR("Bad new size - graph remains unchangedGraph<BitSetT>&- Graph<BitSetT>::create_subgraph");
			return newg;
		}

		//////////////////////
		newg.reset(static_cast<std::size_t>(first_k));
		///////////////////////
				
		auto bbh = WDIV(first_k - 1);

		//copy the relevant vertices of the adjacency matrix
		for (int i = 0; i < newg.NV_; ++i) {
			for (int j = 0; j <= bbh; ++j) {
				newg.adj_[i].block(j) = adj_[i].block(j);
			}
			//trims last bitblock
			newg.adj_[i].block(bbh) &= ~Tables::mask_high[WMOD(first_k - 1)];
		}

		return newg;
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::shrink_to_fit() {

		for (int v = 0; v < NV_; ++v) {
			adj_[v].shrink_to_fit();
		}

	}

	template<class BitSetT>
	inline
		int Graph<BitSetT>::shrink_to_fit(std::size_t N) {

		/*LOG_ERROR("not yet implemented for non-sparse graphs - Graph<BitSetT>::shrink_to_fit");
		LOG_ERROR("graph remains unchanged");*/

		//assertions
		if (NV_ <= N) {
			LOGG_WARNING("Wrong shrinking size ", N, " the graph remains unchanged - Graph<BitSetT>::shrink_to_fit");
			return -1;
		}

		//sets to 0 bitblocks outside the range but
		//does not remove the empty bitbloks
		for (int v = 0; v < NV_; ++v) {
			adj_[v].erase_bit(NV_, -1);
		}

		//resizes adjacency matrix
		adj_.resize(N);
		NV_ = static_cast<int>(N);
		NE_ = 0;												//so that when required, the value will be recomputed
		NBB_ = INDEX_1TO1(NV_);									//maximum number of bitblocks per row (for sparse graphs)		

		return 0;
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::reset(string filename) noexcept {
		if (read_dimacs(filename) == -1) {
			if (read_mtx(filename) == -1) {
				if (read_EDGES(filename) == -1) {
					if (read_01(filename) == -1) {
						LOGG_ERROR("Unable to read a graph from file ", filename, "- Graph<BitSetT>::reset");
						LOG_ERROR("Formats considered: DIMACS / MTX / EDGES / 01");
						LOG_ERROR("exiting...");
						std::exit(EXIT_FAILURE);
					}
				}
			}
		}		
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::add_edge(int v, int w) {
		if (v != w) {
			adj_[v].set_bit(w);
			NE_++;
		}
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::remove_edge(int v, int w) {
		adj_[v].erase_bit(w);
		NE_--;
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::remove_edges(int v) {

		//erases all outgoing edges from v
		adj_[v].erase_bit();

		//erases all ingoing edges
		for (int w = 0; w < NV_; w++) {
			if (w == v) continue;
			adj_[w].erase_bit(v);
		}

		//updates edges
		NE_ = 0;					//resets edges to avoid lazy evaluation later

	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::remove_edges() {
		for (int v = 0; v < NV_; ++v) {
			adj_[v].erase_bit();
		}

		NE_ = 0;
	}

	template <class BitSetT>
	inline
		ostream& Graph<BitSetT>::print_adj(std::ostream& o, bool eofl) const {

		for (int i = 0; i < NV_; ++i) {
			for (int j = 0; j < NV_; ++j) {
				if (is_edge(i, j)) {
					o << "1" << " ";
				}
				else {
					o << "0" << " ";
				}
			}
			o << endl;
		}

		if (eofl) o << endl;
		return o;
	}

	template<class BitSetT>
	inline
		std::ostream& Graph<BitSetT>::timestamp_dimacs(std::ostream& o) const {
		o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();
		return o;
	}

	template<class BitSetT>
	inline
		std::ostream& Graph<BitSetT>::name_dimacs(std::ostream& o) const {
		if (!name_.empty())
			o << "\nc " << name_.c_str() << endl;
		return o;
	}

	template<class BitSetT>
	inline
		std::ostream& Graph<BitSetT>::header_dimacs(std::ostream& o, bool lazy) {
		o << "p edge " << NV_ << " " << num_edges(lazy) << endl << endl;
		return o;
	}

	template<class BitSetT>
	inline
		int Graph<BitSetT>::read_dimacs(const string& filename) noexcept{

		int n = 0, m = 0, v1 = 0, v2 = 0;			

		fstream f(filename.c_str());
		if (!f) {
			LOG_ERROR("Graph<BitSetT>::read_dimacs-File could not be opened reading DIMACS format");
			reset();
			return -1;
		}

		if (gio::dimacs::read_dimacs_header(f, n, m) == -1) {
			reset();
			f.close();
			return -1;
		}

		reset(n);
		gio::skip_empty_lines(f);   
		
		////////////////////////
		//parse edges directly from the stream
		string line; char c;
		for (int e = 0; e < m; e++) {			//iterates on the number of edges, TODO - iterate on lines and check edges
			f >> c;
			if (c != 'e') {
				LOGG_ERROR(filename, ":wrong header for edges reading DIMACS format");
				reset();
				f.close();
				return -1;
			}

			//read and add edge
			f >> v1 >> v2;
#ifdef DIMACS_INDEX_0_FORMAT 
			add_edge(v1, v2);
#else
			add_edge(v1 - 1, v2 - 1);
#endif
					
			//removes remaining part of the line
			std::getline(f, line);		
		}

		f.close();
				
		set_name(filename);			//removes full path
		return 0;
	}

	template<class BitSetT>
	inline
		int Graph<BitSetT>::read_01(const string& filename) noexcept {


		int size;
		char ch, line[250];

		fstream f(filename.c_str());
		if (!f) {
			LOG_ERROR("Graph<BitSetT>::read_01-File could not be opened reading DIMACS format");
			reset();
			f.close();
			return -1;
		}

		//read size
		f >> size;

		///////////////
		reset(size);
		///////////////

		f.getline(line, 250);  //remove final part of the line

		//read rows		
		for (int r = 0; r < size; r++) {
			for (int c = 0; c < size; c++) {
				f >> ch;
				if ((r < c) && ch == '1') {
					add_edge(r, c);
				}
			}
			f.getline(line, 250);  //remove final part of the line
		}
		f.close();

		//name (removes path)
		set_name(filename);
		return 0;
	}

	template<class BitSetT>
	inline
		int  Graph<BitSetT>::read_mtx(const string& filename) noexcept {


		MMI<Graph<BitSetT> > myreader(*this);
		return (myreader.read(filename));
	}

	template<class BitSetT>
	inline
		int  Graph<BitSetT>::read_EDGES(const string& filename) noexcept {

		EDGES<Graph<BitSetT> > myreader(filename, *this);
		return (myreader.read());
	}

	template<class BitSetT>
	inline
		ostream& Graph<BitSetT>::print_data(bool lazy, std::ostream& o, bool eofl) {

		if (!name_.empty()) { o << name_.c_str() << '\t'; }

		////////////////////////////////////////////////////////////////
		o << "n:= " << num_vertices() << "\t"
			<< std::fixed << "m:= " << num_edges(lazy) << "\t"
			<< std::setprecision(3) << "p:= " << density(true);
		////////////////////////////////////////////////////////////////

		/*Note: lazy comp. of density since the number of edges have already been computed previously */

		if (eofl) { o << std::endl; }
		return o;
	}

	template<class BitSetT>
	inline
		ostream& Graph<BitSetT>::print_edges(std::ostream& o, bool eofl) {

		for (int i = 0; i < NV_ - 1; ++i) {
			for (int j = i + 1; j < NV_; ++j) {

				if (is_edge(i, j)) {
					o << "[" << i << "]" << "-->" << "[" << j << "]" << endl;
				}
				if (is_edge(j, i)) {
					o << "[" << j << "]" << "-->" << "[" << i << "]" << endl;
				}

			}
		}

		if (eofl) { o << std::endl; }
		return o;
	}



	template<class BitSetT>
	inline
		std::size_t Graph<BitSetT>::num_edges(const BitSetT& bbn) const {

		std::size_t NE = 0;

		for (int i = 0; i < NV_; ++i) {
			if (bbn.is_bit(i)) {
				for (int j = 0; j < NV_; ++j) {
					if (bbn.is_bit(j)) {						//includes possible self loops
						if (adj_[i].is_bit(j)) { NE++; }
					}
				}
			}
		}

		return NE;
	}

	template<class BitSetT>
	inline
		std::size_t Graph<BitSetT>::num_edges(bool lazy) {

		if (!lazy || NE_ == 0) {					//no lazy evaluation if NE_ = 0
			NE_ = 0;
			for (int i = 0; i < NV_; i++) {
				NE_ += adj_[i].count();
			}
		}

		return NE_;
	}

	template<class BitSetT>
	inline
		double Graph<BitSetT>::density(bool lazy) {
		BITBOARD max_edges = NV_;								//type MUST BE for very large graphs as (I) is bigger than unsigned int
		max_edges *= (max_edges - 1);							//(I)
		return (num_edges(lazy) / (double)max_edges);		//n*(n-1) edges (since it is a directed graph))
	}

	template<class BitSetT>
	inline
		double Graph<BitSetT>::block_density()	const {

		size_t nBB = 0;
		for (auto v = 0u; v < NV_; ++v) {
			for (auto bb = 0u; bb < NBB_; bb++) {
				if (adj_[v].block(bb))		//non-empty bitblock
					nBB++;
			}
		}

		return (nBB / static_cast<double>(NBB_ * NV_));
	}

	template<class BitSetT>
	inline
		double Graph<BitSetT>::block_density_sparse()	const {

		LOG_ERROR("function only for sparse graphs - Graph<BitSetT>::block_density_sparse");
		return -1;
	}

	template<class BitSetT>
	inline
		double Graph<BitSetT>::average_block_density_sparse()	const {

		LOG_ERROR("function only for sparse graphs - Graph<BitSetT>::average_block_density_sparse");
		return -1;
	}


	template<class BitSetT>
	inline
		bool Graph<BitSetT>::is_self_loop() const {
		for (auto i = 0u; i < NV_; i++)
			if (adj_[i].is_bit(i)) {
				return true;
			}
		return false;
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::remove_vertices(const BitSet& bbn, Graph& g) {

		//determine the size of the graph g
		auto pc = bbn.count();
		auto new_size = NV_ - pc;

		if (new_size <= 0) {
			LOG_ERROR("empty graph after deletion - Graph<BitSetT>::remove_vertices");
			g.reset();
			return;
		}

		//initialize new graph
		if (g.reset(new_size) == -1) {
			LOG_ERROR("memory for graph not allocated - Graph<BitSetT>::remove_vertices");
			return;
		}

		//computes the induced graph in g
		g.name(this->name_);
		int l = 0;

		//adds the edges with endpoints not in the bbn set
		for (int i = 0; i < NV_ - 1; i++) {
			if (bbn.is_bit(i)) continue;				//jumps over vertices marked for deletion
			int m = l + 1;
			for (int j = i + 1; j < NV_; j++) {
				if (bbn.is_bit(j)) continue;				//jumps over vertices marked for deletion
				if (adj_[i].is_bit(j)) {
					g.add_edge(l, m);
				}
				m++;
			}
			l++;
		}

	}

	//template<class BitSetT>
	//void Graph<BitSetT>::remove_vertices (const BitSet& bbn){
	/////////////////
	//// Experimental: deletes input list of nodes by creating a temporal graph
	////
	//// OBSERVATIONS:
	//// 1.Inefficient implementation with double allocation of memory
	//
	//	Graph<BitSetT> g;
	//	this->remove_vertices(bbn,g);			//allocation 1
	//	(*this)=g;								//allocation 2	
	//}

	template<class BitSetT>
	inline
		int Graph<BitSetT>::degree_in(int v) const {

		int res = 0;
		for (auto i = 0u; i < NV_; i++) {
			if (i == v) continue;
			if (adj_[i].is_bit(v)) { res++; }

		}
		return res;
	}

	template<class BitSetT>
	inline
		int Graph<BitSetT>::create_complement(Graph& g) const
	{
		//resets g with new allocation
		if (g.reset(NV_) == -1) return -1;

		for (int i = 0; i < NV_ - 1; ++i) {
			for (int j = i + 1; j < NV_; ++j) {

				if (!adj_[i].is_bit(j)) {
					g.add_edge(i, j);
				}

				if (!adj_[j].is_bit(i)) {
					g.add_edge(j, i);
				}
			}
		}

		return 0;
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::make_bidirected() {

		for (int i = 0; i < NV_; ++i) {
			for (int j = 0; j < NV_; ++j) {
				if (is_edge(i, j)) add_edge(j, i);
				if (is_edge(j, i)) add_edge(i, j);
			}
		}

		NE_ = 0;	//resets edges to avoid lazy evaluation later
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::gen_random_edges(double p) {

		//removes all edges
		remove_edges();

		//sets directed edges with probability p
		for (int i = 0; i < NV_; ++i) {
			for (int j = 0; j < NV_; ++j) {
				if (_rand::uniform_dist(p)) {
					add_edge(i, j);
				}
			}
		}
	}

	template<class BitSetT>
	inline
		int Graph<BitSetT>::gen_random_edge(int v, int w, double p) {

		//assert - TODO condition to DEBUG mode
		if (v == w || v >= NV_ || w >= NV_ || p < 0 || p > 1) {
			LOG_ERROR("wrong input params - Graph<BitSetT>::gen_random_edge");
			return -1;
		}

		//generates edge
		if (_rand::uniform_dist(p)) {
			add_edge(v, w);
		}

		return 0;
	}

	template<class BitSetT>
	inline
		void Graph<BitSetT>::write_dimacs(ostream& o) {

		//timestamp comment
		timestamp_dimacs(o);

		//name comment
		name_dimacs(o);

		//dimacs header - recompute edges
		header_dimacs(o, false);

		//write edges 1-based vertex notation 
		for (int v = 0; v < NV_; ++v) {
			for (int w = 0; w < NV_; ++w) {
				if (v == w) continue;
				if (is_edge(v, w)) {								//O(log) for sparse graphs: specialize
					o << "e " << v + 1 << " " << w + 1 << endl;
				}
			}
		}
	}

	template<class BitSetT>
	inline
		void  Graph<BitSetT>::write_EDGES(ostream& o) {

		//timestamp comment
		o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();

		//name comment
		if (!name_.empty())
			o << "\n%  " << name_.c_str() << endl;

		//write edges 1-based vertex notation
		for (int v = 0; v < NV_; ++v) {
			for (int w = 0; w < NV_; ++w) {
				if (v == w) continue;
				if (is_edge(v, w)) {							//O(log) for sparse graphs: specialize
					o << v + 1 << " " << w + 1 << endl;
				}

			}
		}
	}

}//end namespace bitgraph


	//////////////////////////
	//
	// SPECIALIZATIONS (sparse)
	// 

	using GSS = bitgraph::Graph<bitgraph::BBScanSp>;


	////////////////////////
	//
	// Specializations of main template class Graph<BitSetT>
	// for sparse graphs
	//
	////////////////////////

	template<>
	inline
		GSS& GSS::create_subgraph(int first_k, GSS& newg) const {

		//assertions
		if (first_k >= NV_ || first_k <= 0) {
			LOGG_WARNING("Bad new size ", first_k, " - graph remains unchanged - GSS::create_subgraph");
			return newg;
		}

		//allocates memory for the new graph
		newg.reset(first_k);

		//copies first k elements of the adjacency matrix 
		for (auto i = 0; i < newg.NV_; i++) {
			newg.adj_[i] = adj_[i];
			newg.adj_[i].clear_bit(first_k, EMPTY_ELEM);		//closed range
		}

		return newg;
	}

	template<>
	inline
		int GSS::shrink_to_fit(std::size_t N) {

		int num_vert = static_cast<int>(N);

		//assertions
		if (NV_ <= static_cast<int>(N)) {
			LOGG_WARNING("Wrong shrinking size ", N, " the graph remains unchanged - GSS::shrink_to_fit");
			return -1;
		}

		//A) sets to 0 bitblocks outside the range but
		//does not remove the empty bitbloks
		for (int v = 0; v < NV_; ++v) {
			adj_[v].erase_bit(num_vert, NV_);
		}

		//B) EXPERIMENTAL - sets to 0 bitblocks and removes the empty bitblocks 
		//for (auto v = 0; v < size; v++) {
		//	adj_[v].clear_bit(size, EMPTY_ELEM);			
		//}


		//resizes adjacency matrix
		adj_.resize(N);
		NV_ = num_vert;
		NE_ = 0u;											//so that when required, the value will be recomputed
		NBB_ = INDEX_1TO1(num_vert);								//maximum number of bitblocks per row (for sparse graphs)		


		return 0;
	}

	template<>
	inline
		double GSS::block_density()	const {

		std::size_t  nBB = 0;							//number of non-empty bitblocks	
		std::size_t  nBBt = 0;							//number of allocated bitblocks (all should be non-empty in the sparse case)

		for (int v = 0; v < NV_; ++v) {
			const auto sz = adj_[v].size();  
			nBBt += sz;
			const auto deg = static_cast<int>(sz);
			for (int bb = 0; bb < deg; bb++) {
				if (adj_[v].block(bb)) {
					nBB++;								//nBB should be equal to nBBt
				}
			}
		}

		return nBB / static_cast<double>(nBBt);			//density should be 1.0
	}

	template<>
	inline
		double GSS::block_density_sparse() const {

		///////////////////
		assert(NV_ > 0);
		///////////////////

		std::size_t nBBt = 0;							//number of allocated bitblocks (all should be non-empty in the sparse case)

		//number of allocated blocks
		for (int v = 0; v < NV_; ++v) {
			nBBt += adj_[v].size();
		}
				
		BITBOARD blocks_per_row = INDEX_1TO1(NV_);		// ceil(NV_ / double(WORD_SIZE));
		BITBOARD maxBlocks = NV_ * blocks_per_row;

		return static_cast<double>(nBBt) / static_cast<double>(maxBlocks);
	}

	template<>
	inline
		double GSS::average_block_density_sparse() const {

		std::size_t nBB = 0;							//number of non-empty bitblocks	
		std::size_t nBBt = 0;							//number of allocated bitblocks (all should be non-empty in the sparse case)
		double den = 0.0;

		for (int i = 0; i < NV_; ++i) {
			nBB = adj_[i].size();
			nBBt += nBB;
			den += static_cast<double>(adj_[i].count()) /
				(BITBOARD(nBB) * WORD_SIZE);
		}

		return (den / nBBt);
	}

	template<>
	inline
		void GSS::write_dimacs(ostream& o) {


		//timestamp comment
		timestamp_dimacs(o);

		//name comment
		name_dimacs(o);

		//dimacs header - does not recompute edges, can be a heavy operation
		header_dimacs(o, true);


		//write edges 1-based vertex notation
		for (int v = 0; v < NV_; ++v) {

			//non destructive scan of each bitstring
			if (adj_[v].init_scan(bbo::NON_DESTRUCTIVE) != BBObject::noBit) {
				int w = BBObject::noBit;
				while ((w = adj_[v].next_bit()) != BBObject::noBit){
					o << "e " << v + 1 << " " << w + 1 << endl;
				}
			}

		}
	}

#endif
