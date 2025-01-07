/*
 * simple_graph_ew.h file for the classes Base_Graph_EW and Graph_EW for edge-weighted graphs
 *
 * @creation_date 16/01/19
 * @update_date_one 10/11/2021 - train application
 * @last_update 06/01/25
 * @dev pss
 *
 * This code is part of the GRAPH C++ library for bit encoded
 * simple graphs. GRAPH stores the adjacency matrix in full, each
 * row encoded as a bitstring.
 *
 * GRAPH is at the core many state of the art leading exact clique
 * algorithms.
 *
 * TODO - REFACTORING and TESTING (06/01/2025)
 */

#ifndef __SIMPLE_GRAPH_EDGE_WEIGHTED_H__
#define __SIMPLE_GRAPH_EDGE_WEIGHTED_H__

#include <iostream>
#include <fstream>
#include <string>

#include "utils/prec_timer.h"
#include "utils/logger.h"
#include "utils/common.h"
#include "graph/formats/dimacs_format.h"
#include "graph/simple_ugraph.h"

using namespace std;	/* not recommended in headers :-) */
	 
///////////////////////
//
// class Base_Graph_EW 
// (used to be able to specialize functions according to the type of graph only)
//
// comment: user class is Graph_EW (see below) 
//
///////////////////////

template<class Graph_t, class W>
class Base_Graph_EW {							
public:
	using mat_t = vector<vector<W>>;					//matrix type for weights

	using type = Base_Graph_EW<Graph_t, W>;				//own type
	using graph_type = Graph_t;							//graph type	
	using basic_type = typename Graph_t::basic_type;	//bitset type used by graph type 

	//alias types for backward compatibility
	using _gt = graph_type;
	using _bbt = basic_type;
	using _wt = W;																					
	
	//constants - globals
	static const W NOWT;								//default weight value for empty weights	
																				
	//constructors
	Base_Graph_EW					(Graph_t& g_out, mat_t& lwe) :g_(g_out), we_(lwe) {}
	Base_Graph_EW					(int nV, W val = 0.0) { init(nV, val); }												//creates empty graph with size vertices	
	Base_Graph_EW					(string filename);																		//read weighted ASCII file or generate weights using formula- CHECK! (21/11/2021))
	Base_Graph_EW					() {};																		//does not allocate memory (neither graph nor weights)
	
	//destructor
virtual	~Base_Graph_EW() = default;

	/////////////
	// setters and getters	(USE ALWAYS after init(...))

	virtual	int set_we(int v, int w,  W val);															//sets edge(v,w) or (w.v)) to edge-weight value we 					
	virtual	void set_we(W val = 0.0);																	//sets all weights to val 
	virtual	int	set_we(mat_t& lw);
	void set_wv (int v, W val) { we_[v][v] = val; }													//idem to set_we( v, v, W val)


	W get_we(int v, int w) const				{ return we_[v][w]; }
	W get_wv(int v) const						{ return we_[v][v]; }									//specialized for vertex weights
	vector<W> get_wv() const; 
	mat_t& get_we()								{ return we_; }	
	const mat_t& get_we()			   const	{ return we_; }
	Graph_t& graph()							{ return g_; }

	

	//////////////////////////
	// memory allocation 
	int init(int nV, W val = NOWT);																		//allocates memory for n vertices, assigns weight  @val to ALL vertices and edges
	int init(Graph_t& g_out);
	void clear() { g_.clear(); we_.clear(); }

	/////////////////////////
	// consistency
	bool is_consistent();																				//checks that edge_weights are present only in edges

////////////////////////
//useful common interface with layered graph
	int number_of_vertices()	const { return g_.number_of_vertices(); }
	void add_edge(int v, int w) { g_.add_edge(v, w); }
	void set_name(string str) { g_.set_name(str); }
	string get_name()	const { return g_.get_name(); }
	string get_path()	const { return g_.get_path(); }
	void set_path(string path_name) { g_.set_path(path_name); }
	
