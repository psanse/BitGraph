/** 
 * @file graph_vertex_weighted.h
 * @brief  
 *		    
 * @comment: 
 *
 * @authors: pss (main developper)
 * @date 01/02/2026 
 * 
 **/

#ifndef __GRAPH_VERTEX_WEIGHTED_H__
#define __GRAPH_VERTEX_WEIGHTED_H__

#include "graph_types.h"
#include "graph_basic.h"
#include "simple_graph_w.h"                                     // MUST BE AFTER graph_basic.h 


namespace bitgraph {
    
	// specialization for undirected graphs
	
	template<class WeightT>
	class Graph_W<ugraph, WeightT> : public Base_Graph_W<ugraph, WeightT> {
	public:

		using BaseT = Base_Graph_W<ugraph, WeightT>;					//parent type
		using graph_type = typename BaseT::graph_type;
		using bitset_type = typename BaseT::bitset_type;
		using Weight = typename BaseT::Weight;

		using BaseT::NO_WEIGHT;
		using BaseT::ZERO_WEIGHT;
		using BaseT::DEFAULT_WEIGHT;

		//alias types for backward compatibility
		//using _wt = weight_type;											
		using VertexBitset = bitset_type;

		//constructors (inherited from Base class)
		using BaseT::Base_Graph_W;

		/////////////
		//useful interface-specific for undirected weighted graphs
		int max_graph_degree() const { return this->g_.max_graph_degree(); }
		int degree(int v) const { return this->g_.degree(v); }
		int degree(int v, const Bitset& bbn) const { return this->g_.degree(v, bbn); }

		///////////
		//I/O operations

		/*
		* @brief Writes undirected graph to stream in dimacs format
		*
		*		 (self-loops are not considered)
		*/
		ostream& write_dimacs(ostream& o = std::cout) override;
	};
		

    // facade types for vertex-weighted ugraphs
    using ugraph_w = Graph_W<ugraph, double>;                   // simple vertex weighted graph with double weights
    using ugraph_wi = Graph_W<ugraph, int>;                     // simple vertex weighted graph with int weights
}


//////////////////////
// implementation in header files

namespace bitgraph {

	template<class WeightT>
	ostream& Graph_W<ugraph, WeightT>::write_dimacs(ostream& o) {

		//timestamp comment
		this->g_.timestamp_dimacs(o);

		//name comment
		this->g_.name_dimacs(o);

		//dimacs header - recompute edges
		this->g_.header_dimacs(o, false);

		//write DIMACS nodes n <v> <w>
		const int NV = this->g_.num_vertices();
		for (int v = 0; v < NV; ++v) {
			o << "n " << v + 1 << " " << this->weight(v) << endl;
		}

		//write directed edges (1-based vertex notation dimacs)
		for (int v = 0; v < NV - 1; ++v) {
			for (int w = v + 1; w < NV; ++w) {
				if (this->g_.is_edge(v, w))							//O(log) for sparse graphs: specialize
					o << "e " << v + 1 << " " << w + 1 << endl;
			}
		}

		return o;
	}

} //end namespace bitgraph



#endif //end __GRAPH_VERTEX_WEIGHTED_H__