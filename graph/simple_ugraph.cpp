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

//#include "simple_sparse_ugraph.h"				//TODO specializations for sparse graphs?

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
inline
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
void Ugraph<T>::add_edge (int v, int w){
///////////////
// sets v-->w (no self loops allowed)
	if(v != w){
		_mypt::adj_[v].set_bit(w);
		_mypt::adj_[w].set_bit(v);
		_mypt::NE_++;
	}
}

template<class T>
void Ugraph<T>::remove_edge (int v, int w){
///////////////
// removes bidirected edge
	if(v!=w){
		_mypt::adj_[v].erase_bit(w);
		_mypt::adj_[w].erase_bit(v);
		_mypt::NE_--;
	}
}

template<class T>
int Ugraph<T>::max_degree_of_graph () const{
/////////////////////
// degree of graph 

	int max_degree=0, temp=0; 

	for(int i=0; i<_mypt::NV_;i++){
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
	for(int i=0; i<_mypt::NV_; i++){
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
	for(int i=0; i<_mypt::NV_-1; i++){
		if(bbn.is_bit(i)){
			for(int j=i+1; j<_mypt::NV_; j++)
				if(bbn.is_bit(j)){
					if( _mypt::adj_[i].is_bit(j) )
							edges++;
				}
		}
	}
return edges;
}

template<class T>
double Ugraph<T>::density	(bool lazy)	{
	BITBOARD max_edges=_mypt::NV_;
	max_edges*=(max_edges-1);
	return (2*number_of_edges(lazy)/(double)max_edges);
}


template<class T>
inline
BITBOARD Ugraph<T>::number_of_edges	(bool lazy) {
///////////////////
// Should be specialized for sparse graphs
	if(lazy && _mypt::NE_ !=0) 
				return _mypt::NE_;

	BITBOARD  edges=0;
	for(int i=0; i<_mypt::NV_-1; i++){
		for(int j=i+1; j<_mypt::NV_; j++){
			if(_mypt::is_edge(i,j)){				//O(log) for sparse graphs 
				edges++;
			}
		}
	}
	_mypt::NE_=edges;
	
return _mypt::NE_;
}

template<> 
inline
BITBOARD Ugraph<sparse_bitarray>::number_of_edges	(bool lazy) {
/////////////
//Specialization for sparse graphs (is adjacent runs in O(log)) 
	if(lazy && Graph<sparse_bitarray>::NE_ !=0) 
				return Graph<sparse_bitarray>::NE_;
	
	BITBOARD  edges=0;
	for(int i=0; i<Graph<sparse_bitarray>::NV_-1; i++){
		edges+=adj_[i].popcn64(i+1);		//O(log)
	}
	Graph<sparse_bitarray>::NE_=edges;
	
return Graph<sparse_bitarray>::NE_;
}

template<class T>
inline
ostream& Ugraph<T>::print_edges (std::ostream& o) const{
	for(int i=0; i<_mypt::NV_-1; i++){
		for(int j=i+1; j<_mypt::NV_; j++){
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
	for (int i = 0; i < _mypt::NV_; i++) {
		for (int j =0; j < _mypt::NV_; j++) {
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
inline
void Ugraph<T>::write_dimacs (ostream & o) {
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

template<>
inline
void Ugraph<sparse_bitarray>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 
	
	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!Graph<sparse_bitarray>::name_.empty())
		o<<"c "<<Graph<sparse_bitarray>::name_.c_str()<<endl;

	//tamaño del grafo
	o<<"p edge "<<Graph<sparse_bitarray>::NV_<<" "<<number_of_edges()<<endl<<endl;
	
	//Escribir nodos
	for(int v=0; v<Graph<sparse_bitarray>::NV_-1; v++){
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p=Graph<sparse_bitarray>::adj_[v].find_pos(WDIV(v));
		if(p.second==EMPTY_ELEM) continue;					//no more bitblocks
		Graph<sparse_bitarray>::adj_[v].m_scan.bbi=p.second; 
		(p.first)? Graph<sparse_bitarray>::adj_[v].m_scan.pos= WMOD(v) : Graph<sparse_bitarray>::adj_[v].m_scan.pos=MASK_LIM;		//if bitblock contains v, start from that position onwards
		while(1){
			int w=Graph<sparse_bitarray>::adj_[v].next_bit();
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


template<>
inline
void Ugraph<sparse_bitarray>::write_EDGES(ostream& o) {
/////////////////////////
// writes simple unweighted grafs  in edge list format 
// note: loops are not allowed
	
	//timestamp
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!Graph<sparse_bitarray>::name_.empty())
		o<<"% "<<Graph<sparse_bitarray>::name_.c_str()<<endl;

	//writes edges
	for(int v=0; v<Graph<sparse_bitarray>::NV_-1; v++){
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p=Graph<sparse_bitarray>::adj_[v].find_pos(WDIV(v));
		if(p.second==EMPTY_ELEM) continue;										//no more bitblocks
		Graph<sparse_bitarray>::adj_[v].m_scan.bbi=p.second; 
		(p.first)? Graph<sparse_bitarray>::adj_[v].m_scan.pos= WMOD(v) : Graph<sparse_bitarray>::adj_[v].m_scan.pos=MASK_LIM;			//if bitblock contains v, start from that position onwards
		while(1){
			int w=Graph<sparse_bitarray>::adj_[v].next_bit();
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
	if(!Graph<sparse_bitarray>::name_.empty())
		o<<"% "<<Graph<sparse_bitarray>::name_.c_str()<<endl;

	//size and edges
	NE_=0;																	//eliminates lazy evaluation of edge count 
	o<<Graph<sparse_bitarray>::NV_<<" "<<Graph<sparse_bitarray>::NV_<<" "<<number_of_edges()<<endl;
	
	//writes edges
	for(int v=0; v<Graph<sparse_bitarray>::NV_-1; v++){
		//non destructive scan starting from the vertex onwards
		pair<bool, int> p=Graph<sparse_bitarray>::adj_[v].find_pos(WDIV(v));
		if(p.second==EMPTY_ELEM) continue;										//no more bitblocks
		Graph<sparse_bitarray>::adj_[v].m_scan.bbi=p.second; 
		(p.first)? Graph<sparse_bitarray>::adj_[v].m_scan.pos= WMOD(v) : Graph<sparse_bitarray>::adj_[v].m_scan.pos=MASK_LIM;			//if bitblock contains v, start from that position onwards
		while(1){
			int w=Graph<sparse_bitarray>::adj_[v].next_bit();
			if(w==EMPTY_ELEM)
					 break;
			o<<v+1<<" "<<w+1<<endl;	
		}	
	}	
}


template<class T>
inline
int Ugraph<T>::degree (int v) const{
	return _mypt::adj_[v].popcn64();
}

template<>
inline
int Ugraph<sparse_bitarray>::degree (int v) const{
	if(Graph<sparse_bitarray>::adj_[v].is_empty()) return 0;

return Graph<sparse_bitarray>::adj_[v].popcn64();
}

template<class T>
inline
int Ugraph<T>::degree (int v, const BitBoardN& bbn) const	{
////////////////////
// degree of v considering only adjacent vertices in subgraph passed

	int ndeg=0;
	for(int i=0; i<_mypt::NBB_;i++)
		ndeg+=bitblock::popc64(_mypt::adj_[v].get_bitboard(i)& bbn.get_bitboard(i));

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
	for(int i=nBB+1; i<_mypt::NBB_;i++)
		ndeg+=bitblock::popc64(_mypt::adj_[v].get_bitboard(i)& bbn.get_bitboard(i));

	//truncate the bitblock of v
	ndeg+=bitblock::popc64(bitblock::MASK_1(WMOD(v)+1,63) & _mypt::adj_[v].get_bitboard(nBB)& bbn.get_bitboard(nBB));
return ndeg;
}


template<class T>
inline
int Ugraph<T>::degree (int v, int UB, const BitBoardN& bbn) const	{
////////////////////
// degree of v in bbn subgraph. 
// if deg is greater than UB the search is stopped and UB is returned


	int ndeg=0;
	for(int i=0; i<_mypt::NBB_;i++){
		ndeg+=bitblock::popc64(_mypt::adj_[v].get_bitboard(i)& bbn.get_bitboard(i));
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
	for(sparse_bitarray::velem_cit it= adj_[v].begin(); it!=adj_[v].end(); ++it){
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
	for(sparse_bitarray::velem_cit it= adj_[v].begin(); it!=adj_[v].end(); ++it){
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
	sparse_bitarray::velem_cit itv = adj_[v].begin();
	sparse_bitarray::velem_cit itbb = bbs.begin();
	
	while(itv!=adj_[v].end() && itbb!=bbs.end()){

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
	sparse_bitarray::velem_cit itv = adj_[v].begin();
	sparse_bitarray::velem_cit itbb = bbs.begin();
	
	while(itv!=adj_[v].end() && itbb!=bbs.end()){

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
	if( ug.init(_mypt::NV_)==-1 ) return -1;

	for(int i=0; i<_mypt::NV_-1; i++){
		for(int j=i+1; j<_mypt::NV_; j++){
			if(!_mypt::adj_[i].is_bit(j)){
				ug.add_edge(i,j);
			}
		}
	}
		

	//if weighted, copy the weights also
	/*if(this->m_is_wv){
		ug.m_is_wv=true;
		ug.m_wv.resize(this->NV_);
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

	// _mypt::NV_ = _mypt::NV_ + 1;

	//try {
	//	if (neigh == NULL) {
	//		_mypt::adj_.resize(_mypt::NV_);
	//	}
	//	else {
	//		_mypt::adj_.resize(_mypt::NV_, T(*neigh));
	//	}

	//}
	//catch (...) {
	//	LOG_INFO("memory for graph not allocated-Graph<T>::enlarge(...), exiting");
	//	exit(-1);
	//}

	return 0;
}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

template class  Ugraph<bitarray>;
template class  Ugraph<sparse_bitarray>;

//sparse method specializations (now included in the header simple_sparse_graph.h)
//template Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph(int, Graph<sparse_bitarray>&);
//template int Graph<sparse_bitarray>::shrink_to_fit(int);
//template BITBOARD Graph<sparse_bitarray>::number_of_edges(bool);
//template double Graph<sparse_bitarray>::block_density()	const;
//template double Graph<sparse_bitarray>::block_density_index();
//template void Graph<sparse_bitarray>::write_dimacs(ostream& o);

////////////////////////////////////////////













