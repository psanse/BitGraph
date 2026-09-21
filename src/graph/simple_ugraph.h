/**
 * @file simple_ugraph.h 
 * @brief class Ugraph for simple undirected graphs	(no self loops)
 *
 * @created 17/6/10
 * @last_update 23/05/25
 * @author pss
 *
 * This code is part of the GRAPH 1.0 C++ library 
 *
 * @todo implement min_degree functions (03/01/2025)
 **/

#ifndef __SIMPLE_UGRAPH_H__
#define __SIMPLE_UGRAPH_H__

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

		//set of (static) methods to create simple graphs
		static Ugraph make_triangle();
		static Ugraph make_clique(std::size_t n);
		static Ugraph make_cycle(std::size_t n);
		static Ugraph make_star(std::size_t n);

		// @todo other helpers... (03/01/2025)
					
		using BaseT = Graph<BitsetT>;		//parent type

		using bitset_type = typename BaseT::bitset_type;
		using vertex_bitset_t = bitset_type;								// alias for semantic type
		using VertexBitset = vertex_bitset_t;								// alias for backward compatibility

		//constructors - cannot all be inherited	
		Ugraph() : Graph<BitsetT>() {}																// creates empty graph
		explicit Ugraph(std::size_t n) : Graph<BitsetT>(n) {}										// creates empty graph of size n=|V|	
		explicit Ugraph(std::string filename) { this->reset(filename); }							// reads graph from file

		/**
		* @brief Creates a graph from an C-style adjacency matrix
		*
		*		Reads only the upper triangle of the adjacency matrix
		**/
		Ugraph(std::size_t n, int* adj[], string name);			// C-style adjacency matrix

		// @todo: copy constructor, move constructor, copy operator =, move operator = (1/1/2025)

		//destructor
		~Ugraph() = default;

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
		* @brief Computes complement graph
		* @param g output complement graph
		* @return 0 if success, -1 if error
		**/
		int create_complement(Ugraph& g) const;


		/////////////
		// degree-related operations
		// 
		// TODO implement bitstring conversions according to the templates properly (3/4/18) - CHECK (02/01/2025)
	public:

		/**
		* @brief Computes the number of neighbors of v (deg(v))	*
		**/
		int degree(int v) const { return (int)this->adj_[v].count(); }

		/**
		*  @brief number of neighbors of v in a set of vertices
		*
		*		  (specialized for sparse graphs)
		*
		*  @param bbn input non-sparse (bit) set of vertices
		**/
		int degree(int v, const Bitset& bbn) const;

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

namespace bitgraph {
	
	template<class BitsetT>
	template<class VertexSet>
	inline
		int Ugraph<BitsetT>::max_subgraph_degree(VertexSet& sg) const {

		int max_degree = 0;

		int v = BBObject::noBit;
		sg.init_scan(bbo::NON_DESTRUCTIVE);
		while ((v = sg.next_bit()) != BBObject::noBit) {

			//compute max_degree circumscribed to subgraph
			if (max_degree < degree(v, sg)) {
				max_degree = degree(v, sg);
			}
		}
		

		return max_degree;
	}

	template<class BitsetT>
	template<class VertexSet>
	inline int Ugraph<BitsetT>::outgoing_degree(VertexSet& sg) const
	{
		//number of edges
		int nE = 0;

		//scan declaration
		sg.init_scan(bbo::NON_DESTRUCTIVE);

		//main loop
		int v = BBObject::noBit;
		while ((v = sg.next_bit()) != BBObject::noBit) {
			for (auto nBB = 0; nBB < this->NBB_; ++nBB) {
				BITBOARD bb = this->neighbors(v).block(nBB) & ~sg.block(nBB);		//neighbors of v NOT in sg
				nE += bblock::popc64(bb);
			}
		}

		return nE;
	}

	template<class BitsetT>
	inline int Ugraph<BitsetT>::outgoing_degree(std::vector<int> sg) const
	{
		BitsetT bbsg{ this->size(), sg };
		return (outgoing_degree(bbsg));
	}


	template<class BitsetT>
	inline
	ostream& Ugraph<BitsetT>::print_edges(vertex_bitset_t& bbsg, std::ostream& o)
	{
		for (int i = 0; i < this->NV_ - 1; ++i) {
			if (!bbsg.is_bit(i)) continue;
			for (int j = i + 1; j < this->NV_; ++j) {
				if (!bbsg.is_bit(j)) continue;
				if (this->is_edge(i, j)) {
					o << "[" << i << "]" << "--" << "[" << j << "]" << endl;
				}
			}
		}

		return o;
	}
	
