/**
 * @file simple_graph_vw_imp.h
 * @brief Inline template implementations for vertex-weighted graph classes.
 *
 * @details
 * This implementation fragment defines the template members declared by
 * @c Base_Graph_VW and @c Graph_VW. It implements vertex-weight
 * transformations, complement creation, reset and weight assignment,
 * maximum-weight queries, unit/modulus weight generation, DIMACS and external
 * weight-file I/O, and formatted weight output.
 *
 * The implementation maintains the invariant that the vertex-weight vector
 * has one entry for every vertex in the underlying graph. Missing or invalid
 * weights are handled according to the constants and fail-fast policies of
 * the public weighted-graph classes. The file is included by
 * @c simple_graph_vw.h so template definitions remain visible to users.
 *
 * @author pss
 * @date Created 16/01/2019
 * @date Last updated 06/01/2025
 *
 * @see ../simple_graph_vw.h
 * @see ../../formats/detail/dimacs_format.h
 * @see ../weighted_constants.h
 * @warning This implementation fragment should not normally be included
 *          directly by client code; use the public vertex-weighted graph
 *          header instead.
 * @note See the end of the file for the supported template types.
 */


#include "graph/formats/detail/dimacs_format.h"		
#include "utils/logger.h"
#include "utils/collection_utils.h"

#include <fstream>
#include <sstream>			

using namespace bitgraph;

template<class GraphT, class WeightT>
template<class Func>
void Base_Graph_VW<GraphT, WeightT>::transform_weights(Func f)
{
	for (weight_t& weight : vertex_weights_) {
		if (weight != NO_WEIGHT) {
			weight = f(weight);
		}
	}
}

template<class GraphT, class WeightT>
void Base_Graph_VW<GraphT, WeightT>::create_complement(Base_Graph_VW& g) const
{
	graph_.create_complement(g.graph());
	g.set_weights(vertex_weights_);
}

template<class GraphT, class WeightT>
auto Base_Graph_VW<GraphT, WeightT>::create_complement() const
-> Base_Graph_VW
{
	Base_Graph_VW complement;
	create_complement(complement);
	return complement;
}


template<class GraphT, class WeightT>
Base_Graph_VW<GraphT, WeightT>::Base_Graph_VW(const std::string& filename)
{	
	if (!read_dimacs(filename)) {
		LOG_ERROR("error reading DIMACS file -Base_Graph_VW<GraphT, WeightT>::Base_Graph_VW");
		std::terminate();
	}
}

template<class GraphT, class WeightT>
void Base_Graph_VW<GraphT, WeightT>::set_modulus_weights(int modulus)
 {

	if (modulus <= 0) {
		LOG_ERROR("Invalid modulus - Base_Graph_VW::set_modulus_weight(...)");
		std::terminate();
	}

	const int NV = graph_.num_vertices();

	vertex_weights_.clear();
	vertex_weights_.reserve(static_cast<std::size_t>(NV));
		
	for (vertex_t v = 0; v < NV; ++v) {
		vertex_weights_.push_back(static_cast<weight_t>((v + 1) % modulus + 1));
	}

}

template<class GraphT, class WeightT>
bool Base_Graph_VW<GraphT, WeightT>::is_unit_weighted() const noexcept
{
	for (weight_t weight : vertex_weights_) {
		if (weight != DEFAULT_WEIGHT) {
			return false;
		}
	}

	return true;
}

template<class GraphT, class WeightT>
void Base_Graph_VW<GraphT, WeightT>::reset(
	std::size_t NV,
	weight_t weight,
	std::string name) noexcept
{
	try {
		graph_.reset(NV);
		vertex_weights_.assign(NV, weight);
		graph_.set_name(std::move(name));
	}
	catch (...) {
		LOG_ERROR("Failed to reset Base_Graph_VW");
		std::terminate();
	}
}


template <class GraphT, class WeightT>
void Base_Graph_VW<GraphT,WeightT >::set_weights (const vector<weight_t>& weights)
{
	//assert
	if( weights.size() != graph_.size() ){
		LOG_ERROR("Invalid number of vertex weights - Base_Graph_VW::set_weight(...)");
		std::terminate();
	}

	vertex_weights_ = weights;
}

template <class GraphT, class WeightT>
void Base_Graph_VW<GraphT, WeightT >::set_weights(std::vector<weight_t>&& weights)
{
	if (weights.size() != graph_.size()) {
		LOG_ERROR("Invalid number of vertex weights - Base_Graph_VW::set_weight(...)");
		std::terminate();
	}

	vertex_weights_ = std::move(weights);
}