	//used in the framework
	const _bbt& get_neighbors(int v)		const { return g_.get_neighbors(v); }
	_bbt& get_neighbors(int v) { return g_.get_neighbors(v); }
	int number_of_edges() { return g_.number_of_edges(); }
	bool is_edge(int v, int w)  const { return g_.is_edge(v, w); }
	double density() { return g_.density(); }

	////////////////////////
	//weight operations
	void neg_w();																						//negates the weights- we(i, j) = -we(i, j)

	////////////
	// I/O
public:
	int read_dimacs(const string& filename);
	virtual	ostream& write_dimacs(ostream& o);

	virtual ostream& print_data(bool lazy = true, ostream& o = cout, bool endl = true);
	virtual	ostream& print_weights(ostream& o = cout, bool line_format = true)							const;
	virtual	ostream& print_weights(vint& lnodes, ostream& o = cout)										const;

///////////////////
//data members

protected:
	Graph_t g_;
	mat_t   we_;			//extended for vertex and edge weights (20_/11/21))																	
};

//////////////////////////////////////////////////////////////
template<class Graph_t, class W>
//const W Base_Graph_EW<Graph_t, W >::NOWT = 0x1FFFFFFF;																		//[default ]equivalent to CLQMAX_INT- TODO@check which is the best constant for EMPTY WEIGHTS!
const W Base_Graph_EW<Graph_t, W >::NOWT = 0;																				//equivalent to CLQMAX_INT- TODO@check which is the best constant for EMPTY WEIGHTS!
/////////////////////////////////////////////////////////////

///////////////////////
//
// class Graph_EW 
//
///////////////////////

template <class Graph_t, class W>
class Graph_EW: public Base_Graph_EW<Graph_t, W>{

	typedef Base_Graph_EW<Graph_t, W> _mypt;
	
public:
	Graph_EW()												:Base_Graph_EW<Graph_t, W>(){}
	Graph_EW(Graph_t& g_out, typename _mypt::mat_t& lwe)	:Base_Graph_EW<Graph_t, W>(g_out,lwe){}
	Graph_EW(int NV, W val=0.0)								:Base_Graph_EW<Graph_t, W>(NV, val){}									//creates empty graph with size vertices	
	Graph_EW(std::string filename)							:Base_Graph_EW<Graph_t, W>(filename){}
virtual ~Graph_EW() {}
};

///////////////////////
//
// class Graph_EW<ugraph> 
// (specialization for undirected class)
//
// currently the one-and-only user class
//
///////////////////////

template <class W>
class Graph_EW<ugraph, W> : public Base_Graph_EW<ugraph, W> {

	typedef  Base_Graph_EW<ugraph, W> _mypt;
	typedef  typename Base_Graph_EW<ugraph, W>::mat_t _mat_t;


public:

	Graph_EW() :Base_Graph_EW<ugraph, W>() {}
	Graph_EW(ugraph& g_out, _mat_t& lwe) :Base_Graph_EW<ugraph, W>(g_out, lwe) {}
	Graph_EW(int NV, W val = 0.0) :Base_Graph_EW<ugraph, W>(NV, val) {}									//creates empty graph with size vertices	
	Graph_EW(std::string filename) :Base_Graph_EW<ugraph, W>(filename) {}
	virtual ~Graph_EW() {}

	/////////////
	//overrides setters
	virtual	int set_we(int v, int w, W we);
	virtual	void set_we(W val = 0.0);
	virtual	int set_we(_mat_t& lw);

	/////////////
	//useful framework-specific interface for undirected weighted graphs
	int max_degree_of_graph() { return _mypt::g_.max_degree_of_graph(); }
	int degree() { return _mypt::g_.degree(); }
	int degree(int v)							const { return _mypt::g_.degree(v); }
	int degree(int v, const typename _mypt::_bbt & bbn)	const { return _mypt::g_.degree(v, bbn); }

	int create_complement(Graph_EW<ugraph, W>& g)			const;											  //weights (also name, path,...) are also stored in the new graph
	int create_complement(ugraph& g)					    const;											  //weights (also name, path,...) are lost

