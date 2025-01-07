/*
 * simple_graph_ew.cpp implementation file of the classes BaseGraph_EW and Graph_EW 
						for edge-weighted graphs (header simple_graph_ew.h)
 *
 * @created 16/01/2019
 * @last_update 06/01/2025
 * @dev pss
 *
 * @comments see end of file for valid template types
 */

#include "bitscan/bitscan.h"
#include "formats/dimacs_format.h"
#include "simple_graph_ew.h"	
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"
#include <fstream>
#include <iostream>

using namespace std;	


/////////////////////////////////////////////////
template<class Graph_t, class W>
const W Base_Graph_EW <Graph_t, W >::NOWT = 0;					// or 0x1FFFFFFF (best value for empty weight?)									
/////////////////////////////////////////////////


template<class W>
inline
int Graph_EW<ugraph, W>::create_complement (Graph_EW<ugraph, W>& g) const {
	/////////////////////
	//TODO@ name, path info is lost

	g.set_name(this->get_name());
	g.set_path(this->get_path());
	g.get_we() = ptype::we_;	
	ptype::g_.create_complement(g.graph());

	return 0;
}

template<class W>
inline
int Graph_EW<ugraph, W>::create_complement(ugraph& g) const {

	ptype::g_.create_complement(g);
	return 0;
}

template <class W>
inline
int Graph_EW< ugraph, W >::set_we (int v, int w, W val) {

	if (v == w) {
		ptype::we_[v][v] = val;
	}
	else if (g_.is_edge(v, w) ){
		ptype::we_[v][w] = val;
		ptype::we_[w][v] = val;
	}
	else {
		LOG_WARNING ("bizarre petition to add weight to the non-edge", "(", v, ",", w, ")", " - Graph_EW<Graph_t,W >::set_we");
		LOG_WARNING ("weight not added");
		return -1;
	}
		
	return 0;
}

template <class W>
inline
void Graph_EW< ugraph, W >::set_we (W val) {
////////////////////////
// partial specialization for undirected graphs (symmetrical edge weights)
	
	auto NV = ptype::number_of_vertices();

	//set to empty wv and non-edges UPPER-T
	for (int v = 0; v < NV - 1; v++) {
		for (int w = v + 1; w < NV; w++) {
			if (ptype::g_.is_edge(v, w)) {
				ptype::we_[v][w] = val;
				ptype::we_[w][v] = val;
			}
			else {
				ptype::we_[v][w] = ptype::NOWT;
				ptype::we_[w][v] = ptype::NOWT;
			}
		}
	}

	//vertex weights
	for (int v = 0; v < NV; v++) {
		ptype::we_[v][v] = val;
	}
}

template <class W>
inline
int Graph_EW< ugraph, W >::set_we (typename Graph_EW<ugraph, W>::mat_t& lw) {
////////////////////////
// partial specialization for undirected graphs (symmetrical edge weights)
		
	auto NV = ptype::number_of_vertices();

	if (lw.size() != NV) {
		LOG_ERROR("bizarre matrix of weights-Graph_EW< ugraph, W >::set_we(mat_t)");
		return -1;
	}

	//set to empty wv and non-edges UPPER-T
	for (int v = 0; v < NV - 1; v++) {
		for (int w = v + 1; w < NV; w++) {
			if (ptype::g_.is_edge(v, w)) {
				ptype::we_[v][w] = lw[v][w];
				ptype::we_[w][v] = lw[w][v];
			}
			else {
				ptype::we_[v][w] = ptype::NOWT;
				ptype::we_[w][v] = ptype::NOWT;
			}
		}
	}	
	
	//vertex weights
	for (int v = 0; v < NV; v++) {
		ptype::we_[v][v] = lw[v][v];
	}
		   
	return 0;
}


template<class Graph_t, class W>
inline
int Base_Graph_EW<Graph_t, W>::init(int NV, W val, bool reset_name){

	if (g_.reset(NV) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::init");
		return -1;
	}
	
	try {
		we_.assign(NV, vector<W>(NV, val));
	}
	catch (...) {
		LOG_ERROR("bad weight assignment - Base_Graph_EW<Graph_t, W>::init");
		return -1;
	}

	if (reset_name) {
		g_.set_name("");
		g_.set_path("");
	}

return 0;
}


template<class Graph_t, class W>
inline int Base_Graph_EW<Graph_t, W>::reset(std::size_t NV, W val, string name)
{
	if (g_.reset(NV) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::reset");
		return -1;
	}

	try {
		we_.assign(NV, vector<W>(NV, val));
	}
	catch (...) {
		LOG_ERROR("bad weight assignment - Base_Graph_EW<Graph_t, W>::init");
		return -1;
	}

	g_.set_name(name);

	return 0;	
}

