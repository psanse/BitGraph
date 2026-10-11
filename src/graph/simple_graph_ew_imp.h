 /**
   * @file simple_graph_ew_imp.h
   * @brief implementation for Base_Graph_EW and Graph_EW classes (edge-weighted graphs)
   *
   * @created 16/01/19
   * @last_update 06/01/25
   * @author pss
   *
   * @comments see end of file for valid template types
   *
   * This code is part of the GRAPH 1.0 C++ library
   *
   **/

#include "bitscan/bitscan.h"
#include "graph_types.h"
#include "graph/formats/detail/dimacs_format.h"
#include "graph/simple_graph_ew.h"
#include "utils/logger.h"
#include "utils/string_utils.h"
#include "utils/precise_timer.h"

#include <fstream>
#include <iostream>
#include <cassert>							//NDEBUG should be undefined - (normally in compiler DEBUG mode)


using namespace bitgraph;


template<class GraphT, class WeightT>
 void bitgraph::Base_Graph_EW<GraphT, WeightT>::validate_weight_matrix() const
{
	 const std::size_t NV =
		 static_cast<std::size_t>(graph_.num_vertices());

	 // Matrix must have one row per vertex.
	 if (edge_weights_.size() != NV) {
		 LOG_ERROR(
			 "Invalid edge-weight matrix size - "
			 "Base_Graph_EW::validate_weight_matrix");
		 std::terminate();
	 }

	 for (std::size_t u = 0; u < NV; ++u) {

		 // Matrix must be square.
		 if (edge_weights_[u].size() != NV) {
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
				 edge_weights_[u][v] != NO_WEIGHT;

			 // Self-loops are not allowed.
			 if (u == v) {
				 if (is_edge || has_weight) {
					 LOG_ERROR(
						 "Self-loop or self-loop weight found - "
						 "Base_Graph_EW::validate_weight_matrix");
					 std::terminate();
				 }
				 continue;
			 }

			 // Topology and weight matrix must agree.
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
 void Base_Graph_EW<GraphT, WeightT>::initialize_edge_weights(
	 weight_t weight = DEFAULT_WEIGHT)
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

				 edge_weights_[u][v] = DEFAULT_WEIGHT;
			 }
		 }
	 }

 }

 template<class GraphT, class WeightT>
 Base_Graph_EW<GraphT, WeightT>::Base_Graph_EW(const std::string& filename) {
	 read_dimacs(filename);
 }

template <class GraphT, class WeightT>
template <bool EraseNonEdges>
void Base_Graph_EW< GraphT, WeightT>::set_weights(
	weight_matrix_t& edge_weights,
	bool edges_only) 
{

	auto NV = num_vertices();

	/////////////////////////
	assert(lw.size() == NV);
	/////////////////////////

	/*if (lw.size() != NV) {
		LOG_ERROR("bizarre matrix of weights-Base_Graph_EW<GraphT,WeightT >::set_edge_weight(mat_t...)");
		return -1;
	}*/

	//set to empty wv and non-edges
	for (int v = 0; v < NV; ++v) {
		for (int w = 0; w < NV; ++w) {

			if (edges_only && v == w) continue;			//skips vertex weights if required

			if (graph_.is_edge(v, w)) {
				edge_weights_[v][w] = edge_weights[v][w];
			}
			else {
				//cleans non-edge weights if required
				if (EraseNonEdges) { edge_weights_[v][w] = NO_WEIGHT; }
			}
		}
	}
}

