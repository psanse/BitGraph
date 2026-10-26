/**
 * @file simple_sparse_ugraph.h
 * @brief Sparse-graph specializations of the generic Ugraph implementation.
 *
 * @details
 * This header defines the Ugraph<BBScanSp> specializations used by the
 * sparse undirected-graph facade. The specializations provide sparse-aware
 * edge counting and degree computations while preserving the public graph
 * interface supplied by simple_ugraph.h.
 *
 * The implementation operates on sparse BITSCAN adjacency rows and counts
 * undirected edges according to the graph's upper-triangular convention.
 * It is intended to be included as part of the graph implementation rather
 * than used as a standalone public interface.
 *
 * This code is part of the GRAPH 1.0 C++ library.
 *
 * @author pss
 * @created 03/01/2025
 * @last_update 26/01/2025
 *
 * @see simple_ugraph.h
 * @see ../bitscan/bbscan_sparse.h
 */

#ifndef BITGRAPH_SIMPLE_SPARSE_UGRAPH_H
#define BITGRAPH_SIMPLE_SPARSE_UGRAPH_H

#include "simple_ugraph.h"
#include "utils/precise_timer.h"
#include <iostream>
#include <string>
#include <vector>

namespace bitgraph {

	////////////////////////
	//
	// Specializations of class Ugraph<T> methods for sparse graphs
	// 
	// @brief T = Ugraph<BBScanSp> with alias facade type sparse_bitarray
	//
	////////////////////////

	template<>
	inline
	std::size_t Ugraph<BBScanSp>::num_edges(bool lazy) const
	{

		if (lazy || !this->edge_count_valid_ ) {

			this->NE_ = 0;
			for (int i = 0; i < this->NV_ - 1; i++) {

				// Count only neighbors with a greater vertex index so that each
				// undirected edge is counted exactly once.
				this->NE_ += adj_[i].count(i + 1, -1);
			}

			// NE_ now matches the current adjacency matrix.
			this->edge_count_valid_ = true;
		}

		return this->NE_;
	}


	template<>
	inline
	degree_t Ugraph<BBScanSp>::degree_up(vertex_t v) const
	{
		assert(v >= 0 && v < this->NV_);

		degree_t degree = 0;
		auto nBB = WDIV(v);

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
		degree += bblock::popc64(
			bblock::MASK_1(WMOD(v) + 1, 63) &
			it->bb_);

		//add the rest of neighbors in the bitblocks that follow
		++it;
		for (; it != adj_[v].cend(); ++it) {
			degree += bblock::popc64(it->bb_);
		}

		return degree;
	}

	template<>
	inline
	degree_t Ugraph<BBScanSp>::degree(vertex_t v) const 
	{
		assert(v >= 0 && v < this->NV_);

		if (Graph<sparse_bitarray>::adj_[v].is_empty()) {
			return 0;
		}

		return Graph<sparse_bitarray>::adj_[v].count();
	}

	template<>
	inline
	degree_t Ugraph<BBScanSp>::degree(
		vertex_t v, 
		const BBScanSp& vertices) const
	{
		assert(v >= 0 && v < this->NV_);

		degree_t degree = 0;

		auto it_adj = adj_[v].cbegin();
		auto it_vertices = vertices.cbegin();

		while (it_adj != adj_[v].cend() &&
				it_vertices != vertices.cend())
		{
			if (it_adj->idx_ < it_vertices->idx_) {
				++it_adj;
			}
			else if (it_adj->idx_ > it_vertices->idx_) {
				++it_vertices;
			}
			else { //same index
				degree += bblock::popc64(
					it_adj->bb_ & it_vertices->bb_);
				
				++it_adj;
				++it_vertices;
			}
		}
		return degree;
	}

	
	template<>
	inline
	degree_t Ugraph<BBScanSp>::degree(
		vertex_t v, 
		degree_t upper_bound,
		const BBScanSp& vertices) const
	{
		assert(v >= 0 && v < this->NV_);

		degree_t degree = 0;

		auto it_adj = adj_[v].cbegin();
		auto it_vertices = vertices.cbegin();

		while (it_adj != this->adj_[v].cend() &&
			it_vertices != vertices.cend())
		{
			if (it_adj->idx_ < it_vertices->idx_) {
				++it_adj;
			}
			else if (it_adj->idx_ > it_vertices->idx_) {
				++it_vertices;
			}
			else {
				degree += bblock::popc64(
					it_adj->bb_ & it_vertices->bb_);

				if (degree >= upper_bound) {
					return upper_bound;
				}

				++it_adj;
				++it_vertices;
			}
		}

		return degree;
	}
	
	template<>
	inline
	void Ugraph<BBScanSp>::write_dimacs(ostream& o) 
	{

		//timestamp 
		o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();

		//name
		if (!this->name_.empty())
			o << "c " << this->name_.c_str() << endl;

		//tamaño del grafo
		o << "p edge " << this->NV_ << " " << num_edges() << endl << endl;

		//Escribir nodos
		for (int v = 0; v < this->NV_ - 1; v++) {

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
	std::ostream& Ugraph<BBScanSp>::print_edges(std::ostream& o, bool eofl)
	{
		for (int v = 0; v < this->NV_ - 1; ++v) {

			//skip empty bitsets - MUST BE since currently the scanning object does not check this
			if (adj_[v].is_empty()) { continue; }

			//scan the bitstring of v
			int w = BBObject::noBit;
			sparse_bitarray::scan sc(adj_[v]);
			while ((w = sc.next_bit()) != BBObject::noBit) {
				if (v < w) {
					o << "[" << v << "]" << "-->" << "[" << w + WMUL(sc.get_block()) << "]" << std::endl;
				}
			}
		}

		if (eofl) { o << std::endl; }
		return o;
	}


	template<>
	inline
	void  Ugraph<BBScanSp>::write_EDGES(ostream& o)
	{
		// writes simple unweighted grafs  in edge list format 
		// note: loops are not allowed

			//timestamp
		o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();

		//name
		if (!this->name_.empty())
			o << "% " << this->name_.c_str() << endl;

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

	template<>
	inline
	void  Ugraph<BBScanSp>::write_mtx(ostream& o) 
	{
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
}


#endif // end BITGRAPH_SIMPLE_SPARSE_UGRAPH_H