	template<class BitsetT>
	inline
		Ugraph<BitsetT> Ugraph<BitsetT>::make_triangle()
	{
		Ugraph<BitsetT> tri(3);
		tri.add_edge(0, 1);
		tri.add_edge(0, 2);
		tri.add_edge(1, 2);

		return tri;
	}

	template<class BitsetT>
	inline
		Ugraph<BitsetT> Ugraph<BitsetT>::make_clique(std::size_t NV)
	{
		Ugraph<BitsetT> clique(NV);

		//sets the adjacency matrix to ONE except for the main diagonal
		const auto nV = static_cast<int>(NV);
		for (int v = 0; v < nV; ++v) {
			clique.neighbors(v).set_bit(0, nV - 1);
			clique.neighbors(v).erase_bit(v);
		}

		return clique;
	}

	template<class BitsetT>
	inline
		Ugraph<BitsetT> Ugraph<BitsetT>::make_cycle(std::size_t NV)
	{
		Ugraph<BitsetT> cycle(NV);

		const auto nV = static_cast<int>(NV);
		for (int v = 0; v < nV - 1; ++v) {
			cycle.add_edge(v, v + 1);
		}
		cycle.add_edge(nV - 1, 0);

		return cycle;
	}

	template<class BitsetT>
	inline
		Ugraph<BitsetT> Ugraph<BitsetT>::make_star(std::size_t NV) {
		Ugraph<BitsetT> star(NV);

		const auto nV = static_cast<int>(NV);
		for (int v = 1; v < nV; ++v) {
			star.add_edge(0, v);
		}

		return star;
	}


	template <class BitsetT>
	inline
		Ugraph<BitsetT>::Ugraph(std::size_t NV, int* adj[], string name) {

		this->reset(NV);
		this->set_name(name);
		
		for (int i = 0; i < this->NV_ - 1; ++i) {
			for (int j = i + 1; j < this->NV_; ++j) {
				if (adj[i][j] == 1) {
					add_edge(i, j);
				}
			}
		}
	}

	template<class BitsetT>
	inline
		std::size_t Ugraph<BitsetT>::num_edges(bool lazy ) {

		if (!lazy || !this->edge_count_valid_ ) {
			this->NE_ = 0;

			/*
			 * Count only the strict upper triangle of the symmetric adjacency
			 * matrix. Each undirected edge is therefore counted exactly once,
			 * and diagonal entries are excluded.
			 */
			for (vertex_t i = 0; i < this->NV_; ++i) {
				this->NE_ += adj_[i].count(i + 1, -1);
			}
					
			this->edge_count_valid_ = true;
		}

		return this->NE_;
	}

	template<class BitsetT>
	inline
		std::size_t Ugraph<BitsetT>::num_edges(const BitsetT& bbn) const {
		std::size_t NE = 0;

		//reads only the upper triangle of the adjacency matrix
		for (vertex_t i = 0; i < this->NV_ - 1; ++i) {
			if (bbn.is_bit(i)) {
				for (vertex_t j = i + 1; j < this->NV_; ++j) {
					if (bbn.is_bit(j)) {
						if (this->adj_[i].is_bit(j)) { ++NE; }
					}
				}
			}
		}

		return NE;
	}

	template<class BitsetT>
	inline
		int Ugraph<BitsetT>::degree(int v, const Bitset& bbn) const {

		int ndeg = 0;
		for (auto i = 0; i < this->NBB_; i++) {
			ndeg += bblock::popc64(this->adj_[v].block(i) & bbn.block(i));
		}

		return ndeg;
	}

