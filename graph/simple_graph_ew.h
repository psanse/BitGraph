/*
 * simple_graph_ew.h file for the classes Base_Graph_EW and Graph_EW for edge-weighted graphs 
 *					 (note: the edge weights for self-loops are possible, and the interface 
							gives them consideration of vertex weights							)
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
	 
//alias
template<class W>
using vecw = vector<W>;	

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
	
	using type = Base_Graph_EW<Graph_t, W>;				//own type
	using graph_type = Graph_t;							//graph type	
	using basic_type = typename Graph_t::basic_type;	//bitset type used by graph type 
	using mat_t = vector<vector<W>>;					//matrix type for weights


	//alias types for backward compatibility
	using _gt = graph_type;
	using _bbt = basic_type;
	using _wt = W;																					
	
	//constants - globals
	static const W NOWT;								//default weight value for empty weights (0.0)	
																				
	//constructors
	Base_Graph_EW			()										{};											//no memory allocation
	Base_Graph_EW			(mat_t& lwe);																		//creates empty graph with edge weights
	Base_Graph_EW			(_gt& g, mat_t& lwe) : g_(g), we_(lwe)	{}											//creates graph with edge weights
	Base_Graph_EW			(int n, W val = NOWT)					{ init(n, val); }							//creates empty graph with |V|= n and val weights	
	Base_Graph_EW			(string filename);																	//read weighted ASCII file or generate weights using formula- CHECK! (21/11/2021))
			
	//copy constructor, move constructor, copy operator =, move operator =
	Base_Graph_EW					(const Base_Graph_EW& g)	= default;
	Base_Graph_EW					(Base_Graph_EW&& g)			= default;
	Base_Graph_EW& operator =		(const Base_Graph_EW& g)	= default;
	Base_Graph_EW& operator =		(Base_Graph_EW&& g)			= default;

	//destructor
virtual	~Base_Graph_EW()										= default;

/////////////
// setters and getters	
	/*
	*  @brief sets self-loop edge weight (considered as vertex-weight)
	*/
	void set_wv			(int v, W val)						{ we_[v][v] = val; }					
	
	/*
	*  @brief sets edge weight given an edge 
	*	
	*	(currently weights in self-loops can be added, similar to set_wv)
	*
	*  @returns 0 if success or -1 if a weight is added to a non-edge
	*/
	virtual	int set_we	(int v, int w,  W val);															 					
	
	/*
	*  @brief sets edge weight given an edge
	*
	*	(weights in self-loops are included)
	*
	*  @returns 0 if success or -1 if a weight is added to a non-edge
	*/
	virtual	int	set_we	(mat_t& lw);
	
	/*
	*  @brief sets all weights to val
	*/
	virtual	void set_we	(W val = NOWT);																	//sets all weights to val 

	
	W get_we			(int v, int w)			const		{ return we_[v][w]; }
	W get_wv			(int v)					const		{ return we_[v][v]; }									
	
	/*
	*  @brief getter for vertex weights
	*/
	vecw<W> get_wv		()						const;
	
	mat_t& get_we		()									{ return we_; }	
	const mat_t& get_we	()						const		{ return we_; }
	Graph_t& graph		()									{ return g_; }
	const Graph_t& graph()						const		{ return g_; }

	int number_of_vertices	()					const		{ return g_.number_of_vertices(); }
	int number_of_edges		(bool lazy = true)				{ return g_.number_of_edges(lazy); }


	const _bbt& get_neighbors	(int v)			const		{ return g_.get_neighbors(v); }
	_bbt& get_neighbors			(int v)						{ return g_.get_neighbors(v); }

	void set_name				(string str)				{ g_.set_name(str); }
	string get_name				()				const		{ return g_.get_name(); }
	string get_path				()				const		{ return g_.get_path(); }
	void set_path				(string path_name)			{ g_.set_path(path_name); }

//////////////////////////
// memory allocation
 
	/*
	* @brief resets to empty graph with |V|= n and assigns weight val to all vertices and edges
	* @param n number of vertices
	* @param reset_name if true, @name_ and @path_ reset to empty
	* @returns 0 if success, -1 if memory allocation fails
	* 
	* @comment preserved for backward compatibility (use reset(...))
	*/
	int init			(int n, W val = NOWT, bool reset_name = true);															
	void clear			()									 { g_.clear(); we_.clear(); }
		
	/*
	* @brief resets to empty graph |V|= n and assigns weight val to all vertices and edges
	* @param n number of vertices
	* @param name name of the instance
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int reset			(std::size_t n, W val = NOWT, string name = "");

/////////////////////////
//basic graph operations

	/*
	*  @brief adds edge, no self-loops allowed
	*/
	void add_edge		(int v, int w)							{ g_.add_edge(v, w); }

	double density		(bool lazy = true)						{ return g_.density(lazy); }

/////////////////////////
// boolean properties

	bool is_edge		(int v, int w)			const			{ return g_.is_edge(v, w); }
	
	/*
	* @brief consistency check 
	* 
	*		 I. edges with no default weight 
	*		 TODO...
	* @returns true if consistent, false otherwise
	*/
	bool is_consistent	();																				
	
