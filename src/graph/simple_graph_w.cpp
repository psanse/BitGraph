/**
 * @file simple_graph_w.cpp
 * @brief implementation of classes Base_Graph_W and Graph_W for simple weighted graphs
 *
 * @created 16/01/19
 * @last_update 06/01/25
 * @author pss
 *
 * @comments see end of file for valid template types
 *
 */

#include "graph_types.h"
#include "graph/graph_unweighted.h"		// required for valid type instantiation 
#include "graph/simple_graph_w.h"
#include "bitscan/bitscan.h"
#include "graph/formats/detail/dimacs_format.h"			
//#include "utils/common.h"
#include "utils/logger.h"
#include "utils/string_utils.h"
#include "utils/collection_utils.h"
#include "utils/precise_timer.h"

#include <fstream>
#include <iostream>
				
using namespace std;
using namespace bitgraph;

///////////////////////////////////////////////
template<class GraphT, class WeightT>
const WeightT Base_Graph_W <GraphT, WeightT >::NO_WEIGHT;

template<class GraphT, class WeightT>
constexpr WeightT Base_Graph_W <GraphT, WeightT >::ZERO_WEIGHT;

template<class GraphT, class WeightT>
constexpr WeightT Base_Graph_W <GraphT, WeightT >::DEFAULT_WEIGHT;
///////////////////////////////////////////////

template<class GraphT, class WeightT>
void Base_Graph_W<GraphT, WeightT>::create_complement(Base_Graph_W& g) const
{
	g.set_name(name());
	g.set_path(path());
	g.set_weight(vertex_weights_);

	graph_.create_complement(g.graph());
}

template<class GraphT, class WeightT>
auto Base_Graph_W<GraphT, WeightT>::create_complement() const
-> Base_Graph_W
{
	Base_Graph_W complement;
	create_complement(complement);
	return complement;
}


template<class GraphT, class WeightT>
Base_Graph_W<GraphT, WeightT>::Base_Graph_W(const std::string& filename)
{	
	if (read_dimacs(filename) == -1) {
		LOG_ERROR("error reading DIMACS file -Base_Graph_W<GraphT, WeightT>::Base_Graph_W");
		std::terminate();
	}
}