	template<class BitsetT>
	inline
		int Ugraph<BitsetT>::degree(int v, const BitsetSp& bbs)	const {
		LOG_ERROR("function not yet implemented, should not be called - Ugraph<BitsetT>::degree");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}


	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::add_edge(int v, int w) {

		if (v != w) {
			this->adj_[v].set_bit(w);
			this->adj_[w].set_bit(v);
			this->NE_++;
		}
	}

	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::remove_edge(int v, int w) {
		if (v != w) {
			this->adj_[v].erase_bit(w);
			this->adj_[w].erase_bit(v);
			this->NE_--;
		}

		edge_count_valid_ = false;		//invalidate edge count
	}

	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::gen_random_edges(double p) {

		//removes all edges
		this->remove_edges();

		using edge_t = std::pair<int, int>;
		std::vector<edge_t> edges;
		edges.reserve(this->NV_ * (this->NV_ - 1) / 2);
		for (int i = 0; i < this->NV_ - 1; ++i) {
			for (auto j = i + 1; j < this->NV_; ++j) {
				edges.emplace_back(i, j);
			}
		}

		//rounds edges to the nearest integer value
		const std::size_t N = edges.size();
		std::size_t M = static_cast<std::size_t>(std::llround(p * N)); 
		if (M > N) { M = N; };

		std::shuffle(edges.begin(), edges.end(), bitgraph::com::_rand::g_iugen.engine() /*std::mt19937_64(std::random_device{}())*/);

		//take the first M edges
		for (std::size_t i = 0; i < M; ++i) {
			add_edge(edges[i].first, edges[i].second);
		}

	}

	template<class BitsetT>
	inline
		int Ugraph<BitsetT>::max_graph_degree() const {

		int max_degree = 0, temp = 0;

		for (int i = 0; i < this->NV_; ++i) {
			temp = degree(i);
			if (temp > max_degree)
				max_degree = temp;
		}

		return max_degree;
	}

	template<class BitsetT>
	inline
		double Ugraph<BitsetT>::density(bool lazy)
	{

		if (this->NV_ < 2) { return 0.0; }

		/*
		 * Convert before multiplication so that the product is computed using a
		 * 64-bit unsigned type. The value may exceed a 32-bit integer.
		 */
		const BITBOARD num_vertices = static_cast<BITBOARD>(this->NV_);
		const BITBOARD max_num_edges = num_vertices * (num_vertices - 1) / 2;

		return static_cast<double>(this->num_edges(lazy)) /
			static_cast<double>(max_num_edges);
	}

	template<class BitsetT>
	inline
		double Ugraph<BitsetT>::density(const BitsetT& vertices) const
	{
		const BITBOARD num_vertices =
			static_cast<BITBOARD>(vertices.count());

		if (num_vertices < 2) {
			return 0.0;
		}

		const BITBOARD edges =
			static_cast<BITBOARD>(num_edges(vertices));

		const BITBOARD maxEdges =
			num_vertices * (num_vertices - 1) / 2;		// undirected graph

		return static_cast<double>(edges) /
			static_cast<double>(maxEdges);

	}


	template<class BitsetT>
	inline
		ostream& Ugraph<BitsetT>::print_degrees(std::ostream& o) const {
		for (int i = 0; i < this->NV_; ++i) {
			o << "deg(" << i << ")" << ":" << degree(i) << " ";
		}
		return o;
	}

	template<class BitsetT>
	inline
		ostream& Ugraph<BitsetT>::print_edges(std::ostream& o, bool eofl) {

		for (int i = 0; i < this->NV_ - 1; ++i) {
			for (int j = i + 1; j < this->NV_; ++j) {

				if (this->is_edge(i, j)) {
					o << "[" << i << "]" << "--" << "[" << j << "]" << endl;
				}
			}
		}

		if (eofl) { o << std::endl; }
		return o;
	}

	template<class BitsetT>
	inline
		ostream& Ugraph<BitsetT>::print_adjacency_matrix(std::ostream& o) const
	{
		for (int i = 0; i < this->NV_; ++i) {
			for (int j = 0; j < this->NV_; ++j) {
				if (this->is_edge(i, j)) {
					o << "1";
				}
				else {
					o << "0";
				}
			}
			o << endl;
		}
		return o;
	}


	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::write_dimacs(ostream& o) {

		//timestamp comment 
		o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp() << endl;

		//name comment
		if (!this->name_.empty())
			o << "c " << this->name_.c_str() << endl;

		//dimacs header
		o << "p edge " << this->NV_ << " " << num_edges(false /* recompute */) << endl << endl;

		//bidirectional edges (1 based in dimacs)
		for (int v = 0; v < this->NV_ - 1; ++v) {
			for (int w = v + 1; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {										//O(log) for sparse graphs: specialize
					o << "e " << v + 1 << " " << w + 1 << endl;
				}
			}
		}
	}

