/**
  * @file simple_sparse_ugraph.h
  * @brief contains specializations the class Ugraph for sparse graphs
  *
  * @created 03/01/2025 
  * @last_update 26/01/2025
  * @author pss
  *
  * This code is part of the GRAPH 1.0 C++ library
  *
  **/

#ifndef __SIMPLE_SPARSE_UGRAPH_H__
#define __SIMPLE_SPARSE_UGRAPH_H__

#include "simple_ugraph.h"
#include "utils/prec_timer.h"
#include <iostream>
#include <string>
#include <vector>

////////////////////////
//
// Specializations of class Ugraph<T> methods for sparse graphs
// (T = sparse_bitarray)
//
////////////////////////

template<>
inline
BITBOARD Ugraph<sparse_bitarray>::number_of_edges(bool lazy) {
		
	if (lazy || ptype::NE_ == 0) {
		ptype::NE_ = 0;
		for (int i = 0; i < ptype::NV_ - 1; i++) {
			ptype::NE_ += adj_[i].popcn64(i + 1);			//popuation count from i + 1 onwards
		}		
	}

	return ptype::NE_;
}

template<>
inline int Ugraph<sparse_bitarray>::degree_up(int v) const
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
	nDeg += bblock::popc64(	bblock::MASK_1(WMOD(v) + 1, 63) &	it->bb_ );

	//add the rest of neighbours in the bitblocks that follow
	++it;
	for (; it != adj_[v].cend(); ++it) {
		nDeg += bblock::popc64(it->bb_);
	}
	
	return nDeg;
}


template<>
inline int Ugraph<sparse_bitarray>::degree(int v) const {

	if (Graph<sparse_bitarray>::adj_[v].is_empty()) return 0;

	return Graph<sparse_bitarray>::adj_[v].popcn64();
}

template<>
inline
int Ugraph<sparse_bitarray>::degree(int v, const BitSet& bbn) const {
	
	int ndeg = 0;
	for (auto it = adj_[v].cbegin(); it != adj_[v].cend(); ++it) {
		ndeg += bblock::popc64 (it->bb_ & bbn.block(it->idx_) );
	}

	return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree(int v, const BitSetSp& bbs) const {
	
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
int Ugraph<sparse_bitarray>::degree(int v, int UB, const BitSet& bbn) const {
	
	int ndeg = 0;
	for (auto it = adj_[v].cbegin(); it != adj_[v].cend(); ++it) {
		ndeg += bblock::popc64(it->bb_ & bbn.block(it->idx_));
		if (ndeg >= UB) return UB;
	}

	return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree(int v, int UB, const BitSetSp& bbs) const {
	
	int ndeg = 0;
	auto itv = adj_[v].cbegin();
	auto itbb = bbs.cbegin();

	while (itv != adj_[v].cend() && itbb != bbs.cend()) {

		if (itv -> idx_ < itbb -> idx_) {
			++itv;
		}
		else if (itv -> idx_ > itbb -> idx_) {
			++itbb;
		}
		else { //same index
			ndeg += bblock::popc64(itv -> bb_ & itbb -> bb_);
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
void Ugraph<sparse_bitarray>::write_dimacs(ostream& o) {
	/////////////////////////
	// writes file in dimacs format 

		//timestamp 
	o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();

	//name
	if (!this->name_.empty())
		o << "c " << this->name_.c_str() << endl;

	//tamaño del grafo
	o << "p edge " << this->NV_ << " " << number_of_edges() << endl << endl;

	//Escribir nodos
	for (int v = 0; v < this->NV_ - 1; v++) {
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p = this->adj_[v].find_block_pos(WDIV(v));
		if (p.second == EMPTY_ELEM) continue;					//no more bitblocks
		this->adj_[v].scan_block(p.second);
		(p.first) ? this->adj_[v].scan_bit(WMOD(v))  : this->adj_[v].scan_bit(MASK_LIM);		//if bitblock contains v, start from that position onwards
		while (1) {
			int w = this->adj_[v].next_bit();
			if (w == EMPTY_ELEM)
				break;
			o << "e " << v + 1 << " " << w + 1 << endl;
		}

	}
}

template<>
inline
void Ugraph<sparse_bitarray>::write_EDGES(ostream& o) {
	/////////////////////////
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
void Ugraph<sparse_bitarray>::write_mtx(ostream& o) {
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
	o << this->NV_ << " " << this->NV_ << " " << number_of_edges() << endl;

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