template<class Graph_t, class W>
inline bool Base_Graph_EW<Graph_t, W>::is_consistent(){

	auto NV = number_of_vertices();

	for (int i = 0; i < NV; i++) {
		for (int j = 0; j < NV; j++) {
			if ( we_[i][j] == NOWT &&  g_.is_edge(i, j) ) {
				LOG_WARNING	("edge [", i, ", ", j ,")] with def. weight - Base_Graph_EW<Graph_t, W>::is_consistent()");
				return false;
			}
		}
	}
	return true;
}

template<class Graph_t, class W>
inline
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(mat_t& lwe) : we_ (lwe) {

	if (g_.init(lwe.size()) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::Base_Graph_EW");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}


template<class Graph_t, class W>
inline
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(string filename){
	read_dimacs	(filename);							
}


template <class Graph_t, class W>
inline
int Base_Graph_EW<Graph_t, W >::set_we(int v, int w, W val) {
	
	if (v == w || g_.is_edge(v, w)) {		
		we_[v][w] = val;
	}
	else {
		LOG_WARNING ("edge weight cannot be added to the non-edge" , "(" , v , "," , w , ")" , "- Base_Graph_EW<Graph_t,W >::set_we");
		LOG_WARNING ("weight not added");
		return -1;
	}	

	return 0;
}

template <class Graph_t, class W>
inline
void Base_Graph_EW< Graph_t, W>::set_we(W val) {
	   
	auto NV = number_of_vertices();

	//set to empty weight the non-edges
	for (int v = 0; v < NV; v++) {
		for (int w = 0; w < NV; w++) {
			if (v == w || g_.is_edge(v, w)) {
				we_[v][w] = val;
			}
			else {
				we_[v][w] = NOWT;
			}
		}
	}
}

template <class Graph_t, class W>
inline
int Base_Graph_EW< Graph_t, W>::set_we(mat_t& lw) {
	
	auto NV = number_of_vertices();

	if (lw.size() != NV) {
		LOG_ERROR("bizarre matrix of weights-Base_Graph_EW<Graph_t,W >::set_we(mat_t...)");
		return -1;
	}

	//set to empty wv and non-edges
	for (std::size_t v = 0; v < NV; ++v) {
		for (std::size_t w = 0; w < NV; ++w) {
			if (v == w || g_.is_edge(v, w)) {
				we_[v][w] = lw[v][w];
			}
		}
	}

	return 0;
}



template<class Graph_t, class W>
inline void Base_Graph_EW<Graph_t, W>::neg_w(){

	auto NV = number_of_vertices();	

	for(int i=0; i<NV; i++)
		for (int j = 0; j < NV; j++) {
			if(we_[i][j] != NOWT)
				we_[i][j] = -we_[i][j];
		}
}

template<class Graph_t, class W>
inline
vecw<W> Base_Graph_EW<Graph_t, W>::get_wv()  const {

	auto NV = number_of_vertices();

	vector<W> res;
	res.reserve(NV);
		for (int v = 0; v < NV; v++) {
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
inline
int Base_Graph_EW<Graph_t, W>::read_dimacs(const string& filename){
/////////////////////////////
// Reads an unweighted simple directed edge weighted graph in dimacs format
// 'c' comment
// 'p' next in the line is the graph order and number of edges  (header)
// 'e' next in the lines is an edge and an edge-weight
// 'n' o 'v'  next in the line is verte weight information  (6/09/17)- Reading weights in DIMACS format (n <V> <W(V)>)
// 
// RETURN VALUE: 0 if correct, -1 in case of error (no memory allocation)
	
	int size, nEdges, v1, v2, edges=0;
	W we = NOWT, wv = NOWT;
	char token[10]; char line [250]; char c;
	
	fstream f(filename.c_str());
	if(!f){
		LOG_ERROR("File: " , filename , " could not be opened reading DIMACS format- Base_Graph_EW<Graph_t, W>::read_dimacs(...)");
		return -1;
	}

	//read header
	///////////////////////////////////////////////////////////
	if(::gio::dimacs::read_dimacs_header(f, size, nEdges) == -1){
	////////////////////////////////////////////////////////////
		f.close();
		return -1;
	}	
	
	init(size);

	///////////////////////////////////////////////////////////
	::gio::read_empty_lines(f);
	////////////////////////////////////////////////////////////

	///////////////////////
	//read vertex weights format n <x> <w> or v <x> <w> if they exist
	//////////////////////
	c=f.peek();
	if(c== 'n' || c=='v' ){																								/* 'v' format is used by Zavalnij in evil_W benchmark */
		for(int n=0; n< size; n++){
			f >> c >> v1 >> wv;
			if(!f.good()){
				LOG_ERROR("bizarre line related to weights- Base_Graph_EW<Graph_t, W>::read_dimacs");
				clear();
				f.close();
				return -1;
			}
			we_[v1 -1 ][v1 -1] = wv;
			if(wv == 0){
				LOG_ERROR ("wrong header for edges reading DIMACS format-Base_Graph_EW<Graph_t, W>::read_dimacs");
				clear();
				f.close();
				return -1;
			}
			f.getline(line, 250);  //remove remaining part of the line
		}
	
		///////////////////////////////////////////////////////////
		::gio::read_empty_lines(f);
		///////////////////////////////////////////////////////////
	}
	
	/////////////////////	
	//read edges
	/////////////////////

	//read the first edge to determine the type of input
	bool with_edges=true;
	f.getline(line, 250); 
	stringstream sstr(line);
	int nw=com::counting::number_of_words(sstr.str());
	if(nw != 4){
		LOG_ERROR("bizarre edge_weights in file, assumed unweighted - Base_Graph_EW<Graph_t, W>::read_dimacs()");
		with_edges=false;		
	}

	//parse the first edge
	if(with_edges){
		sstr>>c>>v1>>v2>>we;
		g_.add_edge(v1-1,v2-1);
		set_we(v1-1, v2-1, we);
	}else{
		sstr>>c>>v1>>v2;
		g_.add_edge(v1-1,v2-1);
	}
		
	//remaining edges
	for(int e=1; e<nEdges; e++){
		f>>c;
		if(c!='e'){
			LOG_ERROR("wrong header for edges reading DIMACS format- Base_Graph_EW<Graph_t, W>::read_dimacs()");
			clear();
			f.close();
			return -1;
		}
		//add weighted edge	
		if(with_edges){
			f >> v1 >> v2 >> we;
			g_.add_edge(v1 - 1,v2 - 1);
			set_we(v1 - 1, v2 - 1, we);
		}else{
			f >> v1 >> v2;
			g_.add_edge(v1 - 1,v2 - 1);
		}
					
		f.getline(line, 250);  //remove remaining part of the line
	}
	////////////////////////

	f.close();
	
	//name (removes path)
	g_.set_name(filename);
	
	//stores order
	//NV = number_of_vertices();

return 0;
}

template<class Graph_t, class W>
inline
ostream& Base_Graph_EW<Graph_t, W>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 

	const int NV = number_of_vertices();

	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!g_.get_name().empty())
		o<<"nc "<<g_.get_name().c_str()<<endl;

	//tamaño del grafo
	o << "p edge " << NV << " " << g_.number_of_edges() << endl;

	//write vertex weights
	for (int v = 0; v < NV; v++) {
		if (we_[v][v] != NOWT) {
			o << "n " << v + 1 << " " << we_[v][v] << endl;
		}
	}
		
	//write edges and edge weights
	for(int v=0; v<NV; v++){
		for(int w=0; w<NV; w++){
			if (v != w && g_.is_edge(v, w)) {										//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << we_[v][w] << endl;			//1 based vertex notation dimacs
			}
		}
	}
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_EW<Graph_t, W>::print_weights (ostream& o, bool line_format) const{

	auto NV = number_of_vertices();

	o << "\n***********************************" << endl;
		
	if(line_format){
		for(std::size_t i = 0; i < NV; ++i){		
			for(std::size_t j = 0; j < NV; ++j){
				if(we_[i][j] != Base_Graph_EW<Graph_t, W>::NOWT){
					o << "[" << i << "->" << j << " (" << we_[i][j] << ")] " << endl;
				}
			}			
		}	
	}else{								//outputs to stream edge-weights in matrix form
		for(std::size_t i = 0; i < NV; ++i){
			for(std::size_t j = 0; j < NV; ++j){
				if (we_[i][j] != Base_Graph_EW<Graph_t, W>::NOWT) {
					o << we_[i][j] << '\t';
				}
				else {
					o << "--" << '\t';
				}
			}
			o << endl;
		}
	}
	o << "*************************************" << endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_EW<Graph_t, W>::print_weights (vint& ln, ostream& o) const{
/////////////////////////
// Shows non-empty weights related to @ln

	o << "\n***********************************" << endl;
	for(int i=0; i< ln.size(); i++){
		for(int j=0; j< ln.size(); j++){
			if ( we_[ln[i]][ln[j]] != Base_Graph_EW<Graph_t, W>::NOWT) {
						o << "[" << ln[i] << "->" << ln[j] << " (" << we_[ln[i]][ln[j]] << ")] "<<endl;
			}
		}
	}
	o << "*********************************" << endl;
	return o;
}

template<class Graph_t, class W>
inline
ostream& Base_Graph_EW<Graph_t, W>::print_data (bool lazy, std::ostream& o, bool endl) {
	g_.print_data(lazy, o, false);
	o << " ew";								//"w" for vertex-weighted graphs in GRAPH lib
	if (endl) { o << std::endl; }
	return o;
}

template <class W>
inline
ostream& Graph_EW<ugraph, W>::print_weights (ostream& o, bool line_format, bool only_vertex_weights) const{
///////////////////////
// outputs to stream edge-weights (UPPER-TRIANGLE) in different formats

	auto NV = ptype::number_of_vertices();
	
	o << "\n***********************************" << endl;
	if (only_vertex_weights == false) {
		if (line_format) {
			for (int v = 0; v < NV; v++) {
				if (ptype::we_[v][v] != ptype::NOWT) {
					o << "[" << v << " (" << ptype::we_[v][v] << ")] " << endl;
				}
			}

			for (int i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (ptype::we_[i][j] != ptype::NOWT) {
						o << "[" << i << "-" << j << " (" << ptype::we_[i][j] << ")] " << endl;
					}
				}
			}
		}
		else {																			//outputs to stream edge-weights in matrix form
			for (int i = 0; i < NV; i++) {
				for (int j = i; j < NV; j++) {
					if (ptype::we_[i][j] != ptype::NOWT) {
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
		for (int v = 0; v < NV; v++) {
			if (ptype::we_[v][v] != ptype::NOWT) {
				o << "[" << v << " (" << ptype::we_[v][v] << ")] " << endl;
			}
		}
	}
	o << "***********************************" << endl;
	return o;
}

template <class W>
inline
ostream& Graph_EW<ugraph, W>::print_weights (vint& ln, ostream& o, bool only_vertex_weights) const{
/////////////////////////
// Shows relevant vertex and edge weights
	   
	o << "\n***********************************" << endl;
	if (only_vertex_weights == false) {
		for (int i = 0; i < ln.size(); i++) {
			if (ptype::we_[ln[i]][ln[i]] != ptype::NOWT) {
				o << "[" << ln[i] << " (" << ptype::we_[ln[i]][ln[i]] << ")] " << endl;
			}
		}

		for (int i = 0; i < ln.size() - 1; i++) {
			for (int j = i + 1; j < ln.size(); j++) {
				if (ptype::we_[ln[i]][ln[j]] != ptype::NOWT) {
					o << "[" << ln[i] << "-" << ln[j] << " (" << ptype::we_[ln[i]][ln[j]] << ")] " << endl;
				}
			}
		}
	}
	else {
		for (int i = 0; i < ln.size(); i++) {
			if (ptype::we_[ln[i]][ln[i]] != ptype::NOWT) {
				o << "[" << ln[i] << " (" << ptype::we_[ln[i]][ln[i]] << ")] " << endl;
			}
		}
	}
	o << "***********************************" << endl;
	return o;
}

template<class W>
inline
ostream& Graph_EW<ugraph, W>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 
	
	auto NV = ptype::number_of_vertices();

	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!ptype::g_.get_name().empty())
		o<<"c "<< ptype::g_.get_name().c_str()<<endl;

	//tamaño del grafo
	o<<"p edge "<< ptype::number_of_vertices() <<" "<< ptype::g_.number_of_edges()<<endl;
		
	//write vertex weights
	for (int v = 0; v < NV; v++) {
		if (ptype::we_[v][v] != ptype::NOWT) {
			o << "n " << v + 1 << " " << ptype::we_[v][v] << endl;
		}
	}

	//write edges and edge weights
	for(int v=0; v< NV -1; v++){
		for(int w=v+1; w< NV; w++){
			if (ptype::g_.is_edge(v, w)) {														//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << " " << ptype::we_[v][w] << endl;		//1 based vertex notation dimacs
			}
		}
	}
	return o;
}

////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

template class  Base_Graph_EW<ugraph, int>;
template class  Base_Graph_EW<ugraph, double>;
template class  Graph_EW<ugraph, int>;
template class  Graph_EW<ugraph, double>;

//other specializations... (sparse_graph)


////////////////////////////////////////////
