//graph_gen.h header for Erdos-Renyi sparse and non sparse bitstring unidrected graph generation 
//author: pablo san segundo

#ifndef __GRAPH_GEN_H__
#define __GRAPH_GEN_H__

#include <iostream>
#include <sstream>
#include <random>					//uses uniform int random generator

#include "graph.h"
#include "utils/common.h"
	
using namespace std;

//////////////////////////////////////////////////////////////
constexpr int DEFAULT_WEIGHT_MODULUS = 200;							//for modulus weight generation  [Pullman 2008]					
//////////////////////////////////////////////////////////////


struct random_attr_t {
	friend ostream & operator<<(ostream& o, const random_attr_t& r) {o<<"["<<r.nLB<<" "<<r.nUB<<" "<<r.pLB \
										<<" "<<r.pUB<<" "<<r.nRep<<" "<<r.incN<<" "<<r.incP<<"]"<<endl; return o;}
	random_attr_t(int nLB, int nUB, double pLB, double pUB, int nRep, int incN, double incP) { 
					set(nLB, nUB, pLB, pUB, nRep, incN, incP);}
	void set(int nLB, int nUB, double pLB, double pUB, int nRep, int incN, double incP){
		this->nLB=nLB; this->nUB=nUB; this->pLB=pLB; this->pUB=pUB;  this->nRep=nRep; this->incN=incN; 
		this->incP=incP;
	}

	int nLB;
	int nUB;
	double pLB;
	double pUB;
	int nRep;
	int incN;
	double incP;
};

template<class Graph_t>
class RandomGen{
//////////////////
//non sparse graph generators
//////////////////
public:
	static int create_ugraph (Graph_t& g, int size, double p);
	static int create_graph (Graph_t& g, int size, double p);
	
	static int create_ugraph_benchmark (const string& path, const random_attr_t& rd);	
	static int create_graph_benchmark (const string& path, const random_attr_t& rd);
};

template<int logSizeTable=35000>
class SparseRandomGen{
////////////////////////
// Specific massive sparse uniform graph generator

	static const int MAX_RAND=logSizeTable;		
public:
	int create_ugraph (sparse_ugraph& g, int n, double p);
	SparseRandomGen(){init_log_table();}
			
private:
	void init_log_table();					//variable precission at compile time
	double m_log[MAX_RAND];
inline	static void decode	(BITBOARD edge_index, unsigned int & i, unsigned int &j,  unsigned int n);
};

/////////////////
//
// Class WeightGen
//
////////////////

template<class Graph_t>
class WeightGen{
////////////////////
// Weight generation given a template graph Graph_t (with structure) as input

public:
	enum type_t {WMOD=0, WDEG};

//non sparse generators
	static int create_wgraph (Graph_t& g, type_t, int wmod = DEFAULT_WEIGHT_MODULUS);
	static int create_wgraph (Graph_t& g, type_t, string FILE_EXTENSION, string FILE_PATH="",  int wmod= DEFAULT_WEIGHT_MODULUS);
};


////////////////
//specialization for new weighted graph type-testing. Refactor in the future
template<class Graph_t>
inline
int WeightGen<Graph_t>::create_wgraph (Graph_t& g, type_t type, int wmod){
/////////////////////////////////
// adds weight following two criteria:
//
// 1. WDEG- weight is the degree of the vertex
// 2. WMOD- weight is the index%wmod (default wmod=200)
// date of creation: 16/01/19
		
	const int NV=g.graph().number_of_vertices();
	g.set_w(0);

	if(type==WDEG){
		for(int v=0; v<NV; v++){
			g.set_w(v,g.graph().degree(v));
		}
	}else if(type==WMOD){
		for(int v=0; v<NV; v++){
			g.set_w(v,(1 + ((v+1) % wmod) ));
		}
	}else{
		LOG_INFO("WeightGen<Graph_t>::create_wgraph()-incorred weight generation mode");
	}

	return 0;	
}


template<class Graph_t>
inline
int WeightGen<Graph_t>::create_wgraph (Graph_t& g, type_t type, string FILE_EXTENSION, string FILE_PATH, int wmod){
/////////////////////////////////
// adds weight following two criteria:
//
// 1. WDEG- weight is the degree of the vertex
// 2. WMOD- weight is the index%wmod (default wmod=200)
//
// copies weight to file with FILE_EXTENSION appended to the current name of graph
// date of creation: 5/8/17
//
// RETURNS 0-ok, -1 Err

	const int NV=g.graph().number_of_vertices();
	g.set_w(1.0);
	
	if(type==WDEG){
		for(int v=0; v<NV; v++){
			g.set_w(v,g.graph().degree(v));
		}
	}else if(type==WMOD){
		for(int v=0; v<NV; v++){
			g.set_w(v,(1 + ((v+1) % wmod) ));
		}
	}else{
		LOG_INFO("WeightGen<Graph_t>::create_wgraph()-incorred weight generation mode");
	}
	

	//copies weights to file
	ofstream f;
	
	
	string path=FILE_PATH;
	if(!path.empty()){ 	com::dir::append_slash(path); }
	string filename=path + g.graph().get_name() + FILE_EXTENSION;
	f.open(filename, ofstream::out);
	if(!f){
		LOGG_INFO("WeightGen<Graph_t>::create_wgraph ()--cannot write weights to file ", filename);
		return -1;
	}


	//g.write_weights(f);
	g.print_weights(f, false);

	f.close();
	return 0;	
}