template<class GraphT, class WeightT>
template<class Func>
void Base_Graph_EW<GraphT, WeightT>::transform_weights(Func f, int type)
{
	auto NV = num_vertices();

	switch (type) {
		//edge-weights
	case EDGE:
		for (auto i = 0; i < NV - 1; ++i) {
			for (auto j = i + 1; j < NV; ++j) {
				if (edge_weights_[i][j] != NO_WEIGHT) {
					edge_weights_[i][j] = f(edge_weights_[i][j]);
				}
				if (edge_weights_[j][i] != NO_WEIGHT) {
					edge_weights_[j][i] = f(edge_weights_[j][i]);
				}
			}
		}
		break;
		//vertex-weights
	case VERTEX:
		for (auto i = 0; i < NV; ++i) {
			if (edge_weights_[i][i] != NO_WEIGHT) {
				edge_weights_[i][i] = f(edge_weights_[i][i]);
			}
		}
		break;
	case BOTH:
		//vertex and edge-weights
		for (auto i = 0; i < NV; ++i) {
			for (auto j = 0; j < NV; ++j) {
				if (edge_weights_[i][j] != NO_WEIGHT) {
					edge_weights_[i][j] = f(edge_weights_[i][j]);
				}
			}
		}
		break;
	default:
		//should not happen	
		LOG_ERROR("unknown type -  Base_Graph_EW<GraphT, WeightT>::transform_weights");
		LOG_ERROR("exiting");
		std::exit(EXIT_FAILURE);
	}
}

template <class GraphT, class WeightT>
template<bool EraseNonEdges>
void Base_Graph_EW< GraphT, WeightT>::set_weights(Weight val) {

	auto NV = num_vertices();

	//set to empty weight the non-edges
	for (int v = 0; v < NV; v++) {
		for (int w = 0; w < NV; w++) {
			if (graph_.is_edge(v, w)) {
				edge_weights_[v][w] = val;
			}
			else {
				if (EraseNonEdges) { edge_weights_[v][w] = NO_WEIGHT; }
			}
		}
	}
}

template<class GraphT, class WeightT>
template <bool EraseNonEdges>
void Base_Graph_EW<GraphT, WeightT>::set_modulus_edge_weights(int MODULUS) {

	int NV = num_vertices();

	for (int v = 0; v < NV; ++v) {
		for (int w = 0; w < NV; ++w) {

			if (graph_.is_edge(v, w)) {

				///////////////////////////////////////
				set_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
				///////////////////////////////////////
			}
			else {
				if (EraseNonEdges) {
					set_edge_weight(v, w, NO_WEIGHT);
				}
			}
		}
	}
}

template<class GraphT, class WeightT>
void Base_Graph_EW<GraphT, WeightT>::reset(
	std::size_t NV, string name) noexcept
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
 bool Base_Graph_EW<GraphT, WeightT>::is_consistent(){

	auto NV = num_vertices();

	for (auto i = 0; i < NV; ++i) {
		for (auto j = 0; j < NV; ++j) {

			if ( edge_weights_[i][j] == NO_WEIGHT &&  graph_.is_edge(i, j) ) {
				LOGG_WARNING ("edge [", i, ", ", j ,")] with NO_WEIGHT - Base_Graph_EW<GraphT, WeightT>::is_consistent()");
				return false;
			}

		}
	}
	return true;
}



template<class GraphT, class WeightT>
void Base_Graph_EW<GraphT, WeightT>::add_edge(int v, int w, Weight val)
{
	graph_.add_edge(v, w);
	edge_weights_[v][w] = val;	
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
	
	edge_weights_[v][w] = weight;
}


template<class GraphT, class WeightT>
std::vector<WeightT> Base_Graph_EW<GraphT, WeightT>::vertex_weights()  const {

	auto NV = this->num_vertices();

	vector<WeightT> res;
	res.reserve(NV);
	for (auto v = 0; v < NV; ++v) {
		res.emplace_back(edge_weights_[v][v]);
	}

	return res;
}

///////////////
//
// I/O
//
//////////////

