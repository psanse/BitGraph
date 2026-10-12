/**
 * @file ugraph_edge_weighted.h
 *
 * @brief Facade specializations and public aliases for edge-weighted graphs.
 *
 * This header binds the generic edge-weighted graph template
 * `Graph_EW<GraphT, WeightT>` to concrete *facade graph types*, in particular
 * undirected graphs (`ugraph`).
 *
 * It provides:
 *  - Specializations of `Graph_EW` for facade graph types (e.g. `ugraph`)
 *  - A user-facing, stable interface for edge-weighted undirected graphs
 *  - Convenience aliases for commonly used weight types
 *
 *  In addition to declaring the facade specialization
 * `Graph_EW<ugraph, WeightT>`, this file also provides **inline implementations**
 * of facade-specific methods whose behavior depends on the undirected graph
 * interface .
 * 
 * @details
 * This file acts as the *facade binding layer* between:
 *  - the **generic implementation** of edge-weighted graphs
 *    (`simple_graph_ew.h`, `Base_Graph_EW`)
 *  - and the **named facade graph types** defined elsewhere (e.g. `ugraph`)
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

#ifndef BITGAPH_GRAPH_UGRAPH_EDGE_WEIGHTED_H
#define BITGAPH_GRAPH_UGRAPH_EDGE_WEIGHTED_H

#include "graph_types.h"
#include "graph_unweighted.h"
#include "simple_graph_ew.h"											

namespace bitgraph {
		 
	// specialization for undirected graphs

	template <class WeightT>
	class Graph_EW<ugraph, WeightT> : public Base_Graph_EW<ugraph, WeightT> {
	public:

		using base_type = Base_Graph_EW<ugraph, WeightT>;
		using base_t = base_type;
		
		using edge_print_format = typename base_t::edge_print_format;	
		
		// Inherit constructors from the base class
		using base_t::base_t;

		//////////////////
		// Undirected-graph-specific interface.
		// The full underlying graph API remains accessible through graph().
		
		/**
		 * @brief Adds an edge with the specified weight.
		 *
		 * Adds the edge (@p v, @p w) to the graph and assigns @p weight to it.
		 * If the edge already exists, its weight is replaced by @p weight.
		 *
		 * @param v First endpoint of the edge.
		 * @param w Second endpoint of the edge.
		 * @param weight Weight assigned to the edge.
		 */
		void add_edge(
			vertex_t v,
			vertex_t w,
			weight_t weight = DEFAULT_WEIGHT) override;

		/**
		*  @brief sets edge-weight val to the undirected edge {v, w} if the edge exists
		*		  (in a non-edge it can only set the weight to NO_WEIGHT)
		*  @param v input vertex
		*  @param w input vertex
		*  @param we input weight value
		*
		*	(weights in self-loops are always added - considered vertex weights)
		*
		*  @details: asserts v!=w
		*  @details: in a non-edge it can only set weight to NO_WEIGHT
		**/
		void set_weight(
			vertex_t v, 
			vertex_t w,
			weight_t val) override;

		/**
		 * @brief Sets all existing edge weights to the same value.
		 *
		 * Assigns @p weight to every existing edge. Entries corresponding to
		 * non-edges are set to `NO_WEIGHT`.
		 *
		 * @param weight Weight assigned to every existing edge.
		 *
		 * @pre @p weight must be different from `NO_WEIGHT`.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if
		 *          @p weight is `NO_WEIGHT`.
		 */
		void set_weights(weight_t weight = this->DEFAULT_WEIGHT) override;


		/**
		* @brief sets new edge-weights based on modulus operation [Pullan 2008, MODULUS = 200]
		*
		*			I.  we(v, w) = 1 + ((v + w) % MODULUS)	(1-based index)
		*			II. Non-edges and vertex-weights are not modified
		* @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
		* @param MODULUS: modulus value [Pullan 2008, DEFAULT_WEIGHT_MODULUS = 200]
		**/
		void set_modulus_weights(
			int modulus = this->DEFAULT_WEIGHT_MODULUS) override;
						
		/////////////
		//other operations

		/**
		* @brief generates random edges uniformly with probability p and weight val
		**/
		//void gen_random_edges(double p, Weight val = DEFAULT_WEIGHT) ;

		/////////////
		// I/O operations
				
	
		/**
		 * @brief Writes the edge-weighted graph to an output stream in DIMACS format.
		 *
		 * Writes the DIMACS header followed by one record for each existing edge.
		 * Each edge record contains the 1-based endpoint identifiers and the
		 * corresponding edge weight.
		 *
		 * @param os Output stream.
		 * @return Reference to @p os.
		 *
		 * @note This generic implementation writes directed edges. For undirected
		 *       graphs, a specialization should avoid writing each edge twice.
		 */
		std::ostream& write_dimacs(
			std::ostream& os = std::cout) const override;


		/**
		* @brief prints the edges of the graph in line format [v]-(val)->[w], one edge per line
		**/
		std::ostream& print_edges(
			std::ostream& o = std::cout,
			edge_print_format format = edge_print_format::edge_list,
			bool traling_newline = false) const override;

	protected:
		/**
		 * @brief Validates an edge-weight matrix against the current graph topology.
		 *
		 * Checks that @p edge_weights is square, matches the number of graph
		 * vertices, and is consistent with the current graph topology. Existing
		 * edges must have a weight different from `NO_WEIGHT`, while non-edges
		 * must have weight `NO_WEIGHT`.
		 *
		 * @param edge_weights Edge-weight matrix to validate.
		 *
		 * @warning Follows a fail-fast policy and calls `std::terminate()` if
		 *          @p edge_weights is invalid or inconsistent with the graph.
		 */
		void validate_weight_matrix(
			const weight_matrix_t& edge_weights) const override;

	}; //end of Graph_EW class
	      
	// edge-weighted facade types

    using ugraph_ew = Graph_EW<ugraph, double>;                 // simple edge weighted graph with double weights
    using ugraph_ewi = Graph_EW<ugraph, int>;                   // simple edge weighted graph with integer weights
}

