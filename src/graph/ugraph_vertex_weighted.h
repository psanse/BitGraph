/**
 * @file ugraph_vertex_weighted.h
 * @brief Facade specialization for vertex-weighted undirected graphs.
 *
 * Defines the specialization `Graph_VW<ugraph, WeightT>` and the
 * interface-specific operations required by vertex-weighted undirected
 * graphs.
 *
 * This header provides the binding between the generic vertex-weighted
 * graph implementation (`Base_Graph_VW` / `Graph_VW`) and the `ugraph`
 * facade type.
 *
 * It may also contain inline definitions of operations whose implementation
 * depends specifically on the undirected graph interface, such as DIMACS
 * output.
 *
 * Generic vertex-weighted graph functionality should remain in
 * `simple_graph_w.h` and its corresponding implementation header.
 *
 * @note This header depends on the definition of `ugraph`.
 *
 * @author Pablo San Segundo (pss)
 * @details Created 01/02/2026, last updated 10/10/2026.
 */

#ifndef BITGRAPH_GRAPH_UGRAPH_VERTEX_WEIGHTED_H
#define BITGRAPH_GRAPH_UGRAPH_VERTEX_WEIGHTED_H

#include "graph_types.h"
#include "graph_unweighted.h"
#include "simple_graph_vw.h"		

namespace bitgraph {
    
	/**
	 * @brief Facade specialization for vertex-weighted undirected graphs.
	 *
	 * Specializes the user-facing `Graph_VW` interface for `ugraph`. Generic
	 * vertex-weighted functionality is inherited from `Base_Graph_VW`, while
	 * undirected-graph-specific operations are implemented in this specialization.
	 *
	 * @tparam WeightT Type used to represent vertex weights.
	 */
	template<class WeightT>
	class Graph_VW<ugraph, WeightT> : public Base_Graph_VW<ugraph, WeightT> {
	public:

		using base_t = Base_Graph_VW<ugraph, WeightT>;
		
		// Inherit constructors from Base_Graph_VW.
		using base_t::base_t;
	
		//////////////////
		// Undirected-graph-specific interface.
		// The full underlying graph API remains accessible through graph().


		///////////
		//I/O operations

		/**
		 * @brief Writes the undirected vertex-weighted graph in DIMACS format.
		 *
		 * Writes the graph topology and vertex weights to @p os using 1-based
		 * vertex identifiers. Each undirected edge is written exactly once.
		 *
		 * @param os Output stream.
		 * @return Reference to @p os.
		 *
		 * @note Self-loops are not supported and are therefore not written.
		 */
		std::ostream& write_dimacs(
			std::ostream& os = std::cout) const override;
	};
		

	/**
	 * @brief Vertex-weighted undirected graph with `double` weights.
	 */
    using ugraph_w = Graph_VW<ugraph, double>;  

	/**
	 * @brief Vertex-weighted undirected graph with `int` weights.
	 */
    using ugraph_wi = Graph_VW<ugraph, int>;                    
}


//////////////////////
// Necessary implementation in header file

namespace bitgraph {

	template<class WeightT>
	inline std::ostream&
	Graph_VW<ugraph, WeightT>::write_dimacs(std::ostream& os) const {

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

		return os;
	}

} //end namespace bitgraph




#endif // BITGRAPH_GRAPH_UGRAPH_VERTEX_WEIGHTED_H