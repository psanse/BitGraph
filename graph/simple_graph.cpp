/**
 * @file simple_graph.cpp
 * @brief implementation of Graph class for simple graphs
 *
 * @created ?
 * @last_update 31/12/2024
 * @author pss
 *
 * This code is part of the GRAPH 1.0 C++ library
 *
 * TODO use SFINAE to filter types(10 / 01 / 2025)
 *
 **/

 //////////////////
 //switches 

//#define DIMACS_INDEX_0_FORMAT			//0 index DIMACS format [DEF-OFF (real DIMACS format)] 

#include "bitscan/bitscan.h"
#include "graph/formats/dimacs_format.h"
#include "graph/formats/mmio.h"
#include "graph/formats/edges_format.h"
#include "graph/formats/mmx_format.h"
#include "graph/simple_graph.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "simple_sparse_graph.h"				//specializations for sparse graphs

using namespace std;


template<class T>
Graph<T>::Graph(void) :
	NV_(0), NBB_(0), NE_(0),
	name_(""), path_("")
{}

template<class T>
Graph<T>::Graph(string filename) :
	NV_(0), NBB_(0), NE_(0),
	name_(""), path_("")
{
	if (set_graph(filename) == -1) {
		LOGG_ERROR("error when reading file: ", filename, "Graph<T>::Graph");
		LOG_ERROR("exiting...");
		exit(-1);
	}	
}

template<class T>
Graph<T>::Graph(std::size_t NV) {
	name_.clear();
	path_.clear();
	init(NV);
}

template <class T>
Graph<T>::Graph(std::size_t nV, int* adj[], string filename) {
	if (init(nV) == -1) {
		LOG_ERROR("bizarre graph construction-Graph<T>::Graph(...), exiting... ");
		exit(-1); 
	}
	name(filename);		
	
	//add edges
	for (auto i = 0; i < nV; i++) {
		for (int j = 0; j < nV; j++) {
			if (adj[i][j] == 1) {
				add_edge(i, j);
			}
		}
	}
}



template<class T>
void Graph<T>::name(std::string name){

	//update name
	size_t found = name.find_last_of("/\\");
	
	//update path and name
	if (found != string::npos) {
		name_ = name.substr(found + 1);
		path_ = name.substr(0, found + 1);  //includes slash
	}
	else {
		name_ = std::move(name);
		path_.clear();
	}

	
	//if(separate_path){
	//	size_t found=name.find_last_of("/\\");
	//	name_ = str.substr(found + 1);
	//	if (found != string::npos) {			
	//		path_ = str.substr(0, found + 1);  //includes slash
	//	}
	//}else{
	//	name_=name;
	//	path_.clear();
	//}
}

template<class T>
void Graph<T>::clear (){
	adj_.clear(), name_.clear(), path_.clear();
	NV_=0, NBB_=0, NE_=0;
}

template<class T>
int Graph<T>::init (std::size_t NV, bool reset_name){

	if (NV <= 0) {
		LOG_ERROR("Invalid graph size - Graph<T>::init");
		return -1;
	}

	//initialization
	NV_ = NV;
	NBB_ = INDEX_1TO1(NV_);
	NE_ = 0;

	try{
		//////////////////////////////
		adj_.assign(NV, T(NV));				//bitsets initialize to 0		
		//////////////////////////////
		
	}catch(const std::bad_alloc& e){
		LOG_ERROR("memory for graph not allocated - Graph<T>::init");
		LOG_ERROR(e.what());		
		NV_ = 0;
		NBB_ = 0;			
		return -1;
	}
			
	//init bitarrays with zero edges - CHECK if needed (should not be)
	//for(std::size_t i=0; i<NV_; i++){
	//	adj_[i].init(NV_);					//MUST BE! - CHECK THIS COMMENT (31/12/24)
	//}

	//clears name if requested
	if (reset_name) {
		name_.clear();
		path_.clear();
	}

	return 0;
}

