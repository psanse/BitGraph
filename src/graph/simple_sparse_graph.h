 /**
  * @file simple_sparse_graph.h 
  * @brief contains specializations the class Graph for sparse graphs
  *
  * @date 30/12/2024
  * @last modified 01/02/2026
  * @author pss
  * 
  * @todo - check logic for efficiency (01/02/2026)
  */

#ifndef __SIMPLE_SPARSE_GRAPH_H__
#define __SIMPLE_SPARSE_GRAPH_H__

#include "simple_graph.h"
#include <iostream>
#include <string>
#include <vector>

////////////////////////
//
// Specializations of class Graph<BitsetT> methods for sparse graphs
// with T = BBScanSp
//   
// note: this is facade type sparse_graph

namespace bitgraph{

	template<>
	inline Graph<BBScanSp>& Graph<BBScanSp>::create_subgraph(int first_k, Graph<BBScanSp>& newg) const
	{
		//assertions
		if (first_k >= NV_ || first_k <= 0) {
			LOGG_WARNING("Bad new size ", first_k, " - graph remains unchanged - Graph<BBScanSp>::create_subgraph");
			return newg;
		}

		//allocates memory for the new graph
		newg.reset(first_k);

		//copies first k elements of the adjacency matrix 
		for (int i = 0; i < newg.NV_; i++) {
			newg.adj_[i] = adj_[i];
			newg.adj_[i].clear_bit(first_k, EMPTY_ELEM);		//closed range
		}

		return newg;
	}

	template<>
	inline int Graph<BBScanSp>::shrink_to_fit(std::size_t size)
	{
		//assertions
		if (NV_ <= size) {
			LOGG_WARNING("Wrong shrinking size ", size, " the graph remains unchanged - Graph<BBScanSp>::shrink_to_fit");
			return -1;
		}

		//trims vertices 
		for (int i = 0; i < size; i++) {
			adj_[i].clear_bit(size, EMPTY_ELEM);				//closed range
		}

		//resizes adjacency matrix
		adj_.resize(size);
		NV_ = size;
		NE_ = 0;												//so that when required, the value will be recomputed
		NBB_ = INDEX_1TO1(size);								//maximum number of bitblocks per row (for sparse graphs)		

		return 0;
	}

	template<>
	inline double Graph<BBScanSp>::block_density()	const
	{
		std::size_t nBB = 0;							//number of non-empty bitblocks	
		std::size_t nBBt = 0;							//number of allocated bitblocks (all should be non-empty in the sparse case)

		for (std::size_t v = 0; v < NV_; ++v) {
			nBBt += adj_[v].size();
			for (std::size_t bb = 0; bb < adj_[v].size(); bb++) {
				if (adj_[v].block(bb)) {
					nBB++;								//nBB should be equal to nBBt
				}
			}
		}

		return nBB / static_cast<double>(nBBt);			//density should be 1.0
	}

	template<>
	inline double Graph<BBScanSp>::block_density_sparse() const 
	{
		std::size_t nBBt = 0;							//number of allocated bitblocks (all should be non-empty in the sparse case)

		//number of allocated blocks
		for (std::size_t v = 0; v < NV_; ++v) {
			nBBt += adj_[v].size();
		}

		BITBOARD aux = ceil(NV_ / double(WORD_SIZE));
		BITBOARD maxBlock = NV_ * aux;

		return static_cast<double>(nBBt) / maxBlock;
	}

	template<>
	inline double Graph<BBScanSp>::average_block_density_sparse() const
	{
		std::size_t nBB = 0;							//number of non-empty bitblocks	
		std::size_t nBBt = 0;							//number of allocated bitblocks (all should be non-empty in the sparse case)
		double den = 0.0;

		for (std::size_t i = 0; i < NV_; ++i) {
			nBB = adj_[i].size();
			nBBt += nBB;
			den += static_cast<double>(adj_[i].size()) /
				(BITBOARD(nBB) * WORD_SIZE);
		}

		return (den / nBBt);
	}

	template<>
	inline void Graph<BBScanSp>::write_dimacs(ostream& o) 
	{
		//timestamp comment
		timestamp_dimacs(o);

		//name comment
		name_dimacs(o);

		//dimacs header - does not recompute edges, can be a heavy operation
		header_dimacs(o, true);


		//write edges 1-based vertex notation
		for (std::size_t v = 0; v < NV_; ++v) {

			//non destructive scan of each bitstring
			if (adj_[v].init_scan(bbo::NON_DESTRUCTIVE) != EMPTY_ELEM) {
				while (1) {
					int w = adj_[v].next_bit();
					if (w == EMPTY_ELEM) { break; }
					o << "e " << v + 1 << " " << w + 1 << endl;
				}
			}
		}
	}

} //end of namespace bitgraph


#endif // __SIMPLE_SPARSE_GRAPH_H__