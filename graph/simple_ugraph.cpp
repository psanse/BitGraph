/**
 * @file simple_ugraph.cpp 
 * @brief implementation file of the class Ugraph (header simple_ugraph.h)
 *
 * @created 02/01/2024
 * @last_update 26/01/2025
 * @author pss
 *
 * This code is part of the GRAPH 1.0 C++ library 
 *
 **/

#include "simple_ugraph.h"
#include "utils/prec_timer.h"

//specializations for sparse graphs
#include "simple_sparse_ugraph.h"				

using namespace std;

template<class T>
Ugraph<T>::Ugraph(string filename) : Graph<T>()		
{
	//cannot call Graph<T>::Graph(filename) because Graph<T>::add_edge is overriden!

	if (ptype::set_graph(filename) == -1 ) {
		LOGG_ERROR("error when reading file: ", filename, "Graph<T>::Graph");
		LOG_ERROR("exiting...");
		exit(-1);
	}
	
}

template <class T>
Ugraph<T>::Ugraph(std::size_t NV, int* adj[], string name) {
														
	if (ptype::reset(NV) == -1) {
		LOG_ERROR("Bad graph construction - Ugraph<T>::Ugraph(std::size_t , int* adj[], string)");
		LOG_ERROR("exiting...");
		exit(-1);
	}
	ptype::set_name(name);

	for (std::size_t i = 0; i < NV - 1; ++i) {
		for (std::size_t j = i + 1; j < NV; ++j) {
			if (adj[i][j] == 1) {
				add_edge(i, j);
			}
		}
	}
}

template<class T>
BITBOARD Ugraph<T>::number_of_edges(bool lazy) {

	if (!lazy || ptype::NE_ == 0) {
		ptype::NE_ = 0;

		//adds all edges and divides by 2 for efficiency - checks for self loops	
		for (std::size_t i = 0; i < NV_; i++) {
			ptype::NE_ += adj_[i].size();
		}

		//////////////////////////////
		if (ptype::NE_ % 2 != 0) {
			LOG_ERROR("odd number of edges found in simple undirected graph - Ugraph<T>::number_of_edges");
			LOG_ERROR("exiting...");
			exit(-1);					//TODO: throw exception
		}
		//////////////////////////////

		ptype::NE_ /= 2;						//MUST be even at this point			
	}

	return ptype::NE_;
}

template<class T>
BITBOARD Ugraph<T>::number_of_edges(const T& bbn) const{
	BITBOARD NE = 0;

	//reads only the upper triangle of the adjacency matrix
	for (std::size_t i = 0; i < ptype::NV_ - 1; ++i) {
		if ( bbn.is_bit(i) ) {
			for ( std::size_t j = i + 1; j < _mypt::NV_; ++j) {
				if ( bbn.is_bit(j) ) { 
					if (_mypt::adj_[i].is_bit(j)) { ++NE; }
				}
			}
		}
	}
	
	return NE;

// @created 17/6/10 
// @last_updated 02/01/24
}

template<class T>
int Ugraph<T>::degree (int v, const BitSet& bbn) const {

	int ndeg = 0;
	for (int i = 0; i < ptype::NBB_; i++) {
		ndeg += bblock::popc64(ptype::adj_[v].block(i) & bbn.block(i));
	}

	return ndeg;
}


template<class T>
void Ugraph<T>::add_edge (int v, int w){

	if(v != w){
		ptype::adj_[v].set_bit(w);
		ptype::adj_[w].set_bit(v);
		ptype::NE_++;
	}
}

template<class T>
void Ugraph<T>::remove_edge (int v, int w){
	if(v!=w){
		_mypt::adj_[v].erase_bit(w);
		_mypt::adj_[w].erase_bit(v);
		_mypt::NE_--;
	}
}

template<class T>
void Ugraph<T>::gen_random_edges(double p){

	//removes all edges
	remove_edges();

	//sets undirected edges with probability p
	for (std::size_t i = 0; i < NV_ - 1; i++) {
		for (std::size_t j = i + 1; j < NV_; j++) {
			if (::com::rand::uniform_dist(p)) {
				add_edge(i, j);
			}
		}
	}

}

template<class T>
int Ugraph<T>::max_graph_degree () const {

	int max_degree=0, temp=0; 

	for(int i = 0; i < ptype::NV_; ++i){
		temp = degree (i);
		if( temp > max_degree)
				max_degree = temp;
	}

	return max_degree;
}

template<class T>
double Ugraph<T>::density(bool lazy) {
	BITBOARD max_edges = ptype::NV_;
	max_edges *= (max_edges - 1);
	return (2 * number_of_edges(lazy) / static_cast<double> (max_edges));
}

template<class T>
ostream& Ugraph<T>::print_degrees (std::ostream& o) const {
	for(std::size_t i = 0; i < ptype::NV_; ++i){
		o << "deg(" << i << ")" << ":" << degree(i) <<" ";
	}
	return o;
}

template<class T>
ostream& Ugraph<T>::print_edges (std::ostream& o = cout) {

	for(auto i = 0; i < ptype::NV_ - 1; ++i){
		for(auto j = i + 1; j< ptype::NV_; ++j){

			if(ptype::is_edge(i, j)){
				o << "["<<i<<"]" << "--" << "[" << j << "]" << endl;
			}
		}
	}

	return o;
}