template<class T>
int Graph<T>::reset	(std::size_t NV, string name) {

	if (NV <= 0) {
		LOGG_ERROR("Invalid graph size ", NV," - Graph<T>::reset");
		return -1;
	}

	//initialization
	NV_ = NV;
	NBB_ = INDEX_1TO1(NV_);
	NE_ = 0;

	try {
		//////////////////////////////
		adj_.assign(NV, T(NV));				//bitsets initialize to 0		
		//////////////////////////////
	}
	catch (const std::bad_alloc& e) {
		LOG_ERROR("memory for graph not allocated - Graph<T>::init");
		LOG_ERROR(e.what());
		NV_ = 0;
		NBB_ = 0;
		return -1;
	}

	//init bitarrays with zero edges - CHECK if needed (should not be)
	//for (std::size_t i = 0; i < NV_; i++) {
	//	adj_[i].init(NV_);					//MUST BE! - CHECK THIS COMMENT (31/12/24)
	//}

	//update instance name
	this->name(std::move(name));	

	return 0;
}

template<class T>
Graph<T>& Graph<T>::create_subgraph (std::size_t first_k, Graph<T>& newg) const  {

	//assert is size required is greater or equal current size
	if(first_k >= NV_ || first_k <= 0){
		LOG_ERROR("Bad new size - graph remains unchangedGraph<T>&- Graph<T>::create_subgraph");
		return newg;
	}
		
	if (newg.reset(first_k) == -1) {
		LOG_ERROR("memory for graph not allocated - Graph<T>::create_subgraph");
		return newg;
	}

	auto bbh = WDIV (first_k - 1);
	
	//copy the relevant vertices of the adjacency matrix
	for(auto i = 0; i < newg.NV_; ++i){
		for(auto j = 0; j <= bbh; ++j){
			newg.adj_[i].block(j)=adj_[i].block(j);
		}
		//trims last bitblock
		newg.adj_[i].block(bbh) &= ~Tables::mask_high[WMOD(first_k - 1)];
	}
	
	return newg;
}


template<class T>
int Graph<T>::shrink_to_fit(std::size_t size){
	
	LOG_WARNING("Shrinking is valid only for sparse graphs - Graph<T>::shrink_to_fit");
	LOG_WARNING("The graph remains unchanged");
		
	return -1;
}

template<class T>
int Graph<T>::set_graph (string filename){
	if(read_dimacs(filename) == -1){
		if(read_mtx(filename) == -1){
			if(read_EDGES(filename) == -1){
				LOGG_ERROR("Unable to read a graph form file ", filename, "- Graph<T>::set_graph");
				LOG_ERROR("Format considered: DIMACS / MTX / EDGES");
				return -1;
			}
		}
	}
	return 0;
}

template<class T>
void Graph<T>::add_edge (int v, int w){
	if(v != w){
		adj_[v].set_bit(w);
		NE_++;
	}
}

template<class T>
void Graph<T>::remove_edge	(int v, int w){
	adj_[v].erase_bit(w);
	NE_--;
}

template<class T>
void Graph<T>::remove_edges (int v){
	
	//erases all outgoing edges from v
	adj_[v].erase_bit();

	//erases all ingoing edges
	for(int w = 0; w < NV_; w++){
		if(w == v) continue;
		adj_[w].erase_bit(v);
	}

	//updates edges
	NE_ = 0;					//resets edges to avoid lazy evaluation later
	
}

template<class T>
void Graph<T>::remove_edges() {
	for (std::size_t v = 0; v < NV_; ++v) {
		adj_[v].erase_bit();
	}

	NE_ = 0;					
}

template <class T>
ostream& Graph<T>::print_adj(std::ostream& o, bool endofl) const {
	
	for (int i = 0; i < NV_; i++) {
		for (int j = 0; j < NV_; j++) {
			if (is_edge(i, j)) {
				 o << "1" << " ";
			}
			else {
				o << "0" << " ";
			}
		}
		o << endl;
	}

	if (endofl) o << endl;
	return o;
}

template<class T>
std::ostream& Graph<T>::timestamp_dimacs(std::ostream& o) const {
	o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();
	return o;
}

template<class T>
std::ostream& Graph<T>::name_dimacs(std::ostream& o) const {
	if (!name_.empty())
		o << "\nc " << name_.c_str() << endl;
	return o;
}

template<class T>
std::ostream& Graph<T>::header_dimacs(std::ostream& o, bool lazy){
	o << "p edge " << NV_ << " " << number_of_edges(lazy) << endl << endl;
	return o;
}