/////////////////
//
// Class EdgeWeightGen
//
// TODO@refactor to the new type (22/01/19)
//
////////////////

template<class Graph_t>
class EdgeWeightGen{
////////////////////
// Weight generation given a template graph Graph_t (with structure) as input

public:
	enum type_t {WMOD=0};

//non sparse generators
	static int create_wgraph (Graph_t& g, type_t, int wmod=200);
	static int create_wgraph (Graph_t& g, type_t, string FILE_EXTENSION, string FILE_PATH="",  int wmod=200);
};

template<class Graph_t>
inline
int EdgeWeightGen<Graph_t>::create_wgraph (Graph_t& g, type_t type, int wmod){
/////////////////////////////////
// adds edge-weights to EXISTING EDGES according to the following criteria:
//
// 1. WMOD- edge-weight is the standard (v + w) mod(wmod) + 1 weight used (Pullham and others)
//    Note: v and w MUST be 1 based

//
// RETURNS 0-ok, -1 Err
//
	
	const int NV= g.graph().number_of_vertices();
	g.set_we(Graph_t::NOWT);

	if(type==WMOD){
		g.gen_modulus_weights();
	}else{
		LOG_INFO("bad weight generation mode - EdgeWeightGen<Graph_t>::create_wgraph");
		LOG_INFO("all weights set to empty value");
		return -1;
	}	
	
	return 0;
}

template<class Graph_t>
inline
int EdgeWeightGen<Graph_t>::create_wgraph (Graph_t& g, type_t type, string FILE_EXTENSION, string FILE_PATH, int wmod){
/////////////////////////////////
// adds edge-weights to EXISTING EDGES according to the following criteria:
//
// 1. WMOD- edge-weight is the standard (v+w) mod(wmod) +1 weight used (Pullham and others)
//
// copies edge-weights to file with FILE_EXTENSION appended to the current name of graph
// date of creation: 20/07/18
//
// RETURNS 0-ok, -1 Err
//
// COMMENTS: only for undirected graphs because of main loop!

	g.init_we();
	int NV=g.number_of_vertices();

	if(type==WMOD){
		g.gen_modulus_weights();		
	}else{
		LOG_INFO("WeightGen<Graph_t>::create_wgraph()-incorred weight generation mode");
	}
	
	//copies weights to file
	ofstream f;
		
	string path=FILE_PATH;
	if(!path.empty()){ 	com::dir::append_slash(path); }
	string filename=path + g.get_name() + FILE_EXTENSION;
	f.open(filename, ofstream::out);
	if(!f){
		LOG_INFO("WeightGen<Graph_t>::create_wgraph ()--cannot write weights to file "<<filename);
		return -1;
	}
	
	g.write_edge_weights(f);

	f.close();
	return 0;	
}

template<int MAX_RAND>
inline
void SparseRandomGen<MAX_RAND>::decode	(BITBOARD edge_index, unsigned int & i, unsigned int &j, unsigned int n){
/////////////////////////////
// decodes undimensional edge index to upper triangle of adjacency matrix 
//
// REMARKS: From personal communication with [Fast Random graph generation, 2011] authors

	double a=sqrt((pow((2*n)-1.0,2)-8*(edge_index+1)));
	BITBOARD r=ceil( (((2*n)-1.0-a)/2-1.0 )    );
	BITBOARD c=edge_index+1+r-(r*(2*n-r-1)/2);			//type is important else c overflows unsigned int type for very large graphs

	//cast back to unsigned int
	i=(unsigned int)r;
	j=(unsigned int)c;
	
};

template<int logSizeTable>
void SparseRandomGen<logSizeTable>::init_log_table(){
//16_bit table for logarithms
	
	for(int i=0; i<MAX_RAND; i++){
			SparseRandomGen::m_log[i]=log(i/(double)MAX_RAND);
	}
}