template<class T>
ostream& Ugraph<T>::print_matrix(std::ostream& o) const
{
	for (std::size_t i = 0; i < ptype::NV_; ++i) {
		for (std::size_t j = 0; j < ptype::NV_; ++j) {
			if (ptype::is_edge(i, j)) {
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

//template<class T>
//void Ugraph<T>::remove_vertices (const BitSet& bbn){
////////////////////////////
//// Experimental: inefficient implementation: allocating memory twice 
//	Ugraph g;
//	this->_mypt::remove_vertices(bbn,g);			//allocation 1
//	(*this)=g;										//allocation 2	
//}


template<class T>
void Ugraph<T>::write_dimacs (ostream & o)  {
		
	//timestamp comment 
	o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();

	//name comment
	if(!_mypt::name_.empty())
		o << "c " << _mypt::name_.c_str() << endl;

	//dimacs header
	o << "p edge " << ptype::NV_ << " " << number_of_edges (false /* recompute */) << endl << endl;

	//bidirectional edges (1 based in dimacs)
	for( std::size_t v = 0; v < ptype::NV_ - 1; ++v ){
		for( std::size_t w = v + 1; w < ptype::NV_; ++w ){
			if (ptype::is_edge(v, w)) {										//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << endl;
			}
		}
	}
}

template<class T>
void  Ugraph<T>::write_EDGES (ostream& o) {

	//timestamp comment
	o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();

	//name coment
	if(!ptype::name_.empty())
		o << "% " << ptype::name_.c_str() << endl;
	
	//write edges - 1 based vertex notation
	for( std::size_t v = 0; v < ptype::NV_ - 1; ++v ){
		for( std::size_t w = v + 1; w < ptype::NV_; ++w ){
			if ( ptype::is_edge(v, w) ) {							//O(log) for sparse graphs: specialize
				o << v + 1 << " " << w + 1 << endl;
			}
		}
	}
}

template<class T>
void Ugraph<T>::write_mtx(ostream & o){
		
	//header comment
	o << "%%MatrixMarket matrix coordinate pattern symmetric" << endl;
	
	//timestamp comment
	o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();
		
	//name comment
	if(! ptype::name_.empty() )
		o << "% " << ptype::name_.c_str() << endl;

	//number of vertices and edges
	ptype::NE_ = 0;																			//eliminates lazy evaluation of edge count 
	o << ptype::NV_ << " " << ptype::NV_ << " " << number_of_edges() << endl;
	
	//writes edges 1-based vertex notation
	for( std::size_t v = 0; v < ptype::NV_ - 1; ++v ){
		for( std::size_t  w = v + 1; w < ptype::NV_; ++w ){
			if ( ptype::is_edge(v, w) ) {														//O(log) for sparse graphs: specialize
				o << v + 1 << " " << w + 1 << endl;						
			}
		}
	}
}

template<class T>
int Ugraph<T>::degree_up (int v, const BitSet& bbn) const	{

	int nDeg = 0, nBB = WDIV(v);

	for (auto i = nBB + 1; i < ptype::NBB_; ++i) {
		nDeg += bblock::popc64( _mypt::adj_[v].block(i) & bbn.block(i) );
	}

	//truncate the bitblock of v
	nDeg += bblock::popc64 ( bblock::MASK_1( WMOD(v) + 1, 63 ) &
							   ptype::adj_[v].block(nBB) & bbn.block(nBB)	
							 );
	
	return nDeg;
}

template<class T>
int Ugraph<T>::degree_up(int v) const 
{
	int nDeg = 0, nBB = WDIV(v);

	for (auto i = nBB + 1; i < ptype::NBB_; ++i) {
		nDeg += bblock::popc64( _mypt::adj_[v].block(i) );
	}

	//truncate the bitblock of v
	nDeg += bblock::popc64( bblock::MASK_1(WMOD(v) + 1, 63) &
							  ptype::adj_[v].block(nBB)	   );

	return nDeg;
}

template<class T>
int Ugraph<T>::degree (int v, int UB, const BitSet& bbn) const	{

	int nDeg = 0;
	for(auto i = 0; i < _mypt::NBB_; ++i){

		nDeg += bblock::popc64(_mypt::adj_[v].block(i) & bbn.block(i));

		if (nDeg >= UB) { return UB; }
	}

	return nDeg;
}

template<class T>
int Ugraph<T>::create_complement (Ugraph& ug) const	{
	
	//resets ug with new allocation
	if( ug.reset(ptype::NV_) == -1 ) return -1;

	for( std::size_t i=0; i < ptype::NV_ - 1; i++){
		for(std::size_t j=i+1; j < ptype::NV_; j++){
			if(!ptype::adj_[i].is_bit(j)){
				ug.add_edge(i,j);
			}
		}
	}

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
//	if (ug.init(_mypt::NV_) == -1) return -1;
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
int Ugraph<T>::create_subgraph(Ugraph & ug, int v) const
{
	vector<int> vnn;
	ptype::get_neighbors(v).to_vector(vnn);

	return create_subgraph(ug, vnn);	
}

template<class T>
int Ugraph<T>::create_subgraph(Ugraph & ug, vint& lv) const
{
	if (lv.empty()) {
		LOG_ERROR("empty set found while creating an induced graph - Ugraph<T>::create_induced");
		return -1; 
	}
	
	const int NV = lv.size();
	if (ug.reset(NV) == -1) { 
		LOG_ERROR("bad allocation - Ugraph<T>::create_induced"); 
		return -1; 
	} 

	//add appropiate edges
	for (std::size_t i = 0; i < NV - 1; i++) {
		for (std::size_t j = i + 1; j < NV; j++) {
						
			if (_mypt::is_edge(lv[i], lv[j])) {
				ug.add_edge(i, j);						//adds bidirected edge
			}
		}
	}	

	return 0;
}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

template class  Ugraph<bitarray>;
template class  Ugraph<sparse_bitarray>;

//sparse method specializations (now included in the header simple_sparse_graph.h)


////////////////////////////////////////////