template<class T>
int Graph<T>::read_dimacs(const string& filename){

	
	int size, nEdges, v1, v2, edges=0;
	//_wt wv=0.0;
	/*char token[10];*/ char line [250]; char c;
	
	fstream f(filename.c_str());
	if(!f){
		LOG_ERROR("Graph<T>::read_dimacs-File could not be opened reading DIMACS format");
		clear();
		return -1;
	}

	if(::gio::dimacs::read_dimacs_header(f, size, nEdges) == -1){
		clear(); 
		f.close(); 
		return -1;
	}	
	
	init(size);
	::gio::read_empty_lines(f);

	//read weights format n <x> <w> if they exist
	//c=f.peek();
	//if(c=='n' || c=='v' /* used by Zavalnij */){						
	//	LOG_ERROR("Graph<T>::read_dimacs-DIMACS weights found in file: excluding other weights");
	//	init_wv();
	//	m_is_wv=true;
	//	for(int n=0; n<NV_; n++){
	//		f>>c>>v1>>wv;
	//		if(!f.good()){
	//			cerr<<"bad line related to weights"<<endl;
	//			clear();
	//			f.close();
	//			return -1;
	//		}
	//		m_wv[v1-1]=wv;
	//		if(wv==0){
	//			cerr<<filename<<":wrong header for edges reading DIMACS format"<<endl;
	//			clear();
	//			f.close();
	//			return -1;
	//		}
	//		f.getline(line, 250);  //remove remaining part of the line
	//	}

	//	LOG_INFO("Graph<T>::read_dimacs-Weights read correctly from DIMACS file"<<filename);
	//	::gio::dimacs::read_empty_lines(f);
	//}
/////////////////////	
//read edges

	//read the first edge to determine the type of input
	//f.getline(line, 250); 
	//stringstream sstr(line);
	//int nw=com::counting::count_words(sstr.str());
	//if(nw< 3 || nw > 4){
	//	cerr<<filename<<":wrong edge line format reading DIMACS format"<<endl;
	//	clear(); f.close(); 	return -1;
	//}else if(nw==4){  /* edge weighted case */
	//	LOG_INFO("reading edge weights from the file"); 
	//	init_we(0.0);
	//}

	////interpret the first line
	//if(nw==3){
	//	sstr>>c>>v1>>v2;	
	//	add_edge(v1-1,v2-1);
	//}else if(nw==4){
	//	sstr>>c>>v1>>v2>>wv;
	//	add_edge(v1-1,v2-1);
	//	set_we(v1-1, v2-1, wv);	
	//}
	

	//remaining edges
	for(int e = 0 /*1*/; e < nEdges; e++){
		f >> c;
		if(c != 'e'){
			LOGG_ERROR(filename, ":wrong header for edges reading DIMACS format");
			clear();
			f.close();
			return -1;
		}

		//read and add edge
		f >> v1 >> v2;
#ifdef DIMACS_INDEX_0_FORMAT 
		add_edge(v1,v2);
#else
		add_edge(v1-1, v2-1);
#endif

		//add bidirectional edge	
		/*if(nw == 3){
			f>>v1>>v2;
			add_edge(v1-1,v2-1);		
		}else if(nw == 4){
			f>>v1>>v2>>wv;
			add_edge(v1-1,v2-1);
			set_we(v1-1, v2-1, wv);
		}	*/
		
		//removes remaining part of the line
		f.getline(line, 250);  
	}
	f.close();
	
	//name (removes path)
	name(filename);

	//extension for weighted files (9/10/16)
//	string str(filename);					//filename contains the full path

	//read weights from external files if necessary
//	if(!m_is_wv){
//
//#ifdef FILE_EXTENSION_W	
//		str+=".w";
//#elif FILE_EXTENSION_D
//		str+=".d";
//#elif FILE_EXTENSION_WWW
//		str+=".www";
//#else
//		str+=".nofile";
//#endif
//		
//		read_weights(str);
//	}

	return 0;
}

template<class T>
int Graph<T>::read_01(const string& filename) {

		
	int size, val;
	char ch, line[250]; 

	fstream f(filename.c_str());
	if (!f) {
		LOG_ERROR("Graph<T>::read_01-File could not be opened reading DIMACS format");
		clear();
		f.close();
		return -1;
	}

	//read size
	f >> size;
	if (init(size) == -1) {
		LOG_ERROR("Graph<T>::read_01-bizarre graph size");
		f.close();
		return -1;
	}
	f.getline(line, 250);  //remove final part of the line

	//read rows		
	for (int r = 0 ; r < size ; r++) {
		for (int c = 0 ; c < size; c++) {
			f >> ch;
			if ((r<c) && ch == '1') {
				add_edge(r, c);
			}
		}
		f.getline(line, 250);  //remove final part of the line
	}
	f.close();

	//name (removes path)
	name(filename);
	return 0;
}

