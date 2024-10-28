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
#include "formats/dimacs_reader.h"
#include "./formats/mmx_reader.h"
#include "./formats/edges_reader.h"
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

/////////////////////////
// automatic generation mode
#define WEIGHT_AUTO_GEN_MODE						200			// wi = (i % WEIGHT_AUTO_GEN_MODE) + 1, i:1...N, WEIGHT_AUTO_GEN_MODE=1 is the unweighted case							

using namespace std;											/* not use in headers! */

template<class Graph_t, class W>
class Base_Graph_W {	
///////////////////////
//
// Base class used to be able to specialize functions
// according to the type of graph only
//
// The user type class is Graph_W! (see below) 
//
///////////////////////

protected:	
	Graph_t m_g;
	vector<W> m_w;																				//vector of weights (currently automated reading for DIMACS graphs in an auxiliary graph with extension *.w or *.d)

public:
	typedef typename Graph_t::_bbt _bbt;
	typedef W _wt;																				//external type name alias used in the framework
		
	Base_Graph_W						():	m_g(){};											//does not allocate memory
	Base_Graph_W						(vector<W>& lw);										//empty graph structure with vertex weights
	Base_Graph_W						(Graph_t& gout, vector<W>& lw):m_g(gout),m_w(lw){}	
	Base_Graph_W						(Graph_t& gout) :m_g(gout), m_w(gout.number_of_vertices(), 1) {}				//assumes unit weighted graph
	Base_Graph_W						(int nV, W val=1.0):m_g(){init(nV, val);}											//creates empty graph with size vertices	
	Base_Graph_W						(std::string filename);									//TODO@**** (read weighted ASCII file or generate weights using formula)
virtual	~Base_Graph_W					(){clear(); /* not necessary */};		

/////////////
// setters and getters
	void set_w							(int v, W val)				{m_w.at(v)=val;}				
	void set_w							(W val=1.0)					{m_w.assign(m_g.number_of_vertices(), val);}		
	int	 set_w							(vector<W>& lw);										
	

//	Graph_t& graph() { return m_g; }
	Graph_t& graph						()							{return m_g;}	
	W get_w								(int v)		const			{return m_w[v];}	
const vector<W>& get_weights			()			const			{return m_w;}	
	vector<W>& get_weights				()							{return m_w;}	
	
	W maximum_weight					(int& v)	const;	
	
const _bbt& get_neighbors				(int v)		const			{ return m_g.get_neighbors(v); }
 	_bbt& get_neighbors					(int v)						{ return m_g.get_neighbors(v); }
	std::string get_name				()			const			{ return m_g.get_name(); }
	void set_path						(std::string path_name)		{ m_g.set_path(path_name); }
	std::string get_path				()			const			{ return m_g.get_path(); }

///////////////////////////
//weight generation
	int gen_w							(int MODE= WEIGHT_AUTO_GEN_MODE);						//automatic generation of weights TODO@
	bool is_unit_weights				();

//////////////////////////
// memory allocation 
  	int init							(int nV, W val=1.0);									//allocates memory for n vertices, assigns val as weight to all vertices (default unit weights)
	void clear							();														//deallocates memory if required

/////////////////////////
//useful interfaces for the layered graph
	int number_of_vertices()  const { return m_g.number_of_vertices(); }
	int number_of_edges()   { return m_g.number_of_edges(); }	
	void add_edge(int v, int w){m_g.add_edge(v, w);}
	void set_name(string str, bool separate_path = false) {m_g.set_name(str, separate_path);}
	bool is_edge(int v, int w)  const { return m_g.is_edge(v, w); }
	double density()  { return m_g.density(); }

////////////
// I/O
public:
	int read_dimacs						(const string& filename);			
	ostream& write_dimacs				(ostream& o=std::cout);								//CHECK!
		
