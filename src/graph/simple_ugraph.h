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
 * TODO implement min_degree functions (03/01/2025)
 **/

#ifndef __SIMPLE_UGRAPH_H__
#define __SIMPLE_UGRAPH_H__

#include "simple_graph.h"
#include "utils/prec_timer.h"
#include "graph_globals.h"


namespace bitgraph {

	// forward declaration
	namespace _impl {
		class GraphConversion;   
	}

	//////////////////
	//
	// Generic class Ugraph<BitSetT>
	// 
	// (BitSetT is limited to bitarray and sparse_bitarray types)
	// 
	//////////////////

	template<class BitSetT = BBScan>
	class Ugraph : public Graph<BitSetT> {

		friend class _impl::GraphConversion;

	public:

		//set of (static) methods to create simple graphs
		static Ugraph make_triangle();
		static Ugraph make_clique(std::size_t n);
		static Ugraph make_cycle(std::size_t n);
		static Ugraph make_star(std::size_t n);

		//TODO... others (03/01/2025)

	public:

		using Self = Ugraph<BitSetT>;		//own type
		using BaseT = Graph<BitSetT>;		//parent type
		using bitset_type = BitSetT;

		//alias for backward compatibility - to be removed in future versions
		using _bbt = bitset_type;

		//constructors - cannot all be inherited	
		Ugraph() : Graph<BitSetT>() {}															//creates empty graph
		explicit Ugraph(std::size_t n) : Graph<BitSetT>(n) {}									//creates empty graph of size n=|V|	
		explicit Ugraph(std::string filename) { this->reset(filename); }							//reads graph from file

		/**
		* @brief Creates a graph from an C-style adjacency matrix
		*
		*		Reads only the upper triangle of the adjacency matrix
		**/
		Ugraph(std::size_t n, int* adj[], string name);			//old-style adjacency matrix

		//TODO***- copy constructor, move constructor, copy operator =, move operator = (1/1/2025)

		//destructor
		~Ugraph() = default;

		/////////////
		// setters and getters
	public:

		/**
		* @brief Counts the number of edges	(includes self loops)
		* @param lazy if TRUE (reads value @NE_)
		*			  if FALSE counts and updates @NE_
		**/
		std::size_t num_edges(bool lazy = true)						  override;

		/**
		* @brief Counts the number of edges	in an induced subgraph by a set of vertices
		**/
		std::size_t num_edges(const BitSetT&) 								const override;

		/////////////
		// Basic operations

			/**
			* @brief density of the undirected graph
			* @param lazy reads NE_ cached value if TRUE
			**/
		double density(bool lazy = true)						override;

		/**
		* @brief Computes complement graph
		* @param g output complement graph
		* @return 0 if success, -1 if error
		**/
		int create_complement(Ugraph& g)								const;


		/////////////
		// degree-related operations
		// 
		// TODO implement bitstring conversions according to the templates properly (3/4/18) - CHECK (02/01/2025)
	public:

		/**
		* @brief Computes the number of neighbors of v (deg(v))	*
		**/
		int degree(int v)									const { return (int)this->adj_[v].count(); }

		/**
		*  @brief number of neighbors of v in a set of vertices
		*
		*		  (specialized for sparse graphs)
		*
		*  @param bbn input non-sparse (bit) set of vertices
		**/
		int degree(int v, const BitSet& bbn)			const;

		/**
		*  @brief number of neighbors of v in a sparse encoded set of vertices
		*
		*		  (sparse graphs ONLY )
		*
		*  @param bbn input sparse (bit) set of vertices
		*
		* TODO - currently not implemented, only for sparse graphs (03/03/2025)
		**/
		int degree(int v, const BitSetSp& bbs)			const;

		/**
		*  @brief truncated number of neighbors of v in a set of vertices
		*
		*		  (specialized for sparse graphs)
		*
		*  @param bbn input (bit) set of vertices
		*  @returns neighbors of v if <= UB, otherwise UB
		**/
		int degree(int v, int UB, const BitSet& bbn)	const;  //truncated degree (14/2/2016)

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
		int degree(int v, int UB, const BitSetSp& bbs)	const;	//truncated degree  (14/2/2016)

		/**
		*  @brief number of neighbors of v in a vertex set with higher index than v
		*
		*		  (applied as pivotal strategy for clique enumeration)
		*
		* @param v: input vertex
		* @param bbn: input (bit) set of vertices
		*
		**/
		int degree_up(int v, const BitSet& bbn)			const;  //TODO: test (27/4/2016)

