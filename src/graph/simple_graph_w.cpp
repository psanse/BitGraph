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
   * This code is part of the GRAPH 1.0 C++ library
   *
   **/

#include "graph/simple_graph_w.h"

#include "bitscan/bitscan.h"
#include "graph/formats/dimacs_format.h"			
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"
#include <fstream>
#include <iostream>
#include "graph_globals.h"
				
using namespace std;
using namespace bitgraph;

//globals
//const int DEFAULT_WEIGHT_MODULUS = 200;     //for modulus weight generation  [Pullman 2008]		

/////////////////////////////////////////////////
template<class Graph_t, class W>
const W Base_Graph_W <Graph_t, W >::NO_WEIGHT = -1;		

template<class Graph_t, class W>
const W Base_Graph_W <Graph_t, W >::ZERO_WEIGHT = 0;

template<class Graph_t, class W>
const W Base_Graph_W <Graph_t, W >::DEFAULT_WEIGHT = 1.0;												
/////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Necessary implementation of template methods in header file

template<class Graph_t, class W>
void Base_Graph_W<Graph_t, W>::complement_weights()
{
	auto NV = size();

	for (auto v = 0; v < NV; ++v) {
		if (weight(v) != NO_WEIGHT) {
			w_[v] = - w_[v];	
		}
	}
}

template<class Graph_t, class W>
int Base_Graph_W<Graph_t, W>::create_complement(Base_Graph_W<Graph_t, W>& g) const {

	g.name(this->name());
	g.path(this->path());
	g.weight() = w_;
		
	////////////////////////////////////////
	return g_.create_complement(g.graph());
	////////////////////////////////////////
}