 virtual  ostream& print_data			(bool lazy=true, std::ostream& o=std::cout, bool endl=true);	
	int read_weights					(const string& filename);	
	ostream& print_weights				(ostream& o= std::cout, bool show_v=true) const; 
	ostream& print_weights				(_bbt & bbsg, ostream& o= std::cout) const;
	ostream& print_weights				(vint& lnodes, ostream& o= std::cout) const;
	ostream& print_weights				(com::stack_t<int>& lv, ostream& o= std::cout) const;
	ostream& print_weights				(com::stack_t<int>& lv, const vint& mapping, ostream& o= std::cout) const;
	ostream& print_weights				(int* lv, int size, ostream& o= std::cout) const;
};


template<class Graph_t, class W>
class Graph_W: public Base_Graph_W<Graph_t, W> {

public:
	typedef Graph_t _gt;

public:
	Graph_W	()										:Base_Graph_W<Graph_t, W>(){}							//does not allocate memory
	Graph_W	(vector<W>& lw)							:Base_Graph_W<Graph_t, W>(lw){}							//empty graph structure with vertex weights
	Graph_W	(Graph_t& g_out, vector<W>& lw)			:Base_Graph_W<Graph_t, W>(g_out, lw){}
	Graph_W	(int nV, W val=1.0)						:Base_Graph_W<Graph_t, W>(nV, val){}					//creates empty graph with size vertices	
	Graph_W	(std::string filename)					:Base_Graph_W<Graph_t, W>(filename){}
};

////////////////////////////////////
// main specialization- target class for undirected graphs

template<class W>
class Graph_W<ugraph, W>: public Base_Graph_W<ugraph, W> {
public:
	typedef  Base_Graph_W<ugraph, W>  _mypt;
	typedef  ugraph						_gt;
	typedef  W							_wt;

public:
	Graph_W	()										:Base_Graph_W<ugraph, W>(){}							  //does not allocate memory
	Graph_W	(vector<W>& lw)							:Base_Graph_W<ugraph, W>(lw){}							  //empty graph structure with vertex weights
	Graph_W	(ugraph& g_out, vector<W>& lw)			:Base_Graph_W<ugraph, W>(g_out, lw){}
	Graph_W	(ugraph& g_out)							:Base_Graph_W<ugraph, W>(g_out) {}						  //construction from unweighted type -> unit weights
	Graph_W	(int nV, W val=1.0)						:Base_Graph_W<ugraph, W>(nV, val){}						  //creates empty graph with size vertices	
	Graph_W	(std::string filename)					:Base_Graph_W<ugraph, W>(filename){}

/////////////
//useful interface-specific for undirected weighted graphs
	int max_degree_of_graph	()													{ return _mypt::m_g.max_degree_of_graph(); }
	int degree				()													{ return _mypt::m_g.degree(); }
	int degree				(int v)							const				{ return _mypt::m_g.degree(v); }
	int degree				(int v, const BitBoardN& bbn)	const				{ return _mypt::m_g.degree(v, bbn); }

	int create_complement	(Graph_W<ugraph, W>& g)			const;											  //weights (also name, path,...) are also stored in the new graph
	int create_complement	(ugraph& g)					    const;											  //weights (also name, path,...) are lost

///////////
//override
	
	ostream& write_dimacs (ostream& o=std::cout);								
};

template<class W>
inline
int Graph_W<ugraph, W>::create_complement(Graph_W<ugraph, W>& g) const {
	/////////////////////
	//TODO@ name, path info is lost

	g.set_name(this->get_name());
	g.set_path(this->get_path());
	g.get_weights() = _mypt::m_w;
	_mypt::m_g.create_complement(g.graph());

	return 0;
}

template<class W>
inline
int Graph_W<ugraph, W>::create_complement(ugraph& g) const {

	_mypt::m_g.create_complement(g);
	return 0;
}


template<class Graph_t, class W>
Base_Graph_W<Graph_t, W>::Base_Graph_W(string filename){
	read_dimacs	(filename);							
}