		/**
		* @brief number of neighbors of v that come after v
		*
		* @param v: input vertex
		**/
		int degree_up(int v)									const;

		/**
		*  @brief returns the maximum degree of the graph,
		*         i.e., the maximum degree of any of its vertices
		**/
		int max_graph_degree()										const;

		/**
		*  @brief returns the maximum degree of an induced subgraph
		*  @param sg input (bit) set of vertices of the subgraph
		**/
		template<class bitset_t>
		int max_subgraph_degree(bitset_t& sg)							const;


		/**
		* @brief number of edges with a single endpoint in a vertex from @sg
		* @param sg input (bit) set of vertices
		**/
		template<class bitset_t>
		int outgoing_degree(bitset_t& sg)							const;

		/**
		* @brief number of edges with a single endpoint in a vertex from @sg
		* @param sg input (vector) set of vertices
		**/
		int outgoing_degree(std::vector<int> sg)					const;

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
		void add_edge(int v, int w)							override;

		/**
		* @brief Removes bidirectional edge {v, w}
		*		 a) if self_loop (v = w), graph remains unchanged
		*		 b) keeps track of the number of edges
		* @param v endpoint
		* @param w endpoint
		**/
		void remove_edge(int v, int w)							override;

		/**
		* @brief generates undirected edges with probability p.
		*
		*		 I. (v, v) not allowed.
		*		 II. Valid for directed/undirected graphs (TMP design)
		*
		* @param v input endpoint
		* @param w input endpoint
		* @returns 0 is success, -1 if error
		**/
		//void gen_random_edges(double p)								override;


		/**
		* @brief generates undirected edges with probability p, exactly.
		* @param p probability of an edge
		* @details: - uses a different model from OLD gen_random_edges
		**/
		void gen_random_edges(double p)			override;

		//////////////	
		// Induced subgraphs

		//TODO	Graph& create_subgraph	(std::size_t first_k, Graph& g) const  override;

		/**
		*  @brief Computes the subgraph induced by a set of vertices
		*  @param lv input set of vertices (std::vector<int>)
		*  @returns 0 if success, -1 if error
		**/
		int create_subgraph(Ugraph& g, vint& lv)					const;

		/**
		*  @brief Computes the subgraph induced by the neighborhood of a vertex (29/08/21)
		*
		*		 Calls create_subgraph(Ugraph& g, vint& lv)
		*
		*  @param v input vertex which determines the neighborhood
		*  @returns 0 if success, -1 if error
		**/
		int create_subgraph(Ugraph& g, int v)						const;


		////////////
		// Write basic operations
		// Note: Read operations are inherited from the base class Graph (using the Template Pattern)
	public:
		/**
		* @brief writes undirected graph in dimacs format
		* @param o output stream
		**/
		void write_dimacs(std::ostream& filename)				override;

		/**
		* @brief writes undirected graph in edge list format
		* @param o output stream
		**/
		void write_EDGES(std::ostream& filename)				override;

		/**
		* @brief writes undirected graph in MMX (Matrix Exchange) format
		* @param o output stream
		**/
		void write_mtx(std::ostream& filename);				//MTX format only for Ugraph? (03/01/2025)

		/////////////////	
		//	I/O basic operations
	public:

		std::ostream& print_degrees(std::ostream & = std::cout)						const;
		std::ostream& print_edges(std::ostream & = std::cout, bool eofl = false)	override;

		std::ostream& print_edges(BitSetT& bbsg, std::ostream&);
		std::ostream& print_adjacency_matrix(std::ostream & = std::cout)			const;

		//////////////	
		// deleted methods legacy - CHECK	
		virtual void remove_vertices(const BitSet& bbn) override = delete;				//commented out implementation - EXPERIMENTAL

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
	
	template<class BitSetT>
	template<class bitset_t>
	inline
		int Ugraph<BitSetT>::max_subgraph_degree(bitset_t& sg) const {

		int max_degree = 0;

		int v = BBObject::noBit;
		if (sg.init_scan(bbo::NON_DESTRUCTIVE) != BBObject::noBit) {
			while ((v = sg.next_bit()) != BBObject::noBit) {

				//compute max_degree circumscribed to subgraph
				if (max_degree < degree(v, sg)) {
					max_degree = degree(v, sg);
				}
			}
		}

		return max_degree;
	}

