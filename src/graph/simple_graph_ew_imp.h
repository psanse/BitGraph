/**
 * @file simple_graph_ew_imp.h
 * @brief Inline template implementations for edge-weighted graph classes.
 *
 * @details
 * This implementation fragment defines the operations declared by
 * @c Base_Graph_EW and @c Graph_EW. It keeps graph topology and the symmetric
 * edge-weight matrix synchronized while implementing construction, reset and
 * validation, weight assignment and transformation, DIMACS input/output,
 * random-edge generation, consistency checks, and graph/edge printing.
 *
 * The special value @c NO_WEIGHT represents a missing edge weight. Self-loop
 * weights are rejected, and weight matrices must be square and consistent
 * with the graph's adjacency structure. The template definitions are included
 * from @c simple_graph_ew.h so that supported graph and weight combinations can
 * be instantiated where needed.
 *
 * @author pss
 * @date 16/01/2019
 *
 * @see simple_graph_ew.h
 * @see formats/detail/dimacs_format.h
 * @see weighted_constants.h
 * @warning Invalid weight matrices, missing-edge assignments, and self-loop
 *          weights follow the fail-fast error policy implemented by the graph
 *          classes.
 * @copyright Part of the BITGRAPH C++ library.
 */

#include "bitscan/bitscan.h"
#include "graph_types.h"
#include "graph/formats/detail/dimacs_format.h"
#include "graph/simple_graph_ew.h"
#include "utils/logger.h"
#include "utils/string_utils.h"
#include "utils/precise_timer.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>							//NDEBUG should be undefined - (normally in compiler DEBUG mode)


namespace bitgraph {

	template<class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT>::validate_weight_matrix_base(
		const weight_matrix_t& edge_weights) const
	{
		const std::size_t NV =
			static_cast<std::size_t>(graph_.num_vertices());

		if (edge_weights.size() != NV) {
			LOG_ERROR(
				"Invalid edge-weight matrix size - "
				"Base_Graph_EW::validate_weight_matrix");
			std::terminate();
		}

		for (std::size_t u = 0; u < NV; ++u) {

			if (edge_weights[u].size() != NV) {
				LOG_ERROR(
					"Non-square edge-weight matrix - "
					"Base_Graph_EW::validate_weight_matrix");
				std::terminate();
			}

			for (std::size_t v = 0; v < NV; ++v) {

				const vertex_t uu = static_cast<vertex_t>(u);
				const vertex_t vv = static_cast<vertex_t>(v);

				const bool is_edge = graph_.is_edge(uu, vv);
				const bool has_weight =
					edge_weights[u][v] != NO_WEIGHT;

				if (u == v) {
					if (is_edge || has_weight) {
						LOG_ERROR(
							"Self-loop or self-loop weight found - "
							"Base_Graph_EW::validate_weight_matrix");
						std::terminate();
					}
					continue;
				}

				if (is_edge != has_weight) {
					LOG_ERROR(
						"Graph/edge-weight matrix inconsistency - "
						"Base_Graph_EW::validate_weight_matrix");
					std::terminate();
				}
			}
		}
	}

	template<class GraphT, class WeightT>
	bool Base_Graph_EW<GraphT, WeightT>::is_consistent() const noexcept
	{

		const std::size_t NV =
			static_cast<std::size_t>(graph_.num_vertices());

		if (edge_weights_.size() != NV) {
			return false;
		}

		for (std::size_t v = 0; v < NV; ++v) {

			if (edge_weights_[v].size() != NV) {
				return false;
			}

			for (std::size_t w = 0; w < NV; ++w) {

				const vertex_t vv = static_cast<vertex_t>(v);
				const vertex_t ww = static_cast<vertex_t>(w);

				const bool is_edge = graph_.is_edge(vv, ww);
				const bool has_weight =
					edge_weights_[v][w] != NO_WEIGHT;

				// Self-loops are not allowed.
				if (v == w) {
					if (is_edge || has_weight) {
						return false;
					}
					continue;
				}

				// Topology and edge weights must agree.
				if (is_edge != has_weight) {
					return false;
				}
			}
		}

		return true;
	}

