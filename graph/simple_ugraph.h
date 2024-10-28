/*  
 * ugraph.h file from the GRAPH library, a C++ library for bit encoded 
 * simple unweighted graphs. GRAPH stores the adjacency matrix un full 
 * but each row is encoded as a bitsrting. GRAPH is at the core of  BBMC, a 
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

#ifndef __SIMPLE_UGRAPH_H__
#define __SIMPLE_UGRAPH_H__

#include "simple_graph.h"
#include "utils/prec_timer.h"

template<class T>
class Ugraph : public Graph<T>{
	friend class GraphConversion;
public:
	typedef Graph<T> _mypt;
	typedef typename _mypt::_bbt _bbt;
	
	Ugraph						() : Graph<T>(){}									//does not allocate memory
	Ugraph						(int nVert) : Graph<T>(nVert){}						//creates empty graph with size vertices	
	//Ugraph					(const Ugraph& ug) : Graph<T>(ug){}
	Ugraph						(std::string filename);	
	Ugraph						(int nV, int* adj[], string name);
    ~Ugraph						(){_mypt::clear();};

virtual	void add_edge				(int v, int w);									//sets bidirected edge
virtual	void remove_edge			(int v, int w);
virtual	void remove_vertices		(const BitBoardN& bbn);
				
	//degree: TODO@implement bitstring conversions according to the templates properly (3/4/18)
	int degree					(int v)									const;
	int degree					(int v, const BitBoardN& bbn)			const;
	int degree					(int v, int UB, const BitBoardN& bbn)	const;  //truncated degree (14/2/2016)
	int degree					(int v, const BitBoardS& bbs)			const;
	int degree					(int v, int UB, const BitBoardS& bbs)	const;	//truncated degree  (14/2/2016)
	int degree_up				(int v, const BitBoardN& bbn)			const;  //TODO: test (27/4/2016)
	int max_degree_of_graph		()										const;	
	int max_degree_of_subgraph	(T& sg)									const;

virtual	BITBOARD number_of_edges	(bool lazy=true);								
virtual	BITBOARD number_of_edges	(const BitBoardN& )					const;			//on induced graph by list of vertices

	//graph creation	
	int create_complement		(Ugraph& g)								const;			//returns complement graph in g
	int create_induced			(Ugraph& g, int v)						const;			//returns induced subgraph on g by the neighborhood of v (29/08/2021)
	int create_induced			(Ugraph& g, std::vector<int>& lv)		const;			//returns induced subgraph on g by the set of vertices lv

	int add_vertex				(_bbt* neigh = NULL);	//***BUGGY!-***enlarges the graph with a new vertex (provided its neighborhood)

	//properties
	double density				(bool lazy=true);
	
	//I/O
	ostream& print_degrees			(std::ostream& = cout)			const;
virtual	ostream& print_edges		(std::ostream& = cout)			const;
virtual ostream& print_edges		(T& bbsg, std::ostream& = cout)	const;
virtual	ostream& print_matrix		(std::ostream& = cout)			const;

		
virtual	void write_dimacs			(ostream & filename);
virtual	void write_EDGES			(ostream & filename);
		void write_mtx				(ostream & filename);
};


template <class T>
inline
Ugraph<T>::Ugraph(int nV, int* adj[], string name) {
	_mypt::set_name(name);													//name includes the full path
	if (_mypt::init(nV) == -1) { LOG_ERROR("bizarre graph construction-Graph<T>::Graph(...), exiting... "); exit(-1); }

	for (int i = 0; i < nV-1; i++) {
		for (int j = i+1; j < nV; j++) {
			if (adj[i][j] == 1) {
				add_edge(i, j);
			}
		}
	}
}

template<class T>
Ugraph<T>::Ugraph(string filename):Graph<T>(){
	int status = _mypt::set_graph(filename);
	if (status == -1) { LOG_ERROR("error when opening file, exiting...-Ugraph<T>::Ugraph"); exit(-1); }
}

template<class T>
void Ugraph<T>::add_edge (int v, int w){
///////////////
// sets v-->w (no self loops allowed)
	if(v != w){
		_mypt::m_g[v].set_bit(w);
		_mypt::m_g[w].set_bit(v);
		_mypt::m_edges++;
	}
}

template<class T>
void Ugraph<T>::remove_edge (int v, int w){
///////////////
// removes bidirected edge
	if(v!=w){
		_mypt::m_g[v].erase_bit(w);
		_mypt::m_g[w].erase_bit(v);
		_mypt::m_edges--;
	}
}

template<class T>
int Ugraph<T>::max_degree_of_graph () const{
/////////////////////
// degree of graph 

	int max_degree=0, temp=0; 

	for(int i=0; i<_mypt::m_size;i++){
		temp=degree(i);
		if(temp>max_degree)
				max_degree=temp;
	}
return max_degree;
}

template<class T>
int Ugraph<T>::max_degree_of_subgraph (T& sg) const{
/////////////////////
// degree of subgraph 
	int max_degree=0, temp=0; 

	int v=EMPTY_ELEM;
	if(sg.init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			v=sg.next_bit();
			if(v==EMPTY_ELEM) break;

			//compute max_degree circumscribed to subgraph
			temp=degree(v, sg);
			if(temp>max_degree)
				max_degree=temp;
		}
	}
		
return max_degree;
}

template<class T>
ostream& Ugraph<T>::print_degrees (std::ostream& o) const {
	for(int i=0; i<_mypt::m_size; i++){
		o<<"deg("<<i<<")"<<":"<<degree(i)<<" ";
	}
	return o;
}

template<class T>
BITBOARD Ugraph<T>::number_of_edges (const BitBoardN& bbn) const{
////////////////
// Reads upper triangle of the adjacency matrix
// Date of creation: 17/6/10 
// Last Updated: 12/4/12  
	
	unsigned long long  edges=0;
	for(int i=0; i<_mypt::m_size-1; i++){
		if(bbn.is_bit(i)){
			for(int j=i+1; j<_mypt::m_size; j++)
				if(bbn.is_bit(j)){
					if( _mypt::m_g[i].is_bit(j) )
							edges++;
				}
		}
	}
return edges;
}

template<class T>
double Ugraph<T>::density	(bool lazy)	{
	BITBOARD max_edges=_mypt::m_size;
	max_edges*=(max_edges-1);
	return (2*number_of_edges(lazy)/(double)max_edges);
}


template<class T>
inline
BITBOARD Ugraph<T>::number_of_edges	(bool lazy) {
///////////////////
// Should be specialized for sparse graphs
	if(lazy && _mypt::m_edges !=0) 
				return _mypt::m_edges;

	BITBOARD  edges=0;
	for(int i=0; i<_mypt::m_size-1; i++){
		for(int j=i+1; j<_mypt::m_size; j++){
			if(_mypt::is_edge(i,j)){				//O(log) for sparse graphs 
				edges++;
			}
		}
	}
	_mypt::m_edges=edges;
	
return _mypt::m_edges;
}

template<> 
inline
BITBOARD Ugraph<sparse_bitarray>::number_of_edges	(bool lazy) {
/////////////
//Specialization for sparse graphs (is adjacent runs in O(log)) 
	if(lazy && Graph<sparse_bitarray>::m_edges !=0) 
				return Graph<sparse_bitarray>::m_edges;
	
	BITBOARD  edges=0;
	for(int i=0; i<Graph<sparse_bitarray>::m_size-1; i++){
		edges+=m_g[i].popcn64(i+1);		//O(log)
	}
	Graph<sparse_bitarray>::m_edges=edges;
	
return Graph<sparse_bitarray>::m_edges;
}

template<class T>
inline
ostream& Ugraph<T>::print_edges (std::ostream& o) const{
	for(int i=0; i<_mypt::m_size-1; i++){
		for(int j=i+1; j<_mypt::m_size; j++){
			if(_mypt::is_edge(i, j)){
				o<<"["<<i<<"]"<<"--"<<"["<<j<<"]"<<endl;
			}
		}
	}
	return o;
}

template<class T>
inline
ostream& Ugraph<T>::print_matrix(std::ostream& o) const {
	for (int i = 0; i < _mypt::m_size; i++) {
		for (int j =0; j < _mypt::m_size; j++) {
			if (_mypt::is_edge(i, j)) {
				o << "1";
			}
			else {
				o << "0";
			}
		}
		o << endl;
	}
	return o;
}

template<class T>
inline
ostream& Ugraph<T>::print_edges (T& bbsg, std::ostream& o) const{
	for(int i=0; i<_mypt::m_size-1; i++){
		if(!bbsg.is_bit(i)) continue;
		for(int j=i+1; j<_mypt::m_size; j++){
			if(!bbsg.is_bit(j)) continue;	
			if(_mypt::is_edge(i, j)){
				o<<"["<<i<<"]"<<"--"<<"["<<j<<"]"<<endl;
			}
		}
	}

	return o;
}

template<class T>
void Ugraph<T>::remove_vertices (const BitBoardN& bbn){
//////////////////////////
// Experimental: inefficient implementation: allocating memory twice 
	Ugraph g;
	this->_mypt::remove_vertices(bbn,g);			//allocation 1
	(*this)=g;										//allocation 2	
}


template<class T>
inline
void Ugraph<T>::write_dimacs (ostream & o) {
/////////////////////////
// writes file in dimacs format with timestamp
		
	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();

	//name
	if(!_mypt::m_name.empty())
		o<<"c "<<_mypt::m_name.c_str()<<endl;

	//tamaño del grafo
	o<<"p edge "<<_mypt::m_size<<" "<<number_of_edges(false /* recompute */)<<endl<<endl;

	//write DIMACS nodes n <v> <w>
	/*if (_mypt::is_weighted_v()){
		for(int v=0; v<_mypt::m_size; v++){
			o<<"n "<<v+1<<" "<<_mypt::get_wv(v)<<endl;
		}
	}*/

	//bidirectional edges
	for(int v=0; v<_mypt::m_size-1; v++){
		for(int w=v+1; w<_mypt::m_size; w++){
			if(_mypt::is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<"e "<<v+1<<" "<<w+1<<endl;				//1 based vertex notation dimacs
		}
	}
}

