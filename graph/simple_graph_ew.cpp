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

//works when NDEBUG is undefined - (normally when in compiler DEBUG mode)
#include <cassert>

using namespace std;	

/////////////////////////////////////////////////
template<class Graph_t, class W>
const W Base_Graph_EW <Graph_t, W >::NO_WEIGHT =  -1;					// or 0x1FFFFFFF (best value for empty weight?)	

template<class Graph_t, class W>
const W Base_Graph_EW <Graph_t, W >::ZERO_WEIGHT = 0;
/////////////////////////////////////////////////



template<class Graph_t, class W>
 int Base_Graph_EW<Graph_t, W>::reset(std::size_t NV, W val, string name)
{
	if (g_.reset(NV) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::reset");
		return -1;
	}

	try {
		we_.assign(NV, vector<W>(NV, val));
	}
	catch (...) {
		LOG_ERROR("bad weight assignment - Base_Graph_EW<Graph_t, W>::reset");
		return -1;
	}

	g_.name(name);

	return 0;	
}

 template<class Graph_t, class W>
 int Base_Graph_EW<Graph_t, W>::reset_edge_weighted(std::size_t NV, W val, string name)
 {
	 int retVal = reset(NV, val, name);
	 if (retVal != -1) {

		 //sets to NO_WEIGHT the vertex weights
		 for (auto i = 0; i < NV; ++i) {
			 we_[i][i] = NO_WEIGHT;	
		 }
	 }

	 return 0;
 }

template<class Graph_t, class W>
 bool Base_Graph_EW<Graph_t, W>::is_consistent(){

	auto NV = number_of_vertices();

	for (std::size_t i = 0; i < NV; ++i) {
		for (std::size_t j = 0; j < NV; ++j) {
			if ( we_[i][j] == NO_WEIGHT &&  g_.is_edge(i, j) ) {
				LOGG_WARNING ("edge [", i, ", ", j ,")] with def. weight - Base_Graph_EW<Graph_t, W>::is_consistent()");
				return false;
			}
		}
	}
	return true;
}