	template<class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT>::initialize_edge_weights(
		weight_t weight)
	{

		const std::size_t NV =
			static_cast<std::size_t>(graph_.num_vertices());

		edge_weights_.assign(
			NV,
			std::vector<weight_t>(NV, NO_WEIGHT));

		for (std::size_t u = 0; u < NV; ++u) {
			for (std::size_t v = 0; v < NV; ++v) {
				if (graph_.is_edge(
					static_cast<vertex_t>(u),
					static_cast<vertex_t>(v))) {

					edge_weights_[u][v] = weight;
				}
			}
		}

	}

	template<class GraphT, class WeightT>
	Base_Graph_EW<GraphT, WeightT>::Base_Graph_EW(const std::string& filename) {
		if (read_dimacs(filename) != 0) {
			graph_initialization_error(
				"Failed to read DIMACS file: " + filename);
		}
	}

	template<class GraphT, class WeightT>
	template<class Func>
	void Base_Graph_EW<GraphT, WeightT>::transform_weights(Func f)
	{
		const vertex_t NV = graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = 0; w < NV; ++w) {

				const std::size_t vv = static_cast<std::size_t>(v);
				const std::size_t ww = static_cast<std::size_t>(w);

				if (edge_weights_[vv][ww] != NO_WEIGHT) {
					const weight_t weight = f(edge_weights_[vv][ww]);

					if (weight == NO_WEIGHT) {
						LOG_ERROR(
							"Weight transformation generated NO_WEIGHT - "
							"Base_Graph_EW::transform_weights");
						std::terminate();
					}

					edge_weights_[vv][ww] = weight;
				}
			}
		}
	}

	template <class GraphT, class WeightT>
	void Base_Graph_EW< GraphT, WeightT>::set_weights(weight_t weight)
	{
		if (weight == NO_WEIGHT) {
			LOG_ERROR(
				"Attempting to assign NO_WEIGHT to existing edges - "
				"Base_Graph_EW::set_weights");
			std::terminate();
		}

		const vertex_t NV = graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = 0; w < NV; ++w) {
				edge_weights_[static_cast<std::size_t>(v)]
					[static_cast<std::size_t>(w)] =
					graph_.is_edge(v, w) ? weight : NO_WEIGHT;
			}
		}
	}

	template<class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT>::set_weights(
		const weight_matrix_t& edge_weights)
	{
		validate_weight_matrix_base(edge_weights);
		edge_weights_ = edge_weights;
	}

	template<class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT>::set_modulus_weights(int modulus)
	{
		if (modulus < 2) {
			LOG_ERROR(
				"Invalid modulus - "
				"Base_Graph_EW::set_modulus_weights");
			std::terminate();
		}

		const vertex_t NV = graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = 0; w < NV; ++w) {

				if (graph_.is_edge(v, w)) {
					const weight_t weight =
						static_cast<weight_t>(
							2 + ((v + w) % (modulus - 1)));
					set_weight(v, w, weight);
				}
				else {
					edge_weights_[static_cast<std::size_t>(v)]
						[static_cast<std::size_t>(w)] = NO_WEIGHT;
				}
			}
		}
	}


	template<class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT>::reset(
		std::size_t NV, std::string name) noexcept
	{
		try {
			graph_.reset(NV);
			edge_weights_.assign(
				NV,
				weights_t(NV, NO_WEIGHT));
			graph_.set_name(std::move(name));
		}
		catch (...) {
			LOG_ERROR("Failed to reset Base_Graph_EW");
			std::terminate();
		}
	}


	template<class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT>::add_edge(
		vertex_t v,
		vertex_t w,
		weight_t weight)
	{
		assert(v != w);
		assert(weight != NO_WEIGHT);

		graph_.add_edge(v, w);
		edge_weights_[static_cast<std::size_t>(v)]
			[static_cast<std::size_t>(w)] = weight;
	}

	template <class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT >::set_weight(
		vertex_t v,
		vertex_t w,
		weight_t weight) {

		if (v == w) {
			LOG_ERROR(
				"Attempting to set a self-loop weight - "
				"Base_Graph_EW::set_weight");
			std::terminate();
		}

		if (!graph_.is_edge(v, w)) {
			LOG_ERROR(
				"Attempting to set an edge weight to a non-edge - "
				"Base_Graph_EW::set_weight");
			std::terminate();
		}

		if (weight == NO_WEIGHT) {
			LOG_ERROR(
				"Attempting to assign NO_WEIGHT to an existing edge - "
				"Base_Graph_EW::set_weight");
			std::terminate();
		}

		edge_weights_[static_cast<std::size_t>(v)]
			[static_cast<std::size_t>(w)] = weight;
	}

	///////////////
	// I/O

	template<class GraphT, class WeightT>
	int Base_Graph_EW<GraphT, WeightT>::read_dimacs(const std::string& filename)
	{
		std::ifstream input(filename);
		if (!input) {
			LOGG_ERROR("error when reading file ", filename,
				" in DIMACS format - Base_Graph_EW::read_dimacs");
			reset();
			return -1;
		}

		int nV = 0;
		int nEdges = 0;
		if (io::detail::dimacs::read_dimacs_header(input, nV, nEdges) == -1 ||
			nV < 0 || nEdges < 0) {
			LOG_ERROR("error when reading DIMACS header - Base_Graph_EW::read_dimacs");
			reset();
			return -1;
		}

		// isolated vertices, all edge weights set to NO_WEIGHT
		reset(static_cast<std::size_t>(nV));

		int read_edges = 0;
		int weighted_edges = 0;
		std::string line;

		while (std::getline(input, line)) {
			std::istringstream record(line);
			std::string token;
			if (!(record >> token)) {
				continue;											// empty line
			}

			/* instance specific - not DIMACS protocol */
			if (token == "END" || token == "end") {
				break;
			}

			const char tag = token.front();

			if (tag == 'c') {
				continue;											// comment
			}

			// vertex weights are not supported in edge-weighted graphs:
			// the diagonal of the weight matrix must remain NO_WEIGHT
			if (tag == 'n' || tag == 'v') {
				LOGG_DEBUG("vertex weight ignored: ", line,
					" - Base_Graph_EW::read_dimacs");
				continue;
			}

			if (tag != 'e') {
				LOGG_WARNING("Bad tag found: ", tag,
					" - Base_Graph_EW::read_dimacs");
				reset();
				return -1;
			}

			//////////////////
			// edge: e <v> <w> [<weight>]

			vertex_t first = -1;
			vertex_t second = -1;
			if (!(record >> first >> second) ||
				first < 1 || first > nV ||
				second < 1 || second > nV) {
				LOGG_WARNING("Bad edge found (", first, ", ", second,
					") - Base_Graph_EW::read_dimacs");
				reset();
				return -1;
			}

			if (first == second) {
				LOGG_WARNING("Self-loop found at vertex ", first,
					" - Base_Graph_EW::read_dimacs");
				reset();
				return -1;
			}

			if (read_edges >= nEdges) {
				LOGG_WARNING("Bizarre number of edges found: ", read_edges,
					" - expected ", nEdges,
					" - Base_Graph_EW::read_dimacs");
				reset();
				return -1;
			}

			// an edge with no weight is assigned DEFAULT_WEIGHT (provisionally if
			// no edge in the file has a weight: see modulus weights below)
			weight_t weight = DEFAULT_WEIGHT;
			record >> std::ws;
			if (!record.eof()) {
				if (!(record >> weight) || weight == NO_WEIGHT) {
					LOGG_WARNING("Bad edge weight found in line: ", line,
						" - Base_Graph_EW::read_dimacs");
					reset();
					return -1;
				}
				++weighted_edges;
			}

			// add_edge keeps graph and weights synchronized
			add_edge(first - 1, second - 1, weight);
			++read_edges;
		}

		if (read_edges != nEdges) {
			LOGG_WARNING("Wrong number of edges found: ", read_edges,
				" - expected ", nEdges,
				" - Base_Graph_EW::read_dimacs");
			reset();
			return -1;
		}

		// no weights found in the file: generate Pullan modulus weights
		if (read_edges > 0 && weighted_edges == 0) {
			LOGG_DEBUG("no edge weights found in ", filename,
				" - generating modulus weights - Base_Graph_EW::read_dimacs");
			set_modulus_weights();
		}

		graph_.set_name(filename);
		return 0;
	}

	template<class GraphT, class WeightT>
	std::ostream& Base_Graph_EW<GraphT, WeightT>::write_dimacs(std::ostream& os) const
	{
		const int NV = num_vertices();

		//timestamp 
		graph_.timestamp_dimacs(os);

		//name
		graph_.name_dimacs(os);

		//dimacs header - recompute edges
		graph_.header_dimacs(os, false);

		// Directed edges and edge weights.
		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = 0; w < NV; ++w) {

				if (graph_.is_edge(v, w)) {
					os << "e "
						<< v + 1 << ' '
						<< w + 1 << ' '
						<< edge_weights_[static_cast<std::size_t>(v)]
						[static_cast<std::size_t>(w)]
						<< '\n';
				}
			}
		}

		return os;
	}


	template<class GraphT, class WeightT>
	std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edges(
		const vertex_set_t& vertices,
		std::ostream& os) const
	{
		const std::size_t vertex_count = vertices.size();

		for (std::size_t i = 0; i < vertex_count; ++i) {
			for (std::size_t j = 0; j < vertex_count; ++j) {

				const vertex_t v = vertices[i];
				const vertex_t w = vertices[j];

				if (graph_.is_edge(v, w)) {
					print_edge(v, w, os, true);
				}
			}
		}

		return os;
	}

	template<class GraphT, class WeightT>
	void Base_Graph_EW<GraphT, WeightT>::gen_random_edges(double p, weight_t weight)
	{
		graph_.gen_random_edges(p);
		set_weights(weight);
	}

	template<class GraphT, class WeightT>
	std::ostream& Base_Graph_EW<GraphT, WeightT>::print_data(
		bool lazy,
		std::ostream& os,
		bool trailing_newline) const
	{

		graph_.print_data(lazy, os, false);
		os << " [type: ew]";

		if (trailing_newline) {
			os << '\n';
		}
		return os;
	}

	template<class GraphT, class WeightT>
	std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edge(
		vertex_t v,
		vertex_t w,
		std::ostream& os,
		bool trailing_newline) const
	{
		if (graph_.is_edge(v, w)) {
			os << "[" << v << "]--("
				<< edge_weights_[static_cast<std::size_t>(v)]
				[static_cast<std::size_t>(w)]
				<< ")-->"
				<< "[" << w << "]";
		}
		else {
			os << "[" << v << "]--(NO_EDGE)-->"
				<< "[" << w << "]";
		}

		if (trailing_newline) {
			os << '\n';
		}

		return os;
	}

	template<class GraphT, class WeightT>
	std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edges(
		std::ostream& os,
		enum edge_print_format format,
		bool trailing_newline) const
	{
		const vertex_t NV = this->num_vertices();

		switch (format) {

		case edge_print_format::edge_list:

			for (vertex_t v = 0; v < NV; ++v) {
				for (vertex_t w = 0; w < NV; ++w) {

					if (this->graph_.is_edge(v, w)) {
						this->print_edge(v, w, os, true);
					}
				}
			}
			break;
		case edge_print_format::edge_matrix:

			for (vertex_t v = 0; v < NV; ++v) {
				for (vertex_t w = 0; w < NV; ++w) {

					const weight_t weight =
						edge_weights_[static_cast<std::size_t>(v)]
						[static_cast<std::size_t>(w)];

					if (weight != NO_WEIGHT) {
						os << weight << '\t';
					}
					else {
						os << "--\t";
					}
				}

				os << '\n';
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


	template<class GraphT, class WeightT>
	bool bitgraph::Base_Graph_EW<GraphT, WeightT>::is_unit_weighted() const noexcept
	{
		const vertex_t NV = graph_.num_vertices();

		for (vertex_t v = 0; v < NV; ++v) {
			for (vertex_t w = 0; w < NV; ++w) {

				if (graph_.is_edge(v, w) &&
					edge_weights_[static_cast<std::size_t>(v)]
					[static_cast<std::size_t>(w)] != DEFAULT_WEIGHT) {
					return false;
				}
			}
		}

		return true;
	}

} // namespace bitgraph