template<>
inline
void Ugraph<sparse_bitarray>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 
	
	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!Graph<sparse_bitarray>::m_name.empty())
		o<<"c "<<Graph<sparse_bitarray>::m_name.c_str()<<endl;

	//tamaño del grafo
	o<<"p edge "<<Graph<sparse_bitarray>::m_size<<" "<<number_of_edges()<<endl<<endl;
	
	//Escribir nodos
	for(int v=0; v<Graph<sparse_bitarray>::m_size-1; v++){
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p=Graph<sparse_bitarray>::m_g[v].find_pos(WDIV(v));
		if(p.second==EMPTY_ELEM) continue;					//no more bitblocks
		Graph<sparse_bitarray>::m_g[v].m_scan.bbi=p.second; 
		(p.first)? Graph<sparse_bitarray>::m_g[v].m_scan.pos= WMOD(v) : Graph<sparse_bitarray>::m_g[v].m_scan.pos=MASK_LIM;		//if bitblock contains v, start from that position onwards
		while(1){
			int w=Graph<sparse_bitarray>::m_g[v].next_bit();
			if(w==EMPTY_ELEM)
					 break;
			o<<"e "<<v+1<<" "<<w+1<<endl;	
		}	
	
	}
}

template<class T>
inline
void  Ugraph<T>::write_EDGES(ostream& o){
/////////////////////////
// writes simple unweighted grafs  in edge list format 
// note: loops are not allowed
		
	//timestamp 
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();

	//name
	if(!_mypt::m_name.empty())
		o<<"% "<<_mypt::m_name.c_str()<<endl;
	
	//write edges
	for(int v=0; v<_mypt::m_size-1; v++){
		for(int w=v+1; w<_mypt::m_size; w++){
			if(_mypt::is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<v+1<<" "<<w+1<<endl;						//1 based vertex notation dimacs
		}
	}
}


template<>
inline
void Ugraph<sparse_bitarray>::write_EDGES(ostream& o) {
/////////////////////////
// writes simple unweighted grafs  in edge list format 
// note: loops are not allowed
	
	//timestamp
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!Graph<sparse_bitarray>::m_name.empty())
		o<<"% "<<Graph<sparse_bitarray>::m_name.c_str()<<endl;

	//writes edges
	for(int v=0; v<Graph<sparse_bitarray>::m_size-1; v++){
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p=Graph<sparse_bitarray>::m_g[v].find_pos(WDIV(v));
		if(p.second==EMPTY_ELEM) continue;										//no more bitblocks
		Graph<sparse_bitarray>::m_g[v].m_scan.bbi=p.second; 
		(p.first)? Graph<sparse_bitarray>::m_g[v].m_scan.pos= WMOD(v) : Graph<sparse_bitarray>::m_g[v].m_scan.pos=MASK_LIM;			//if bitblock contains v, start from that position onwards
		while(1){
			int w=Graph<sparse_bitarray>::m_g[v].next_bit();
			if(w==EMPTY_ELEM)
					 break;
			o<<v+1<<" "<<w+1<<endl;	
		}	
	}
}

