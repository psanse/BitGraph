 /**
  * @file simple_ugraph_impl.h
  * @brief Provides template and inline implementations for the Ugraph class.
  *
  * This file contains the definitions of template member functions and inline
  * operations for simple undirected graphs. Ugraph represents loopless
  * undirected graphs using bitset-based adjacency rows.
  *
  * This implementation header is included at the end of simple_ugraph.h and
  * should not normally be included directly by client code.
  *
  * @note All definitions in this file belong to the bitgraph namespace.
  *
  * @date Created: 17/06/2010
  * @date Last updated: 21/09/2026
  * @author Pablo San Segundo
  *
  * @todo Implement the minimum-degree operations (03/01/2025)
  */

#ifndef BITGRAPH_GRAPH_SIMPLE_UGRAPH_IMP_H
#define BITGRAPH_GRAPH_SIMPLE_UGRAPH_IMP_H


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
		const vertex_t vertex_count = clique.num_vertices();
		if (vertex_count == 0) {
			return clique;
		}

		const vertex_t last_vertex = vertex_count - 1;
				
		for (vertex_t v = 0; v < vertex_count; ++v) {
			clique.neighbors(v).set_bit(0, last_vertex);
			clique.neighbors(v).erase_bit(v);
		}
	 
		return clique;
	}

	template<class BitsetT>
	inline
		Ugraph<BitsetT> Ugraph<BitsetT>::make_cycle(std::size_t NV)
	{
		if (NV < 3) {
			BaseT::graph_initialization_error(
				"A simple cycle graph requires at least three vertices.");
		}

		Ugraph<BitsetT> cycle(NV);

		//sets the adjacency matrix to ONE except for the main diagonal
		const vertex_t vertex_count = cycle.num_vertices();
		if (vertex_count == 0) {
			return cycle;
		}
				
		for (vertex_t v = 0; v + 1 < vertex_count; ++v) {
			cycle.add_edge(v, v + 1);
		}

		cycle.add_edge(vertex_count - 1, 0);

	
		return cycle;
	}

	template<class BitsetT>
	inline
		Ugraph<BitsetT> Ugraph<BitsetT>::make_star(std::size_t NV)
	{
				
		if (NV == 0) {
			BaseT::graph_initialization_error(
				"A star graph requires at least one vertex - make_star(std::size_t NV) ");
		}

		Ugraph<BitsetT> star(NV);

		//sets the adjacency matrix to ONE except for the main diagonal
		const vertex_t vertex_count = star.num_vertices();
				
		for (vertex_t v = 1; v < vertex_count; ++v) {
			star.add_edge(0, v);
		}		
		
		return star;
	}


	template <class BitsetT>
	inline
		Ugraph<BitsetT>::Ugraph(
			std::size_t NV,
			const int * const adj[], 
			string name) noexcept
		: BaseT()
	{
		// A null matrix is valid only when constructing an empty graph.
		assert(adj != nullptr || NV == 0);
			
		this->reset(NV, std::move(name));

		const vertex_t num_vertex = static_cast<vertex_t>(NV);	
		
		// Read only the strict upper triangle. add_edge() stores both directions.
		for (vertex_t i = 0; i < num_vertex - 1; ++i) {
			for (vertex_t j = i + 1; j < num_vertex; ++j) {
				if (adj[i][j] != 0) {
					this->add_edge(i, j);
				}
			}
		}
		
	}


	template<class BitsetT>
	inline
		std::size_t Ugraph<BitsetT>::num_edges(bool lazy ) const {

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
		std::size_t Ugraph<BitsetT>::num_edges(const BitsetT& bbn) const
	{
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
		int Ugraph<BitsetT>::degree(vertex_t v, const Bitset& bbn) const {

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

		std::shuffle(edges.begin(), edges.end(), bitgraph::com::detail::g_iugen.engine() /*std::mt19937_64(std::random_device{}())*/);

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
		double Ugraph<BitsetT>::density(bool lazy) const
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
	void Ugraph<BitsetT>::create_complement(Ugraph& ugcomp) const 
	{
		if (this == &ugcomp) {
			BaseT::graph_initialization_error(
				"Input and output graphs must be different in "
				"Ugraph::create_complement(Ugraph& ugcomp).");
		}

		ugcomp.reset(static_cast<std::size_t>(this->NV_));		
			
		for (vertex_t i = 0; i + 1 < this->NV_; ++i) {
			for (vertex_t j = i + 1; j < this->NV_; ++j) {

				if (!this->adj_[i].is_bit(j)) {
					ugcomp.add_edge(i, j);
				}

			}
		}	
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



#endif // BITGRAPH_GRAPH_SIMPLE_UGRAPH_H






