	template<class BitsetT>
	inline
		void  Ugraph<BitsetT>::write_EDGES(ostream& o) {

		//timestamp comment
		o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp() << endl;

		//name coment
		if (!this->name_.empty())
			o << "% " << this->name_.c_str() << endl;

		//write edges - 1 based vertex notation
		for (int v = 0; v < this->NV_ - 1; ++v) {
			for (int w = v + 1; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {							//O(log) for sparse graphs: specialize
					o << v + 1 << " " << w + 1 << endl;
				}
			}
		}
	}

	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::write_mtx(ostream& o) {

		//header comment
		o << "%%MatrixMarket matrix coordinate pattern symmetric" << endl;

		//timestamp comment
		o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp() << endl;

		//name comment
		if (!this->name_.empty())
			o << "% " << this->name_.c_str() << endl;

		//number of vertices and edges
		this->NE_ = 0;																			//eliminates lazy evaluation of edge count 
		o << this->NV_ << " " << this->NV_ << " " << num_edges() << endl;

		//writes edges 1-based vertex notation
		for (int v = 0; v < this->NV_ - 1; ++v) {
			for (int w = v + 1; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {														//O(log) for sparse graphs: specialize
					o << v + 1 << " " << w + 1 << endl;
				}
			}
		}
	}

	template<class BitsetT>
	inline
		int Ugraph<BitsetT>::degree_up(int v, const Bitset& bbn) const {

		int nDeg = 0, nBB = WDIV(v);

		for (int i = nBB + 1; i < this->NBB_; ++i) {
			nDeg += bblock::popc64(this->adj_[v].block(i) & bbn.block(i));
		}

		//truncate the bitblock of v
		nDeg += bblock::popc64(bblock::MASK_1(WMOD(v) + 1, 63) &
			this->adj_[v].block(nBB) & bbn.block(nBB)
		);

		return nDeg;
	}

	template<class BitsetT>
	inline
		int Ugraph<BitsetT>::degree_up(int v) const
	{
		int nDeg = 0, nBB = WDIV(v);

		for (int i = nBB + 1; i < this->NBB_; ++i) {
			nDeg += bblock::popc64(this->adj_[v].block(i));
		}

		//truncate the bitblock of v
		nDeg += bblock::popc64(bblock::MASK_1(WMOD(v) + 1, 63) &
			this->adj_[v].block(nBB));

		return nDeg;
	}

	template<class BitsetT>
	inline
		int Ugraph<BitsetT>::degree(int v, int UB, const Bitset& bbn) const {

		int nDeg = 0;
		for (int i = 0; i < this->NBB_; ++i) {

			nDeg += bblock::popc64(this->adj_[v].block(i) & bbn.block(i));

			if (nDeg >= UB) { return UB; }
		}

		return nDeg;
	}

	template<class BitsetT>
	int Ugraph<BitsetT>::degree(int v, int UB, const BitsetSp& bbn) const {
		LOG_ERROR("function not yet implemented, should not be called - Ugraph<BitsetT>::degree");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}


	template<class BitsetT>
	int Ugraph<BitsetT>::create_complement(Ugraph& ug) const {

		//resets ug with new allocation
		ug.reset(this->NV_);
			
		for (int i = 0; i < this->NV_ - 1; ++i) {
			for (int j = i + 1; j < this->NV_; ++j) {

				if (!this->adj_[i].is_bit(j)) {
					ug.add_edge(i, j);
				}

			}
		}

		return 0;
	}


	template<class BitsetT>
	int Ugraph<BitsetT>::create_subgraph(Ugraph& ug, int v) const
	{
		vector<int> vnn;
		this->neighbors(v).extract(vnn);

		return create_subgraph(ug, vnn);
	}

	template<class BitsetT>
	int Ugraph<BitsetT>::create_subgraph(Ugraph& ug, Vertices& lv) const
	{
		if (lv.empty()) {
			LOG_ERROR("empty set found while creating an induced graph - Ugraph<BitsetT>::create_induced");
			return -1;
		}
				
		///////////////
		ug.reset(lv.size());
		///////////////
		
		//add appropiate edges
		const int nV = static_cast<int>(lv.size());
		for (int i = 0; i < nV - 1; i++) {
			for (int j = i + 1; j < nV; j++) {

				if (this->is_edge(lv[i], lv[j])) {
					ug.add_edge(i, j);						//adds bidirected edge
				}
			}
		}

		return 0;
	}

}//end namespace bitgraph



#endif // end  __SIMPLE_UGRAPH_H__






