template<class Graph_t, class W>
Base_Graph_W<Graph_t, W>::Base_Graph_W(vector<W>& lw){
	try{
		m_g.init(lw.size());
		m_w=lw;
	}catch(...){
		LOG_ERROR("Base_Graph_W<T, W>::init(vector<W>& lw)-error during memory graph allocation");
		return;
	}	
}

template<class Graph_t, class W>
inline int Base_Graph_W<Graph_t, W>::gen_w(int MODE){
//////////////////////////
// weight generation: (v + 1) % MODE	[Pullan 2008-MODE=200]

	m_w.clear();
	const int NV = m_g.number_of_vertices();
	for (int i = 0; i < NV; i++) {
		m_w.push_back((i + 1) % MODE + 1);
	}
	return 0;
}

template<class Graph_t, class W>
inline bool Base_Graph_W<Graph_t, W>::is_unit_weights()
{
	for (int v : m_w) {
		if (v != 1) return false;
	}
	return true;
}

template<class Graph_t, class W>
int Base_Graph_W<Graph_t, W>::init(int size, W val){
///////////////////////////
// Allocates memory for the empty graph (deallocates previous)
	clear();
		 
	try{
		m_g.init(size);
		m_w.assign(size, val);
	}catch(...){
		LOG_ERROR("Base_Graph_W<Graph_t, W>::init()-error during memory graph allocation");
		return -1;
	}
		
return 0;
}

template<class Graph_t, class W>
void Base_Graph_W<Graph_t, W>::clear	(){
	m_g.clear();
	m_w.clear();
}

template <class Graph_t, class W>
inline
int	Base_Graph_W<Graph_t,W >::set_w (vector<W>& w_out){
/////////////
// sets the weights of all vertices for the given graph structure

	if(m_g.number_of_vertices()!=w_out.size()){
		LOG_ERROR("bizarre number of weights "<< w_out.size() <<" in a "<< m_g.number_of_vertices() <<" vertex graph-Base_Graph_W<Graph_t,W >::set_weights()");
		return -1;
	}

	m_w=w_out;
	return 0;
}

template <class Graph_t, class W>
inline
W Base_Graph_W<Graph_t, W>::maximum_weight(int& v) const{
/////////////
// param@v: vertex with maximum weight (REGraph_tURNED)
// 
// RETURNS maximum weight value

	typename vector<W>::const_iterator it=std::max_element(m_w.begin(), m_w.end());
	v=it-m_w.begin();
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
	if(DIMACS_READER::read_dimacs_header(f, size, nEdges)==-1){
		clear(); f.close(); return -1;
	}	
	
	init(size);
	DIMACS_READER::read_empty_lines(f);

	//read weights format n <x> <w> if they exist
	c=f.peek();
	if(c=='n' || c=='v' ){																						/* 'v' format is used by Zavalnij in evil_W benchmark */
		//LOG_INFO("Base_Graph_W<Graph_t, W>::read_dimacs-DIMACS weights found in file: excluding other weights");
		for(int n=0; n<m_g.number_of_vertices(); n++){
			f>>c>>v1>>wv;
			if(!f.good()){
				cerr<<"bad line related to weights"<<endl;
				clear();
				f.close();
				return -1;
			}
			m_w[v1-1]=wv;
			if(wv==0){
				cerr<<filename<<":wrong header for edges reading DIMACS format"<<endl;
				clear();
				f.close();
				return -1;
			}
			f.getline(line, 250);  //remove remaining part of the line
		}

		//LOG_INFO("Base_Graph_W<Graph_t, W>::read_dimacs-Weights read correctly from DIMACS file"<<filename);
		DIMACS_READER::read_empty_lines(f);
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
		LOG_ERROR(filename<<"read_dimacs()-wrong edge line format reading DIMACS format-Base_Graph_W<Graph_t, W>:");
		clear(); f.close(); return -1;
	}
	
	//parse the first edge
	if(nw==3){
		sstr>>c>>v1>>v2;	
		m_g.add_edge(v1-1,v2-1);
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
		m_g.add_edge(v1-1,v2-1);
			
		f.getline(line, 250);  //remove remaining part of the line
	}
	f.close();
	
	//name (removes path)
	m_g.set_name(filename, true);

	//extension for weighted files (9/10/16)
	string str(filename);					//filename contains the full path
	

	//////////////////////////////////////////////////////////////////////////////
	//read weights from external files if necessary (deprecated 2023/09/26)
	if(m_w.empty()){
			

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
	if(!m_g.get_name().empty())
		o<<"\nc "<<m_g.get_name().c_str()<<endl;

	//tamaño del grafo
	const int NV=m_g.number_of_vertices();
	o<<"p edge "<<NV<<" "<<m_g.number_of_edges()<<endl<<endl;

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
			if(m_g.is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<"e "<<v+1<<" "<<w+1<<endl;			//1 based vertex notation dimacs
			
		}
	}

	return o;
}

