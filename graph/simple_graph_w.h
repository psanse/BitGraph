//simple_graph_w.h: a new layered type class for vertex weighted graphs made up of a greneric graph type and a 
//					collection of weights
//					Designed to simplify the generic graph type
//dat@16/01/19

/* 
 * simple_graph_w.h file from the GRAPH library, a C++ library for bit encoded 
 * simple vertex weighted graphs. GRAPH stores the adjacency matrix un full 
 * but each row is encoded as a bitstring. GRAPH is at the core of  BBMC, a 
 * state of the art leading exact maximum clique algorithm. 
 * (see license file (legal.txt) for references)
 *
 * Copyright (C)
 * Author: Pablo San Segundo
 * Intelligent Control Research Group CAR(UPM-CSIC) 
 *
 * Permission to use, modify and distribute this software is
 * granted provided that this copyright notice appears in all 
 * copies, in source code or in binaries. For precise terms 
 * see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any 
 * kind, express or implied, and with no claim as to its
 * suitability for any purpose.
 *
 */

#ifndef __SIMPLE_GRAPH_WEIGHTED_H__
#define __SIMPLE_GRAPH_WEIGHTED_H__

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <ios>

#include "bitscan/bitscan.h"
#include "filter_graph_encoding_type.h"
#include "formats/dimacs_format.h"
#include "formats/mmx_format.h"
#include "formats/edges_format.h"
#include <math.h>
#include "utils/prec_timer.h"
#include "utils/logger.h"
#include "utils/common.h"

#include "graph/simple_graph.h"

////////////
//file extension for weights (currently switched off - read from DIMACS file format n <V> <W> at the beginning of the file - 2023/09/26)

//#define FILE_EXTENSION_W							0x100		//LAMSADE (deprecated 3/4/17): weights according to degree
//#define FILE_EXTENSION_D							0x101		//Classical weights used in tests: 1-200 in a loop
//#define FILE_EXTENSION_WWW						0x102		//LAMSADE (2018): weights preprocessed for the MEWCP

constexpr int WEIGHT_AUTO_GEN_MODE = 200;			//for modulus weight generation (see Base_Graph_W::gen_mode_weights)						

using namespace std;								// TODO- remove from header! 

///////////////////////
//
// Base_Graph_W class 
// 
// (used to be able to specialize functions
// according to the type of graph only)
//
// User type class is Graph_W 
//
////////////////////////

template<class Graph_t, class W>
class Base_Graph_W {	
																					
public:

	using type = Base_Graph_W<Graph_t, W>;				//own type
	using graph_type = Graph_t;							//graph type	
	using basic_type = typename Graph_t::basic_type;	//bitset type used by graph type 
		
	//alias types for backward compatibility
	using _gt = graph_type;								
	using _bbt = basic_type;							
	using _wt =	 W;										
	
	//constructors
	Base_Graph_W						():	g_(){};																		//creates empty graph
	Base_Graph_W						(std::vector<W>& lw);															//creates empty graph with vertex weights
	Base_Graph_W						(_gt& gout, vector<W>& lw)  :g_(gout), w_(lw) {}								//creates graph with vertex weights	
	Base_Graph_W						(_gt& gout)					:g_(gout), w_ (gout.number_of_vertices(), 1) {}		//creates graph with unit weights
	Base_Graph_W						(int nV, W val=1.0)			:g_()	{ init(nV, val); }							//creates empty graph of size nV with unit weights	
	
	/*
	* @brief Reads weighted graph from ASCII file in DIMACS format
	*		
	*		 If unable to read weights generates modulus weights [Pullman 2008]
	* 
	*		 TODO: add support for other formats
	*/
	Base_Graph_W						(std::string filename);															

	
	//TODO other constructors and assign operators (1/1/2025)

	//destructor
	virtual	~Base_Graph_W()	 = default;		

/////////////
// setters and getters
	
	void set_w							(int v, W val)				{ w_.at(v)=val;}				
	void set_w							(W val=1.0)					{ w_.assign(g_.number_of_vertices(), val);}		
	
	/*
	* @brief sets vertex weights to all vertices
	* @param lw vector of weights of size |V|
	* @returns 0 if success, -1 if error
	*/
	int	 set_w							(std::vector<W>& lw);
	
	Graph_t& graph						()							{ return g_;}
const Graph_t& graph					()			const			{ return g_; }