//////////////////////
// implementation in header files

namespace bitgraph {
		
	
	template<class WeightT>
	void Graph_EW<ugraph, WeightT>::set_modulus_weights(int modulus)
	{
		if (modulus < 2) {
			LOG_ERROR(
				"Invalid modulus - "
				"Graph_EW<ugraph>::set_modulus_weights");
			std::terminate();
		}

		const vertex_t NV = this->graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = v + 1; w < NV; ++w) {

				if (this->graph_.is_edge(v, w)) {

					const weight_t weight =
						static_cast<weight_t>(
							2 + ((v + w) % (modulus - 1)));

					this->set_weight(v, w, weight);
				}
				else {
					this->edge_weights_[static_cast<std::size_t>(v)]
						[static_cast<std::size_t>(w)] =
						this->NO_WEIGHT;

					this->edge_weights_[static_cast<std::size_t>(w)]
						[static_cast<std::size_t>(v)] =
						this->NO_WEIGHT;
				}
			}
		}
	}

	template<class WeightT>
	std::ostream& Graph_EW<ugraph, WeightT>::print_edges(
		std::ostream& os,
		edge_print_format format,
		bool trailing_newline) const 
	{
		const vertex_t NV = this->num_vertices();

		switch (format) {

		case edge_print_format::edge_list:

			for (vertex_t v = 0; v < NV; ++v) {
				for (vertex_t w = v + 1; w < NV; ++w) {

					if (this->graph_.is_edge(v, w)) {
						this->print_edge(v, w, os, true);
					}
				}
			}
			break;
		case edge_print_format::edge_matrix:

			for (vertex_t v = 0; v < NV; ++v) {
				for (vertex_t w = v + 1; w < NV; ++w) {

					const Weight weight =
						this->edge_weights_[static_cast<std::size_t>(v)]
						[static_cast<std::size_t>(w)];

					if (weight != NO_WEIGHT) {
						os << weight << '\t';
					}
					else {
						os << "--\t";
					}
				}
			}
			break;
		default:
			LOG_ERROR(
				"Invalid edge print format - "
				"Base_Graph_EW::print_edges");
			std::terminate();
		}

		if (trailing_newline) {
			os << '\n';
		}

		return os;
	}

	template<class WeightT>
	void Graph_EW<ugraph, WeightT>::add_edge(
		vertex_t v,
		vertex_t w, 
		weight_t weight) 
	{
		//sets undirected edge
		this->graph_.add_edge(v, w);

		//sets both weights
		this->edge_weights_[static_cast<std::size_t>(v)]
						   [static_cast<std::size_t>(w)] = weight;
		this->edge_weights_[static_cast<std::size_t>(w)]
						   [static_cast<std::size_t>(v)] = weight;
	}

	template <class W>
	void Graph_EW< ugraph, W >::set_weight(
		vertex_t v,
		vertex_t w, 
		weight_t weight)
	{

		if (v == w) {
			LOG_ERROR(
				"Attempting to set a self-loop weight - "
				"Graph_EW<ugraph>::set_weight");
			std::terminate();
		}

		if (!this->graph_.is_edge(v, w)) {
			LOG_ERROR(
				"Attempting to set an edge weight to a non-edge - "
				"Graph_EW<ugraph>::set_weight");
			std::terminate();
		}

		if (weight == this->NO_WEIGHT) {
			LOG_ERROR(
				"Attempting to assign NO_WEIGHT to an existing edge - "
				"Graph_EW<ugraph>::set_weight");
			std::terminate();
		}

		this->edge_weights_[static_cast<std::size_t>(v)]
			[static_cast<std::size_t>(w)] = weight;

		this->edge_weights_[static_cast<std::size_t>(w)]
			[static_cast<std::size_t>(v)] = weight;
	}

	template<class WeightT>
	void Graph_EW<ugraph, WeightT>::set_weights(weight_t weight)
	{
		if (weight == this->NO_WEIGHT) {
			LOG_ERROR(
				"Attempting to assign NO_WEIGHT to existing edges - "
				"Base_Graph_EW::set_weights");
			std::terminate();
		}

		const vertex_t NV = this->graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = v + 1; w < NV; ++w) {

				const bool is_edge = this->graph_.is_edge(v, w);
				const weight_t edge_weight =
					is_edge ? weight : this->NO_WEIGHT;

				this->edge_weights_[static_cast<std::size_t>(v)]
					[static_cast<std::size_t>(w)] = edge_weight;

				this->edge_weights_[static_cast<std::size_t>(w)]
					[static_cast<std::size_t>(v)] = edge_weight;
			}
		}	
	}


	template<class WeightT>
	ostream& Graph_EW<ugraph, WeightT>::write_dimacs(ostream& os) const
	{
		const int NV = this->num_vertices();

		//timestamp 
		this->graph_.timestamp_dimacs(os);

		//name
		this->graph_.name_dimacs(os);

		//dimacs header - recompute edges
		this->graph_.header_dimacs(os, false);

		//write vertex weights
		for (vertex_t v = 0; v < NV; ++v) {
			if (this->edge_weights_[v][v] != NO_WEIGHT) {
				os << "n " << v + 1 << " " << this->edge_weights_[v][v] << endl;
			}
		}

		//write undirected edges and edge weights
		for (vertex_t v = 0; v < NV - 1; ++v) {
			for (vertex_t w = v + 1; w < NV; ++w) {
				if (this->graph_.is_edge(v, w)) {														//O(log) for sparse graphs: specialize
					os << "e " 
					   << v + 1
					   << " "
					   << w + 1
					   << " "
					   << this->edge_weights_[static_cast<std::size_t>(v)]
											[static_cast<std::size_t>(w)] 
					   << '\n';	
				}
			}
		}

		return os;
	}

	template<class WeightT>
	void Graph_EW<ugraph, WeightT>::validate_weight_matrix(
		const weight_matrix_t& edge_weights) const
	{
				
		// Validate dimensions and consistency with the graph topology.
		base_t::validate_weight_matrix(edge_weights);
		 
		const vertex_t NV = this->graph_.num_vertices();

		// Undirected edges must have symmetric weights.
		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = v + 1; w < NV; ++w) {

				if (edge_weights[static_cast<std::size_t>(v)]
					[static_cast<std::size_t>(w)]
					!=
					edge_weights[static_cast<std::size_t>(w)]
					[static_cast<std::size_t>(v)]) {

					LOG_ERROR(
						"Asymmetric edge weights in undirected graph - "
						"Graph_EW<ugraph>::validate_weight_matrix");
					std::terminate();
				}
			}
		}	
	}
}



#endif // BITGAPH_GRAPH_UGRAPH_EDGE_WEIGHTED_H
