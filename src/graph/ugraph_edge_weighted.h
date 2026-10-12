/**
 * @file ugraph_edge_weighted.h
 * @brief Facade specialization for edge-weighted undirected graphs.
 *
 * Defines the specialization `Graph_EW<ugraph, WeightT>` and the public
 * aliases for commonly used edge-weighted undirected graph types.
 *
 * This header provides the binding between the generic edge-weighted graph
 * implementation (`Base_Graph_EW` / `Graph_EW`) and the `ugraph` facade type.
 * It also contains inline implementations of operations whose behavior
 * depends specifically on the undirected graph interface.
 *
 * In particular, the specialization preserves the symmetry of edge weights
 * and ensures that each undirected edge is handled only once where required,
 * for example when printing or writing graph instances.
 *
 * @note Generic edge-weighted graph functionality should remain in
 *       `simple_graph_ew.h`. Only behavior specific to `ugraph` should be
 *       implemented in this header.
 *
 * @note This header depends on the definition of the `ugraph` facade type.
 *
 * @author Pablo San Segundo (pss)
 *
 * @details Created 01/02/2026, last updated 12/10/2026.
 */

#ifndef BITGAPH_GRAPH_UGRAPH_EDGE_WEIGHTED_H
#define BITGAPH_GRAPH_UGRAPH_EDGE_WEIGHTED_H

#include "graph_types.h"
#include "graph_unweighted.h"
#include "simple_graph_ew.h"											

namespace bitgraph {
		 
	/**
	 * @brief Facade specialization for edge-weighted undirected graphs.
	 *
	 * Specializes the user-facing `Graph_EW` interface for `ugraph`. Generic
	 * edge-weighted functionality is inherited from `Base_Graph_EW`, while
	 * operations that depend on the undirected graph representation are
	 * implemented in this specialization.
	 *
	 * In particular, edge weights are kept symmetric: for every edge
	 * `(v,w)`, the entries `(v,w)` and `(w,v)` store the same weight.
	 * Undirected edges are also processed only once where appropriate, such
	 * as when printing or writing graph instances.
	 *
	 * The underlying graph API remains accessible through `graph()`.
	 *
	 * @tparam WeightT Type used to represent edge weights.
	 */
	template <class WeightT>
	class Graph_EW<ugraph, WeightT> : public Base_Graph_EW<ugraph, WeightT> {
	public:

		using base_t = Base_Graph_EW<ugraph, WeightT>;
		using base_type = base_t;						// alias for backward compatibility				
		
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
		 * @brief Prints an undirected edge and its weight.
		 *
		 * Overrides the generic edge representation provided by `Base_Graph_EW`
		 * to use an undirected format. If (@p v, @p w) is an existing edge,
		 * prints it in the form `[v]--(weight)--[w]`. Otherwise, prints
		 * `[v]--(NO_EDGE)--[w]`.
		 *
		 * @param v First endpoint of the edge.
		 * @param w Second endpoint of the edge.
		 * @param os Output stream.
		 * @param trailing_newline If `true`, appends a newline after the output.
		 * @return Reference to @p os.
		 */
		std::ostream& print_edge(
			vertex_t v,
			vertex_t w,
			std::ostream& os = std::cout,
			bool trailing_newline = true) const override;

		/**
		 * @brief Prints the edges of the graph and their weights.
		 *
		 * Edges are written using the format specified by @p format. In
		 * `edge_list` mode, each existing edge is printed individually. In
		 * `edge_matrix` mode, the complete edge-weight matrix is printed, using
		 * `NO_WEIGHT` entries to represent non-edges.
		 *
		 * @param os Output stream.
		 * @param format Edge-printing format.
		 * @param trailing_newline If `true`, appends an additional newline after
		 *                         the complete output.
		 *
		 * @return Reference to @p os.
		 */
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

	/**
	 * @brief Edge-weighted undirected graph with `double` weights.
	*/
    using ugraph_ew = Graph_EW<ugraph, double>;               
   
	/**
	 * @brief Edge-weighted undirected graph with `int` weights.
	 */
	using ugraph_ewi = Graph_EW<ugraph, int>;                  
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
	std::ostream& Graph_EW<ugraph, WeightT>::print_edge(
		vertex_t v,
		vertex_t w,
		std::ostream& os, 
		bool trailing_newline) const
	{
		if (graph_.is_edge(v, w)) {
			os << "[" << v << "]--("
				<< edge_weights_[static_cast<std::size_t>(v)]
				[static_cast<std::size_t>(w)]
				<< ")--"
				<< "[" << w << "]";
		}
		else {
			os << "[" << v << "]--(NO_EDGE)--"
				<< "[" << w << "]";
		}

		if (trailing_newline) {
			os << '\n';
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