	/////////////
	//overrides I/O
	virtual	ostream& print_weights(ostream& o = cout, bool line_format = true, bool only_vertex_weights = false) const;
	virtual	ostream& print_weights(vint& lv, ostream& o = cout, bool only_vertex_weights = false) const;

	virtual	ostream& write_dimacs(ostream& o);
};

//////////////////////////////////////////////////////////////////////////////////////////
//Graph_EW< ugraph, W > implementation



template<class W>
inline
int Graph_EW<ugraph, W>::create_complement(Graph_EW<ugraph, W>& g) const {
	/////////////////////
	//TODO@ name, path info is lost

	g.set_name(this->get_name());
	g.set_path(this->get_path());
	g.get_we() = _mypt::we_;
	g.NV = _mypt::NV;
	_mypt::g_.create_complement(g.graph());

	return 0;
}

template<class W>
inline
int Graph_EW<ugraph, W>::create_complement(ugraph& g) const {

	_mypt::g_.create_complement(g);
	return 0;
}

template <class W>
inline
int Graph_EW< ugraph, W >::set_we (int v, int w, W val) {
////////////////////////
// partial specialization for undirected graphs (symmetrical edge weights)
//
// RETURNS -1 if a weight is added to a non-edge

	if (v == w) {
		_mypt::we_[v][v] = val;
	}else if (val == _mypt::NOWT || _mypt::g_.is_edge(v, w)) {
		_mypt::we_[v][w] = val;
		_mypt::we_[w][v] = val;		
	}
	else {
		LOG_ERROR("bizarre petition to add weight to the non-edge", "[", v , "," , w , "]" , "-Base_Graph_EW<Graph_t,W >::set_we(int, int, W)");
		LOG_ERROR("weight not added");
		return -1;
	}
	return 0;
}

template <class W>
inline
void Graph_EW< ugraph, W >::set_we(W val) {
////////////////////////
// partial specialization for undirected graphs (symmetrical edge weights)
	
	auto NV = _mypt::g_.number_of_vertices();

	//set to empty wv and non-edges UPPER-T
	for (int v = 0; v < NV - 1; v++) {
		for (int w = v + 1; w < NV; w++) {
			if (_mypt::g_.is_edge(v, w)) {
				_mypt::we_[v][w] = val;
				_mypt::we_[w][v] = val;
			}
			else {
				_mypt::we_[v][w] = _mypt::NOWT;
				_mypt::we_[w][v] = _mypt::NOWT;
			}
		}
	}

	//vertex weights
	for (int v = 0; v < NV; v++) {
		_mypt::we_[v][v] = val;
	}
}

template <class W>
inline
int Graph_EW< ugraph, W >::set_we(typename Graph_EW<ugraph, W>::_mat_t& lw) {
////////////////////////
// partial specialization for undirected graphs (symmetrical edge weights)
		
	auto NV = _mypt::g_.number_of_vertices();

	if (lw.size() != NV) {
		LOG_ERROR("bizarre matrix of weights-Graph_EW< ugraph, W >::set_we(mat_t)");
		return -1;
	}

	//set to empty wv and non-edges UPPER-T
	for (int v = 0; v < NV - 1; v++) {
		for (int w = v + 1; w < NV; w++) {
			if (_mypt::g_.is_edge(v, w)) {
				_mypt::we_[v][w] = lw[v][w];
				_mypt::we_[w][v] = lw[w][v];
			}
			else {
				_mypt::we_[v][w] = _mypt::NOWT;
				_mypt::we_[w][v] = _mypt::NOWT;
			}
		}
	}	
	
	//vertex weights
	for (int v = 0; v < NV; v++) {
		_mypt::we_[v][v] = lw[v][v];
	}
		   
	return 0;
}

