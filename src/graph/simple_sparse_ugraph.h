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

 //useful alias
namespace bitgraph {
	using USS = bitgraph::Ugraph<BBScanSp>;
}

////////////////////////
//
// Specializations of class Ugraph<T> methods for sparse graphs
// (T = sparse_bitarray)
//
////////////////////////

template<>
inline
std::size_t USS::num_edges(bool lazy) {

	if (lazy || this->NE_ == 0) {

		this->NE_ = 0;
		for (int i = 0; i < this->NV_ - 1; i++) {

			//popuation count from i + 1 onwards
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
	nDeg += bblock::popc64(	bblock::MASK_1(WMOD(v) + 1, 63) &	it->bb_ );

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

	return Graph<sparse_bitarray>::adj_[v].size();
}

template<>
inline
int USS::degree(int v, const BitSet& bbn) const {
	
	int ndeg = 0;
	for (auto it = adj_[v].cbegin(); it != adj_[v].cend(); ++it) {
		ndeg += bblock::popc64 (it->bb_ & bbn.block(it->idx_) );
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
void USS::write_dimacs(ostream& o) {

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
		while ( (w = this->adj_[v].next_bit()) != BBObject::noBit ) {
			o << "e " << v + 1 << " " << w + 1 << endl;
		}
	}
}

template<>
inline
std::ostream& USS::print_edges (std::ostream& o, bool eofl)  {

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
void USS::write_EDGES(ostream& o)  {
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