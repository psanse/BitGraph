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
void Base_Graph_EW<GraphT, WeightT>::validate_weight_matrix(
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
	validate_weight_matrix(edge_weights);
	edge_weights_ = edge_weights;
}

template<class GraphT, class WeightT>
void Base_Graph_EW<GraphT, WeightT>::set_modulus_weights(int modulus)
{
	if (modulus <= 0) {
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

//template<class GraphT, class WeightT>
//template <bool EraseNonEdges>
//void Base_Graph_EW<GraphT, WeightT>::set_modulus_edge_weights(int MODULUS) {
//
//	int NV = num_vertices();
//
//	for (int v = 0; v < NV; ++v) {
//		for (int w = 0; w < NV; ++w) {
//
//			if (graph_.is_edge(v, w)) {
//
//				///////////////////////////////////////
//				set_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
//				///////////////////////////////////////
//			}
//			else {
//				if (EraseNonEdges) {
//					set_edge_weight(v, w, NO_WEIGHT);
//				}
//			}
//		}
//	}
//}

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
	
	edge_weights_[v][w] = weight;
}

///////////////
// I/O

template<class GraphT, class WeightT>
int Base_Graph_EW<GraphT, WeightT>::read_dimacs (const std::string& filename)
{
		
	std::string line;	
		
	std::ifstream f(filename);
	if(!f){
		LOGG_ERROR("error when reading file ", filename,
			" in DIMACS format - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		reset();
		return -1;
	}

	//read header
	 int nV = -1, nEdges = -1;
	 if (io::detail::dimacs::read_dimacs_header (f, nV, nEdges) == -1)
	 {
		 LOG_ERROR("error when reading dimacs header / allocation - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		 reset();		
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
		 return -1;
	 } 
	 next = static_cast<char>(c);
	 

	 if (next != 'e') {
		 LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<GraphT, WeightT>::read_dimacs");
		 reset();		
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
	 return 0;
}

template<class GraphT, class WeightT>
std::ostream& Base_Graph_EW<GraphT, WeightT>::write_dimacs (std::ostream& os) const
{
	const int NV = num_vertices();

	//timestamp 
	graph_.timestamp_dimacs(os);
	
	//name
	graph_.name_dimacs(os);

	//dimacs header - recompute edges
	graph_.header_dimacs(os, false);
		
	// Directed edges and edge weights.
	for(vertex_t v = 0; v < NV; ++v){
		for(vertex_t w = 0; w < NV; ++w){

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
std::ostream& Base_Graph_EW<GraphT, WeightT>::print_edge_weights(
	vertex_set_t& vertices, 
	std::ostream& os) const
{
	const auto vertex_count = vertices.size();

	for (std::size_t i = 0; i < vertex_count; ++i) {
		for (std::size_t j = 0; j < vertex_count; j++) {

			const int v = vertices[i];
			const int w = vertices[j];

			if (edge_weights_[static_cast<std::size_t>(v)]
							 [static_cast<std::size_t>(w)] != NO_WEIGHT) {

				
				os  << "[" << v << "->" << w 
				    << " (" << edge_weights_[static_cast<std::size_t>(v)]
											[static_cast<std::size_t>(w)]
					<< ")] " << '\n';
				
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
ostream& Base_Graph_EW<GraphT, WeightT>::print_data (bool lazy, std::ostream& o, bool eofl) {
	graph_.print_data(lazy, o, false);
	o << " [type:ew]";								//"WeightT" for vertex-weighted graphs in GRAPH lib
	if (eofl) { o << std::endl; }
	return o;
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
		   << edge_weights_[v][w] 
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
