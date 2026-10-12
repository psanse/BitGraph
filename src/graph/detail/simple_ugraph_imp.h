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

#include <algorithm>
#include "graph/simple_ugraph.h"

namespace bitgraph {
	
	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::max_subgraph_degree(
			vertex_bitset_t& vertices) const
	{
		degree_t max_degree = 0;

		vertex_t v = BBObject::noBit;
		vertices.init_scan(bbo::NON_DESTRUCTIVE);

		while ((v = vertices.next_bit()) != BBObject::noBit) {

			const degree_t vertex_degree =
				degree(v, vertices);

			if (vertex_degree > max_degree) {
				max_degree = vertex_degree;
			}
		}
				
		return max_degree;
	}

	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::outgoing_degree(
			vertex_bitset_t& vertices) const
	{
		degree_t degree = 0;

		vertices.init_scan(bbo::NON_DESTRUCTIVE);

		vertex_t v = BBObject::noBit;

		while ((v = vertices.next_bit()) != BBObject::noBit) {

			for (block_index_t block_index = 0;
				block_index < this->NBB_;
				++block_index) 
			{
				const BITBOARD bb =
					this->neighbors(v).block(block_index) &
					~vertices.block(block_index);

				degree += bblock::count(bb);
			}
		}

		return degree;
	}

	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::outgoing_degree(
			const vertex_set_t& vertices) const
	{
		vertex_bitset_t vertices_bb { this->size(), vertices };
		return (outgoing_degree(vertices_bb));
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
		Ugraph<BitsetT> Ugraph<BitsetT>::make_complete_graph(std::size_t NV)
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
	 
		// update edge count
		clique.NE_ =
			static_cast<std::size_t>(vertex_count) *
			(static_cast<std::size_t>(vertex_count) - 1) / 2;

		return clique;
	}

	template<class BitsetT>
	inline
		Ugraph<BitsetT> Ugraph<BitsetT>::make_cycle(std::size_t NV)
	{
		if (NV < 3) {
			base_t::graph_initialization_error(
				"A simple cycle graph requires at least three vertices.");
		}

		Ugraph<BitsetT> cycle(NV);
				
		const vertex_t vertex_count = cycle.num_vertices();
		
				
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
				
		if(NV == 0) {
			base_t::graph_initialization_error(
				"A star graph requires at least one vertex.");
		}

		Ugraph<BitsetT> star(NV);

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
			std::string name) noexcept
		: base_t()
	{
		// A null matrix is valid only when constructing an empty graph.
		assert(adj != nullptr || NV == 0);
			
		this->reset(NV, std::move(name));

		const vertex_t vertex_count =
			static_cast<vertex_t>(NV);
				
		// Read only the strict upper triangle.
		// add_edge() stores both directions.
		for (vertex_t v = 0; v < vertex_count - 1; ++v) {
			assert(adj[v] != nullptr);

			for (vertex_t w = v + 1; w < vertex_count; ++w) {
				if (adj[v][w] != 0) {
					this->add_edge(v, w);
				}
			}
		}		
	}

	template<class BitsetT>
	inline
	std::size_t Ugraph<BitsetT>::num_edges(bool lazy ) const 
	{
		if (!lazy || !this->edge_count_valid_ ) {

			this->NE_ = 0;

			/*
			 * Count only the strict upper triangle of the symmetric adjacency
			 * matrix. Each undirected edge is therefore counted exactly once,
			 * and diagonal entries are excluded.
			 */
			for (vertex_t v = 0; v + 1 < this->NV_; ++v) {
				this->NE_ += adj_[static_cast<std::size_t>(v)]
					.count(v + 1, -1);
			}
					
			this->edge_count_valid_ = true;
		}

		return this->NE_;
	}

	template<class BitsetT>
	inline
		std::size_t Ugraph<BitsetT>::num_edges(
			const vertex_bitset_t& vertices) const
	{
		std::size_t NE = 0;

		// Count edges in the subgraph induced by vertices.
		for (vertex_t v = 0; v + 1< this->NV_ ; ++v) {

			if (!vertices.is_bit(v)) {
				continue;
			}

			for (vertex_t w = v + 1; w < this->NV_; ++w) {
				if (vertices.is_bit(w)
					&& this->adj_[v].is_bit(w)) {
					++NE;
				}
			}
			
		}

		return NE;
	}

	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::degree(
			vertex_t v,
			const vertex_bitset_t& vertices) const
	{
		assert(v >= 0 && v < this->NV_);

		degree_t degree = 0;

		for (int i = 0; i < this->NBB_; ++i) {
			degree += bblock::count(
				this->adj_[static_cast<std::size_t>(v)].block(i) &
				vertices.block(i));
		}
		
		return degree;
	}

	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::add_edge(vertex_t v, vertex_t w)
	{
		assert(v >= 0 && v < NV_);
		assert(w >= 0 && w < NV_);
		
		if (v != w && !adj_[v].is_bit(w)) {
			this->adj_[v].set_bit(w);
			this->adj_[w].set_bit(v);
			this->NE_++;
		}
	}

	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::remove_edge(vertex_t v, vertex_t w)
	{
		
		if (v != w && this->adj_[v].is_bit(w)) {
			this->adj_[v].erase_bit(w);
			this->adj_[w].erase_bit(v);
			--this->NE_;
		}

		// edge count is valid because we just decremented it
	}