template<class GraphT, class WeightT>
int Base_Graph_EW<GraphT, WeightT>::read_dimacs (string filename){
		
	string line;	
		
	fstream f(filename.c_str());
	if(!f){
		LOGG_ERROR("error when reading file ", filename, " in DIMACS format - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		reset();
		return -1;
	}

	//read header
	 int nV = -1, nEdges = -1;
	 if (io::detail::dimacs::read_dimacs_header (f, nV, nEdges) == -1)
	 {
		 LOG_ERROR("error when reading dimacs header / allocation - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }

	 //allocates memory for the graph - no allocation for edge-weights
	 reset(nV);

	 //skips empty lines
	 io::detail::skip_empty_lines(f);

	 //////////////
	//read vertex-weights format <n> <vertex index> <weight> if they exist
	 int v1 = -1, v2 = -1;
	 WeightT wv = NO_WEIGHT;
	 
	 //peek next non-empty character
	 int c = f.peek();
	 if (c == EOF) {
		 LOG_ERROR("bizarre EOF when peeking for first char - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }
	 char next = static_cast<char>(c);
	 
	 switch (next) {
	 case 'n':
	 case 'v':									// 'v' format used by Zavalnij in evil_W benchmark 

		 for (int n = 0; n < nV; ++n) {
			 f >> next >> v1 >> wv;

			 //assert
			 if (f.bad()) {
				 LOG_ERROR("error when reading vertex-weights - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
				 reset();
				 f.close();
				 return -1;
			 }

			 //non-positive vertex-weight check
			 if (wv <= static_cast<WeightT>(0)) {
				 LOGG_WARNING("non-positive weight read: ", wv, "- Base_Graph_EW<GraphT, WeightT>::read_dimacs");
			 }

			 //////////////////////////
			 edge_weights_[v1 - 1][v1 - 1] = wv;
			 //////////////////////////
			 
			 // Consume rest of line
			 std::getline(f, line);  
		 }

		 // Skip possible empty lines after vertex section
		 io::detail::skip_empty_lines(f);
		 break;
	 default:
		 LOGG_DEBUG("missing vertex-weights in file ", filename, " setting unit weights - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
	 }//end of switch
	 	
	 /////////////////////	
	 //read edges
	 
	 //read the first edge line - 4 tokens expected (e <v> <w> <edge-weight>)
	 c = f.peek();
	 if (c == EOF) {
		 LOG_ERROR("bizarre EOF when peeking for first char - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
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

	 // Read first edge line to detect presence of edge weights
	
	 std::getline(f, line);
	 stringstream sstr(line);

	 //determine if edge weights are present
	 int nw = utils::number_of_words(line);
	 bool edge_weights_found = true;
	 switch (nw) {
	 case 4: edge_weights_found = true;	break; 		//expected: e <v> <w> <edge-weight> 
	 case 3: edge_weights_found = false;  break;	//expeceted: e <v> <w>  	
	 default:
		 LOG_ERROR("Wrong edge format - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }

	 WeightT we = NO_WEIGHT;

	 //parse the first edge
	 if (edge_weights_found) {

		 /////////////////////////////
		 sstr >> next >> v1 >> v2 >> we;
		 ////////////////////////////

		 graph_.add_edge(v1 - 1, v2 - 1);
		 set_weight (v1 - 1, v2 - 1, we);
	 }
	 else {	

		 ///////////////////////
		 sstr >> next >> v1 >> v2;
		 ///////////////////////

		 graph_.add_edge(v1 - 1, v2 - 1);
		 set_weight (v1 - 1, v2 - 1, NO_WEIGHT);				//no edge-weights in file - set NO_WEIGHT value
	 }
	 
	 // Parse remaining edges
	 for (int e = 1; e < nEdges; ++e) {
		 f >> next;
		 if ( f.bad() || next != 'e'  ) {
			 LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
			 reset();
			 f.close();
			 return -1;
		 }

		 //add weighted edge	
		 if (edge_weights_found) {

			 /////////////////////
			 f >> v1 >> v2 >> we;
			 /////////////////////

			 graph_.add_edge (v1 - 1, v2 - 1);
			 set_weight (v1 - 1, v2 - 1, we);
		 }
		 else {
			 ///////////////
			 f >> v1 >> v2;
			 ///////////////

			 graph_.add_edge(v1 - 1, v2 - 1);
			 set_weight (v1 - 1, v2 - 1, NO_WEIGHT);			//no edge-weights in file - set NO_WEIGHT value
		 }

		 // Consume rest of edge line
		 std::getline(f, line); 
	 }

	 //name (removes path)
	 graph_.set_name(filename);
	 f.close();	
	 return 0;
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::write_dimacs (std::ostream& o) {
	
	auto NV = num_vertices();

	//timestamp 
	graph_.timestamp_dimacs(o);
	
	//name
	graph_.name_dimacs(o);

	//dimacs header - recompute edges
	graph_.header_dimacs(o, false);

	//write vertex weights (edge weights of self loops are considered vertex weights)
	for (int v = 0; v < NV; ++v) {
		if (edge_weights_[v][v] != NO_WEIGHT) {
			o << "n " << v + 1 << " " << edge_weights_[v][v] << std::endl;
		}
	}
		
	//write (directed) edges and edge-weights
	for(int v = 0; v < NV; ++v){
		for(int w = 0; w < NV; ++w){
			if (v != w && graph_.is_edge(v, w)) {										
				o << "e " << v + 1 << " " << w + 1 << edge_weights_[v][w] << std::endl;			//1-based vertex notation dimacs format	
			}
		}
	}

	return o;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_EW<GraphT, WeightT>::print_weights (ostream& o, bool line_format, int type) const{

	//o << endl << "**************************" << endl;
	switch (type) {
	case EDGE:
		print_edge_weights(o, line_format);
		break;
	case VERTEX:
		print_vertex_weights(o);
		break;
	case BOTH:
		print_edge_weights(o, line_format);
		//o << endl << "**************************" << endl;
		print_vertex_weights(o);
		break;
	default:
		LOG_ERROR("unknown type - Base_Graph_EW<GraphT, WeightT>::print_weights");
		LOG_ERROR("exiting");
		std::exit(EXIT_FAILURE);
	}
	//o << endl << "**************************" << endl;
	return o;
}
template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edge_weights(std::ostream& o, bool line_format) const
{
	const auto NV = graph_.num_vertices();

	//streams edge-weights 
	if (line_format) {
		print_edges(o, false);
		/*for (auto i = 0; i < NV; ++i) {
			for (auto j = 0; j < NV; ++j) {
				if (edge_weights_[i][j] != Base_Graph_EW<GraphT, WeightT>::NO_WEIGHT) {
					o << "[" << i << "->" << j << " (" << edge_weights_[i][j] << ")] " << endl;
				}
			}
		}*/
	}
	else {								//outputs to stream edge-weights in matrix form
		for (int i = 0; i < NV; ++i) {
			for (int j = 0; j < NV; ++j) {
				if (edge_weights_[i][j] != Base_Graph_EW<GraphT, WeightT>::NO_WEIGHT) {
					o << edge_weights_[i][j] << '\t';
				}
				else {
					o << "--" << '\t';
				}
			}
			o << endl;
		}
	}

	return o;
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_vertex_weights(std::ostream& o) const
{
	const auto NV = graph_.num_vertices();	

	for (int v = 0; v < NV; v++) {
		if (edge_weights_[v][v] != Base_Graph_EW<GraphT, WeightT>::NO_WEIGHT) {
			o << "[" << v << ":(" << edge_weights_[v][v] << ")] " << endl;
		}
	}
	return o;
}

template <class GraphT, class WeightT>
ostream& Base_Graph_EW<GraphT, WeightT>::print_weights (Vertices& lv, ostream& o, int type) const{

	//o << endl << "**************************" << endl;
	switch (type) {
	case EDGE:
		print_edge_weights(lv, o);
		break;
	case VERTEX:
		print_vertex_weights(o);
		break;
	case BOTH:
		print_edge_weights(lv, o);
		//o << endl << "**************************" << endl;
		print_vertex_weights(lv, o);
		break;
	default:
		LOG_ERROR("unknown type - Base_Graph_EW<GraphT, WeightT>::print_weights");
		LOG_ERROR("exiting");
		std::exit(EXIT_FAILURE);
	}
	//o << endl << "**************************" << endl;

	return o;
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edge_weights(Vertices& lv, std::ostream& o) const
{
	for (auto i = 0u; i < lv.size(); ++i) {
		for (auto j = 0u; j < lv.size(); j++) {
			if (edge_weights_[lv[i]][lv[j]] != Base_Graph_EW<GraphT, WeightT>::NO_WEIGHT) {

				///////////////////////////////////////////////////////////////////////////////////
				o << "[" << lv[i] << "->" << lv[j] << " (" << edge_weights_[lv[i]][lv[j]] << ")] " << endl;
				////////////////////////////////////////////////////////////////////////////////////
			}
		}
	}

	return o;
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_vertex_weights(Vertices& lv, std::ostream& o) const
{
	for (auto i = 0u; i < lv.size(); ++i) {
		if (edge_weights_[lv[i]][lv[i]] != Base_Graph_EW<GraphT, WeightT>::NO_WEIGHT) {

			/////////////////////////////////////////////////////////////////
			o << "[" << lv[i] << ":(" << edge_weights_[lv[i]][lv[i]] << ")] " << endl;
			/////////////////////////////////////////////////////////////////
		}
	}

	return o;
}

template<class GraphT, class WeightT>
ostream& Base_Graph_EW<GraphT, WeightT>::print_data (bool lazy, std::ostream& o, bool eofl) {
	graph_.print_data(lazy, o, false);
	o << " [type:ew]";								//"WeightT" for vertex-weighted graphs in GRAPH lib
	if (eofl) { o << std::endl; }
	return o;
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edge(int v, int w, std::ostream& o, bool eofl) const
{
	if (graph_.is_edge(v, w)) {
		o << "[" << v << "]--(" << edge_weights_[v][w] << ")-->" << "[" << w << "]";
	}
	else {
		o << "[" << v << "]--(NO_EDGE)-->" << "[" << w << "]";
	}

	if (eofl) { o << std::endl; }	
	return o;
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_vertex(int v, std::ostream& o, bool eofl) const
{
	o << "[" << v << ":(" << edge_weights_[v][v] << ")" << "]"; 
	
	if (eofl) { o << std::endl; }
	return o; 
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edges(std::ostream& o, bool eofl) const{

	const auto NV = graph_.num_vertices();
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (is_edge(i, j)) {
				o << "[" << i << "]" << "--(" << weight(i, j) << ")-->" << "[" << j << "]" << endl;
			}
			if (is_edge(j, i)) {
				o << "[" << j << "]" << "--(" << weight(i, j) << ")--->" <<  "[" << i << "]" << endl;
			}

		}
	}

	if (eofl) {	o << std::endl;	}
	return o;
}

template<class GraphT, class WeightT>
void Base_Graph_EW<GraphT, WeightT>::create_complement(Base_Graph_EW<graph_type, Weight>& g) const
{
	g.set_name(this->name());
	g.set_path(this->path());
	g.weights() = edge_weights_;

	/////////////////////////////////////////
	graph_.create_complement(g.graph());
	/////////////////////////////////////////
}

template<class GraphT, class WeightT>
void Base_Graph_EW<GraphT, WeightT>::gen_random_edges(double p, Weight val)
{
	const auto NV = graph_.num_vertices();

	//removes all edges
	graph_.remove_edges();

	//sets directed edges with probability p
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			//considers both directed edges separately
			if (utils::uniform_dist(p)) {
				add_edge(i, j, val);				
			}
			if (utils::uniform_dist(p)) {
				add_edge(j, i, val);
			}
		}
	}
}

template<class GraphT, class WeightT>
void Base_Graph_EW<GraphT, WeightT>::erase_non_edge_weights() {

	const auto NV = graph_.num_vertices();

	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (!graph_.is_edge(i, j)) {
				this->set_weight(i, j, Base_Graph_EW<GraphT, WeightT>::NO_WEIGHT);
			}
		}
	}
}

template<class GraphT, class WeightT>
void Base_Graph_EW<GraphT, WeightT>::make_edge_weighted(bool erase_non_edges)
{
	//sets all vertex weights to NO_WEIGHT
	for (auto v = 0u; v < graph_.size(); ++v) {
		edge_weights_[v][v] = Base_Graph_EW<GraphT, WeightT>::NO_WEIGHT;	
	}

	if (erase_non_edges) {
		erase_non_edge_weights();
	}
}


////////////////////////////////////////////
// list of valid types to allow generic code in *.cpp files 
// DEPRECATED (10/10/2026)

//namespace bitgraph {
//
//	template class  Base_Graph_EW<ugraph, int>;
//	template class  Base_Graph_EW<ugraph, double>;
//	//template class  Graph_EW<ugraph, int>;
//	//template class  Graph_EW<ugraph, double>;
//
//	//other specializations... (sparse_graph)
//
//} //namespace bitgraph

////////////////////////////////////////////