template <class GraphT, class WeightT>
auto Base_Graph_VW<GraphT, WeightT>::maximum_weight(vertex_t& v) const -> weight_t
{
	if (vertex_weights_.empty()) {
		LOG_ERROR("Empty weight vector - Base_Graph_VW::maximum_weight(...)");
		std::terminate();
	}

	const auto it = std::max_element(vertex_weights_.cbegin(), vertex_weights_.cend());

	v = static_cast<vertex_t>(
		std::distance(vertex_weights_.cbegin(), it));

	return *it;
}

///////////////
// I/O operations

template<class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::write_dimacs(ostream& os) const
{
	//timestamp comment
	graph_.timestamp_dimacs(os);
	
	//name comment
	graph_.name_dimacs(os);
	
	//dimacs header - recompute edges
	graph_.header_dimacs(os, false);
		
	//write DIMACS nodes n <v> <w>
	const vertex_t NV = graph_.num_vertices();
	for (vertex_t v = 0; v < NV; ++v ) {
		os << "n " << v + 1 << " " << weight(v) << '\n';
	}
		
	// TODO
	//graph_.write_dimacs_edges(os, false);		// delegate to the specific graph type (specialization)

	// Only works for directed graphs - used here for testing
	for (vertex_t v = 0; v < NV; ++v) {
		for (vertex_t w = 0; w < NV; ++w) {
			if (v != w && graph_.is_edge(v, w)) {						//O(log) for sparse graphs: specialize
				os << "e " << v + 1 << " " << w + 1 << '\n';			//1 based vertex notation dimacs
			}
		}
	}
	return os;
}


template<class GraphT, class WeightT>
bool Base_Graph_VW<GraphT, WeightT>::read_dimacs(
	const std::string& filename, weight_file_extension type)
{
	std::ifstream input(filename);
	if (!input) {
		LOGG_ERROR("error when reading file ", filename,
			" in DIMACS format - typed Base_Graph_VW::read_dimacs");
		reset();
		return false;
	}

	int nV = 0;
	int nEdges = 0;
	if (io::detail::dimacs::read_dimacs_header(input, nV, nEdges) == -1 ||
		nV < 0 || nEdges < 0) {
		reset();
		return false;
	}

	reset(nV);
	bool has_inline_weights = false;
	int read_edges = 0;
	std::string line;

	while (std::getline(input, line)) {
		std::istringstream record(line);
		std::string token;
		if (!(record >> token)) {
			continue;
		}

		/* instance specific - not DIMACS protocol */
		if (token == "END" || token == "end") {
			break;
		}
		char tag = token.front();

		/* again, not DIMACS protocol */
		/*if (tag >= 'A' && tag <= 'Z') {
			tag = static_cast<char>(tag - 'A' + 'a');
		}*/

		if (tag == 'c') {
			continue;
		}

		////////////////////
		// vertex weights

		if (tag == 'n' || tag == 'v') {
			vertex_t vertex = 0;
			weight_t weight{};
			if (!(record >> vertex >> weight) || vertex < 1 || vertex > nV) {
				LOGG_WARNING(
					"Bad vertex weight found ",
					" vertex: ", vertex, 
					" weight: ", weight);
				reset();
				return false;
			}
			vertex_weights_[static_cast<std::size_t>(vertex - 1)] = weight;
			has_inline_weights = true;
			continue;
		}

		//////////////////
		// edges

		if (tag != 'e') {
			LOGG_WARNING( 
				"Bad tag for edge found ",
				tag );
			reset();
			return false;
		}

		vertex_t first = BBObject::noBit;
		vertex_t second = BBObject::noBit;
		if (!(record >> first >> second) ||
			first < 1 || first > nV ||
			second < 1 || second > nV) 
		{
			LOGG_WARNING(
				"Bad edge found ",
				"(", first, ", ", second, ")",
				" - Base_Graph_VW::read_dimacs");
			reset();
			return false;
		}

		if (read_edges >= nEdges) {
			LOGG_WARNING(
				"Bizarre number of edged found ",
				read_edges,
				" - expected ", nEdges, 
				" - Base_Graph_VW::read_dimacs");
			reset();
			return false;
		}

		if (first == second) {
			LOGG_WARNING(
				"Self-loop found at vertex ",
				first,
				" - Base_Graph_VW::read_dimacs");

			reset();
			return false;
		}

		graph_.add_edge(first - 1, second - 1);
		++read_edges;
	} // end of reading lines

	if (read_edges != nEdges) {
		reset();
		return false;
	}

	// Read weights from a separate file if they were not included inline and a weight file type was specified.
	if (!has_inline_weights && type != weight_file_extension::none) {
		std::string weight_filename = filename;
		switch (type) {
		case weight_file_extension::w:   weight_filename += ".w"; break;
		case weight_file_extension::d:   weight_filename += ".d"; break;
		case weight_file_extension::www: weight_filename += ".www"; break;
		case weight_file_extension::none: break;
		default:
			reset();
			return false;
		}

		std::ifstream weights_input(weight_filename);
		if (!weights_input) {
			reset();
			return false;
		}
		std::vector<weight_t> weights(static_cast<std::size_t>(nV));
		for (weight_t& weight : weights) {
			if (!(weights_input >> weight)) {
				reset();
				return false;
			}
		}
		vertex_weights_ = std::move(weights);
	} // end of reading weights from a separate file


	graph_.set_name(filename);
	return true;
}


