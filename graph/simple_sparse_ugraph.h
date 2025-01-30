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
	auto it = adj_[v].begin();
	for (; it != adj_[v].end(); ++it) {
		if (it->index == nBB) break;
	}

	//check no neighbors
	if (it == adj_[v].end()) {
		return 0;							//no neighbors
	}

	//truncate the bitblock of v and count the number of neighbors
	nDeg += bblock::popc64(	bblock::MASK_1(WMOD(v) + 1, 63) &	it->bb );

	//add the rest of neighbours in the bitblocks that follow
	++it;
	for (; it != adj_[v].end(); ++it) {
		nDeg += bblock::popc64(it->bb);
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
int Ugraph<sparse_bitarray>::degree(int v, const BitBoardN& bbn) const {
	
	int ndeg = 0;
	for (auto it = adj_[v].begin(); it != adj_[v].end(); ++it) {
		ndeg += bblock::popc64(it->bb & bbn.get_bitboard(it->index));
	}

	return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree(int v, const BitBoardS& bbs) const {
	
	int ndeg = 0;
	auto itv = adj_[v].begin();
	auto itbb = bbs.begin();

	while (itv != adj_[v].end() && itbb != bbs.end()) {

		if (itv->index < itbb->index) {
			++itv;
		}
		else if (itv->index > itbb->index) {
			++itbb;
		}
		else { //same index
			ndeg += bblock::popc64(itv->bb & itbb->bb);
			++itv; ++itbb;
		}
	}

	return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree(int v, int UB, const BitBoardN& bbn) const {
	
	int ndeg = 0;
	for (auto it = adj_[v].begin(); it != adj_[v].end(); ++it) {
		ndeg += bblock::popc64(it->bb & bbn.get_bitboard(it->index));
		if (ndeg >= UB) return UB;
	}

	return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree(int v, int UB, const BitBoardS& bbs) const {
	
	int ndeg = 0;
	auto itv = adj_[v].begin();
	auto itbb = bbs.begin();

	while (itv != adj_[v].end() && itbb != bbs.end()) {

		if (itv -> index < itbb -> index) {
			++itv;
		}
		else if (itv -> index > itbb -> index) {
			++itbb;
		}
		else { //same index
			ndeg += bblock::popc64(itv -> bb & itbb -> bb);
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
	if (!Graph<sparse_bitarray>::name_.empty())
		o << "c " << Graph<sparse_bitarray>::name_.c_str() << endl;

	//tamaño del grafo
	o << "p edge " << Graph<sparse_bitarray>::NV_ << " " << number_of_edges() << endl << endl;

	//Escribir nodos
	for (int v = 0; v < Graph<sparse_bitarray>::NV_ - 1; v++) {
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p = Graph<sparse_bitarray>::adj_[v].find_pos(WDIV(v));
		if (p.second == EMPTY_ELEM) continue;					//no more bitblocks
		Graph<sparse_bitarray>::adj_[v].m_scan.bbi = p.second;
		(p.first) ? Graph<sparse_bitarray>::adj_[v].m_scan.pos = WMOD(v) : Graph<sparse_bitarray>::adj_[v].m_scan.pos = MASK_LIM;		//if bitblock contains v, start from that position onwards
		while (1) {
			int w = Graph<sparse_bitarray>::adj_[v].next_bit();
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
	if (!Graph<sparse_bitarray>::name_.empty())
		o << "% " << Graph<sparse_bitarray>::name_.c_str() << endl;

	//writes edges
	for (int v = 0; v < Graph<sparse_bitarray>::NV_ - 1; v++) {
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p = Graph<sparse_bitarray>::adj_[v].find_pos(WDIV(v));
		if (p.second == EMPTY_ELEM) continue;										//no more bitblocks
		Graph<sparse_bitarray>::adj_[v].m_scan.bbi = p.second;
		(p.first) ? Graph<sparse_bitarray>::adj_[v].m_scan.pos = WMOD(v) : Graph<sparse_bitarray>::adj_[v].m_scan.pos = MASK_LIM;			//if bitblock contains v, start from that position onwards
		while (1) {
			int w = Graph<sparse_bitarray>::adj_[v].next_bit();
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
	if (!Graph<sparse_bitarray>::name_.empty())
		o << "% " << Graph<sparse_bitarray>::name_.c_str() << endl;

	//size and edges
	NE_ = 0;																	//eliminates lazy evaluation of edge count 
	o << Graph<sparse_bitarray>::NV_ << " " << Graph<sparse_bitarray>::NV_ << " " << number_of_edges() << endl;

	//writes edges
	for (int v = 0; v < Graph<sparse_bitarray>::NV_ - 1; v++) {
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p = Graph<sparse_bitarray>::adj_[v].find_pos(WDIV(v));
		if (p.second == EMPTY_ELEM) continue;										//no more bitblocks
		Graph<sparse_bitarray>::adj_[v].m_scan.bbi = p.second;
		(p.first) ? Graph<sparse_bitarray>::adj_[v].m_scan.pos = WMOD(v) : Graph<sparse_bitarray>::adj_[v].m_scan.pos = MASK_LIM;			//if bitblock contains v, start from that position onwards
		while (1) {
			int w = Graph<sparse_bitarray>::adj_[v].next_bit();
			if (w == EMPTY_ELEM)
				break;
			o << v + 1 << " " << w + 1 << endl;
		}
	}
}


#endif