template<class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_data(bool lazy, std::ostream& o, bool endl) {
	m_g.print_data(lazy, o, false);
	o<<" w";
	if(endl) o<<std::endl;	
	return o;
}


template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (com::stack_t<int>& lv, ostream& o) const{

	for(int i=0; i<lv.pt; i++){
		o<<"["<<lv.get_elem(i)<<","<<m_w[lv.get_elem(i)]<<"] ";
	}
	o<<"("<<lv.pt<<")"<<endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (int* lv, int size, ostream& o) const{

	for(int i=0; i<size; i++){
		o<<"["<<lv[i]<<","<<m_w[lv[i]]<<"] ";
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
		o<<"["<<mapping[lv.get_elem(i)]<<","<<m_w[mapping[lv.get_elem(i)]]<<"] ";
	}
	o<<"("<<lv.pt<<")"<<endl;
	return o;
}
			

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (vint& lv, ostream& o) const{
	for(int i=0; i<lv.size(); i++){
		o<<"["<<lv[i]<<","<<m_w[lv[i]]<<"] ";
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
		o<<"["<<v<<","<<m_w[v]<<"] ";
	}
	o<<"("<<bbsg.popcn64()<<")"<<endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (ostream& o, bool show_v) const{

	const int NV=m_g.number_of_vertices();
	if(show_v){
		for(int i=0; i<NV; i++){
			o<<"["<<i<<","<<m_w[i]<<"] ";
		}
		o<<endl;
	}else{
		com::stl::print_collection<vector<W>>(m_w, o, true);
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
		LOG_DEBUG("could not open weight file: " <<filename);
		return -1;
	}	

	LOG_INFO("reading vertex weights from: "<<filename.c_str()); 

	//allocate memory
	const int NV=m_g.number_of_vertices();
	m_w.assign(NV, 0.0);
	
	//simple reading: assumes no empty lines and no
	double w=-1.0;
	for(int i=0; i<NV; i++){
		f>>w;
		if(f.fail()){
			LOG_ERROR("bad reading of weights in:" <<filename);
			return -1;		
		}
		m_w[i]=w;		/*memory is already allocated for m_w*/
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
	if(!_mypt::m_g.get_name().empty())
		o<<"\nc "<< _mypt::m_g.get_name().c_str()<<endl;

	//tamaño del grafo
	const int NV= _mypt::m_g.number_of_vertices();
	o<<"p edge "<<NV<<" "<< _mypt::m_g.number_of_edges()<<endl<<endl;

	//write DIMACS nodes n <v> <w>
	//if (is_weighted_v()){
		for(int v=0; v<NV; v++){
			o<<"n "<<v+1<<" "<< _mypt::get_w(v)<<endl;
		}
	//}
	
	//write edges
	for(int v=0; v<NV-1; v++){
		for(int w=v+1; w<NV; w++){
			//if(v==w) continue;
			if(_mypt::m_g.is_edge(v,w) )					//O(log) for sparse graphs: specialize
					o<<"e "<<v+1<<" "<<w+1<<endl;			//1 based vertex notation dimacs
			
		}
	}

	return o;
}


#endif