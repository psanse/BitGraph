/**
 * @file graph_vertex_weighted.h
 *
 * @brief Facade specializations and inline implementations for vertex-weighted graphs.
 *
 * This header binds the generic vertex-weighted graph template
 * `Graph_W<GraphT, WeightT>` to concrete *facade graph types*, in particular
 * undirected graphs (`ugraph`).
 *
 * In addition to declaring the facade specialization
 * `Graph_W<ugraph, WeightT>`, this file also provides **inline implementations**
 * of facade-specific methods whose behavior depends on the undirected graph
 * interface (e.g. DIMACS output routines).
 *
 * @details
 * This file acts as the *facade binding layer* between:
 *  - the **generic implementation** of vertex-weighted graphs
 *    (`simple_graph_w.h`, `Base_Graph_W`)
 *  - and the **named facade graph types** defined in the basic graph layer
 *    (e.g. `ugraph`)
 *
 * Only code that is:
 *  - specific to the `ugraph` facade, and
 *  - safe to define inline in a header (ODR-compliant)
 * should be placed here.
 *
 * Representation-dependent code and generic algorithms must remain in the
 * corresponding implementation headers.
 *
 * @note
 * This header must be included **after** `graph_basic.h`, as it relies on the
 * definition of the `ugraph` facade type.
 *
 * @author
 * Pablo San Segundo (pss)
 *
 * @date
 * 01/02/2026
 */

#ifndef __GRAPH_VERTEX_WEIGHTED_H__
#define __GRAPH_VERTEX_WEIGHTED_H__

#include "graph_types.h"
#include "graph_basic.h"
#include "simple_graph_w.h"												 // MUST BE AFTER graph_basic.h 

namespace bitgraph {
    
	// specialization for undirected graphs
	
	template<class WeightT>
	class Graph_W<ugraph, WeightT> : public Base_Graph_W<ugraph, WeightT> {
	public:

		using BaseT = Base_Graph_W<ugraph, WeightT>;					
		using graph_type = typename BaseT::graph_type;
		using bitset_type = typename BaseT::bitset_type;
		using Weight = typename BaseT::Weight;
		using VertexBitset = bitset_type;						// alias for semantic information

		using BaseT::NO_WEIGHT;
		using BaseT::ZERO_WEIGHT;
		using BaseT::DEFAULT_WEIGHT;
				
		//constructors (inherited from Base class)
		using BaseT::BaseT;

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
		std::ostream& write_dimacs(std::ostream& o = std::cout) override;
	};
		

    // facade types for vertex-weighted ugraphs
    using ugraph_w = Graph_W<ugraph, double>;                   // simple vertex weighted graph with double weights
    using ugraph_wi = Graph_W<ugraph, int>;                     // simple vertex weighted graph with int weights
}


//////////////////////
// implementation in header file

namespace bitgraph {

	template<class WeightT>
	inline std::ostream&
	Graph_W<ugraph, WeightT>::write_dimacs(std::ostream& o) {

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