template<class GraphT, class WeightT>
bool Base_Graph_VW<GraphT, WeightT>::read_weights(const std::string& filename) 
{
	ifstream f(filename.c_str());

	if (!f) {
		LOGG_WARNING(
			"Weight file ", filename,
			" could not be found - Base_Graph_VW::read_weights");
		return false;
	}
	

	LOGG_DEBUG(
		"Reading vertex weights from: ", filename,
		" - Base_Graph_VW::read_weights");


	const int NV = graph_.num_vertices();
	vertex_weights_.resize(static_cast<std::size_t>(NV));

	for (vertex_t v = 0; v < NV; ++v) {
		if (!(f >> vertex_weights_[v])) {
			LOGG_ERROR(
				"Error when reading weights from: ", filename,
				" - Base_Graph_VW::read_weights");

			vertex_weights_.clear();
			return false;
		}
	}

	return true;
}

template<class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::print_data(
	bool lazy,
	std::ostream& os, 
	bool trailing_newline) const
{
	graph_.print_data(lazy, os, false);
	os << " [type: vw]";					// vertex-weighted graph			
	
	if (trailing_newline) {
		os << '\n';
	}
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::print_weights (
	const utils::FixedStack<int>& vertices,
	ostream& os) const
{
	const int vertex_count = static_cast<int>(vertices.size());

	for(int i = 0; i < vertex_count; ++i){
		const vertex_t v = vertices[i];

		os << "[" << v << ":(" 
			<< vertex_weights_[static_cast<std::size_t>(v)]
			<< ")] ";
	}
	os << "(" << vertex_count << ")" << '\n';
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::print_weights (
	const vertex_t* vertices, 
	int NV,
	ostream& os) const
{
	for(int i = 0; i < NV; ++i){
		const vertex_t v = vertices[i];
		os << "[" << v
			<< ":(" << vertex_weights_[static_cast<std::size_t>(v)] 
			<< ")] ";
	}
	os << "(" << NV << ")" << '\n';
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::print_weights (
	const utils::FixedStack<int>& vertices,
	const VertexMapping& mapping,
	ostream& os) const
{
	const int vertex_count = static_cast<int>(vertices.size());

	for(int i = 0; i < vertex_count; ++i){
		const vertex_t v = mapping[vertices[i]];

		os << "[" << v << ":(" << vertex_weights_[static_cast<std::size_t>(v)] << ")] ";
	}
	os << "(" << vertex_count << ")" << endl;
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::print_weights (
	vertex_set_t& vertices, 
	ostream& os) const
{
	const int vertex_count = static_cast<int>(vertices.size());

	for(vertex_t v = 0; v < vertex_count; ++v){
		os << "[" << vertices[v] << ":(" << vertex_weights_[vertices[v]] << ")] ";
	}
	os << "(" << vertices.size() << ")" << endl;
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::print_weights (
	vertex_bitset_t& vertices, 
	ostream& os) const
{
	vertex_t v = bbo::noBit;

	vertices.init_scan(bbo::NON_DESTRUCTIVE);		// CHECK sparse graphs
							
	while((v = vertices.next_bit())!= bbo::noBit){
		os << "[" << v
			<< ":(" << vertex_weights_[v]
			<< ")] ";
	}

	os << "(" << vertices.count() << ")" << '\n';
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_VW<GraphT, WeightT>::print_weights (
	ostream& os,
	bool show_vertices) const
{

	if (!show_vertices) {
		utils::print_collection<weights_t>(
			vertex_weights_, os, true);
		return os;
	}

	const int vertex_count = num_vertices();

	for (vertex_t v = 0; v < vertex_count; ++v) {
		const weight_t weight = vertex_weights_[static_cast<std::size_t>(v)];
		os << "[" << v
			<< ":(" << weight << ")] ";
	}
	os << '\n';
	
	return os;
}


//////////////////////////////////////////////
// list of valid types for generic code in *.cpp files 
// DEPRECATED (10/10/2026)
//
//namespace bitgraph {
//	
//	template class  Base_Graph_VW<ugraph, int>;
//	template class  Base_Graph_VW<ugraph, double>;
//	//template class  Graph_W<ugraph, int>;
//	//template class  Graph_W<ugraph, double>;
//
//	//other specializations... (sparse_graph)
//
//} // namespace bitgraph