template<int logSizeTable>
int SparseRandomGen<logSizeTable>::create_ugraph (sparse_ugraph& g, int n, double p){
		
	BITBOARD m=((BITBOARD)n*(n-1)/2);			//max num edges
	BITBOARD e=0;								//**check
	double logp=log(1-p);
	BITBOARD skip;
	unsigned int i=0, j=0;					
	int num=0;									//optional index which counts edges added to the graph

	default_random_engine generator;
	uniform_int_distribution<int> distribution(0,MAX_RAND-1);
	generator.seed((unsigned) time(NULL));
	
	g.init(n);
	do{
		int l=ceil(m_log[distribution(generator)]/logp)-1; 
		(l>0)? skip=l : skip=0;
		if((e+=++skip)>=m) 
			break;

		decode(e,i,j,n);					//perfectly valid option and similar in O
		/*while(skip>0){
		if( (m_n-1-j)>=skip ){
		j+=skip;
		break;
		}else{
		i++;
		skip-=(m_n-1-j);
		j=i;
		}
		}*/

		num++;
		g.add_edge(i,j);					//O(log) because it is a sparse graph
		
	}while(e<m);

	//opional
	/*stringstream sstr("");
	sstr<<m<<":"<<"["<<num<<"]"<<std::endl;
	cout<<sstr.str();*/
	
return 0;
}

template<class Graph_t>
int RandomGen<Graph_t>::create_ugraph (Graph_t& g, int n, double p){
///////////////////////////
// Generates uniform simple random undirected graph  (no self loops)
// with name r<N>_0.<P>.txt
//
// RETURNS: -1 if ERROR, 0 if OK
// Notes: no self loops

	//ASSERTS
	if(n<=0){
		cerr<<"wrong size for random graph"<<std::endl;
		return -1;
	}

	if(g.init(n)){
		cerr<<"error during allocation: undirected random graph not generated"<<std::endl;
		return -1;
	}


	//gen undirected edges
	for(int i=0; i<n-1; i++){
		for(int j=i+1; j<n; j++){
			if(::com::rand::uniform_dist(p)){
				g.add_edge(i,j);
			}
		}
	}

	//name random
	std::stringstream sstr;
	sstr<<"r"<<n<<"_"<<p<<".txt";
	g.set_name(sstr.str());

return 0;
}

template<class Graph_t>
int RandomGen<Graph_t>::create_graph (Graph_t& g, int n, double p){
///////////////////////////
// Generates uniform simple random directed graph  (no self loops)
// with name r<N>_0.<P>.txt
//
// RETURNS: -1 if ERROR, 0 if OK
// Notes: no self loops

	//ASSERTS
	if(n<=0){
		cerr<<"wrong size for random graph"<<std::endl;
		return -1;
	}

	if(g.init(n)){
		cerr<<"error during allocation: directed random graph not generated"<<std::endl;
		return -1;
	}
	
	//gen undirected edges
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			if(i==j) continue;				//remove self-loops
			if(com::rand::uniform_dist(p)){
				g.add_edge(i,j);
			}
		}
	}

	//name random
	std::stringstream sstr;
	sstr<<"r"<<n<<"_"<<p<<".txt";
	g.set_name(sstr.str());

return 0;
}

template<class Graph_t>
int RandomGen<Graph_t>::create_ugraph_benchmark (const string& path, const random_attr_t& rd){
///////////////////////////
// Generates full benchmark, each file with notation r<N>_0.<P>.txt
//
// REMARKS: could be written directly to the stream without using graph object (*** TO DO)

	std::ostringstream o;
	Graph_t g;
	std::ofstream f;
	
	//add slash '/' at the end to the path if required
	string mypath(path);
	com::dir::append_slash(mypath);
	

	for(int i=rd.nLB ; i<=rd.nUB; i+=rd.incN){
		for(double j=rd.pLB ; j<=rd.pUB; j+=rd.incP){
			for(int k=0; k<rd.nRep; k++){
				create_ugraph(g,i,j);
				o.str("");
				o<<mypath.c_str()<<"r"<<i<<"_"<<j<<"_"<<k<<".txt";		
				f.open(o.str().c_str());
				if(!f){
					cerr<<"Error al generar el archivo"<<endl;
					return -1;
				}
				g.write_dimacs(f);		
				f.close();
			}
		}
	}
return 0;		//OK
}

template<class Graph_t>
int RandomGen<Graph_t>::create_graph_benchmark (const string& path, const random_attr_t& rd){
///////////////////////////
// Generates full benchmark, each file with notation r<N>_0.<P>.txt
//
// REMARKS: could be written directly to the stream without using graph object 

	std::ostringstream o;
	Graph_t g;
	std::ofstream f;
	
	//add slash '/' at the end to the path if required
	string mypath(path);
	com::dir::append_slash(mypath);
	

	for(int i=rd.nLB ; i<=rd.nUB; i+=rd.incN){
		for(double j=rd.pLB ; j<=rd.pUB; j+=rd.incP){
			for(int k=0; k<rd.nRep; k++){
				create_graph(g,i,j);
				o.str("");
				o<<mypath.c_str()<<"r"<<i<<"_"<<j<<"_"<<k<<".txt";			
				f.open(o.str().c_str());
				if(!f){
					cerr<<"Error when opening file to write"<<endl;
					return -1;
				}
				g.write_dimacs(f);		
				f.close();
			}
		}
	}
return 0;		//OK
}



#endif