template<class T>
int  Graph<T>::read_mtx	(const string& filename){


	MMI<Graph<T> > myreader(*this);
	return (myreader.read(filename));
}

template<class T>
int  Graph<T>::read_EDGES (const string& filename){

	EDGES<Graph<T> > myreader(filename, *this);
	return (myreader.read());
}

template<class T>
ostream& Graph<T>::print_data( bool lazy, std::ostream& o, bool endl) {

	if (!name_.empty()) { o << name_.c_str() << '\t'; }

	////////////////////////////////////////////////////////////////
	o	<< "n= " << number_of_vertices() << "\t"
		<< std::fixed << "m= "<< number_of_edges(lazy) << "\t"
		<< std::setprecision(3) << "p= "<< density(true);
	////////////////////////////////////////////////////////////////

	/*Note: lazy comp. of density since the number of edges have already been computed previously */

	if (endl) { o << std::endl; }
	return o;
}

template<class T>
ostream& Graph<T>::print_edges (std::ostream& o) {

	for(auto i = 0; i < NV_-1; ++i){
		for(auto j = i + 1; j < NV_; ++j){

			if(is_edge(i,j)){
				o<< "[" << i << "]" << "-->" << "[" << j << "]" << endl;
			}
			if(is_edge(j,i)){
				o << "[" << j << "]" << "-->" << "[" << i << "]" <<endl;
			}

		}
	}
	return o;
}



template<class T>
BITBOARD Graph<T>::number_of_edges	(const T& bbn) const{
	
	BITBOARD NE = 0;

	for(std::size_t i = 0; i < NV_; i++){
		if(bbn.is_bit(i)){						
			for (std::size_t j = 0; j < NV_; j++) {
				if (bbn.is_bit(j)) {						//includes possible self loops
					if (adj_[i].is_bit(j)) { NE++; }
				}
			}
		}
	}

	return NE;
}

template<class T>
BITBOARD Graph<T>::number_of_edges	(bool lazy) {
		
	if (!lazy || NE_ == 0) {					//no lazy evaluation if NE_ = 0
		NE_ = 0;
		for (std::size_t i = 0; i < NV_; i++) {
			NE_ += adj_[i].size();			
		}
	}
	
	return NE_;
}

template<class T>
double Graph<T>::density (bool lazy) {
	BITBOARD max_edges = NV_;								//type MUST BE for very large graphs as (I) is bigger than unsigned int
	max_edges *= (max_edges - 1);							//(I)
	return (number_of_edges(lazy) / (double)max_edges);		//n*(n-1) edges (since it is a directed graph))
}

template<class T>
double Graph<T>::block_density	()	const {

	size_t nBB = 0;
	for(int v = 0; v < NV_; ++v){
		for(int bb = 0; bb < NBB_; bb++){
			if(adj_[v].block(bb))		//non-empty bitblock				
				nBB++;						
		}
	}

	return (nBB / static_cast<double>(NBB_ * NV_));
}

//template<class T>
//double Graph<T>::block_density_index()	const {
//
//	LOG_ERROR("function only for sparse graphs - Graph<T>::block_density_index");
//	return -1;
//}

//template<class T>
//double Graph<T>::average_block_density_index()	const {
//
//	LOG_ERROR("function only for sparse graphs - Graph<T>::average_block_density_index");
//	return -1;
//}


template<class T>
bool Graph<T>::is_self_loop () const{
	for(int i = 0; i < NV_; i++)
		if( adj_[i].is_bit(i)) {			
			return true;
		}
	return false;
}