////////////////////////
//weight operations
	
	/*
	* @brief changes the sign of the weights 
	*		we(i, j) = -we(i, j)
	*		 
	* @returns true if consistent, false otherwise
	*/
	void neg_w();																						

////////////
// I/O
public:

	int read_dimacs					(const string& filename);
	virtual	ostream& write_dimacs	(ostream& o);

	virtual ostream& print_data		(bool lazy = true, ostream& o = cout, bool endl = true);
	virtual	ostream& print_weights	(ostream& o = cout, bool line_format = true)							const;
	virtual	ostream& print_weights	(vint& lnodes, ostream& o = cout)										const;

///////////////////
//data members

protected:
	Graph_t g_;
	mat_t   we_;			//extended for vertex and edge weights (20_/11/21))																	
};

//////////////////////////////////////////////////////////////
template<class Graph_t, class W>
//const W Base_Graph_EW<Graph_t, W >::NOWT = 0x1FFFFFFF;												 //[default ]equivalent to CLQMAX_INT- TODO@check which is the best constant for EMPTY WEIGHTS!
const W Base_Graph_EW <Graph_t, W >::NOWT = 0;															 //equivalent to CLQMAX_INT- TODO@check which is the best constant for EMPTY WEIGHTS!
/////////////////////////////////////////////////////////////

////////////////////////
//
// Graph_WE class 
// 
// I.  user generic class to specialize for different types of graphs
// II. currently only defined for ugraph type
//
///////////////////////

template <class Graph_t, class W>
class Graph_EW : public Base_Graph_EW<Graph_t, W> {};

////////////////////////////////////
// 
// Specialization for undirected graphs
//
////////////////////////////////////

template <class W>
class Graph_EW<ugraph, W> : public Base_Graph_EW<ugraph, W> {
public:

	using type = Graph_EW<ugraph, W>;					//own type
	using ptype = Base_Graph_EW<ugraph, W>;				//parent type
	using graph_type = ugraph;							//graph type
	using basic_type = typename graph_type::basic_type;	//bitset type used by graph type 
	using mat_t = typename ptype::mat_t;				//matrix type for weights

	//alias types for backward compatibility
	using _wt = W;										//weight number type for backward compatibility
	using _gt = graph_type;
	
	//constructors (inherited)
	using Base_Graph_EW<ugraph, W>::Base_Graph_EW;

	/////////////
	//overrides setters
	virtual	int set_we	(int v, int w, W we);
	virtual	void set_we	(W val = 0.0);
	virtual	int set_we	(mat_t& lw);

	/////////////
	//useful framework-specific interface for undirected weighted graphs
	int max_degree_of_graph() { return ptype::g_.max_degree_of_graph(); }
	int degree() { return ptype::g_.degree(); }
	int degree(int v)							const { return ptype::g_.degree(v); }
	int degree(int v, const typename ptype::_bbt & bbn)	const { return ptype::g_.degree(v, bbn); }

	int create_complement(Graph_EW<ugraph, W>& g)			const;											  //weights (also name, path,...) are also stored in the new graph
	int create_complement(ugraph& g)					    const;											  //weights (also name, path,...) are lost

	/////////////
	//overrides I/O
	virtual	ostream& print_weights(ostream& o = cout, bool line_format = true, bool only_vertex_weights = false) const;
	virtual	ostream& print_weights(vint& lv, ostream& o = cout, bool only_vertex_weights = false) const;

	virtual	ostream& write_dimacs(ostream& o);
};

template<class W>
inline
int Graph_EW<ugraph, W>::create_complement (Graph_EW<ugraph, W>& g) const {
	/////////////////////
	//TODO@ name, path info is lost

	g.set_name(this->get_name());
	g.set_path(this->get_path());
	g.get_we() = ptype::we_;
	g.NV = ptype::NV;
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
////////////////////////
// partial specialization for undirected graphs (symmetrical edge weights)
//
// RETURNS -1 if a weight is added to a non-edge

	if (v == w) {
		ptype::we_[v][v] = val;
	}else if (val == ptype::NOWT || ptype::g_.is_edge(v, w)) {
		ptype::we_[v][w] = val;
		ptype::we_[w][v] = val;		
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
inline int Base_Graph_EW<Graph_t, W>::reset(std::size_t n, W val, string name)
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
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(mat_t& lwe) : _we (lwe) {

	if (g_.init(lwe.size()) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::Base_Graph_EW");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

template<class Graph_t, class W>
Base_Graph_W<Graph_t, W>::Base_Graph_W(vector<W>& lw) : w_(lw) {

	if (g_.reset(lw.size()) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::Base_Graph_W");
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
		LOG_DEBUG("edge weight cannot be added to the non-edge" , "(" , v , "," , w , ")" , "- Base_Graph_EW<Graph_t,W >::set_we");
		LOG_DEBUG("weight not added");
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
	res.reserve(NV)
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
///////////////////////
// outputs to stream weights in different formats

	o << "\n***********************************"<<endl;
	const int NV=number_of_vertices();
	
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


#endif