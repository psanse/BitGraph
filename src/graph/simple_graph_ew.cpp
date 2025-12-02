 /**
   * @file simple_graph_ew.cpp
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
#include "graph/formats/dimacs_format.h"
#include "graph/simple_graph_ew.h"	
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"
#include <fstream>
#include <iostream>
#include <cassert>							//NDEBUG should be undefined - (normally in compiler DEBUG mode)
#include "graph_globals.h"

using namespace std;	
using namespace bitgraph;

/////////////////////////////////////////////////
template<class Graph_t, class W>
const W Base_Graph_EW <Graph_t, W >::NO_WEIGHT =  -1;					// or 0x1FFFFFFF (best value for empty weight?)	

template<class Graph_t, class W>
const W Base_Graph_EW <Graph_t, W >::ZERO_WEIGHT = 0;
/////////////////////////////////////////////////

template<class Graph_t, class W>
 bool Base_Graph_EW<Graph_t, W>::is_consistent(){

	auto NV = number_of_vertices();

	for (auto i = 0; i < NV; ++i) {
		for (auto j = 0; j < NV; ++j) {

			if ( we_[i][j] == NO_WEIGHT &&  g_.is_edge(i, j) ) {
				LOGG_WARNING ("edge [", i, ", ", j ,")] with NO_WEIGHT - Base_Graph_EW<Graph_t, W>::is_consistent()");
				return false;
			}

		}
	}
	return true;
}

 template<class Graph_t, class W>
 Base_Graph_EW<Graph_t, W>::Base_Graph_EW(int NV, W val, bool edge_weighted)
 {
	 if (edge_weighted) {

		if (reset<true>(NV, val) == -1) {
			LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::Base_Graph_EW");
			LOG_ERROR("exiting...");
			std::exit(EXIT_FAILURE);
		}
	}
	else {

		if (reset<false>(NV, val) == -1) {
			LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::Base_Graph_EW");
			LOG_ERROR("exiting...");
			std::exit(EXIT_FAILURE);
		}
	}
 }

 template<class Graph_t, class W>
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(string filename){
	read_dimacs	(filename);							
}


template<class Graph_t, class W>
void Base_Graph_EW<Graph_t, W>::add_edge(int v, int w, W val)
{
	g_.add_edge(v, w);
	we_[v][w] = val;	
}

template<class Graph_t, class W>
void Base_Graph_EW<Graph_t, W>::set_vertex_weight( W val) {

	auto NV = number_of_vertices();

	for (int v = 0; v < NV; v++) {
			we_[v][v] = val;
	}	

}

template <class Graph_t, class W>
void Base_Graph_EW<Graph_t, W >::set_weight(int v, int w, W val) {
	
	////////////////
	assert(v != w);
	////////////////

	bool is_edge = g_.is_edge(v, w);	

	if (is_edge || (val == Base_Graph_EW<Graph_t, W >::NO_WEIGHT && !is_edge)) {
		we_[v][w] = val;
	}
	else {
		LOGG_WARNING("attempting to set an edge-weight to a non-edge", "(", v, ",", w, ")",
						 "- Base_Graph_EW<Graph_t,W >::set_weight"							);		
	}
}



template<class Graph_t, class W>
 void Base_Graph_EW<Graph_t, W>::complement_weights(int type){

	auto NV = number_of_vertices();	

	switch (type) {
	case EDGE:
		for (auto i = 0; i < NV-1; ++i) {
			for (auto j = i + 1; j < NV; ++j) {
				if (we_[i][j] != NO_WEIGHT) {

					////////////////////////
					we_[i][j] = - we_[i][j];
					////////////////////////
				}
			}
		}
		break;
	case VERTEX:
		for (auto i = 0; i < NV; ++i) {
			if (we_[i][i] != NO_WEIGHT) {

				////////////////////////
				we_[i][i] = - we_[i][i];
				////////////////////////
			}
		}	
		break;
	case BOTH:
		//vertex and edge-weights
		for (auto i = 0; i < NV; ++i) {
			for (auto j = 0; j < NV; ++j) {
				if (we_[i][j] != NO_WEIGHT) {
					//////////////////////////
					we_[i][j] = - we_[i][j];
					///////////////////////////
				}
			}
		}
		break;
	default:	
		LOG_ERROR("unknown type -  Base_Graph_EW<Graph_t, W>::complement_weights");
		LOG_ERROR("exiting");
		std::exit(EXIT_FAILURE);
	}
	
}




template<class Graph_t, class W>
vecw<W> Base_Graph_EW<Graph_t, W>::vertex_weights()  const {

	auto NV = number_of_vertices();

	vector<W> res;
	res.reserve(NV);
	for (auto v = 0; v < NV; ++v) {
		res.emplace_back(we_[v][v]);
	}

	return res;
}

///////////////
//
// I/O
//
//////////////

template<class Graph_t, class W>
int Base_Graph_EW<Graph_t, W>::read_dimacs (string filename){
		
	string line;	
		
	fstream f(filename.c_str());
	if(!f){
		LOGG_ERROR("error when reading file ", filename, " in DIMACS format - Base_Graph_EW<Graph_t, W>::read_dimacs");
		reset();
		return -1;
	}

	//read header
	 int nV = -1, nEdges = -1;
	 if (gio::dimacs::read_dimacs_header (f, nV, nEdges) == -1 ||
		 reset(nV) == -1	)  //allocates memory for the graph, default NO_WEIGHT edge-weights
	 {
		 LOG_ERROR("error when reading dimacs header / allocation - Base_Graph_EW<Graph_t, W>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }

	
	/* if (reset(nV) == -1) {
		 reset();
		 f.close();
		 return -1;
	 }*/

	 //skips empty lines
	 gio::skip_empty_lines(f);

	 //////////////
	//read vertex-weights format <n> <vertex index> <weight> if they exist
	 int v1 = -1, v2 = -1;
	 W wv = NO_WEIGHT;
	 char c;
	 c = f.peek();
	 switch (c) {
	 case 'n':
	 case 'v':									// 'v' format used by Zavalnij in evil_W benchmark 

		 for (int n = 0; n < nV; ++n) {
			 f >> c >> v1 >> wv;

			 //assert
			 if (f.bad()) {
				 LOG_ERROR("error when reading vertex-weights - Base_Graph_EW<Graph_t, W>::read_dimacs");
				 reset();
				 f.close();
				 return -1;
			 }

			 //non-positive vertex-weight check
			 if (wv <= 0.0) {
				 LOGG_WARNING("non-positive weight read: ", wv, "- Base_Graph_EW<Graph_t, W>::read_dimacs");
			 }

			 //////////////////////////
			 we_[v1 - 1][v1 - 1] = wv;
			 //////////////////////////
			 			 
			 std::getline(f, line);  //remove remaining part of the line
		 }

		 //skip empty lines
		 gio::skip_empty_lines(f);

		 break;
	 default:
		 LOGG_DEBUG("missing vertex-weights in file ", filename, " setting unit weights - Base_Graph_EW<Graph_t, W>::read_dimacs");
	 }//end of switch
	 	
	 /////////////////////	
	 //read edges
	 
	 //read the first edge line - 4 tokens expected (e <v> <w> <edge-weight>)
	 c = f.peek();
	 if (c != 'e') {
		 LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<Graph_t, W>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }

	 W we = NO_WEIGHT;
	 std::getline(f, line);
	 stringstream sstr(line);

	 //determine if edge weights are present
	 int nw = _count::number_of_words(line);
	 bool edge_weights_found = true;
	 switch (nw) {
	 case 4:
		 edge_weights_found = true;		//expected: e <v> <w> <edge-weight> 
		 break;
	 case 3:
		 edge_weights_found = false;	//expeceted: e <v> <w>  	
		 break;
	 default:
		 LOG_ERROR("Wrong edge format - Base_Graph_EW<Graph_t, W>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }

	 //parse the first edge
	 if (edge_weights_found) {

		 /////////////////////////////
		 sstr >> c >> v1 >> v2 >> we;
		 ////////////////////////////

		 g_.add_edge(v1 - 1, v2 - 1);
		 set_weight (v1 - 1, v2 - 1, we);
	 }
	 else {	

		 ///////////////////////
		 sstr >> c >> v1 >> v2;
		 ///////////////////////

		 g_.add_edge(v1 - 1, v2 - 1);
		 set_weight (v1 - 1, v2 - 1, NO_WEIGHT);				//no edge-weights in file - set NO_WEIGHT value
	 }
	 
	 //remaining edges
	 for (int e = 1; e < nEdges; ++e) {
		 f >> c;
		 if (c != 'e' || f.bad()) {
			 LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<Graph_t, W>::read_dimacs");
			 reset();
			 f.close();
			 return -1;
		 }

		 //add weighted edge	
		 if (edge_weights_found) {

			 /////////////////////
			 f >> v1 >> v2 >> we;
			 /////////////////////

			 g_.add_edge (v1 - 1, v2 - 1);
			 set_weight (v1 - 1, v2 - 1, we);
		 }
		 else {
			 ///////////////
			 f >> v1 >> v2;
			 ///////////////

			 g_.add_edge(v1 - 1, v2 - 1);
			 set_weight (v1 - 1, v2 - 1, NO_WEIGHT);			//no edge-weights in file - set NO_WEIGHT value
		 }

		 std::getline(f, line);  //remove remaining part of the line
	 }

	 //name (removes path)
	 g_.name(filename);

	 f.close();	

	 return 0;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::write_dimacs (std::ostream& o) {
	
	auto NV = number_of_vertices();

	//timestamp 
	g_.timestamp_dimacs(o);
	
	//name
	g_.name_dimacs(o);

	//dimacs header - recompute edges
	g_.header_dimacs(o, false);

	//write vertex weights (edge weights of self loops are considered vertex weights)
	for (int v = 0; v < NV; ++v) {
		if (we_[v][v] != NO_WEIGHT) {
			o << "n " << v + 1 << " " << we_[v][v] << std::endl;
		}
	}
		
	//write (directed) edges and edge-weights
	for(int v = 0; v < NV; ++v){
		for(int w = 0; w < NV; ++w){
			if (v != w && g_.is_edge(v, w)) {										
				o << "e " << v + 1 << " " << w + 1 << we_[v][w] << std::endl;			//1-based vertex notation dimacs format	
			}
		}
	}

	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_weights (ostream& o, bool line_format, int type) const{

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
		LOG_ERROR("unknown type - Base_Graph_EW<Graph_t, W>::print_weights");
		LOG_ERROR("exiting");
		std::exit(EXIT_FAILURE);
	}
	//o << endl << "**************************" << endl;
	return o;
}
template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_edge_weights(std::ostream& o, bool line_format) const
{
	int NV = g_.size();

	//streams edge-weights 
	if (line_format) {
		print_edges(o, false);
		/*for (auto i = 0; i < NV; ++i) {
			for (auto j = 0; j < NV; ++j) {
				if (we_[i][j] != Base_Graph_EW<Graph_t, W>::NO_WEIGHT) {
					o << "[" << i << "->" << j << " (" << we_[i][j] << ")] " << endl;
				}
			}
		}*/
	}
	else {								//outputs to stream edge-weights in matrix form
		for (auto i = 0; i < NV; ++i) {
			for (auto j = 0; j < NV; ++j) {
				if (we_[i][j] != Base_Graph_EW<Graph_t, W>::NO_WEIGHT) {
					o << we_[i][j] << '\t';
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

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_vertex_weights(std::ostream& o) const
{
	const int NV = g_.size();	

	for (auto v = 0; v < NV; v++) {
		if (we_[v][v] != Base_Graph_EW<Graph_t, W>::NO_WEIGHT) {
			o << "[" << v << ":(" << we_[v][v] << ")] " << endl;
		}
	}
	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_weights (vint& lv, ostream& o, int type) const{

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
		LOG_ERROR("unknown type - Base_Graph_EW<Graph_t, W>::print_weights");
		LOG_ERROR("exiting");
		std::exit(EXIT_FAILURE);
	}
	//o << endl << "**************************" << endl;

	return o;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_edge_weights(vint& lv, std::ostream& o) const
{
	for (auto i = 0u; i < lv.size(); ++i) {
		for (auto j = 0u; j < lv.size(); j++) {
			if (we_[lv[i]][lv[j]] != Base_Graph_EW<Graph_t, W>::NO_WEIGHT) {

				///////////////////////////////////////////////////////////////////////////////////
				o << "[" << lv[i] << "->" << lv[j] << " (" << we_[lv[i]][lv[j]] << ")] " << endl;
				////////////////////////////////////////////////////////////////////////////////////
			}
		}
	}

	return o;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_vertex_weights(vint& lv, std::ostream& o) const
{
	for (auto i = 0u; i < lv.size(); ++i) {
		if (we_[lv[i]][lv[i]] != Base_Graph_EW<Graph_t, W>::NO_WEIGHT) {

			/////////////////////////////////////////////////////////////////
			o << "[" << lv[i] << ":(" << we_[lv[i]][lv[i]] << ")] " << endl;
			/////////////////////////////////////////////////////////////////
		}
	}

	return o;
}

template<class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_data (bool lazy, std::ostream& o, bool eofl) {
	g_.print_data(lazy, o, false);
	o << " [type:ew]";								//"w" for vertex-weighted graphs in GRAPH lib
	if (eofl) { o << std::endl; }
	return o;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_edge(int v, int w, std::ostream& o, bool eofl) const
{
	if (g_.is_edge(v, w)) {
		o << "[" << v << "]--(" << we_[v][w] << ")-->" << "[" << w << "]";
	}
	else {
		o << "[" << v << "]--(NO_EDGE)-->" << "[" << w << "]";
	}

	if (eofl) { o << std::endl; }	
	return o;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_vertex(int v, std::ostream& o, bool eofl) const
{
	o << "[" << v << ":(" << we_[v][v] << ")" << "]"; 
	
	if (eofl) { o << std::endl; }
	return o; 
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_edges(std::ostream& o, bool eofl) const{

	const int NV = g_.number_of_vertices();
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



///////////////////////////
//
// Graph_EW implementation
//
///////////////////////////

template<class W>
std::ostream& Graph_EW<ugraph, W>::print_edges(std::ostream& o, bool eofl)  const {	

	const int NV = ptype::g_.number_of_vertices();	

	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (ptype::g_.is_edge(i, j)) {
				o << "[" << i << "]" << "--(" << ptype::we_[i][j]  << ")-->" << "[" << j << "]" << endl;
			}
		}
	}

	if (eofl) { o << std::endl; }
	return o;
}

template<class W>
void Graph_EW<ugraph, W>::add_edge(int v, int w, W val)
{
	//sets undirected edge
	ptype::g_.add_edge(v, w);

	//sets both weights
	ptype::we_[v][w] = val;
	ptype::we_[w][v] = val;
}

template <class W>
void Graph_EW< ugraph, W >::set_weight(int v, int w, W val) {

	//////////////////
	assert(v != w);
	//////////////////

	auto is_edge = ptype::g_.is_edge(v, w);


	if (is_edge || (val == ptype::NO_WEIGHT && !is_edge)) {
		ptype::we_[v][w] = val;
		ptype::we_[w][v] = val;
	}
	else {
		LOGG_WARNING("edge-weight cannot be added to a non-edge", 
					  "(", v, ",", w, ")", "- Graph_EW<Graph_t,W >::set_weight" );	
	}
	
}

template<class W>
void Graph_EW<ugraph, W>::gen_random_edges(double p, W val){

	const int NV = ptype::g_.number_of_vertices();

	//removes all edges
	ptype::g_.remove_edges();

	//sets directed edges with probability p
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {
						
			if (_rand::uniform_dist(p)) {
				add_edge(i, j, val);					
			}
		}
	}
}

template<class W>
std::ostream& Graph_EW<ugraph, W>::print_edge_weights(std::ostream& o, bool line_format) const
{
	int NV = ptype::g_.size();

	//streams edge-weights 
	if (line_format) {
		print_edges(o, false);		
	}
	else {								//outputs to stream edge-weights in matrix form
		for (auto i = 0; i < NV - 1; ++i) {
			for (auto j = i + 1; j < NV; ++j) {
				if (ptype::we_[i][j] != ptype::NO_WEIGHT) {
					o << ptype::we_[i][j] << '\t';
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

template<class W>
std::ostream& Graph_EW<ugraph, W>::print_edge_weights(vint& lv, std::ostream& o) const
{	
	for (auto i = 0u; i < lv.size() - 1; ++i) {
		for (auto ju = i + 1; j < lv.size(); ++j) {
			if (ptype::we_[lv[i]][lv[j]] != ptype::NO_WEIGHT) {

				/////////////////////////////////////////////////////////////////////////////////////////	
				//o << "[" << lv[i] << "-" << lv[j] << " (" << ptype::we_[lv[i]][lv[j]] << ")] " << endl;
				///////////////////////////////////////////////////////////////////////////////////////////

				///////////////////////////////////////////////////////////////////////////////////////////
				o << "[" << i << "]" << "--(" << ptype::weight(i, j) << ")-->" << "[" << j << "]" << endl;
				///////////////////////////////////////////////////////////////////////////////////////////
			}
		}
	}

	return o;
}


template<class W>
ostream& Graph_EW<ugraph, W>::write_dimacs (ostream& o) {
	
	auto NV = ptype::number_of_vertices();

	//timestamp 
	ptype::g_.timestamp_dimacs(o);
	
	//name
	ptype::g_.name_dimacs(o);

	//dimacs header - recompute edges
	ptype::g_.header_dimacs(o, false);
		
	//write vertex weights
	for (int v = 0; v < NV; ++v) {
		if (ptype::we_[v][v] != ptype::NO_WEIGHT) {
			o << "n " << v + 1 << " " << ptype::we_[v][v] << endl;
		}
	}

	//write undirected edges and edge weights
	for(int v = 0; v < NV - 1; ++v){
		for(int w = v + 1; w < NV; ++w){
			if (ptype::g_.is_edge(v, w)) {														//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << " " << ptype::we_[v][w] << endl;			//1-based vertex notation dimacs
			}
		}
	}

	return o;
}


template<class Graph_t, class W>
int Base_Graph_EW<Graph_t, W>::create_complement(Base_Graph_EW<Graph_t, W>& g) const
{

	g.name(this->name());
	g.path(this->path());
	g.weights() = we_;

	/////////////////////////////////////////
	return g_.create_complement(g.graph());
	/////////////////////////////////////////
}

template<class Graph_t, class W>
void Base_Graph_EW<Graph_t, W>::gen_random_edges(double p, W val)
{
	const int NV = g_.number_of_vertices();

	//removes all edges
	g_.remove_edges();

	//sets directed edges with probability p
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			//considers both directed edges separately
			if (_rand::uniform_dist(p)) {
				add_edge(i, j, val);				
			}
			if (_rand::uniform_dist(p)) {
				add_edge(j, i, val);
			}
		}
	}
}

template<class Graph_t, class W>
void Base_Graph_EW<Graph_t, W>::erase_non_edge_weights() {

	const int NV = g_.number_of_vertices();

	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (!g_.is_edge(i, j)) {
				this->set_weight(i, j, Base_Graph_EW<Graph_t, W>::NO_WEIGHT);
			}
		}
	}
}

template<class Graph_t, class W>
void Base_Graph_EW<Graph_t, W>::make_edge_weighted(bool erase_non_edges)
{
	//sets all vertex weights to NO_WEIGHT
	for (auto v = 0u; v < g_.size(); ++v) {
		we_[v][v] = Base_Graph_EW<Graph_t, W>::NO_WEIGHT;	
	}

	if (erase_non_edges) {
		erase_non_edge_weights();
	}
}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

namespace bitgraph {
	namespace _impl {

		template class  Base_Graph_EW<ugraph, int>;
		template class  Base_Graph_EW<ugraph, double>;
		template class  Graph_EW<ugraph, int>;
		template class  Graph_EW<ugraph, double>;

		//other specializations... (sparse_graph)

} //namespace _impl
} //namespace bitgraph

////////////////////////////////////////////