	W get_w								(int v)		const			{ return w_[v];}	
const vector<W>& get_weights			()			const			{ return w_;}	
	vector<W>& get_weights				()							{ return w_;}	
	
	/*
	* @brief Determines weight and vertex of maximum weight
	* @param v ouptut vector of maximum weight
	* @returns weight of vertex v
	*/
	W maximum_weight					(int& v)	const;	
	
const _bbt& get_neighbors				(int v)		const			{ return g_.get_neighbors(v); }
 	_bbt& get_neighbors					(int v)						{ return g_.get_neighbors(v); }
	
	void set_name						(std::string str)			{ g_.set_name(str); }
	std::string get_name				()			const			{ return g_.get_name(); }
	void set_path						(std::string path_name)		{ g_.set_path(path_name); }
	std::string get_path				()			const			{ return g_.get_path(); }

	int number_of_vertices				()			const			{ return g_.number_of_vertices(); }
	int number_of_edges					(bool lazy = true)			{ return g_.number_of_edges(lazy); }


//////////////////////////
// memory allocation 

	/*
	* @brief resets to empty graph with |V|= n and assigns weight val to all vertices
	* @param n number of vertices
	* @param reset_name if true, @name_ and @path_ reset to empty
	* @returns 0 if success, -1 if memory allocation fails
	* @comment preserved for backward compatibility (use reset(...))
	*/
	int init (std::size_t n, W val = 1.0, bool reset_name = true);								
	

	/*
	* @brief resets to empty graph given name and number of vertices
	* @param n number of vertices
	* @param name name of the instance
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int reset (std::size_t n, W val = 1.0,  string name = "");

	/*
	* @brief deallocates memory and resets to default values
	*/
	void clear();													


/////////////////////////
//basic graph operations

	void add_edge						(int v, int w)					{ g_.add_edge(v, w); }
	double density						(bool lazy = true)				{ return g_.density(lazy); }

/////////////
// boolean properties

	bool is_edge						(int v, int w)		const		{ return g_.is_edge(v, w); }
	bool is_unit_weighted				();


///////////////////////////
//weight generation
	
	/*
	* @brief generates weights based on modulus operation [Pullan 2008, MODE = 200]
	* 
	*			w(v) = (v + 1) % MODE	(v 1-based index)
	*/
	int gen_mode_weights				(int MODE = WEIGHT_AUTO_GEN_MODE);						
	
////////////
// I/O
public:
	int read_dimacs						(const std::string& filename);			
	virtual ostream& write_dimacs		(std::ostream& o = std::cout);												//CHECK!
	int read_weights					(const std::string& filename);
		
	ostream& print_data					(bool lazy = true, std::ostream& o = std::cout, bool endl = true);	
	
	ostream& print_weights				(std::ostream& o= std::cout, bool show_v=true)								const;
	ostream& print_weights				(_bbt & bbsg, std::ostream& o= std::cout)									const;
	ostream& print_weights				(vint& lnodes, std::ostream& o= std::cout)									const;
	ostream& print_weights				(com::stack_t<int>& lv, ostream& o= std::cout)								const;
	ostream& print_weights				(com::stack_t<int>& lv, const vint& mapping, std::ostream& o= std::cout)	const;
	ostream& print_weights				(int* lv, int size, std::ostream& o= std::cout)								const;

/////////////////////////////////////
// data members

protected:
	Graph_t g_;
	vector<W> w_;					//vector of weights (currently automated reading for DIMACS graphs in an auxiliary graph with extension *.w or *.d)

};

///////////////////////
//
// Graph_W class 
// (user generic class to specialize for different types of graphs) 
//
///////////////////////

template<class Graph_t, class W>
class Graph_W : public Base_Graph_W <Graph_t, W> {};

////////////////////////////////////
// 
// Specialization for undirected graphs
//
////////////////////////////////////

template<class W>
class Graph_W<ugraph, W>: public Base_Graph_W<ugraph, W> {
public:

	using type = Graph_W<ugraph, W>;					//own type
	using ptype = Base_Graph_W<ugraph, W>;				//parent type
	using graph_type = ugraph;							//graph type
	using basic_type = typename graph_type::basic_type;	//bitset type used by graph type 

	//alias types for backward compatibility
	using _wt = W;										//weight number type for backward compatibility
	using _gt = graph_type;								

public:
	//constructors (inherited)
	using Base_Graph_W<ugraph, W>::Base_Graph_W;		

/////////////
//useful interface-specific for undirected weighted graphs
	int max_graph_degree	()													{ return ptype::g_.max_graph_degree(); }
	int degree				(int v)							const				{ return ptype::g_.degree(v); }
	int degree				(int v, const BitBoardN& bbn)	const				{ return ptype::g_.degree(v, bbn); }