template <class W>
inline
ostream& Graph_EW<ugraph, W>::print_weights (ostream& o, bool line_format, bool only_vertex_weights) const{
///////////////////////
// outputs to stream edge-weights (UPPER-TRIANGLE) in different formats

	auto NV = _mypt::g_.number_of_vertices();
	
	o << "\n***********************************" << endl;
	if (only_vertex_weights == false) {
		if (line_format) {
			for (int v = 0; v < NV; v++) {
				if (_mypt::we_[v][v] != _mypt::NOWT) {
					o << "[" << v << " (" << _mypt::we_[v][v] << ")] " << endl;
				}
			}

			for (int i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (_mypt::we_[i][j] != _mypt::NOWT) {
						o << "[" << i << "-" << j << " (" << _mypt::we_[i][j] << ")] " << endl;
					}
				}
			}
		}
		else {																			//outputs to stream edge-weights in matrix form
			for (int i = 0; i < NV; i++) {
				for (int j = i; j < NV; j++) {
					if (_mypt::we_[i][j] != _mypt::NOWT) {
						o << "[" << i << "-" << j << " (" << _mypt::we_[i][j] << ")] ";
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
			if (_mypt::we_[v][v] != _mypt::NOWT) {
				o << "[" << v << " (" << _mypt::we_[v][v] << ")] " << endl;
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
			if (_mypt::we_[ln[i]][ln[i]] != _mypt::NOWT) {
				o << "[" << ln[i] << " (" << _mypt::we_[ln[i]][ln[i]] << ")] " << endl;
			}
		}

		for (int i = 0; i < ln.size() - 1; i++) {
			for (int j = i + 1; j < ln.size(); j++) {
				if (_mypt::we_[ln[i]][ln[j]] != _mypt::NOWT) {
					o << "[" << ln[i] << "-" << ln[j] << " (" << _mypt::we_[ln[i]][ln[j]] << ")] " << endl;
				}
			}
		}
	}
	else {
		for (int i = 0; i < ln.size(); i++) {
			if (_mypt::we_[ln[i]][ln[i]] != _mypt::NOWT) {
				o << "[" << ln[i] << " (" << _mypt::we_[ln[i]][ln[i]] << ")] " << endl;
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
	
	auto NV = _mypt::g_.number_of_vertices();

	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!_mypt::g_.get_name().empty())
		o<<"c "<< _mypt::g_.get_name().c_str()<<endl;

	//tamaño del grafo
	o<<"p edge "<< _mypt::g_.number_of_vertices() <<" "<< _mypt::g_.number_of_edges()<<endl;
		
	//write vertex weights
	for (int v = 0; v < NV; v++) {
		if (_mypt::we_[v][v] != _mypt::NOWT) {
			o << "n " << v + 1 << " " << _mypt::we_[v][v] << endl;
		}
	}

	//write edges and edge weights
	for(int v=0; v< NV -1; v++){
		for(int w=v+1; w< NV; w++){
			if (_mypt::g_.is_edge(v, w)) {														//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << " " << _mypt::we_[v][w] << endl;		//1 based vertex notation dimacs
			}
		}
	}
	return o;
}

///////////////////////////////////////////////////////////////////////////////////////
//Base_Graph_EW implementation

template<class Graph_t, class W>
inline
int Base_Graph_EW<Graph_t, W>::init(int size, W val){
///////////////////////////
// Allocates memory for the empty graph (deallocates previous)

	try{
		g_.init(size);
		we_.assign(size, vector<W>(size, val));
	}catch(...){
		LOG_ERROR("bad allocation- Base_Graph_EW<Graph_t, W>::init(int, W)");
		return -1;
	}		
return 0;
}

template<class Graph_t, class W>
inline
int Base_Graph_EW<Graph_t, W>::init(Graph_t& g_out) {

	try {
		g_ = g_out;
		NV = g_.number_of_vertices();
		we_.assign(NV, vector<W>(NV, NOWT));		
	}
	catch (...) {
		LOG_ERROR("bad allocation- Base_Graph_EW<Graph_t, W>::init(Graph_t&)");
		return -1;
	}
	return 0;
}

template<class Graph_t, class W>
inline bool Base_Graph_EW<Graph_t, W>::is_consistent(){

	auto NV = g_.number_of_vertices();

	for (int i = 0; i < NV; i++) {
		for (int j = 0; j < NV; j++) {
			if ( we_[i][j] == NOWT &&  g_.is_edge(i, j) ) {
				LOG_ERROR("bizarre edge-weight-Base_Graph_EW<Graph_t, W>::is_consistent()");
				LOG_ERROR("["<<i<<","<<j<<" ("<<we_[i][j]<<")]");
				return false;
			}
		}
	}
	return true;
}


template<class Graph_t, class W>
inline
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(string filename){
	read_dimacs	(filename);							
}


template <class Graph_t, class W>
inline
int Base_Graph_EW<Graph_t, W >::set_we(int v, int w, W val) {
//////////////////
// sets edge-weight if there is at least an edge between v and w (v->w or w->v)
// sets vertex weight if @v = @w
//
// RETURNS -1 if no edge- weight will not be added
	
	if (v == w || g_.is_edge(v, w)) {
		we_[v][w] = val;
	}
	else {
		LOG_ERROR("bizarre petition to add weight to the non-edge" , "[" , v , "," , w , "]" , "-Base_Graph_EW<Graph_t,W >::set_we(int, int, W)");
		LOG_ERROR("weight not added");
		return -1;
	}	
	return 0;
}

template <class Graph_t, class W>
inline
void Base_Graph_EW< Graph_t, W>::set_we(W val) {
	   
	auto NV = g_.number_of_vertices();

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
/////////////////
//sets all edges-weights in existing edges and vertex weights to @lw
	
	auto NV = g_.number_of_vertices();

	if (lw.size() != NV) {
		LOG_ERROR("bizarre matrix of weights-Base_Graph_EW<Graph_t,W >::set_we(mat_t...)");
		return -1;
	}

	//set to empty wv and non-edges
	for (int v = 0; v < NV; v++) {
		for (int w = 0; w < NV; w++) {
			if (v == w || g_.is_edge(v, w)) {
				we_[v][w] = lw[v][w];
			}
		}
	}
	return 0;
}

///////////////
//
// I/O
//
//////////////

template<class Graph_t, class W>
inline void Base_Graph_EW<Graph_t, W>::neg_w(){

	auto NV = g_.number_of_vertices();	

	for(int i=0; i<NV; i++)
		for (int j = 0; j < NV; j++) {
			if(we_[i][j] != NOWT)
				we_[i][j] = -we_[i][j];
		}
}

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
	//NV = g_.number_of_vertices();

return 0;
}

template<class Graph_t, class W>
inline
ostream& Base_Graph_EW<Graph_t, W>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 

	const int NV = g_.number_of_vertices();

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
///////////////////////
// outputs to stream weights in different formats

	o << "\n***********************************"<<endl;
	const int NV=g_.number_of_vertices();
	
	if(line_format){
		for(int i=0; i<NV; i++){		
			for(int j=0; j<NV; j++){
				if(we_[i][j] != Base_Graph_EW<Graph_t, W>::NOWT){
					o<<"["<<i<<"->"<<j<<" ("<<we_[i][j]<<")] "<<endl;
				}
			}			
		}	
	}else{					//outputs to stream edge-weights in matrix form
		for(int i=0; i<NV; i++){
			for(int j=0; j<NV; j++){
				if (we_[i][j] != Base_Graph_EW<Graph_t, W>::NOWT) {
					o << we_[i][j] << "\t";
				}
				else {
					o << "--" << "\t";
				}
			}
			o<<endl;
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
ostream& Base_Graph_EW<Graph_t, W>::print_data(bool lazy, std::ostream& o, bool endl) {
	g_.print_data(lazy, o, false);
	o<<" ew";
	if(endl) o<<std::endl;	
	return o;
}

template<class Graph_t, class W>
inline
vector<W> Base_Graph_EW<Graph_t, W>::get_wv()  const {
	vector<W> res;
	for (int v = 0; v< NV; v++) {
		res.push_back(we_[v][v]);
	}
	return res;
}

#endif