template<class GraphT, class WeightT>
void Base_Graph_W<GraphT, WeightT>::set_modulus_weight(int modulus)
 {

	if (modulus <= 0) {
		LOG_ERROR("Invalid modulus - Base_Graph_W::set_modulus_weight(...)");
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
bool Base_Graph_W<GraphT, WeightT>::is_unit_weighted() const noexcept
{
	for (weight_t weight : vertex_weights_) {
		if (weight != DEFAULT_WEIGHT) {
			return false;
		}
	}

	return true;
}

template<class GraphT, class WeightT>
void Base_Graph_W<GraphT, WeightT>::reset(
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
		LOG_ERROR("Failed to reset Base_Graph_W");
		std::terminate();
	}
}


template <class GraphT, class WeightT>
void Base_Graph_W<GraphT,WeightT >::set_weight (const vector<weight_t>& weights)
{
	//assert
	if( weights.size() != graph_.size() ){
		LOG_ERROR("Invalid number of vertex weights - Base_Graph_W::set_weight(...)");
		std::terminate();
	}

	vertex_weights_ = weights;
}

template <class GraphT, class WeightT>
void Base_Graph_W<GraphT, WeightT >::set_weight(std::vector<weight_t>&& weights)
{
	if (weights.size() != graph_.size()) {
		LOG_ERROR("Invalid number of vertex weights - Base_Graph_W::set_weight(...)");
		std::terminate();
	}

	vertex_weights_ = std::move(weights);
}

template <class GraphT, class WeightT>
auto Base_Graph_W<GraphT, WeightT>::maximum_weight(vertex_t& v) const -> weight_t
{
	if (vertex_weights_.empty()) {
		LOG_ERROR("Empty weight vector - Base_Graph_W::maximum_weight(...)");
		std::terminate();
	}

	const auto it = std::max_element(vertex_weights_.cbegin(), vertex_weights_.cend());

	v = static_cast<vertex_t>(
		std::distance(vertex_weights_.cbegin(), it));

	return *it;
}

///////////////
//
// I/O operations


template<class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::write_dimacs(ostream& os) const
{
	//timestamp comment
	graph_.timestamp_dimacs(os);
	
	//name comment
	graph_.name_dimacs(os);
	
	//dimacs header - recompute edges
	graph_.header_dimacs(os, false);
		
	//write DIMACS nodes n <v> <w>
	const int NV = graph_.num_vertices();
	for (int v = 0; v < NV; ++v ) {
		os << "n " << v + 1 << " " << weight(v) << endl;
	}
	
	//write undirected edges (1-based vertex notation dimacs)
	for (int v = 0; v < NV; ++v) {
		for (int w = 0; w < NV; ++w) {
			if (v == w) continue;
			if (graph_.is_edge(v, w)) {									//O(log) for sparse graphs: specialize
				os << "e " << v + 1 << " " << w + 1 << endl;			//1 based vertex notation dimacs
			}
		}
	}

	return os;
}


template<class GraphT, class WeightT>
int Base_Graph_W<GraphT, WeightT>::read_dimacs (string filename, int type)
{
	std::string line;
	
	fstream f(filename.c_str());
	if(!f){
		LOGG_ERROR("error when reading file ", filename, " in DIMACS format - Base_Graph_W<GraphT, WeightT>::read_dimacs");
		reset();
		return -1;
	}

	//read header
	int nV = -1, nEdges = -1;
	if(io::detail::dimacs::read_dimacs_header(f, nV, nEdges) == -1){
		reset(); 
		f.close();
		return -1;
	}	
	
	//allocates memory for the graph, assigns default unit weights

	/////////////
	reset(nV);
	////////////
	
	//skips empty lines
	io::detail::skip_empty_lines(f);
	
	//////////////
	//read vertex weights format <n> <vertex index> <weight> if they exist
	int v1 = -1, v2 = -1;
	Weight wv = -1;
	int c = f.peek();
	if(c == EOF){
		LOG_ERROR("bizarre EOF when peeking for first char - Base_Graph_W<GraphT, WeightT>::read_dimacs");
		reset();
		f.close();
		return -1;
	}
	char next = static_cast<char>(c);
	
	switch (next) {
	case 'n':
	case 'v':						// 'v' format used by Zavalnij in evil_W benchmark 

		for (int n = 0; n < nV; ++n) {
			f >> next >> v1 >> wv;
			
			//assert
			if (f.bad()) {
				LOG_ERROR("error when reading vertex-weights - Base_Graph_W<GraphT, WeightT>::read_dimacs");
				reset();
				f.close();
				return -1;
			}

			//non-positive vertex-weight check
			if (wv <= 0.0) {
				LOGG_WARNING("non-positive weight read: ", wv, "- Base_Graph_W<GraphT, WeightT>::read_dimacs");
			}

			////////////////////
			vertex_weights_[v1 - 1] = wv;
			////////////////////
						
			std::getline(f, line);  //remove remaining part of the line
		}

		//skip empty lines
		io::detail::skip_empty_lines(f);

		break;
	default:
		LOGG_DEBUG("Bad weights in file ", filename, " setting unit weights - Base_Graph_W<GraphT, WeightT>::read_dimacs");
	}
			
	//read weights from external files if necessary 
	//( @date 9/10/16, the use of additional weight files is deprecated now (26/09/23) )
	if (vertex_weights_.empty()) {
		string strExt(filename);					

		switch (type) {
		case Wext:
			strExt += ".w";
			read_weights(strExt);
			break;
		case Dext:
			strExt += ".d";
			read_weights(strExt);
			break;
		case WWWext:
			strExt += ".www";
			read_weights(strExt);
			break;
		default:
			;				//no LOG - no weights expected to be read
		}
	}
	
	////////////////	
	//read edges

	//read the first edge line - 3 tokens expected (no edge-weights)
	c = f.peek();
	if (c == EOF) {
		LOG_ERROR("bizarre EOF when peeking for first char - Base_Graph_W<GraphT, WeightT>::read_dimacs");
		reset();
		f.close();
		return -1;
	}
	next = static_cast<char>(c);

	if (next != 'e') {
		LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		reset();
		f.close();
		return -1;
	}

	std::getline(f, line);
	stringstream sstr(line);
	int nw = utils::number_of_words (line /*sstr.str()*/);

	//assert
	if(nw != 3){
		LOGG_ERROR ("Wrong edge format reading first edge line - Base_Graph_W<GraphT, WeightT>::read_dimacs");
		reset();
		f.close();
		return -1;
	}
	
	//parse the first edge
	if(nw == 3){
		sstr >> next >> v1 >> v2;
		graph_.add_edge(v1 - 1,v2 - 1);
	}
	
	//remaining edges
	for(int e = 1; e < nEdges; ++e){
		f >> next;
		if(next != 'e' || f.bad()){
			LOG_ERROR("Wrong edge format reading edges - Base_Graph_W<GraphT, WeightT>::read_dimacs");
			reset();
			f.close();
			return -1;
		}
		//add bidirectional edge	
		f >> v1 >> v2;
		graph_.add_edge(v1 - 1,v2 - 1);
			
		std::getline(f, line);  //remove remaining part of the line
	}
	f.close();
	
	//set name 
	graph_.set_name(filename);
		
	return 0;
}

template<class GraphT, class WeightT>
int Base_Graph_W<GraphT, WeightT>::read_weights(string filename) 
{
	////////////////////////////////
	ifstream f(filename.c_str());
	////////////////////////////////

	//assert
	if (!f) {
		LOGG_WARNING("Weight file ", filename, "could not be found - Base_Graph_W<GraphT, WeightT>::read_weights");
		return -1;
	}

	//debugging IO
	LOGG_DEBUG("reading vertex weights from: ", filename, "- Base_Graph_W<GraphT, WeightT>::read_weights");

	//allocation of memory for weights
	int NV = graph_.num_vertices();
	vertex_weights_.clear();
	vertex_weights_.reserve(NV);

	//reads weights
	double w = -1.0;
	for (Vertex i = 0; i < NV; ++i) {
		f >> w;
		if (f.fail()) {
			LOGG_ERROR("bad reading of weights in:", filename, "- Base_Graph_W<GraphT, WeightT>::read_weights");
			vertex_weights_.clear();
			return -1;
		}
		//////////////
		vertex_weights_[i] = w;		
		//////////////
	}

	/////////////////
	f.close();
	////////////////

	return 0;
}

template<class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::print_data(
	bool lazy,
	std::ostream& os, 
	bool trailing_newline) const
{
	graph_.print_data(lazy, os, false);
	os << " [type: vw]";								//adds tag to indicate it is weighted		
	
	if (trailing_newline) {
		os << '\n';
	}
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::print_weights (utils::FixedStack<int>& vertices, ostream& os) const
{
	const int SIZE = static_cast<int>(vertices.size());
	for(vertex_t v = 0; v < SIZE; ++v){
		os << "[" << vertices[v] << ":(" << vertex_weights_[vertices[v]] << ")] ";
	}
	os << "(" << vertices.size() << ")" <<endl;
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::print_weights (
	int* lv, 
	int NV,
	ostream& os) const
{
	for(vertex_t v = 0; v < NV; ++v){
		os << "[" << lv[v] << ":(" << vertex_weights_[lv[v]] << ")] ";
	}
	os << "(" << NV << ")" << endl;
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::print_weights (
	utils::FixedStack<int>& vertices,
	const VertexMapping& mapping,
	ostream& os) const
{
	const int vertex_count = static_cast<int>(vertices.size());
	for(vertex_t v = 0; v < vertex_count; ++v){
		os << "[" << mapping[vertices[v]] << ":(" << vertex_weights_[mapping[vertices[v]]] << ")] ";
	}
	os << "(" << vertices.size() << ")" << endl;
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::print_weights (vertex_set_t& vertices, ostream& o) const
{
	const int vertex_count = static_cast<int>(vertices.size());

	for(vertex_t v = 0; v < vertex_count; ++v){
		o << "[" << vertices[v] << ":(" << vertex_weights_[vertices[v]] << ")] ";
	}
	o << "(" << vertices.size() << ")" << endl;
	return o;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::print_weights (vertex_bitset_t& bbsg, ostream& os) const
{
	vertex_t v = bbo::noBit;

	bbsg.init_scan(bbo::NON_DESTRUCTIVE);										/* CHECK sparse graphs */
	while((v = bbsg.next_bit())!= bbo::noBit){
		os << "[" << v << ":(" << vertex_weights_[v] << ")] ";
	}
	os << "(" << bbsg.count() << ")" << endl;
	return os;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_W<GraphT, WeightT>::print_weights (ostream& os, bool show_v) const
{
	const int NV = num_vertices();
	if(show_v){
		for(Vertex i = 0; i < NV; ++i){
			os << "[" << i << ":(" << vertex_weights_[i] << ")] ";
		}
		os << endl;
	}else{
		utils::print_collection<vector<Weight>>(vertex_weights_, os, true);
	}
	return os;
}


////////////////////////////////////////////
//list of valid types for generic code in *.cpp files 

namespace bitgraph {
	
	template class  Base_Graph_W<ugraph, int>;
	template class  Base_Graph_W<ugraph, double>;
	//template class  Graph_W<ugraph, int>;
	//template class  Graph_W<ugraph, double>;

	//other specializations... (sparse_graph)

} // namespace bitgraph

////////////////////////////////////////////




