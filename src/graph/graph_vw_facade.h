/**
 * @file ugraph_vertex_weighted.h
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

#ifndef BITGRAPH_GRAPH_UGRAPH_VERTEX_WEIGHTED_H
#define BITGRAPH_GRAPH_UGRAPH_VERTEX_WEIGHTED_H

#include "graph_types.h"
#include "graph_unweighted.h"
#include "simple_graph_vw.h"		

namespace bitgraph {
    
	// specialization for undirected graphs
	
	template<class WeightT>
	class Graph_W<ugraph, WeightT> : public Base_Graph_W<ugraph, WeightT> {
	public:

		using base_t = Base_Graph_W<ugraph, WeightT>;
		using graph_t = typename base_t::graph_type;
		using bitset_t = typename base_t::bitset_type;
		using weight_t = typename base_t::weight_t;
		using vertex_bitset_t = bitset_t;


		//using BaseT = Base_Graph_W<ugraph, WeightT>;					
		//using graph_type = typename BaseT::graph_type;
		//using bitset_type = typename BaseT::bitset_type;
		//using Weight = typename BaseT::Weight;
		//using vertex_bitset_t = bitset_type;						// alias for semantic information


		using base_t::NO_WEIGHT;
		using base_t::ZERO_WEIGHT;
		using base_t::DEFAULT_WEIGHT;

		/*using BaseT::NO_WEIGHT;
		using BaseT::ZERO_WEIGHT;
		using BaseT::DEFAULT_WEIGHT;*/
				
		
		// Inherit constructors from Base_Graph_W.
		using base_t::base_t;

		//////////////////
		// Undirected-graph-specific interface.

		int max_graph_degree() const { return this->graph_.max_graph_degree(); }
		int degree(vertex_t v) const { return this->graph_.degree(v); }
		int degree(vertex_t v, const vertex_bitset_t& vertices) const { 
			return this->graph_.degree(v, vertices);
		}

		///////////
		//I/O operations

		/*
		* @brief Writes undirected graph to stream in dimacs format
		*
		*		 (self-loops are not considered)
		*/
		std::ostream& write_dimacs(std::ostream& o = std::cout) const;
	};
		

    // facade types for vertex-weighted ugraphs
    using ugraph_w = Graph_W<ugraph, double>;                   
    using ugraph_wi = Graph_W<ugraph, int>;                    
}


//////////////////////
// Necessary implementation in header file

namespace bitgraph {

	template<class WeightT>
	inline std::ostream&
	Graph_W<ugraph, WeightT>::write_dimacs(std::ostream& os) const {

		//timestamp comment
		this->graph_.timestamp_dimacs(os);

		//name comment
		this->graph_.name_dimacs(os);

		//dimacs header - recompute edges
		this->graph_.header_dimacs(os, false);

		//write DIMACS nodes n <v> <w>
		const vertex_t NV = this->graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			os << "n " << v + 1 << " " << this->weight(v) << '\n';
		}

		//write directed edges (1-based vertex notation dimacs)
		for (vertex_t v = 0; v + 1 < NV ; ++v) {
			for (vertex_t w = v + 1; w < NV; ++w) {
				if (this->graph_.is_edge(v, w))							//O(log) for sparse graphs: specialize
					os << "e " << v + 1 << " " << w + 1 << '\n';
			}
		}

		return o;
	}

} //end namespace bitgraph




#endif // BITGRAPH_GRAPH_UGRAPH_VERTEX_WEIGHTED_H