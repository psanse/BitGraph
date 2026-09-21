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

#include "graph_types.h"
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
	inline int Graph<BBScanSp>::shrink_to_fit(std::size_t new_size)
	{	
		
		if (new_size >= static_cast<std::size_t>(NV_)) {
			LOGG_WARNING(
				"Invalid shrinking size ",
				new_size,
				": expected a value smaller than ",
				NV_,
				". The graph remains unchanged - Graph<BBScanSp>::shrink_to_fit.");

			return -1;
		}

		// Safe because newSize < NV_ and NV_ is representable as int.
		const int new_size_int = static_cast<int>(new_size);

		//trims vertices 
		for (vertex_t v = 0; v < new_size_int; ++v) {
			adj_[v].clear_bit(new_size_int, EMPTY_ELEM);		// closed range
		}

		//resizes adjacency matrix
		adj_.resize(new_size);
		NV_ = new_size_int;
		NE_ = 0;												// resets edge cached value
		edge_count_valid_ = false;								// so that when num edges are required, the value will be recomputed
		NBB_ = INDEX_1TO1(new_size_int);						// maximum number of bitblocks per row (for sparse graphs)		

		return 0;
	}

	template<>
	inline double Graph<BBScanSp>::block_density()	const
	{
		std::size_t nBB = 0;							//number of non-empty bitblocks	
		std::size_t nBBt = 0;							//number of allocated bitblocks (all should be non-empty in the sparse case)

		for (Vertex v = 0; v < NV_; ++v) {
			nBBt += adj_[v].size();
			for (std::size_t bb = 0; bb < adj_[v].size(); ++bb) {
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
		for (Vertex v = 0; v < NV_; ++v) {
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

		for (Vertex v = 0; v < NV_; ++v) {
			nBB = adj_[v].size();
			nBBt += nBB;
			den += static_cast<double>(adj_[v].size()) /
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
		for (Vertex v = 0; v < NV_; ++v) {

			//non destructive scan of each bitstring
			adj_[v].init_scan(bbo::NON_DESTRUCTIVE);
			
			while (true) {
				int w = adj_[v].next_bit();
				if (w == EMPTY_ELEM) { break; }
				o << "e " << v + 1 << " " << w + 1 << endl;
			}
			
		}
	}

} //end of namespace bitgraph


#endif // __SIMPLE_SPARSE_GRAPH_H__