template<class T>
void Graph<T>::remove_vertices (const BitSet& bbn, Graph& g){

	//determine the size of the graph g
	int pc = bbn.size();
	int new_size= NV_ - pc;

	if (new_size <= 0) {
		LOG_ERROR("empty graph after deletion - Graph<T>::remove_vertices");
		g.clear();
		return;
	}

	//initialize new graph
	if (g.reset(new_size) == -1) {
		LOG_ERROR("memory for graph not allocated - Graph<T>::remove_vertices");
		return;
	}
	
	//computes the induced graph in g
	g.name(this->name_);
	int l = 0;
	
	//adds the edges with endpoints not in the bbn set
	for(int i = 0; i < NV_ - 1; i++){
		if( bbn.is_bit(i) ) continue;				//jumps over vertices marked for deletion
		int m = l + 1;										
		for(int j = i + 1; j < NV_; j++){
			if(bbn.is_bit(j)) continue;				//jumps over vertices marked for deletion
			if(adj_[i].is_bit(j)){
				g.add_edge(l,m);
			}
			m++;
		}
		l++;
	}

}

//template<class T>
//void Graph<T>::remove_vertices (const BitSet& bbn){
/////////////////
//// Experimental: deletes input list of nodes by creating a temporal graph
////
//// OBSERVATIONS:
//// 1.Inefficient implementation with double allocation of memory
//
//	Graph<T> g;
//	this->remove_vertices(bbn,g);			//allocation 1
//	(*this)=g;								//allocation 2	
//}

template<class T>
int Graph<T>::degree_in (int v) const{

	int res=0;
	for(int i=0; i<NV_; i++){
		if(i == v) continue;
		if (adj_[i].is_bit(v)) { res++; }

	}
	return res;
}

template<class T>
void Graph<T>::make_bidirected (){

	for (std::size_t i = 0; i < NV_; ++i) {
		for (std::size_t j = 0; j < NV_; ++j) {
			if (is_edge(i, j)) add_edge(j, i);
			if (is_edge(j, i)) add_edge(i, j);
		}
	}
	
	NE_ = 0;	//resets edges to avoid lazy evaluation later
}

template<class T>
void Graph<T>::gen_random_edges(double p) {

	//removes all edges
	remove_edges();

	//sets directed edges with probability p
	for (std::size_t i = 0; i < NV_ ; ++i ) {
		for (std::size_t j = 0; j < NV_; ++j ) {
			if (::com::rand::uniform_dist(p)) {
				add_edge(i, j);
			}
		}
	}
}

template<class T>
int Graph<T>::gen_random_edge(int v, int w, double p){

	//assert - TODO condition to DEBUG mode
	if (v == w || v >= NV_ || w >= NV_ || p < 0 || p > 1) {
		LOG_ERROR("wrong input params - Graph<T>::gen_random_edge");
		return -1;
	}

	//generates edge
	if (::com::rand::uniform_dist(p)) {
		 add_edge(v, w);
	}
	
	return 0;
}

template<class T>
void Graph<T>::write_dimacs ( ostream& o)  {

	//timestamp comment
	timestamp_dimacs(o);

	//name comment
	name_dimacs(o);

	//dimacs header - recompute edges
	header_dimacs(o, false);

	//write edges 1-based vertex notation 
	for ( std::size_t v = 0; v < NV_; ++v ){
		for ( std::size_t w = 0; w < NV_; ++w ){
			if( v == w ) continue;
			if ( is_edge(v, w) ) {								//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << endl;		
			}
		}
	}
}

template<class T>
void  Graph<T>::write_EDGES	(ostream& o)   {

	//timestamp comment
	o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();
	
	//name comment
	if(!name_.empty())
		o << "\n%  "<< name_.c_str() << endl;
	
	//write edges 1-based vertex notation
	for( std::size_t v = 0; v < NV_; ++v ){
		for( std::size_t w = 0; w < NV_; ++w ){
			if(v == w) continue;
			if (is_edge(v, w)) {							//O(log) for sparse graphs: specialize
				o << v + 1 << " " << w + 1 << endl;				
			}
			
		}
	}
}

//////////////////////////
//
// SPECIALIZATIONS - deprecated
// (now included in the header simple_sparse_graph.h)
//
//////////////////////////

