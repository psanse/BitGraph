/**
 * @file ugraph_edge_weighted.h
 * @brief Edge-weighted undirected-graph facade specialization.
 *
 * @details
 * Defines `Graph_EW<ugraph, WeightT>` and the public aliases used for
 * edge-weighted undirected graphs. The specialization binds the generic
 * edge-weighted implementation (`Base_Graph_EW` / `Graph_EW`) to the
 * `ugraph` facade and supplies operations whose behavior depends on the
 * undirected graph interface.
 *
 * Edge weights remain symmetric for both orientations of an edge. Operations
 * that enumerate, print, or write edges process each undirected edge only
 * once where required by the corresponding graph format or API contract.
 *
 * Generic edge-weighted behavior belongs in `simple_graph_ew.h`; this header
 * is reserved for behavior specific to the `ugraph` facade.
 *
 * @note This header depends on the definition of the `ugraph` facade type.
 *
 * @author Pablo San Segundo (pss)
 * @date Created 01/02/2026
 * @date Last updated 12/10/2026
 *
 * @see simple_graph_ew.h
 * @see graph_unweighted.h
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

		// Names from the dependent base class are not found by unqualified lookup
		// in a template (required by GCC/Clang)
		using graph_t = typename base_t::graph_t;
		using weight_t = typename base_t::weight_t;
		using weight_matrix_t = typename base_t::weight_matrix_t;
		using base_t::NO_WEIGHT;
		using base_t::DEFAULT_WEIGHT;

		// Inherit constructors from the base class
		using base_t::Base_Graph_EW;

		/**
		 * @brief Constructs an undirected edge-weighted graph from a DIMACS file.
		 *
		 * Hides the inherited file constructor: reading is done here, so that the
		 * undirected `add_edge()` (symmetric weights) is used for every edge.
		 * On failure the graph is empty.
		 *
		 * @param filename Name of the DIMACS file.
		 */
		explicit Graph_EW(const std::string& filename) {
			this->read_dimacs(filename);
		}

		/**
		 * @brief Makes the base-class `set_weights` overloads visible.
		 */
		using base_t::set_weights;

		/**
		 * @brief Makes the base-class `print_edges` overloads visible.
		 */
		using base_t::print_edges;


		///////////////////////
		// construction / destruction

		Graph_EW() = default;
		
		Graph_EW(
			const graph_t& graph,
			const weight_matrix_t& edge_weights)
			: base_t(graph, edge_weights)
		{
			validate_weight_symmetry(edge_weights);
		}

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
		void set_weights(weight_t weight = DEFAULT_WEIGHT) override;


		/**
		* @brief Assigns Pullan-style modulus-based weights to all existing edges
		*		 (symmetric specialization for undirected graphs).
		*
		*			I.   w(v, w) = 2 + ((v + w) % (modulus - 1))	(0-based index),
		*			     i.e., weights lie in [2, modulus]
		*			II.  Non-edges are set to NO_WEIGHT (both directions)
		*			III. The diagonal and vertex weights are not modified
		*
		* @param modulus: maximum generated weight (DEFAULT_WEIGHT_MODULUS = 200)
		*
		* @note Not identical to Pullan's (2008) 1-based formula w(i, j) = 1 + ((i + j) % modulus).
		* @warning Calls std::terminate() if @p modulus is less than 2.
		**/
		void set_modulus_weights(
			int modulus = DEFAULT_WEIGHT_MODULUS) override;
			
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
			const weight_matrix_t& edge_weights)
		{
			this->validate_weight_matrix_base(edge_weights);
			validate_weight_symmetry(edge_weights);
		}

		
		void validate_weight_symmetry(
			const weight_matrix_t& edge_weights) const;

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

					const weight_t weight =
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
		if (this->graph_.is_edge(v, w)) {
			os << "[" << v << "]--("
				<< this->edge_weights_[static_cast<std::size_t>(v)]
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
	void Graph_EW<ugraph, WeightT>::validate_weight_symmetry(
		const weight_matrix_t& edge_weights) const
	{
		const vertex_t NV = this->graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = v + 1; w < NV; ++w) {

				if (edge_weights[static_cast<std::size_t>(v)]
					[static_cast<std::size_t>(w)]
					!=
					edge_weights[static_cast<std::size_t>(w)]
					[static_cast<std::size_t>(v)]) {

					LOG_ERROR(
						"Asymmetric edge weights - "
						"Graph_EW<ugraph>::validate_weight_symmetry");
					std::terminate();
				}
			}
		}
	}
} // end of namespace bitgraph




#endif // BITGAPH_GRAPH_UGRAPH_EDGE_WEIGHTED_H