	template<class BitsetT>
	inline void Ugraph<BitsetT>::remove_edges(vertex_t v)
	{
		assert(v >= 0 && v < this->NV_);

		const degree_t removed_edges = this->degree(v);

		for (vertex_t w = 0; w < this->NV_; ++w) {
			if (this->adj_[v].is_bit(w)) {
				this->adj_[w].erase_bit(v);
			}
		}

		this->adj_[v].erase_bit();

		this->NE_ -= static_cast<std::size_t>(removed_edges);
	
	}

	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::gen_random_edges(double p) 
	{
		assert(p >= 0.0 && p <= 1.0);


		//removes all edges
		this->remove_edges();

		using edge_t = std::pair<vertex_t, vertex_t>;
		std::vector<edge_t> edges;

		const std::size_t NV = static_cast<std::size_t>(this->NV_);
		if (NV < 2) {
			return;
		}
		edges.reserve(NV * (NV - 1) / 2);
				
		for (vertex_t v = 0; v < this->NV_ - 1; ++v) {
			for (vertex_t w = v + 1; w < this->NV_; ++w) {
				edges.emplace_back(v, w);
			}
		}
				
		const std::size_t N = edges.size();
		const std::size_t M =
			static_cast<std::size_t>(std::llround(p * N));
		

		std::shuffle(
			edges.begin(), 
			edges.end(), 
			bitgraph::utils::detail::g_iugen.engine());

		//take the first M edges
		for (std::size_t i = 0; i < M; ++i) {
			add_edge(edges[i].first, edges[i].second);
		}

	}

	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::max_degree() const
	{
		degree_t max_degree = 0;

		for (vertex_t v = 0; v < this->NV_; ++v) {
			max_degree = std::max(max_degree, degree(v));
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
		double Ugraph<BitsetT>::density(const vertex_bitset_t& vertices) const
	{
		const std::size_t num_vertices = vertices.count();

		if (num_vertices < 2) {
			return 0.0;
		}

		const std::size_t num_edges = this->num_edges(vertices);
		
		const std::size_t max_edges =
			num_vertices * (num_vertices - 1) / 2;		// undirected graph, no self-loops

		return static_cast<double>(num_edges) /
			static_cast<double>(max_edges);
	}

	template<class BitsetT>
	inline
		std::ostream& Ugraph<BitsetT>::print_degrees(std::ostream& o) const
	{
		for (vertex_t v = 0; v < this->NV_; ++v) {
			os << "deg(" << v << ")" 
			  << ":" << degree(v) << " ";
		}
		return os;
	}

	template<class BitsetT>
	inline
		ostream& Ugraph<BitsetT>::print_edges(
			std::ostream& os,
			bool trailing_newline)
	{

		for (vertex_t v = 0; v < this->NV_; ++v) {
			for (vertex_t w = v + 1; w < this->NV_; ++w) {

				if (this->is_edge(v, w)) {
					os << '[' << v << ']' << "--" << '[' << w << ']' << '\n';
				}
			}
		}

		if (trailing_newline) {
			os << '\n';
		}

		return os;
	}

	template<class BitsetT>
	inline
		ostream& Ugraph<BitsetT>::print_adjacency_matrix(std::ostream& os) const
	{
		for (vertex_t v = 0; v < this->NV_; ++v) {
			for (vertex_t w = 0; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {
					os << "1";
				}
				else {
					os << "0";
				}
			}
			os << endl;
		}
		return os;
	}
		
	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::write_dimacs(ostream& os) {

		// Timestamp comment 
		this->timestamp_dimacs(os);
	
		// Graph name
		this->name_dimacs(os);
		
		// DIMACS header
		this->header_dimacs(os, false);
			
		// Write undirected edges (1-based vertex indices)
		// O(log) for sparse graphs: TODO - specialize
		for (vertex_t v = 0; v < this->NV_ - 1; ++v) {
			for (vertex_t w = v + 1; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {
					os << "e " << v + 1 << " " << w + 1 << '\n';
				}
			}
		}
	}

	template<class BitsetT>
	inline
		void  Ugraph<BitsetT>::write_EDGES(ostream& os) {

		// Timestamp comment 
		os << "% File written by BITGRAPH:" 
			<< PrecisionTimer::local_timestamp() << endl;

		// Graph name
		if (!this->name_.empty())
			os << "% " << this->name_.c_str() << endl;

		// write edges - 1 based vertex notation
		// O(log) for sparse graphs: specialize
		for (vertex_t v = 0; v < this->NV_ - 1; ++v) {
			for (vertex_t w = v + 1; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {							
					os << v + 1 << " " << w + 1 << '\n';
				}
			}
		}
	}

	template<class BitsetT>
	inline
		void Ugraph<BitsetT>::write_mtx(ostream& os) {

		// Matrix Market header
		os << "%%MatrixMarket matrix coordinate pattern symmetric" << endl;

		// Timestamp comment
		os << "% File written by GRAPH:" << PrecisionTimer::local_timestamp() << endl;

		// Graph name
		if (!this->name_.empty())
			os << "% " << this->name_.c_str() << endl;

		// Matrix dimensions and number of entries
		os << this->NV_ << " "
			<< this->NV_ << " "
			<< this->num_edges(false /* recompute */) << '\n';

		// Write undirected edges (1-based vertex indices)
		for (vertex_t v = 0; v < this->NV_ - 1; ++v) {
			for (vertex_t w = v + 1; w < this->NV_; ++w) {
				if (this->is_edge(v, w)) {
					os << v + 1 << " " << w + 1 << '\n';
				}
			}
		}
	}

	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::degree_up(
			vertex_t v, 
			const vertex_bitset_t& bbn) const
	{
		assert(v >= 0 && v < this->NV_);

		degree_t degree = 0;

		const int block_index = WDIV(v);

		// Count complete blocks strictly above the block containing v.
		for (int i = block_index + 1; i < this->NBB_; ++i) {
			degree += bblock::count(
				this->adj_[static_cast<size_t>(v)].block(i) &
				bbn.block(i));
		}

		// Count only bits with index greater than v in v's own block.
		degree += bblock::count(
			bblock::MASK_1_HIGH(WMOD(v) + 1) &
			this->adj_[static_cast<size_t>(v)].block(block_index) & 
			bbn.block(block_index)
		);

		return degree;
	}

	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::degree_up(vertex_t v) const
	{
		degree_t degree = 0;
		
		const int block_index = WDIV(v);

		for (int i = block_index + 1; i < this->NBB_; ++i)
		{
			degree += bblock::count(this->adj_[v].block(i));
		}

		// Count only neighbors with index greater than v in v's own block.
		degree += bblock::count(
			bblock::MASK_1_HIGH(WMOD(v) + 1) &
			this->adj_[v].block(block_index));

		return degree;
	}

	template<class BitsetT>
	inline
		degree_t Ugraph<BitsetT>::degree(
			vertex_t v, 
			degree_t upper_bound, 
			const vertex_bitset_t& vertices) const 
	{

		assert(v >= 0 && v < this->NV_);

		degree_t degree = 0;

		for (int i = 0; i < this->NBB_; ++i) {

			degree += bblock::count(
				this->adj_[v].block(i) &
				vertices.block(i));

			if (degree >= upper_bound) {
				return upper_bound; 
			}
		}

		return degree;
	}


	template<class BitsetT>
	void Ugraph<BitsetT>::create_complement(Ugraph& ugcomp) const 
	{
		if (this == &ugcomp) {
			base_t::graph_initialization_error(
				"Input and output graphs must be different in "
				"Ugraph::create_complement(Ugraph&).");
		}

		ugcomp.reset(static_cast<std::size_t>(this->NV_));

		for (vertex_t v = 0; v + 1 < this->NV_; ++v) {
			for (vertex_t w = v + 1; w < this->NV_; ++w) {

				if (!this->adj_[static_cast<std::size_t>(v)].is_bit(w)) {
					ugcomp.add_edge(v, w);
				}
			}
		}
	}

	template<class BitsetT>
	void Ugraph<BitsetT>::create_subgraph(Ugraph& ug, vertex_t v) const
	{
		assert(v >= 0 && v < NV_);
		assert(&ug != this);

		vertex_set_t vnn;
		this->neighbors(v).extract(vnn);

		create_subgraph(ug, vnn);
	}

	template<class BitsetT>
	void Ugraph<BitsetT>::create_subgraph(
		Ugraph& ug,
		const vertex_set_t& vertices) const
	{
		const int NV = static_cast<int>(vertices.size());

		// Preconditions
		assert(&ug != this);
		assert(NV > 0);

		for (vertex_t v : vertices) {
			assert(v >= 0 && v < NV_);
		}

		// Initialize output graph
		ug.reset(NV);
				
		
		// Add undirected edges
		for (vertex_t v = 0; v < NV - 1; ++v) {
			for (vertex_t w = v + 1; w < NV; ++w) {
				if (this->is_edge(vertices[v], vertices[w])) {
					ug.add_edge(v, w);
				}
			}
		}		
	}

}//end namespace bitgraph



#endif // BITGRAPH_GRAPH_SIMPLE_UGRAPH_H






