template<class Graph_t, class W>
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(mat_t& lwe) : we_ (lwe) {

	if (g_.reset(lwe.size()) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::Base_Graph_EW");
		LOG_ERROR("exiting...");
		std::exit(-1);
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
void Base_Graph_EW<Graph_t, W>::add_vertex_weight( W val) {

	auto NV = number_of_vertices();

	for (std::size_t v = 0; v < NV; v++) {
			we_[v][v] = val;
	}	

}

template <class Graph_t, class W>
void Base_Graph_EW<Graph_t, W >::add_edge_weight(int v, int w, W val) {
	
	////////////////
	assert(v != w);
	////////////////

	if (g_.is_edge(v, w)) {
		we_[v][w] = val;
	}
	else {
		LOGG_WARNING("edge-weight cannot be added to a non-edge", "(", v, ",", w, ")", "- Base_Graph_EW<Graph_t,W >::add_edge_weight");
	}
}

template <class Graph_t, class W>
void Base_Graph_EW< Graph_t, W>::add_edge_weight(W val) {
	   
	auto NV = number_of_vertices();

	//set to empty weight the non-edges
	for (int v = 0; v < NV; v++) {
		for (int w = 0; w < NV; w++) {
			if (g_.is_edge(v, w)) {
				we_[v][w] = val;
			}
			else {
				we_[v][w] = NO_WEIGHT;
			}
		}
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
		std::exit(-1);
	}
	
}


template<class Graph_t, class W>
 void Base_Graph_EW<Graph_t, W>::gen_modulus_edge_weights(int MODULUS) {

	auto NV = number_of_vertices();

	//vertex-weights NO_WEIGHT
	add_vertex_weight(NO_WEIGHT);

	for (std::size_t v = 0; v < NV; ++v) {
		for (std::size_t w = 0; w < NV; ++w) {
			
			if (g_.is_edge(v, w)) {

				///////////////////////////////////////
				add_edge_weight(v, w, ( 1 + ( (v + w + 2 /* 0-based index*/) % MODULUS) )   );
				///////////////////////////////////////
			}			
		}
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
	 if (::gio::dimacs::read_dimacs_header (f, nV, nEdges) == -1) {
		 reset();
		 f.close();
		 return -1;
	 }

	 //allocates memory for the graph, assigns default NO_WEIGHT edge-weights
	 if (reset(nV) == -1) {
		 reset();
		 f.close();
		 return -1;
	 }

	 //skips empty lines
	 ::gio::read_empty_lines(f);

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
		 ::gio::read_empty_lines(f);

		 break;
	 default:
		 LOGG_DEBUG("missing vertex-weights in file ", filename, " setting unit weights - Base_Graph_EW<Graph_t, W>::read_dimacs");
	 }
	 	
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
	 int nw = ::com::counting::number_of_words(line);
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
		 add_edge_weight (v1 - 1, v2 - 1, we);
	 }
	 else {	

		 ///////////////////////
		 sstr >> c >> v1 >> v2;
		 ///////////////////////

		 g_.add_edge(v1 - 1, v2 - 1);
		 add_edge_weight (v1 - 1, v2 - 1, NO_WEIGHT);				//no edge-weights in file - set NO_WEIGHT value
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
			 add_edge_weight (v1 - 1, v2 - 1, we);
		 }
		 else {
			 ///////////////
			 f >> v1 >> v2;
			 ///////////////

			 g_.add_edge(v1 - 1, v2 - 1);
			 add_edge_weight (v1 - 1, v2 - 1, NO_WEIGHT);			//no edge-weights in file - set NO_WEIGHT value
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
	for (std::size_t v = 0; v < NV; ++v) {
		if (we_[v][v] != NO_WEIGHT) {
			o << "n " << v + 1 << " " << we_[v][v] << std::endl;
		}
	}
		
	//write (directed) edges and edge-weights
	for(std::size_t v = 0; v < NV; ++v){
		for(std::size_t w = 0; w < NV; ++w){
			if (v != w && g_.is_edge(v, w)) {										
				o << "e " << v + 1 << " " << w + 1 << we_[v][w] << std::endl;			//1-based vertex notation dimacs format	
			}
		}
	}

	return o;
}


template <class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_weights (ostream& o, bool line_format, bool only_vertex_weights) const{

	auto NV = number_of_vertices();

	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		if (line_format) {
			for (std::size_t i = 0; i < NV; ++i) {
				for (std::size_t j = 0; j < NV; ++j) {
					if (we_[i][j] != Base_Graph_EW<Graph_t, W>::NO_WEIGHT) {
						o << "[" << i << "->" << j << " (" << we_[i][j] << ")] " << endl;
					}
				}
			}
		}
		else {								//outputs to stream edge-weights in matrix form
			for (std::size_t i = 0; i < NV; ++i) {
				for (std::size_t j = 0; j < NV; ++j) {
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
	}
	else {	
		//streams vertex weights (weights in self-loops)
		for (std::size_t v = 0; v < NV; v++) {
			if (we_[v][v] != NO_WEIGHT) {
				o << "[" << v << " (" << we_[v][v] << ")] " << endl;
			}
		}
	}

	o << "**********************************************************" << endl;

	return o;
}
template <class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_weights (vint& lv, ostream& o, bool only_vertex_weights) const{

	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		for (std::size_t i = 0; i < lv.size(); ++i) {
			for (std::size_t j = 0; j < lv.size(); j++) {
				if (we_[lv[i]][lv[j]] != Base_Graph_EW<Graph_t, W>::NO_WEIGHT) {
					o << "[" << lv[i] << "->" << lv[j] << " (" << we_[lv[i]][lv[j]] << ")] " << endl;
				}
			}
		}
	}
	else {
		//streams vertex weights (weights in self-loops)
		for (std::size_t i = 0; i < lv.size(); i++) {
			if (we_[lv[i]][lv[i]] != NO_WEIGHT) {
				o << "[" << lv[i] << " (" << we_[lv[i]][lv[i]] << ")] " << endl;
			}
		}
	}

	o << "**********************************************************" << endl;

	return o;
}

template<class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_data (bool lazy, std::ostream& o, bool endl) {
	g_.print_data(lazy, o, false);
	o << " [type:ew]";								//"w" for vertex-weighted graphs in GRAPH lib
	if (endl) { o << std::endl; }
	return o;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_edges(std::ostream& o, bool eofl) {

	const int NV = g_.number_of_vertices();
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (is_edge(i, j)) {
				o << "[" << i << "]" << "-(" << weight(i, j) << ")->" << "[" << j << "]" << endl;
			}
			if (is_edge(j, i)) {
				o << "[" << j << "]" << "-(" << weight(i, j) << ")->" <<  "[" << i << "]" << endl;
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
int Graph_EW<ugraph, W>::create_complement(Graph_EW<ugraph, W>& g) const {

	g.name(this->name());
	g.path(this->path());
	g.weights() = ptype::we_;
	ptype::g_.create_complement(g.graph());

	return 0;
}

template<class W>
std::ostream& Graph_EW<ugraph, W>::print_edges(std::ostream& o, bool eofl) 
{
	// TODO: Insertar una instrucción "return" aquí
	const int NV = g_.number_of_vertices();	
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (is_edge(i, j)) {
				o << "[" << i << "]" << "-" << weight(i, j)  << "->" << "[" << j << "]" << endl;
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
	g_.add_edge(v, w);

	//sets both weights
	we_[v][w] = val;
	we_[w][v] = val;
}

template <class W>
void Graph_EW< ugraph, W >::add_edge_weight(int v, int w, W val) {

	//////////////////
	assert(v != w);
	//////////////////

	if (ptype::g_.is_edge(v, w)) {
		ptype::we_[v][w] = val;
		ptype::we_[w][v] = val;
	}
	else {
		LOGG_WARNING("edge-weight cannot be added to a non-edge", "(", v, ",", w, ")", "- Graph_EW<Graph_t,W >::add_edge_weight");
	}

	/*if (v == w) {
		ptype::we_[v][v] = val;
	}
	else if (ptype::g_.is_edge(v, w)) {
		ptype::we_[v][w] = val;
		ptype::we_[w][v] = val;
	}
	else {
		LOGG_DEBUG	("bizarre petition to add weight to the non-edge", "(", v, ",", w, ")", " - Graph_EW<Graph_t,W >::add_edge_weight");
		LOG_DEBUG	("weight not added");
		return -1;
	}*/

	//return 0;
}

template <class W>
void Graph_EW< ugraph, W >::add_edge_weight(W val) {

	auto NV = ptype::number_of_vertices();

	//set to empty wv and non-edges UPPER-T
	for (auto v = 0; v < NV - 1; v++) {
		for (auto w = v + 1; w < NV; w++) {
			if (ptype::g_.is_edge(v, w)) {
				ptype::we_[v][w] = val;
				ptype::we_[w][v] = val;
			}
			else {
				ptype::we_[v][w] = ptype::NO_WEIGHT;
				ptype::we_[w][v] = ptype::NO_WEIGHT;
			}
		}
	}

	//vertex weights
	/*for (int v = 0; v < NV; v++) {
		ptype::we_[v][v] = val;
	}*/
}

template <class W>
template<bool Erase>
void Graph_EW< ugraph, W >::add_edge_weight(typename Graph_EW<ugraph, W>::mat_t& lw) {

	auto NV = ptype::number_of_vertices();

	/////////////////////////
	assert(lw.size() == NV);
	/////////////////////////

	////assert
	//if (lw.size() != NV) {
	//	LOG_ERROR("bizarre matrix of weights - Graph_EW< ugraph, W >::add_edge_weight");
	//	LOG_ERROR("weights remain unchanged");
	//	return -1;
	//}

	//sets edge-weights
	for (auto v = 0; v < NV - 1; ++v) {
		for (auto w = v + 1; w < NV; ++w) {
			if (ptype::g_.is_edge(v, w)) {
				ptype::we_[v][w] = lw[v][w];
				ptype::we_[w][v] = lw[w][v];
			}
			else {
				if (Erase) {
					ptype::we_[v][w] = ptype::NO_WEIGHT;
					ptype::we_[w][v] = ptype::NO_WEIGHT;
				}				
			}
		}
	}

	//vertex weights
	/*for (int v = 0; v < NV; v++) {
		ptype::we_[v][v] = lw[v][v];
	}*/

}

template<class W>
void Graph_EW<ugraph, W>::gen_modulus_edge_weights(int MODULUS)
{
	auto NV = number_of_vertices();

	//vertex-weights NO_WEIGHT
	add_vertex_weight(this->NO_WEIGHT);

	//sets weights of undirected edges
	for (std::size_t v = 0; v < NV - 1; ++v) {
		for (std::size_t w = v + 1; w < NV; ++w) {
			if (ptype::g_.is_edge(v, w)) {

				///////////////////////////////////////
				add_edge_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
				///////////////////////////////////////

			}			
		}
	}
}

template<class W>
void Graph_EW<ugraph, W>::gen_random_edges(double p, W val)
{

	const int NV = g_.number_of_vertices();

	//removes all edges
	g_.remove_edges();

	//sets directed edges with probability p
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {
						
			if (::com::rand::uniform_dist(p)) {
				add_edge(i, j, val);					
			}
		}
	}

}

template <class W>
ostream& Graph_EW<ugraph, W>::print_weights (ostream& o, bool line_format, bool only_vertex_weights) const{

	auto NV = ptype::number_of_vertices();
	
	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		if (line_format) {
			for (auto v = 0; v < NV; v++) {
				if (ptype::we_[v][v] != ptype::NO_WEIGHT) {
					o << "[" << v << " (" << ptype::we_[v][v] << ")] " << endl;
				}
			}

			for (auto i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (ptype::we_[i][j] != ptype::NO_WEIGHT) {
						o << "[" << i << "-" << j << " (" << ptype::we_[i][j] << ")] " << endl;
					}
				}
			}
		}
		else {																			//outputs to stream edge-weights in matrix form
			for (auto i = 0; i < NV; ++i) {
				for (auto j = i; j < NV; ++j) {
					if (ptype::we_[i][j] != ptype::NO_WEIGHT) {
						o << "[" << i << "-" << j << " (" << ptype::we_[i][j] << ")] ";
					}
					else {
						o << "--" << " ";
					}
				}
				o << endl;
			}
		}
	}else{
		//streams vertex weights (weights in self-loops)
		for (auto v = 0; v < NV; v++) {
			if (ptype::we_[v][v] != ptype::NO_WEIGHT) {
				o << "[" << v << " (" << ptype::we_[v][v] << ")] " << endl;
			}
		}
	}

	o << "**********************************************************" << endl;

	return o;
}

template <class W>
ostream& Graph_EW<ugraph, W>::print_weights (vint& ln, ostream& o, bool only_vertex_weights) const{
	   
	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		for (auto i = 0; i < ln.size(); i++) {
			if (ptype::we_[ln[i]][ln[i]] != ptype::NO_WEIGHT) {
				o << "[" << ln[i] << " (" << ptype::we_[ln[i]][ln[i]] << ")] " << endl;
			}
		}

		for (auto i = 0; i < ln.size() - 1; ++i) {
			for (auto j = i + 1; j < ln.size(); ++j) {
				if (ptype::we_[ln[i]][ln[j]] != ptype::NO_WEIGHT) {
					o << "[" << ln[i] << "-" << ln[j] << " (" << ptype::we_[ln[i]][ln[j]] << ")] " << endl;
				}
			}
		}
	}
	else {
		//streams vertex weights (weights in self-loops)
		for (auto i = 0; i < ln.size(); ++i) {
			if (ptype::we_[ln[i]][ln[i]] != ptype::NO_WEIGHT) {
				o << "[" << ln[i] << " (" << ptype::we_[ln[i]][ln[i]] << ")] " << endl;
			}
		}
	}
	o << "**********************************************************" << endl;

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
	for (std::size_t v = 0; v < NV; ++v) {
		if (ptype::we_[v][v] != ptype::NO_WEIGHT) {
			o << "n " << v + 1 << " " << ptype::we_[v][v] << endl;
		}
	}

	//write undirected edges and edge weights
	for(std::size_t v = 0; v < NV - 1; ++v){
		for(std::size_t w = v + 1; w < NV; ++w){
			if (ptype::g_.is_edge(v, w)) {														//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << " " << ptype::we_[v][w] << endl;			//1-based vertex notation dimacs
			}
		}
	}

	return o;
}


template<class Graph_t, class W>
void Base_Graph_EW<Graph_t, W>::gen_random_edges(double p, W val)
{
	const int NV = g_.number_of_vertices();

	//removes all edges
	g_.remove_edges();

	//sets directed edges with probability p
	for (auto i = 0; i < NV-1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			//considers both directed edges separately
			if (::com::rand::uniform_dist(p)) {
				add_edge(i, j, val);				
			}
			if (::com::rand::uniform_dist(p)) {
				add_edge(j, i, val);
			}
		}
	}
}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

template class  Base_Graph_EW<ugraph, int>;
template class  Base_Graph_EW<ugraph, double>;
template class  Graph_EW<ugraph, int>;
template class  Graph_EW<ugraph, double>;

//other specializations... (sparse_graph)


////////////////////////////////////////////
