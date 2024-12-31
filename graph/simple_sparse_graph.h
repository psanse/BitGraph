/*  
 * simple_sparse_graph.h stores specializations of the methods of the Graph class for the sparse_bitarray type (sparse graphs)
 * @creation 30/12/2024
 * @last_update 30/12/2024
 * @dev pss
 */

#ifndef __SIMPLE_SPARSE_GRAPH_H__
#define __SIMPLE_SPARSE_GRAPH_H__

#include "simple_graph.h"
#include <iostream>
#include <string>
#include <vector>


////////////////////////
//
// Specializations of class Graph<T> methods for sparse graphs
//
////////////////////////

template<>
inline Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph(std::size_t size, Graph<sparse_bitarray>& newg) {
	//////////////////////////
	// creates new subgraph with vertex set V=[1,2,..., size]
	//
	// RETURNS: reference to the new subgraph

		//assert is size required is greater or equal current size
	if (size >= NV_ || size <= 0) {
		LOG_ERROR("Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph-wrong shrinking size for graph. Remains unchanged");
		return *this;
	}

	newg.init(size);

	//copies to the new graph
	for (int i = 0; i < newg.NV_; i++) {
		newg.adj_[i] = adj_[i];
		newg.adj_[i].clear_bit(size, EMPTY_ELEM);		//from size to the end
	}

	return newg;
}

template<>
inline int Graph<sparse_bitarray>::shrink_to_fit(std::size_t size) {
	/////////////////////
	// shrinks graph to the size passed (must be less than current size)
	if (NV_ <= size) {
		LOG_ERROR("Graph<sparse_bitarray>::shrink_to_fit-wrong shrinking size for graph, remains unchanged");
		return -1;
	}

	//trims vertices 
	for (int i = 0; i < size; i++) {
		adj_[i].clear_bit(size, EMPTY_ELEM);		//from size to the end
	}

	//resizes adjacency matrix
	adj_.resize(size);
	NV_ = size;
	NE_ = 0;									//so that when needed will be recomputed

	return 0;
}


template<>
inline BITBOARD Graph<sparse_bitarray>::number_of_edges(bool lazy) {
	//specialization for sparse graphs (is adjacent runs in O(log)) 
	// 

	if (lazy && NE_ != 0)
		return NE_;

	BITBOARD  edges = 0;
	for (int i = 0; i < NV_; i++) {
		edges += adj_[i].popcn64();
	}
	NE_ = edges;

	return NE_;
}

template<>
inline double Graph<sparse_bitarray>::block_density()	const {
	/////////////////////////
	// specialization for sparse graphs
	//
	// RESULT: should be a 1.0 ratio, since only non-zero bitblocks are stored

	size_t nBB = 0; size_t nBBt = 0;
	for (int v = 0; v < NV_; ++v) {
		nBBt += adj_[v].number_of_bitblocks();
		for (int bb = 0; bb < adj_[v].number_of_bitblocks(); bb++) {
			if (adj_[v].get_bitboard(bb))
				nBB++;
		}
	}

	return nBB / double(nBBt);
}

template<>
inline double Graph<sparse_bitarray>::block_density_index() {
	/////////////////////////
	// a measure of sparsity in relation to the number of bitblocks //
	size_t nBB = 0; size_t nBBt = 0;
	for (int v = 0; v < NV_; ++v) {
		nBBt += adj_[v].number_of_bitblocks();
	}

	BITBOARD aux = ceil(NV_ / double(WORD_SIZE));
	BITBOARD maxBlock = NV_ * aux;

	cout << NV_ << ":" << aux << ":" << nBBt << ":" << maxBlock << endl;
	return (double(nBBt)) / maxBlock;

}

template<>
inline void Graph<sparse_bitarray>::write_dimacs(ostream& o) {
	/////////////////////////
	// writes file in dimacs format 

		//***timestamp 

		//name
	if (!name_.empty())
		o << "\nc " << name_.c_str() << endl;

	//tama�o del grafo
	o << "p edge " << NV_ << " " << number_of_edges() << endl << endl;

	//Escribir nodos
	for (int v = 0; v < NV_; v++) {
		//non destructive scan of each bitstring
		if (adj_[v].init_scan(bbo::NON_DESTRUCTIVE) != EMPTY_ELEM) {
			while (1) {
				int w = adj_[v].next_bit();
				if (w == EMPTY_ELEM)
					break;
				o << "e " << v + 1 << " " << w + 1 << endl;
			}
		}
	}
}



#endif