 /*
  * simple_ugraph.cpp implementation file of the class Ugraph (header simple_ugraph.h)
  *
  * @created 02/01/2024
  * @dev pss
  *
  * @comments see end of file for valid template types
  *
  */


#include "simple_ugraph.h"
#include "utils/prec_timer.h"

#include "simple_sparse_ugraph.h"				//specializations for sparse graphs

using namespace std;

template<class T>
Ugraph<T>::Ugraph(string filename) : Graph<T>()		
{
	//cannot call Graph<T>::Graph(filename) because Graph<T>::add_edge is overriden!

	if (ptype::set_graph(filename) == -1) {
		LOGG_ERROR("error when reading file: ", filename, "Graph<T>::Graph");
		LOG_ERROR("exiting...");
		exit(-1);
	}
	
}

template <class T>
Ugraph<T>::Ugraph(std::size_t nV, int* adj[], string name) {
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
BITBOARD Ugraph<T>::number_of_edges(bool lazy) {

	if (!lazy || ptype::NE_ == 0) {
		ptype::NE_ = 0;

		//adds all edges and divides by 2 for efficiency - checks for self loops	
		for (int i = 0; i < NV_; i++) {
			ptype::NE_ += adj_[i].popcn64();
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
int Ugraph<T>::degree (int v, const BitBoardN& bbn) const {

	int ndeg = 0;
	for (int i = 0; i < ptype::NBB_; i++) {
		ndeg += bitblock::popc64(ptype::adj_[v].get_bitboard(i) & bbn.get_bitboard(i));
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
int Ugraph<T>::max_graph_degree () const {

	int max_degree=0, temp=0; 

	for(int i=0; i<ptype::NV_; ++i){
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
ostream& Ugraph<T>::print_edges (std::ostream& o) const{
	for(std::size_t i=0; i< ptype::NV_-1; i++){
		for(std::size_t j = i + 1; j< ptype::NV_; j++){
			if(ptype::is_edge(i, j)){
				o << "["<<i<<"]" << "--" << "[" << j << "]" << endl;
			}
		}
	}
	return o;
}

template<class T>
ostream& Ugraph<T>::print_matrix(std::ostream& o) const {
	for (std::size_t i = 0; i < ptype::NV_; i++) {
		for (std::size_t j = 0; j < ptype::NV_; j++) {
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

template<class T>
ostream& Ugraph<T>::print_edges (T& bbsg, std::ostream& o) const{
	for(int i=0; i<_mypt::NV_-1; i++){
		if(!bbsg.is_bit(i)) continue;
		for(int j=i+1; j<_mypt::NV_; j++){
			if(!bbsg.is_bit(j)) continue;	
			if(_mypt::is_edge(i, j)){
				o<<"["<<i<<"]"<<"--"<<"["<<j<<"]"<<endl;
			}
		}
	}

	return o;
}

//template<class T>
//void Ugraph<T>::remove_vertices (const BitBoardN& bbn){
////////////////////////////
//// Experimental: inefficient implementation: allocating memory twice 
//	Ugraph g;
//	this->_mypt::remove_vertices(bbn,g);			//allocation 1
//	(*this)=g;										//allocation 2	
//}


template<class T>
void Ugraph<T>::write_dimacs (ostream & o)  {
/////////////////////////
// writes file in dimacs format with timestamp
		
	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();

	//name
	if(!_mypt::name_.empty())
		o<<"c "<<_mypt::name_.c_str()<<endl;

	//tamaño del grafo
	o<<"p edge "<<_mypt::NV_<<" "<<number_of_edges(false /* recompute */)<<endl<<endl;

	//write DIMACS nodes n <v> <w>
	/*if (_mypt::is_weighted_v()){
		for(int v=0; v<_mypt::NV_; v++){
			o<<"n "<<v+1<<" "<<_mypt::get_wv(v)<<endl;
		}
	}*/

	//bidirectional edges
	for(int v=0; v<_mypt::NV_-1; v++){
		for(int w=v+1; w<_mypt::NV_; w++){
			if(_mypt::is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<"e "<<v+1<<" "<<w+1<<endl;				//1 based vertex notation dimacs
		}
	}
}


template<class T>
void  Ugraph<T>::write_EDGES(ostream& o){
/////////////////////////
// writes simple unweighted grafs  in edge list format 
// note: loops are not allowed
		
	//timestamp 
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();

	//name
	if(!_mypt::name_.empty())
		o<<"% "<<_mypt::name_.c_str()<<endl;
	
	//write edges
	for(int v=0; v<_mypt::NV_-1; v++){
		for(int w=v+1; w<_mypt::NV_; w++){
			if(_mypt::is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<v+1<<" "<<w+1<<endl;						//1 based vertex notation dimacs
		}
	}
}


template<class T>
void Ugraph<T>::write_mtx(ostream & o){
/////////////////////////
// writes simple unweighted grafs  in edge list format 
// note: loops are not allowed
		
	//header
	o<<"%%MatrixMarket matrix coordinate pattern symmetric"<<endl;
	
	//timestamp
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();
		
	//name
	if(!_mypt::name_.empty())
		o<<"% "<<_mypt::name_.c_str()<<endl;

	//size and edges
	_mypt::NE_=0;																	//eliminates lazy evaluation of edge count 
	o<<_mypt::NV_<<" "<<_mypt::NV_<<" "<<number_of_edges()<<endl;
	
	//writes edges
	for(int v=0; v<_mypt::NV_-1; v++){
		for(int w=v+1; w<_mypt::NV_; w++){
			if(_mypt::is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<v+1<<" "<<w+1<<endl;						//1 based vertex notation dimacs
		}
	}
}


template<class T>
int Ugraph<T>::degree_up (int v, const BitBoardN& bbn) const	{
////////////////////
// degree of v considering only adjacent vertices with higher index in the subgraph passed
// (applied as pivot strategy for enumeration)

	int ndeg=0;
	int nBB=WDIV(v);
	for(int i=nBB+1; i<_mypt::NBB_;i++)
		ndeg+=bitblock::popc64(_mypt::adj_[v].get_bitboard(i)& bbn.get_bitboard(i));

	//truncate the bitblock of v
	ndeg+=bitblock::popc64(bitblock::MASK_1(WMOD(v)+1,63) & _mypt::adj_[v].get_bitboard(nBB)& bbn.get_bitboard(nBB));
return ndeg;
}

template<class T>
int Ugraph<T>::degree (int v, int UB, const BitBoardN& bbn) const	{

	int ndeg=0;
	for(int i=0; i<_mypt::NBB_;i++){
		ndeg+=bitblock::popc64(_mypt::adj_[v].get_bitboard(i)& bbn.get_bitboard(i));
		if(ndeg >= UB) return UB;		
	}

return ndeg;
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