template<class Graph_t, class W>
Base_Graph_W<Graph_t, W>::Base_Graph_W(vector<W>& lw) : w_(lw) {

	if (g_.reset(lw.size()) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::Base_Graph_W");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}
}

template<class Graph_t, class W>
Base_Graph_W<Graph_t, W>::Base_Graph_W(std::string filename){	
	
	if (read_dimacs(filename) == -1) {
		LOG_ERROR("error reading DIMACS file -Base_Graph_W<Graph_t, W>::Base_Graph_W");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}
}

template<class Graph_t, class W>
 int Base_Graph_W<Graph_t, W>::set_modulus_weight(int MODE){

	const std::size_t NV = g_.number_of_vertices();

	w_.clear();
	w_.reserve(NV);
		
	for (std::size_t i = 0; i < NV; i++) {
		w_.emplace_back((i + 1) % MODE + 1);
	}

	return 0;
}


template<class Graph_t, class W>
bool Base_Graph_W<Graph_t, W>::is_unit_weighted()
{
	for (W v : w_) {
		if (v != 1.0) return false;
	}
	return true;
}

template<class Graph_t, class W>
int Base_Graph_W<Graph_t, W>::reset(std::size_t NV, W val, string name)
{
	if (g_.reset(NV) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::reset");
		return -1;
	}

	try {
		w_.assign(NV, val);
	}
	catch (...) {
		LOG_ERROR("bad weight assignment - Base_Graph_W<Graph_t, W>::reset");
		return -1;
	}

	g_.name(name);

	return 0;
}

template <class Graph_t, class W>
int	Base_Graph_W<Graph_t,W >::set_weight (vector<W>& lw){

	//assert
	if( g_.number_of_vertices() != lw.size() ){
		LOG_ERROR ("bizarre number of weights - Base_Graph_W<Graph_t,W >::set_w");
		LOG_ERROR ("weights remain unchanged");
		return -1;
	}

	w_ = lw;
	return 0;
}

template <class Graph_t, class W>
W Base_Graph_W<Graph_t, W>::maximum_weight(int& v) const{

	auto it = std::max_element(w_.begin(), w_.end());
	v = it - w_.begin();
	return *it;
}

///////////////
//
// I/O operations
//
//////////////

template<class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::write_dimacs(ostream& o) {
	
	//timestamp comment
	g_.timestamp_dimacs(o);
	
	//name comment
	g_.name_dimacs(o);
	
	//dimacs header - recompute edges
	g_.header_dimacs(o, false);
		
	//write DIMACS nodes n <v> <w>
	const int NV = g_.number_of_vertices();
	for (int v = 0; v < NV; ++v ) {
		o << "n " << v + 1 << " " << weight(v) << endl;
	}
	
	//write undirected edges (1-based vertex notation dimacs)
	for (std::size_t v = 0; v < NV; ++v) {
		for (std::size_t w = 0; w < NV; ++w) {
			if (v == w) continue;
			if (g_.is_edge(v, w)) {									//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << endl;			//1 based vertex notation dimacs
			}
		}
	}

	return o;
}


template<class Graph_t, class W>
int Base_Graph_W<Graph_t, W>::read_dimacs (string filename, int type){
			
	std::string line;
	
	fstream f(filename.c_str());
	if(!f){
		LOGG_ERROR("error when reading file ", filename, " in DIMACS format - Base_Graph_W<Graph_t, W>::read_dimacs");
		reset();
		return -1;
	}

	//read header
	int nV = -1, nEdges = -1;
	if(gio::dimacs::read_dimacs_header(f, nV, nEdges) == -1){
		reset(); 
		f.close();
		return -1;
	}	
	
	//allocates memory for the graph, assigns default unit weights
	if (reset(nV) == -1) {
		reset();
		f.close();
		return -1;
	}

	//skips empty lines
	gio::skip_empty_lines(f);
	
	//////////////
	//read vertex weights format <n> <vertex index> <weight> if they exist
	int v1 = -1, v2 = -1;
	W wv = -1;
	char c;
	c = f.peek();
	switch (c) {
	case 'n':
	case 'v':						// 'v' format used by Zavalnij in evil_W benchmark 

		for (int n = 0; n < nV; ++n) {
			f >> c >> v1 >> wv;
			
			//assert
			if (f.bad()) {
				LOG_ERROR("error when reading vertex-weights - Base_Graph_W<Graph_t, W>::read_dimacs");
				reset();
				f.close();
				return -1;
			}

			//non-positive vertex-weight check
			if (wv <= 0.0) {
				LOGG_WARNING("non-positive weight read: ", wv, "- Base_Graph_W<Graph_t, W>::read_dimacs");
			}

			////////////////////
			w_[v1 - 1] = wv;
			////////////////////
						
			std::getline(f, line);  //remove remaining part of the line
		}

		//skip empty lines
		gio::skip_empty_lines(f);

		break;
	default:
		LOGG_DEBUG("Bad weights in file ", filename, " setting unit weights - Base_Graph_W<Graph_t, W>::read_dimacs");
	}
			
	//read weights from external files if necessary 
	//( @date 9/10/16, the use of additional weight files is deprecated now (26/09/23) )
	if (w_.empty()) {
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
	if (c != 'e') {
		LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<Graph_t, W>::read_dimacs");
		reset();
		f.close();
		return -1;
	}

	std::getline(f, line);
	stringstream sstr(line);
	int nw = _count::number_of_words (line /*sstr.str()*/);

	//assert
	if(nw != 3){
		LOGG_ERROR ("Wrong edge format reading first edge line - Base_Graph_W<Graph_t, W>::read_dimacs");
		reset();
		f.close();
		return -1;
	}
	
	//parse the first edge
	if(nw == 3){
		sstr >> c >> v1 >> v2;	
		g_.add_edge(v1 - 1,v2 - 1);
	}
	
	//remaining edges
	for(int e = 1; e < nEdges; ++e){
		f >> c;
		if(c != 'e' || f.bad()){
			LOG_ERROR("Wrong edge format reading edges - Base_Graph_W<Graph_t, W>::read_dimacs");
			reset();
			f.close();
			return -1;
		}
		//add bidirectional edge	
		f >> v1 >> v2;
		g_.add_edge(v1 - 1,v2 - 1);
			
		std::getline(f, line);  //remove remaining part of the line
	}
	f.close();
	
	//set name 
	g_.name(filename);
		
	return 0;
}

template<class Graph_t, class W>
int Base_Graph_W<Graph_t, W>::read_weights(string filename) {
	
	////////////////////////////////
	ifstream f(filename.c_str());
	////////////////////////////////

	//assert
	if (!f) {
		LOGG_WARNING("Weight file ", filename, "could not be found - Base_Graph_W<Graph_t, W>::read_weights");
		return -1;
	}

	//debugging IO
	LOGG_DEBUG("reading vertex weights from: ", filename, "- Base_Graph_W<Graph_t, W>::read_weights");

	//allocation of memory for weights
	auto NV = g_.number_of_vertices();
	w_.clear();
	w_.reserve(NV);

	//reads weights
	double w = -1.0;
	for (std::size_t i = 0; i < NV; ++i) {
		f >> w;
		if (f.fail()) {
			LOGG_ERROR("bad reading of weights in:", filename, "- Base_Graph_W<Graph_t, W>::read_weights");
			w_.clear();
			return -1;
		}
		//////////////
		w_[i] = w;		
		//////////////
	}

	/////////////////
	f.close();
	////////////////

	return 0;
}

template<class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::print_data(bool lazy, std::ostream& o, bool endl) {
	g_.print_data(lazy, o, false);
	o << " [type:w]";								//adds tag to indicate it is weighted		
	if (endl) { o << std::endl; }
	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::print_weights (com::stack<int>& lv, ostream& o) const{

	for(auto i = 0; i < lv.nE_; ++i){
		o << "[" << lv.at(i) << ":(" << w_[lv.at(i)] << ")] ";
	}
	o << "(" << lv.nE_ << ")" <<endl;
	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::print_weights (int* lv, int NV, ostream& o) const{

	for(auto i = 0; i < NV; ++i){
		o << "[" << lv[i] << ":(" << w_[lv[i]] << ")] ";
	}
	o << "(" << NV << ")" << endl;
	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::print_weights (com::stack<int>& lv, const vint& mapping, ostream& o) const{

	for(auto i = 0; i < lv.nE_; ++i){
		o << "[" << mapping[lv.at(i)] << ":(" << w_[mapping[lv.at(i)]] << ")] ";
	}
	o << "(" << lv.nE_ << ")" << endl;
	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::print_weights (vint& lv, ostream& o) const{
	
	for(auto i = 0; i < lv.size(); i++){
		o << "[" << lv[i] << ":(" << w_[lv[i]] << ")] ";
	}
	o << "(" << lv.size() << ")" << endl;
	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::print_weights (_bbt& bbsg, ostream& o) const{
	
	bbsg.init_scan(bbo::NON_DESTRUCTIVE);										/* CHECK sparse graphs */
	while(true){
		int v = bbsg.next_bit();
		if(v == EMPTY_ELEM) break;
		o << "[" << v << ":(" << w_[v] << ")] ";
	}
	o << "(" << bbsg.size() << ")" << endl;
	return o;
}

template <class Graph_t, class W>
ostream& Base_Graph_W<Graph_t, W>::print_weights (ostream& o, bool show_v) const{

	const std::size_t NV = g_.number_of_vertices();

	if(show_v){
		for(std::size_t i = 0; i < NV; ++i){
			o << "[" << i << ":(" << w_[i] << ")] ";
		}
		o << endl;
	}else{
		_stl::print_collection<vector<W>>(w_, o, true);
	}
	return o;
}

///////////////////
// Specialization for undirected graphs

template<class W>
ostream& Graph_W<ugraph, W>::write_dimacs(ostream& o) {

	//timestamp comment
	ptype::g_.timestamp_dimacs(o);

	//name comment
	ptype::g_.name_dimacs(o);

	//dimacs header - recompute edges
	ptype::g_.header_dimacs(o, false);

	//write DIMACS nodes n <v> <w>
	const int NV = ptype::g_.number_of_vertices();
	for (int v = 0; v < NV; ++v) {
		o << "n " << v + 1 << " " << ptype::weight(v) << endl;
	}

	//write directed edges (1-based vertex notation dimacs)
	for (std::size_t v = 0; v < NV - 1; ++v) {
		for (std::size_t w = v + 1; w < NV; ++w) {
			if (ptype::g_.is_edge(v, w))							//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << endl;
		}
	}

	return o;
}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

namespace bitgraph {
	namespace _impl {

		template class  Base_Graph_W<ugraph, int>;
		template class  Base_Graph_W<ugraph, double>;
		template class  Graph_W<ugraph, int>;
		template class  Graph_W<ugraph, double>;

		//other specializations... (sparse_graph)

	} // namespace _impl
} // namespace bitgraph

////////////////////////////////////////////