template<class T>
inline
void Ugraph<T>::write_mtx(ostream & o){
/////////////////////////
// writes simple unweighted grafs  in edge list format 
// note: loops are not allowed
		
	//header
	o<<"%%MatrixMarket matrix coordinate pattern symmetric"<<endl;
	
	//timestamp
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();
		
	//name
	if(!_mypt::m_name.empty())
		o<<"% "<<_mypt::m_name.c_str()<<endl;

	//size and edges
	_mypt::m_edges=0;																	//eliminates lazy evaluation of edge count 
	o<<_mypt::m_size<<" "<<_mypt::m_size<<" "<<number_of_edges()<<endl;
	
	//writes edges
	for(int v=0; v<_mypt::m_size-1; v++){
		for(int w=v+1; w<_mypt::m_size; w++){
			if(_mypt::is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<v+1<<" "<<w+1<<endl;						//1 based vertex notation dimacs
		}
	}
}
	
template<>
inline
void Ugraph<sparse_bitarray>::write_mtx(ostream & o){
/////////////////////////
// writes simple unweighted grafs  in edge list format 
// note: loops are not allowed
		
	//header
	o<<"%%MatrixMarket matrix coordinate pattern symmetric"<<endl;
	
	//timestamp
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();
		
	//name
	if(!Graph<sparse_bitarray>::m_name.empty())
		o<<"% "<<Graph<sparse_bitarray>::m_name.c_str()<<endl;

	//size and edges
	m_edges=0;																	//eliminates lazy evaluation of edge count 
	o<<Graph<sparse_bitarray>::m_size<<" "<<Graph<sparse_bitarray>::m_size<<" "<<number_of_edges()<<endl;
	
	//writes edges
	for(int v=0; v<Graph<sparse_bitarray>::m_size-1; v++){
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p=Graph<sparse_bitarray>::m_g[v].find_pos(WDIV(v));
		if(p.second==EMPTY_ELEM) continue;										//no more bitblocks
		Graph<sparse_bitarray>::m_g[v].m_scan.bbi=p.second; 
		(p.first)? Graph<sparse_bitarray>::m_g[v].m_scan.pos= WMOD(v) : Graph<sparse_bitarray>::m_g[v].m_scan.pos=MASK_LIM;			//if bitblock contains v, start from that position onwards
		while(1){
			int w=Graph<sparse_bitarray>::m_g[v].next_bit();
			if(w==EMPTY_ELEM)
					 break;
			o<<v+1<<" "<<w+1<<endl;	
		}	
	}	
}


template<class T>
inline
int Ugraph<T>::degree (int v) const{
	return _mypt::m_g[v].popcn64();
}

template<>
inline
int Ugraph<sparse_bitarray>::degree (int v) const{
	if(Graph<sparse_bitarray>::m_g[v].is_empty()) return 0;

return Graph<sparse_bitarray>::m_g[v].popcn64();
}

template<class T>
inline
int Ugraph<T>::degree (int v, const BitBoardN& bbn) const	{
////////////////////
// degree of v considering only adjacent vertices in subgraph passed

	int ndeg=0;
	for(int i=0; i<_mypt::m_BB;i++)
		ndeg+=bitblock::popc64(_mypt::m_g[v].get_bitboard(i)& bbn.get_bitboard(i));

return ndeg;
}

template<class T>
inline
int Ugraph<T>::degree_up (int v, const BitBoardN& bbn) const	{
////////////////////
// degree of v considering only adjacent vertices with higher index in the subgraph passed
// (applied as pivot strategy for enumeration)

	int ndeg=0;
	int nBB=WDIV(v);
	for(int i=nBB+1; i<_mypt::m_BB;i++)
		ndeg+=bitblock::popc64(_mypt::m_g[v].get_bitboard(i)& bbn.get_bitboard(i));

	//truncate the bitblock of v
	ndeg+=bitblock::popc64(bitblock::MASK_1(WMOD(v)+1,63) & _mypt::m_g[v].get_bitboard(nBB)& bbn.get_bitboard(nBB));
return ndeg;
}


template<class T>
inline
int Ugraph<T>::degree (int v, int UB, const BitBoardN& bbn) const	{
////////////////////
// degree of v in bbn subgraph. 
// if deg is greater than UB the search is stopped and UB is returned


	int ndeg=0;
	for(int i=0; i<_mypt::m_BB;i++){
		ndeg+=bitblock::popc64(_mypt::m_g[v].get_bitboard(i)& bbn.get_bitboard(i));
		if(ndeg>=UB) return UB;
	}

return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree (int v, const BitBoardN& bbn) const	{
////////////////////
// degree of v considering only adjacent vertices in subgraph passed
	
	int ndeg=0;
	for(sparse_bitarray::velem_cit it= m_g[v].begin(); it!=m_g[v].end(); ++it){
		ndeg+=bitblock::popc64(it->bb & bbn.get_bitboard(it->index));
	}

return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree (int v, int UB, const BitBoardN& bbn) const	{
////////////////////
// degree of v considering only adjacent vertices in subgraph passed
// if deg is greater than UB the search is truncated and UB is returned
	
	int ndeg=0;
	for(sparse_bitarray::velem_cit it= m_g[v].begin(); it!=m_g[v].end(); ++it){
		ndeg+=bitblock::popc64(it->bb & bbn.get_bitboard(it->index));
		if(ndeg>=UB) return UB;
	}

return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree (int v, const BitBoardS & bbs) const	{
////////////////////
// degree of v considering only adjacent vertices in subgraph passed

	int ndeg=0;
	sparse_bitarray::velem_cit itv = m_g[v].begin();
	sparse_bitarray::velem_cit itbb = bbs.begin();
	
	while(itv!=m_g[v].end() && itbb!=bbs.end()){

		if(itv->index<itbb->index){ 
			itv++;
		}else if(itv->index>itbb->index){
			itbb++;
		}else{ //same index
			ndeg+=bitblock::popc64(itv->bb & itbb->bb);
			itv++; itbb++;
		}
	}
	return ndeg;
}

template<>
inline
int Ugraph<sparse_bitarray>::degree (int v, int UB, const BitBoardS & bbs) const	{
////////////////////
// degree of v considering only adjacent vertices in subgraph passed
// if deg is greater than UB the search is truncated and UB is returned

	int ndeg=0;
	sparse_bitarray::velem_cit itv = m_g[v].begin();
	sparse_bitarray::velem_cit itbb = bbs.begin();
	
	while(itv!=m_g[v].end() && itbb!=bbs.end()){

		if(itv->index<itbb->index){ 
			itv++;
		}else if(itv->index>itbb->index){
			itbb++;
		}else{ //same index
			ndeg+=bitblock::popc64(itv->bb & itbb->bb);
			if(ndeg>=UB) return UB;
			itv++; itbb++;
		}
	}
	return ndeg;
}


template<class T>
int Ugraph<T>::create_complement (Ugraph& ug) const	{
/////////////////////////
// computes complement graph in ug (includes memory allocation memory)
// param@ug: complement graph 

// RETURN VAL: -1 if ERROR, O if OK
		
	//memory allocation of new complement graph
	if( ug.init(_mypt::m_size)==-1 ) return -1;

	for(int i=0; i<_mypt::m_size-1; i++){
		for(int j=i+1; j<_mypt::m_size; j++){
			if(!_mypt::m_g[i].is_bit(j)){
				ug.add_edge(i,j);
			}
		}
	}
		

	//if weighted, copy the weights also
	/*if(this->m_is_wv){
		ug.m_is_wv=true;
		ug.m_wv.resize(this->m_size);
		copy(this->m_wv.begin(), this->m_wv.end(), ug.m_wv.begin());
	}*/
	return 0;
}

//template<class T>
//inline int Ugraph<T>::create_induced(Ugraph & ug, int v) const
///////////////////////
//// generates the graph induced by the N(v) 
////
//// PARAMS
//// ug@ the new induced subgraph
//// v@ the vertex whose neighborhood induces the subgraph
////
//// RETURN VAL: -1 if ERROR, O if OK
//{
//
//	//memory allocation of the new complement graph
//	if (ug.init(_mypt::m_size) == -1) return -1;
//
//	vector<int> vnn;
//	_mypt::get_neighbors(v).to_vector(vnn);
//	//com::stl::print_collection(vnn);
//
//	for (int i = 0; i < vnn.size()-1; i++) {
//		for (int j = i + 1; j < vnn.size(); j++) {
//
//			//add edges accordingly
//			if (_mypt::is_edge(vnn[i], vnn[j])) {
//					ug.add_edge(vnn[i], vnn[j]);
//			}
//		}
//	}	
//	return 0;
//}

template<class T>
inline int Ugraph<T>::create_induced(Ugraph & ug, int v) const
/////////////////////
// generates the graph induced by N(v) 
//
// PARAMS
// ug@ the new induced subgraph
// v@ the vertex whose neighborhood induces the subgraph
//
// RETURN VAL: -1 if ERROR, O if OK
{
	vector<int> vnn;
	_mypt::get_neighbors(v).to_vector(vnn);

	return create_induced(ug, vnn);	
}

template<class T>
inline int Ugraph<T>::create_induced(Ugraph & ug, std::vector<int>& lv) const
/////////////////////
// generates the graph the induced by the set of vertices lv
//
// PARAM
// ug@: the output graph
// lv@: the set of vertices that induce ug
//
// RETURN VAL: -1 if ERROR, O if OK
{
	//ASSERT
	if (lv.empty()) { LOG_ERROR("empty set found while creating an induced graph-::create_induced(...)"); return -1; }
	
	const int NV = lv.size();
	if (ug.init(NV) == -1) { LOG_ERROR("error during allocation-Ugraph<T>::create_induced(...)");  return -1; } 
		
	for (int i = 0; i < NV - 1; i++) {
		for (int j = i + 1; j < NV; j++) {

			//add edges accordingly
			if (_mypt::is_edge(lv[i], lv[j])) {
				ug.add_edge(i, j);						//CHECK!
			}
		}
	}	
	return 0;
}

template<class T>
inline int Ugraph<T>::add_vertex(_bbt * neigh)
{
	LOG_ERROR("buggy- should not be called-Ugraph<T>::add_vertex");
	LOG_ERROR("exiting...");
	exit (-1);

	// _mypt::m_size = _mypt::m_size + 1;

	//try {
	//	if (neigh == NULL) {
	//		_mypt::m_g.resize(_mypt::m_size);
	//	}
	//	else {
	//		_mypt::m_g.resize(_mypt::m_size, T(*neigh));
	//	}

	//}
	//catch (...) {
	//	LOG_INFO("memory for graph not allocated-Graph<T>::enlarge(...), exiting");
	//	exit(-1);
	//}

	return 0;
}
#endif