	template<class BitSetT>
	template<class bitset_t>
	inline int Ugraph<BitSetT>::outgoing_degree(bitset_t& sg) const
	{
		//number of edges
		int nE = 0;

		//scan declaration
		int retVal = sg.init_scan(bbo::NON_DESTRUCTIVE);

		////////////////////////
		assert(retVal != BBObject::noBit);
		///////////////////////

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

	template<class BitSetT>
	inline int Ugraph<BitSetT>::outgoing_degree(std::vector<int> sg) const
	{
		BitSetT bbsg{ this->size(), sg };
		return (outgoing_degree(bbsg));
	}


	template<class BitSetT>
	inline
	ostream& Ugraph<BitSetT>::print_edges(BitSetT& bbsg, std::ostream& o)
	{
		for (std::size_t i = 0; i < this->NV_ - 1; ++i) {
			if (!bbsg.is_bit(i)) continue;
			for (std::size_t j = i + 1; j < this->NV_; ++j) {
				if (!bbsg.is_bit(j)) continue;
				if (this->is_edge(i, j)) {
					o << "[" << i << "]" << "--" << "[" << j << "]" << endl;
				}
			}
		}

		return o;
	}



	/////////////////////////////////////


	template<class BitSetT>
	inline
		Ugraph<BitSetT> Ugraph<BitSetT>::make_triangle()
	{
		Ugraph<BitSetT> tri(3);
		tri.add_edge(0, 1);
		tri.add_edge(0, 2);
		tri.add_edge(1, 2);

		return tri;
	}

	template<class BitSetT>
	inline
		Ugraph<BitSetT> Ugraph<BitSetT>::make_clique(std::size_t NV)
	{
		Ugraph<BitSetT> clique(NV);

		//sets the adjacency matrix to ONE except for the main diagonal
		const auto nV = static_cast<int>(NV);
		for (int v = 0; v < nV; ++v) {
			clique.neighbors(v).set_bit(0, nV - 1);
			clique.neighbors(v).erase_bit(v);
		}

		return clique;
	}

	template<class BitSetT>
	inline
		Ugraph<BitSetT> Ugraph<BitSetT>::make_cycle(std::size_t NV)
	{
		Ugraph<BitSetT> cycle(NV);

		const auto nV = static_cast<int>(NV);
		for (int v = 0; v < nV - 1; ++v) {
			cycle.add_edge(v, v + 1);
		}
		cycle.add_edge(nV - 1, 0);

		return cycle;
	}

	template<class BitSetT>
	inline
		Ugraph<BitSetT> Ugraph<BitSetT>::make_star(std::size_t NV) {
		Ugraph<BitSetT> star(NV);

		const auto nV = static_cast<int>(NV);
		for (int v = 1; v < nV; ++v) {
			star.add_edge(0, v);
		}

		return star;
	}


	template <class BitSetT>
	inline
		Ugraph<BitSetT>::Ugraph(std::size_t NV, int* adj[], string name) {

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

	template<class BitSetT>
	inline
		std::size_t Ugraph<BitSetT>::num_edges(bool lazy ) {

		if (!lazy || this->NE_ == 0) {
			this->NE_ = 0;

			//adds all edges and divides by 2 for efficiency - checks for self loops	
			for (int i = 0; i < this->NV_; ++i) {
				this->NE_ += this->adj_[i].count();
			}

			//////////////////////////////
			if (this->NE_ % 2 != 0) {
				LOG_ERROR("odd number of edges found in simple undirected graph - Ugraph<BitSetT>::num_edges");
				LOG_ERROR("exiting...");
				exit(-1);
			}
			//////////////////////////////

			this->NE_ /= 2;						//MUST be even at this point			
		}

		return this->NE_;
	}

	template<class BitSetT>
	inline
		std::size_t Ugraph<BitSetT>::num_edges(const BitSetT& bbn) const {
		std::size_t NE = 0;

		//reads only the upper triangle of the adjacency matrix
		for (int i = 0; i < this->NV_ - 1; ++i) {
			if (bbn.is_bit(i)) {
				for (int j = i + 1; j < this->NV_; ++j) {
					if (bbn.is_bit(j)) {
						if (this->adj_[i].is_bit(j)) { ++NE; }
					}
				}
			}
		}

		return NE;
	}

	template<class BitSetT>
	inline
		int Ugraph<BitSetT>::degree(int v, const BitSet& bbn) const {

		int ndeg = 0;
		for (auto i = 0; i < this->NBB_; i++) {
			ndeg += bblock::popc64(this->adj_[v].block(i) & bbn.block(i));
		}

		return ndeg;
	}

	template<class BitSetT>
	inline
		int Ugraph<BitSetT>::degree(int v, const BitSetSp& bbs)	const {
		LOG_ERROR("function not yet implemented, should not be called - Ugraph<BitSetT>::degree");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}


	template<class BitSetT>
	inline
		void Ugraph<BitSetT>::add_edge(int v, int w) {

		if (v != w) {
			this->adj_[v].set_bit(w);
			this->adj_[w].set_bit(v);
			this->NE_++;
		}
	}

	template<class BitSetT>
	inline
		void Ugraph<BitSetT>::remove_edge(int v, int w) {
		if (v != w) {
			this->adj_[v].erase_bit(w);
			this->adj_[w].erase_bit(v);
			this->NE_--;
		}
	}

	//template<class BitSetT>
	//inline
	//	void Ugraph<BitSetT>::gen_random_edges(double p) {

	//	//removes all edges
	//	this->remove_edges();

	//	//sets undirected edges with probability p
	//	for (std::size_t i = 0; i < this->NV_ - 1; i++) {
	//		for (std::size_t j = i + 1; j < this->NV_; j++) {
	//			if (_rand::uniform_dist(p)) {
	//				add_edge(i, j);
	//			}
	//		}
	//	}

	//}

	template<class BitSetT>
	inline
		void Ugraph<BitSetT>::gen_random_edges(double p) {

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

	template<class BitSetT>
	inline
		int Ugraph<BitSetT>::max_graph_degree() const {

		int max_degree = 0, temp = 0;

		for (auto i = 0; i <(int)this->NV_; ++i) {
			temp = degree(i);
			if (temp > max_degree)
				max_degree = temp;
		}

		return max_degree;
	}

	template<class BitSetT>
	inline
		double Ugraph<BitSetT>::density(bool lazy) {

		BITBOARD max_edges = this->NV_;
		max_edges *= (max_edges - 1);
		return (2 * num_edges(lazy) / static_cast<double> (max_edges));
	}

	template<class BitSetT>
	inline
		ostream& Ugraph<BitSetT>::print_degrees(std::ostream& o) const {
		for (std::size_t i = 0; i < this->NV_; ++i) {
			o << "deg(" << i << ")" << ":" << degree(i) << " ";
		}
		return o;
	}

	template<class BitSetT>
	inline
		ostream& Ugraph<BitSetT>::print_edges(std::ostream& o, bool eofl) {

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

	template<class BitSetT>
	inline
		ostream& Ugraph<BitSetT>::print_adjacency_matrix(std::ostream& o) const
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


	template<class BitSetT>
	inline
		void Ugraph<BitSetT>::write_dimacs(ostream& o) {

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

	template<class BitSetT>
	inline
		void  Ugraph<BitSetT>::write_EDGES(ostream& o) {

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

	template<class BitSetT>
	inline
		void Ugraph<BitSetT>::write_mtx(ostream& o) {

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
		for (auto v = 0; v < this->NV_ - 1; ++v) {
			for (auto w = v + 1; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {														//O(log) for sparse graphs: specialize
					o << v + 1 << " " << w + 1 << endl;
				}
			}
		}
	}

	template<class BitSetT>
	inline
		int Ugraph<BitSetT>::degree_up(int v, const BitSet& bbn) const {

		int nDeg = 0, nBB = WDIV(v);

		for (auto i = nBB + 1; i < this->NBB_; ++i) {
			nDeg += bblock::popc64(this->adj_[v].block(i) & bbn.block(i));
		}

		//truncate the bitblock of v
		nDeg += bblock::popc64(bblock::MASK_1(WMOD(v) + 1, 63) &
			this->adj_[v].block(nBB) & bbn.block(nBB)
		);

		return nDeg;
	}

	template<class BitSetT>
	inline
		int Ugraph<BitSetT>::degree_up(int v) const
	{
		int nDeg = 0, nBB = WDIV(v);

		for (auto i = nBB + 1; i < this->NBB_; ++i) {
			nDeg += bblock::popc64(this->adj_[v].block(i));
		}

		//truncate the bitblock of v
		nDeg += bblock::popc64(bblock::MASK_1(WMOD(v) + 1, 63) &
			this->adj_[v].block(nBB));

		return nDeg;
	}

	template<class BitSetT>
	inline
		int Ugraph<BitSetT>::degree(int v, int UB, const BitSet& bbn) const {

		int nDeg = 0;
		for (auto i = 0; i < this->NBB_; ++i) {

			nDeg += bblock::popc64(this->adj_[v].block(i) & bbn.block(i));

			if (nDeg >= UB) { return UB; }
		}

		return nDeg;
	}

	template<class BitSetT>
	int Ugraph<BitSetT>::degree(int v, int UB, const BitSetSp& bbn) const {
		LOG_ERROR("function not yet implemented, should not be called - Ugraph<BitSetT>::degree");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}


	template<class BitSetT>
	int Ugraph<BitSetT>::create_complement(Ugraph& ug) const {

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


	template<class BitSetT>
	int Ugraph<BitSetT>::create_subgraph(Ugraph& ug, int v) const
	{
		vector<int> vnn;
		this->neighbors(v).extract(vnn);

		return create_subgraph(ug, vnn);
	}

	template<class BitSetT>
	int Ugraph<BitSetT>::create_subgraph(Ugraph& ug, vint& lv) const
	{
		if (lv.empty()) {
			LOG_ERROR("empty set found while creating an induced graph - Ugraph<BitSetT>::create_induced");
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

	using USS = bitgraph::Ugraph<bitgraph::BBScanSp>;

	////////////////////////
	//
	// Specializations of class Ugraph<BitSetT> methods for sparse graphs
	// (BitSetT = sparse_bitarray)
	//
	////////////////////////

	template<>
	inline
		bitgraph::BITBOARD USS::num_edges(bool lazy) {

		if (lazy || this->NE_ == 0) {

			this->NE_ = 0;
			for (int i = 0; i < this->NV_ - 1; ++i) {

				//population count from i + 1 onwards
				this->NE_ += adj_[i].count(i + 1, -1);
			}
		}

		return this->NE_;
	}

	template<>
	inline
		int USS::degree_up(int v) const
	{
		int nDeg = 0, nBB = WDIV(v);

		//find the bitblock of v
		auto it = adj_[v].cbegin();
		for (; it != adj_[v].cend(); ++it) {
			if (it->idx_ == nBB) break;
		}

		//check no neighbors
		if (it == adj_[v].cend()) {
			return 0;							//no neighbors
		}

		//truncate the bitblock of v and count the number of neighbors
		nDeg += bblock::popc64(bblock::MASK_1(WMOD(v) + 1, 63) & it->bb_);

		//add the rest of neighbours in the bitblocks that follow
		++it;
		for (; it != adj_[v].cend(); ++it) {
			nDeg += bblock::popc64(it->bb_);
		}

		return nDeg;
	}


	template<>
	inline
		int USS::degree(int v) const {

		if (Graph<sparse_bitarray>::adj_[v].is_empty()) return 0;

		return Graph<sparse_bitarray>::adj_[v].count();
	}

	template<>
	inline
		int USS::degree(int v, const BitSet& bbn) const {

		int ndeg = 0;
		for (auto it = adj_[v].cbegin(); it != adj_[v].cend(); ++it) {
			ndeg += bblock::popc64(it->bb_ & bbn.block(it->idx_));
		}

		return ndeg;
	}

	template<>
	inline
		int USS::degree(int v, const BitSetSp& bbs) const {

		int ndeg = 0;
		auto itv = adj_[v].cbegin();
		auto itbb = bbs.cbegin();

		while (itv != adj_[v].cend() && itbb != bbs.cend()) {

			if (itv->idx_ < itbb->idx_) {
				++itv;
			}
			else if (itv->idx_ > itbb->idx_) {
				++itbb;
			}
			else { //same index
				ndeg += bblock::popc64(itv->bb_ & itbb->bb_);
				++itv; ++itbb;
			}
		}

		return ndeg;
	}

	template<>
	inline
		int USS::degree(int v, int UB, const BitSet& bbn) const {

		int ndeg = 0;
		for (auto it = adj_[v].cbegin(); it != adj_[v].cend(); ++it) {
			ndeg += bblock::popc64(it->bb_ & bbn.block(it->idx_));
			if (ndeg >= UB) return UB;
		}

		return ndeg;
	}

	template<>
	inline
		int USS::degree(int v, int UB, const BitSetSp& bbs) const {

		int ndeg = 0;
		auto itv = adj_[v].cbegin();
		auto itbb = bbs.cbegin();

		while (itv != adj_[v].cend() && itbb != bbs.cend()) {

			if (itv->idx_ < itbb->idx_) {
				++itv;
			}
			else if (itv->idx_ > itbb->idx_) {
				++itbb;
			}
			else { //same index
				ndeg += bblock::popc64(itv->bb_ & itbb->bb_);
				if (ndeg >= UB) return UB;
				++itv; ++itbb;
			}
		}

		return ndeg;
	}

	//////////////////////
	// I/O specialized functions


	template<>
	inline
		void USS::write_dimacs(ostream& o) {

		//timestamp 
		o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();

		//name
		if (!this->name_.empty())
			o << "c " << this->name_.c_str() << endl;

		//tamaño del grafo
		o << "p edge " << this->NV_ << " " << num_edges() << endl << endl;

		//Escribir nodos
		for (auto v = 0; v < this->NV_ - 1; v++) {

			//non destructive scan starting from the vertex onwards
			auto block_v = WDIV(v);
			pair<bool, int> p = this->adj_[v].find_block_pos(block_v);

			if (p.second == BBObject::noBit) {
				continue;
			}

			//bitscan - if block contains v, start from that position onwards
			this->adj_[v].scan_block(p.second);
			(p.first) ? this->adj_[v].scan_bit(v - WMUL(block_v) /*WMOD(v)*/) : this->adj_[v].scan_bit(MASK_LIM);

			int w = BBObject::noBit;
			while ((w = this->adj_[v].next_bit()) != BBObject::noBit) {
				o << "e " << v + 1 << " " << w + 1 << endl;
			}
		}
	}

	template<>
	inline
		ostream& USS::print_edges(std::ostream& o, bool eofl) {

		for (auto v = 0; v < this->NV_ - 1; ++v) {

			//skip empty bitsets - MUST BE since currently the scanning object does not check this
			if (adj_[v].is_empty()) { continue; }

			//scan the bitstring of v
			int w = BBObject::noBit;
			sparse_bitarray::scan sc(adj_[v]);
			while ((w = sc.next_bit()) != BBObject::noBit) {
				if ((int)v < w) {
					o << "[" << v << "]" << "-->" << "[" << w + WMUL(sc.get_block()) << "]" << std::endl;
				}
			}
		}

		if (eofl) { o << std::endl; }
		return o;
	}


	template<>
	inline
		void USS::write_EDGES(ostream& o) {
		/////////////////////////
		// writes simple unweighted grafs  in edge list format 
		// note: loops are not allowed

			//timestamp
		o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();

		//name
		if (!this->name_.empty())
			o << "% " << this->name_.c_str() << endl;

		//writes edges
		for (auto v = 0; v < this->NV_ - 1; v++) {
			//non destructive scan starting from the vertex onwards
			pair<bool, int> p = this->adj_[v].find_block_pos(WDIV(v));
			if (p.second == EMPTY_ELEM) continue;										//no more bitblocks
			this->adj_[v].scan_block(p.second);
			(p.first) ? this->adj_[v].scan_bit(WMOD(v)) : this->adj_[v].scan_bit(MASK_LIM);			//if bitblock contains v, start from that position onwards
			while (1) {
				int w = this->adj_[v].next_bit();
				if (w == EMPTY_ELEM)
					break;
				o << v + 1 << " " << w + 1 << endl;
			}
		}
	}

	template<>
	inline
		void USS::write_mtx(ostream& o) {
		/////////////////////////
		// writes simple unweighted grafs  in edge list format 
		// note: loops are not allowed

		//header
		o << "%%MatrixMarket matrix coordinate pattern symmetric" << endl;

		//timestamp
		o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();

		//name
		if (!this->name_.empty())
			o << "% " << this->name_.c_str() << endl;

		//size and edges
		NE_ = 0;																	//eliminates lazy evaluation of edge count 
		o << this->NV_ << " " << this->NV_ << " " << num_edges() << endl;

		//writes edges
		for (int v = 0; v < this->NV_ - 1; v++) {
			//non destructive scan starting from the vertex onwards
			pair<bool, int> p = this->adj_[v].find_block_pos(WDIV(v));
			if (p.second == EMPTY_ELEM) continue;										//no more bitblocks
			this->adj_[v].scan_block(p.second);
			(p.first) ? this->adj_[v].scan_bit(WMOD(v)) : this->adj_[v].scan_bit(MASK_LIM);			//if bitblock contains v, start from that position onwards
			while (1) {
				int w = this->adj_[v].next_bit();
				if (w == EMPTY_ELEM)
					break;
				o << v + 1 << " " << w + 1 << endl;
			}
		}
	}




#endif






