//template<>
//Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph(int size, Graph<sparse_bitarray>& newg) {
//	//////////////////////////
//	// creates new subgraph with vertex set V=[1,2,..., size]
//	//
//	// RETURNS: reference to the new subgraph
//
//		//assert is size required is greater or equal current size
//	if (size >= NV_ || size <= 0) {
//		LOG_ERROR("Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph-wrong shrinking size for graph. Remains unchanged");
//		return *this;
//	}
//
//	newg.init(size);
//
//	//copies to the new graph
//	for (int i = 0; i < newg.NV_; i++) {
//		newg.adj_[i] = adj_[i];
//		newg.adj_[i].clear_bit(size, EMPTY_ELEM);		//from size to the end
//	}
//
//	return newg;
//}
//
//template<>
//int Graph<sparse_bitarray>::shrink_to_fit(int size) {
//	/////////////////////
//	// shrinks graph to the size passed (must be less than current size)
//	if (NV_ <= size) {
//		LOG_ERROR("Graph<sparse_bitarray>::shrink_to_fit-wrong shrinking size for graph, remains unchanged");
//		return -1;
//	}
//
//	//trims vertices 
//	for (int i = 0; i < size; i++) {
//		adj_[i].clear_bit(size, EMPTY_ELEM);		//from size to the end
//	}
//
//	//resizes adjacency matrix
//	adj_.resize(size);
//	NV_ = size;
//	NE_ = 0;									//so that when needed will be recomputed
//
//	return 0;
//}
//
//
//template<>
//BITBOARD Graph<sparse_bitarray>::number_of_edges(bool lazy) {
//	//specialization for sparse graphs (is adjacent runs in O(log)) 
//	// 
//
//	if (lazy && NE_ != 0)
//		return NE_;
//
//	BITBOARD  edges = 0;
//	for (int i = 0; i < NV_; i++) {
//		edges += adj_[i].size();
//	}
//	NE_ = edges;
//
//	return NE_;
//}
//
//template<>
//double Graph<sparse_bitarray>::block_density()	const {
//	/////////////////////////
//	// specialization for sparse graphs
//	//
//	// RESULT: should be a 1.0 ratio, since only non-zero bitblocks are stored
//
//	size_t nBB = 0; size_t nBBt = 0;
//	for (int v = 0; v < NV_; ++v) {
//		nBBt += adj_[v].number_of_bitblocks();
//		for (int bb = 0; bb < adj_[v].number_of_bitblocks(); bb++) {
//			if (adj_[v].block(bb))
//				nBB++;
//		}
//	}
//
//	return nBB / double(nBBt);
//}
//
//template<>
//double Graph<sparse_bitarray>::block_density_index() {
//	/////////////////////////
//	// a measure of sparsity in relation to the number of bitblocks //
//	size_t nBB = 0; size_t nBBt = 0;
//	for (int v = 0; v < NV_; ++v) {
//		nBBt += adj_[v].number_of_bitblocks();
//	}
//
//	BITBOARD aux = ceil(NV_ / double(WORD_SIZE));
//	BITBOARD maxBlock = NV_ * aux;
//
//	cout << NV_ << ":" << aux << ":" << nBBt << ":" << maxBlock << endl;
//	return (double(nBBt)) / maxBlock;
//
//}
//
//template<>
//void Graph<sparse_bitarray>::write_dimacs(ostream& o) {
//	/////////////////////////
//	// writes file in dimacs format 
//
//		//***timestamp 
//
//		//name
//	if (!name_.empty())
//		o << "\nc " << name_.c_str() << endl;
//
//	//tamaño del grafo
//	o << "p edge " << NV_ << " " << number_of_edges() << endl << endl;
//
//	//Escribir nodos
//	for (int v = 0; v < NV_; v++) {
//		//non destructive scan of each bitstring
//		if (adj_[v].init_scan(bbo::NON_DESTRUCTIVE) != EMPTY_ELEM) {
//			while (1) {
//				int w = adj_[v].next_bit();
//				if (w == EMPTY_ELEM)
//					break;
//				o << "e " << v + 1 << " " << w + 1 << endl;
//			}
//		}
//	}
//}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

template class  Graph<bitarray>;
template class  Graph<sparse_bitarray>;

//sparse method specializations (now included in the header simple_sparse_graph.h)
//template Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph(int, Graph<sparse_bitarray>&);
//template int Graph<sparse_bitarray>::shrink_to_fit(int);
//template BITBOARD Graph<sparse_bitarray>::number_of_edges(bool);
//template double Graph<sparse_bitarray>::block_density()	const;
//template double Graph<sparse_bitarray>::block_density_index();
//template void Graph<sparse_bitarray>::write_dimacs(ostream& o);

////////////////////////////////////////////