	/*
	* @brief Complement graph (currently name info of original graph is lost)
	* @param g output graph
	* @returns 0 if success, -1 if error
	*/
	int create_complement	(Graph_W& g)					const;	

	/*
	* @brief Complement undirected graph (weights are lost)
	* @param g output complement undirected unweighted graph
	* @returns 0 if success, -1 if error
	*/
	int create_complement	(ugraph& g)					    const				{ return ptype::g_.create_complement(g);}		 



///////////
//I/O operations
	
	ostream& write_dimacs	(ostream& o = std::cout)		override;								
};


////////////////////////////////////////////////////////////////
// Necessary implementation of template methods in header file

template<class W>
inline
int Graph_W<ugraph, W>::create_complement(Graph_W& g) const {

	g.set_name(this->get_name());
	g.set_path(this->get_path());
	g.get_weights() = ptype::w_;
	ptype::g_.create_complement(g.graph());

	return 0;
}




template<class Graph_t, class W>
Base_Graph_W<Graph_t, W>::Base_Graph_W(vector<W>& lw){
	try{
		g_.init(lw.size());
		w_=lw;
	}catch(...){
		LOG_ERROR("Base_Graph_W<T, W>::init(vector<W>& lw)-error during memory graph allocation");
		return;
	}	
}

template<class Graph_t, class W>
inline Base_Graph_W<Graph_t, W>::Base_Graph_W(std::string filename){	
	
	if (read_dimacs(filename) == -1) {
		LOG_ERROR("Base_Graph_W<Graph_t, W>::Base_Graph_W(std::string)-error reading DIMACS file");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

template<class Graph_t, class W>
inline int Base_Graph_W<Graph_t, W>::gen_mode_weights(int MODE){

	const std::size_t NV = g_.number_of_vertices();

	w_.clear();
	w_.reserve(NV);
		
	for (std::size_t i = 0; i < NV; i++) {
		w_.emplace_back((i + 1) % MODE + 1);
	}

	return 0;
}

template<class Graph_t, class W>
inline bool Base_Graph_W<Graph_t, W>::is_unit_weighted()
{
	for (int v : w_) {
		if (v != 1) return false;
	}
	return true;
}

template<class Graph_t, class W>
int Base_Graph_W<Graph_t, W>::init(std::size_t NV , W val, bool reset_name = true){
			 
	try{
		g_.reset(NV);
		w_.assign(NV, val);
	}catch(...){
		LOG_ERROR("Bad memory allocation - Base_Graph_W<Graph_t, W>::init");
		return -1;
	}

	if (reset_name) {
		g_.set_name("");
		g_.set_path("");
	}
		
return 0;
}

template<class Graph_t, class W>
inline int Base_Graph_W<Graph_t, W>::reset(std::size_t NV, W val, string name)
{
	try {
		g_.reset(NV);
		w_.assign(NV, val);
		g_.set_name(name);
	}
	catch (...) {
		LOG_ERROR("bad allocation- Base_Graph_W<Graph_t, W>::reset(int, string)");
		return -1;
	}
	return 0;
}

template<class Graph_t, class W>
void Base_Graph_W<Graph_t, W>::clear	(){
	g_.clear();
	w_.clear();
}

template <class Graph_t, class W>
inline
int	Base_Graph_W<Graph_t,W >::set_w (vector<W>& lw){

	//assert
	if(g_.number_of_vertices() != lw.size()){
		LOG_ERROR ("bizarre number of weights - Base_Graph_W<Graph_t,W >::set_w");
		LOG_ERROR ("weights remain unchanged");
		return -1;
	}

	w_ = lw;
	return 0;
}

template <class Graph_t, class W>
inline
W Base_Graph_W<Graph_t, W>::maximum_weight(int& v) const{

	typename vector<W>::const_iterator it = std::max_element(w_.begin(), w_.end());
	v = it - w_.begin();
	return *it;
}

///////////////
//
// I/O
//
//////////////

template<class Graph_t, class W>
int Base_Graph_W<Graph_t, W>::read_dimacs(const string& filename){
/////////////////////////////
// Reads an unweighted simple directed graph in dimacs format
// 'c' comment
// 'p' next numbers in the line are size and number of edges
// 'e' next in the lines is an edge
// 
// RETURN VALUE: 0 if correct, -1 in case of error (no memory allocation)
//
// Extension (9/10/16): Will aways attempt to read weights for nodes from 
//						filename.w file
//
// Extension (6/09/17): Reading weights in DIMACS format (n <V> <W(V)>)
//////////////////////
	
	int size, nEdges, v1, v2, edges=0;
	W wv=0.0;
	char token[10]; char line [250]; char c;
	
	fstream f(filename.c_str());
	if(!f){
		LOG_ERROR("read_dimacs-File could not be opened reading DIMACS format-Base_Graph_W<Graph_t, W>");
		clear();
		return -1;
	}

	//read header
	if(::gio::dimacs::read_dimacs_header(f, size, nEdges)==-1){
		clear(); f.close(); return -1;
	}	
	
	init(size);
	::gio::read_empty_lines(f);

	//read weights format n <x> <w> if they exist
	c=f.peek();
	if(c=='n' || c=='v' ){																						/* 'v' format is used by Zavalnij in evil_W benchmark */
		//LOG_INFO("Base_Graph_W<Graph_t, W>::read_dimacs-DIMACS weights found in file: excluding other weights");
		for(int n=0; n<g_.number_of_vertices(); n++){
			f>>c>>v1>>wv;
			if(!f.good()){
				cerr<<"bad line related to weights"<<endl;
				clear();
				f.close();
				return -1;
			}
			w_[v1-1]=wv;
			if(wv==0){
				cerr<<filename<<":wrong header for edges reading DIMACS format"<<endl;
				clear();
				f.close();
				return -1;
			}
			f.getline(line, 250);  //remove remaining part of the line
		}

		//LOG_INFO("Base_Graph_W<Graph_t, W>::read_dimacs-Weights read correctly from DIMACS file"<<filename);
		::gio::read_empty_lines(f);
	}else{
		LOG_INFO("read_dimacs-no weights in file, initially assuming unit weights 1.0-Base_Graph_W<Graph_t, W>");
	}

/////////////////////	
//read edges

	//read the first edge to determine the type of input
	f.getline(line, 250); 
	stringstream sstr(line);
	int nw=com::counting::count_words(sstr.str());
	if(nw!=3){
		LOGG_ERROR(filename , "read_dimacs()-wrong edge line format reading DIMACS format-Base_Graph_W<Graph_t, W>:");
		clear(); f.close(); return -1;
	}
	
	//parse the first edge
	if(nw==3){
		sstr>>c>>v1>>v2;	
		g_.add_edge(v1-1,v2-1);
	}
	
	//remaining edges
	for(int e=1; e<nEdges; e++){
		f>>c;
		if(c!='e'){
			cerr<<filename<<":wrong header for edges reading DIMACS format"<<endl;
			clear();
			f.close();
			return -1;
		}
		//add bidirectional edge	
		f>>v1>>v2;
		g_.add_edge(v1-1,v2-1);
			
		f.getline(line, 250);  //remove remaining part of the line
	}
	f.close();
	
	//name (removes path)
	g_.set_name(filename);

	//extension for weighted files (9/10/16)
	string str(filename);					//filename contains the full path
	

	//////////////////////////////////////////////////////////////////////////////
	//read weights from external files if necessary (deprecated 2023/09/26)
	if(w_.empty()){
			

#ifdef FILE_EXTENSION_W	
		str+=".w";
#elif FILE_EXTENSION_D
		str+=".d";
#elif FILE_EXTENSION_WWW
		str+=".www";
#else
		str+=".nofile";
#endif
		
		read_weights(str);
	}
	///////////////////////////////////////////////////////////////////////////////////
return 0;
}

template<class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 
	
	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!g_.get_name().empty())
		o<<"\nc "<<g_.get_name().c_str()<<endl;

	//tamaño del grafo
	const int NV=g_.number_of_vertices();
	o<<"p edge "<<NV<<" "<<g_.number_of_edges()<<endl<<endl;

	//write DIMACS nodes n <v> <w>
	//if (is_weighted_v()){
		for(int v=0; v<<NV; v++){
			o<<"n "<<v+1<<" "<<get_w(v)<<endl;
		}
	//}
	
	//write edges
	for(int v=0; v<NV; v++){
		for(int w=0; w<NV; w++){
			if(v==w) continue;
			if(g_.is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<"e "<<v+1<<" "<<w+1<<endl;			//1 based vertex notation dimacs
			
		}
	}

	return o;
}

template<class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_data(bool lazy, std::ostream& o, bool endl) {
	g_.print_data(lazy, o, false);
	o<<" w";
	if(endl) o<<std::endl;	
	return o;
}


template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (com::stack_t<int>& lv, ostream& o) const{

	for(int i=0; i<lv.pt; i++){
		o<<"["<<lv.get_elem(i)<<","<<w_[lv.get_elem(i)]<<"] ";
	}
	o<<"("<<lv.pt<<")"<<endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (int* lv, int size, ostream& o) const{

	for(int i=0; i<size; i++){
		o<<"["<<lv[i]<<","<<w_[lv[i]]<<"] ";
	}
	o<<"("<<size<<")"<<endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (com::stack_t<int>& lv, const vint& mapping, ostream& o) const{
////////////////
//
	for(int i=0; i<lv.pt; i++){
		o<<"["<<mapping[lv.get_elem(i)]<<","<<w_[mapping[lv.get_elem(i)]]<<"] ";
	}
	o<<"("<<lv.pt<<")"<<endl;
	return o;
}
			

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (vint& lv, ostream& o) const{
	for(int i=0; i<lv.size(); i++){
		o<<"["<<lv[i]<<","<<w_[lv[i]]<<"] ";
	}
	o<<"("<<lv.size()<<")"<<endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (_bbt& bbsg, ostream& o) const{
	bbsg.init_scan(bbo::NON_DESTRUCTIVE);										/* CHECK sparse graphs */
	while(true){
		int v=bbsg.next_bit();
		if(v==EMPTY_ELEM) break;
		o<<"["<<v<<","<<w_[v]<<"] ";
	}
	o<<"("<<bbsg.popcn64()<<")"<<endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (ostream& o, bool show_v) const{

	const int NV=g_.number_of_vertices();
	if(show_v){
		for(int i=0; i<NV; i++){
			o<<"["<<i<<","<<w_[i]<<"] ";
		}
		o<<endl;
	}else{
		com::stl::print_collection<vector<W>>(w_, o, true);
	}
	return o;
}

template<class Graph_t, class W>
inline
int Base_Graph_W<Graph_t, W>::read_weights(const string& filename){
///////////////
// reads weights from an external file (only weights)
// 
// REMARKS
// I.assumes the format are separated (line, space,...) ordered weights
// i.e. (5 6 7 ...)->w(1)=5, w(2)=6, w(3)=7,...
// II. allocates memory for m_wv
	
	
	ifstream f(filename.c_str());
	if(f.fail()){
		LOG_DEBUG("could not open weight file: " ,filename);
		return -1;
	}	

	LOG_INFO("reading vertex weights from: " , filename.c_str()); 

	//allocate memory
	const int NV=g_.number_of_vertices();
	w_.assign(NV, 0.0);
	
	//simple reading: assumes no empty lines and no
	double w=-1.0;
	for(int i=0; i<NV; i++){
		f>>w;
		if(f.fail()){
			LOG_ERROR("bad reading of weights in:" , filename);
			return -1;		
		}
		w_[i]=w;		/*memory is already allocated for m_w*/
	}
	f.close();

	//check to screen
	/*for(int i=0; i<m_size; i++){
		cout<<m_wv[i]<<" ";
	}
	cout<<endl;*/
		
	return 0; 
}


template<class W>
inline
ostream& Graph_W<ugraph, W>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 
	
	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!ptype::g_.get_name().empty())
		o<<"\nc "<< ptype::g_.get_name().c_str()<<endl;

	//tamaño del grafo
	const int NV= ptype::g_.number_of_vertices();
	o<<"p edge "<<NV<<" "<< ptype::g_.number_of_edges()<<endl<<endl;

	//write DIMACS nodes n <v> <w>
	//if (is_weighted_v()){
		for(int v=0; v<NV; v++){
			o<<"n "<<v+1<<" "<< ptype::get_w(v)<<endl;
		}
	//}
	
	//write edges
	for(int v=0; v<NV-1; v++){
		for(int w=v+1; w<NV; w++){
			//if(v==w) continue;
			if(ptype::g_.is_edge(v,w) )					//O(log) for sparse graphs: specialize
					o<<"e "<<v+1<<" "<<w+1<<endl;			//1 based vertex notation dimacs
			
		}
	}

	return o;
}


#endif