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

#ifndef BITGRAPH_GRAPH_SIMPLE_SPARSE_GRAPH_H
#define BITGRAPH_GRAPH_SIMPLE_SPARSE_GRAPH_H

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
			LOGG_WARNING(
				"Bad new size ", 
				first_k, 
				" - graph remains unchanged - Graph<BBScanSp>::create_subgraph (int first_k, Graph<BBScanSp>& newg)");
			return newg;
		}

		//allocates memory for the new graph
		newg.reset(first_k);

		//copies first k elements of the adjacency matrix 
		for (vertex_t v = 0; v < newg.NV_; v++) {
			newg.adj_[v] = adj_[v];
			newg.adj_[v].clear_bit(first_k, EMPTY_ELEM);		//closed range
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
			this->adj_[v].clear_bit(new_size_int, EMPTY_ELEM);		// closed range
		}

		//resizes adjacency matrix
		this->adj_.resize(new_size);
		this->NV_ = new_size_int;
		this->NE_ = 0;												// resets edge cached value
		this->edge_count_valid_ = false;								// so that when num edges are required, the value will be recomputed
		this->NBB_ = INDEX_1TO1(new_size_int);						// maximum number of bitblocks per row (for sparse graphs)		

		return 0;
	}

	template<>
	inline double Graph<BBScanSp>::block_density()	const
	{
		std::size_t non_empty_blocks = 0;				
		std::size_t alloc_blocks = 0;							

		for (vertex_t v = 0; v < this->NV_; ++v) {
			const std::size_t row_blocks = this->adj_[v].size();	
			alloc_blocks += row_blocks;

			for (std::size_t block = 0; block < row_blocks; ++block) {
				if (this->adj_[v].block(block) != 0) {
					++non_empty_blocks;
				}
			}		
		}

		if (alloc_blocks == 0) {
			return 0.0;
		}

		/*
		 * Sparse storage is expected to contain only nonempty blocks, so this
		 * value should normally be 1.0.
		 */
		return static_cast<double>(non_empty_blocks) /
			static_cast<double>(alloc_blocks);	
	}

	template<>
	inline double Graph<BBScanSp>::block_density_sparse() const 
	{

		if (this->NV_ == 0 || this->NBB_ == 0) {
			return 0.0;
		}

		std::size_t alloc_blocks = 0;

		for (vertex_t vertex = 0; vertex < this->NV_; ++vertex) {
			alloc_blocks += this->adj_[vertex].size();
		}

		const std::size_t maxBlocks =
			static_cast<std::size_t>(this->NV_) *
			static_cast<std::size_t>(this->NBB_);

		return static_cast<double>(alloc_blocks) /
			static_cast<double>(maxBlocks);
	}

	template<>
	inline double Graph<BBScanSp>::average_block_density_sparse() const
	{

		if (this->NV_ == 0) {
			return 0.0;
		}

		double density_sum = 0.0;

		for (vertex_t vertex = 0; vertex < this->NV_; ++vertex) {
			const std::size_t alloc_blocks = adj_[vertex].size();

			// An empty adjacency row contributes a density of zero.
			if (alloc_blocks == 0) {
				continue;
			}

			const std::size_t set_bits =
				static_cast<std::size_t>(adj_[vertex].count());

			const std::size_t alloc_bits =
				alloc_blocks * static_cast<std::size_t>(WORD_SIZE);

			density_sum +=
				static_cast<double>(set_bits) /
				static_cast<double>(alloc_bits);
		}

		return density_sum / static_cast<double>(this->NV_);

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
		for (vertex_t v = 0; v < this->NV_; ++v) {

			//non destructive scan of each bitstring
			adj_[v].init_scan(bbo::NON_DESTRUCTIVE);
			
			while (true) {
				int w = this->adj_[v].next_bit();
				if (w == EMPTY_ELEM) { break; }
				o << "e " << v + 1 << " " << w + 1 << endl;
			}
			
		}
	}

} //end of namespace bitgraph


#endif // BITGRAPH_GRAPH_SIMPLE_SPARSE_GRAPH_H