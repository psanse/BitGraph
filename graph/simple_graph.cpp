/*  
 * simple_graph.cpp implementation file of the class Graph (header simple_graph.h)
 * 
 * @comments: see end of file for valid template types
 */

 //////////////////
 //switches 

 //#define DIMACS_REFERENCE_VERTICES_0			//DEFAULT OFF! (real DIMACS format)


#include "bitscan/bitscan.h"
#include "formats/dimacs_reader.h"
#include "formats/mmio.h"
#include "formats/edges_reader.h"
#include "formats/mmx_reader.h"
#include "simple_graph.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

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
	//if (status == -1) { LOG_ERROR("error when opening file, exiting...-Graph<T>::Graph"); exit(-1); }
}

template<class T>
Graph<T>::Graph(int size) {
	//creates empty graph of size
	name_.clear();
	path_.clear();
	init(size);
}

template <class T>
Graph<T>::Graph(int nV, int* adj[], string filename) {
	if (init(nV) == -1) {
		LOG_ERROR("bizarre graph construction-Graph<T>::Graph(...), exiting... ");
		exit(-1); 
	}
	set_name(filename);		
	
	//add edges
	for (int i = 0; i < nV; i++) {
		for (int j = 0; j < nV; j++) {
			if (adj[i][j] == 1) {
				add_edge(i, j);
			}
		}
	}
}

template <class T>
ostream& Graph<T>::print_adj (std::ostream& o, bool add_endl){
////////////////////
// prints adjacency 1-0 matrix (vertex weights removed, cannot be here!)
//
// COMMENTS: vertex weights removed! cannot be  here (28/07/19)
//
// TODO@ test (3/12/17)

		
	for(int i=0; i<NV_; i++){
		for(int j=0; j<NV_; j++){
			/*if(i==j && m_is_wv){
				o<<m_wv[i]<<" ";
			}else*/ if(is_edge(i,j)){
				o<<"1"<<" ";
			}else {o<<"0"<<" ";}			
		}
		o<<endl;
	}

	if(add_endl) o<<endl;
	return o;	
}



template<class T>
void Graph<T>::set_name(std::string name, bool separate_path){
//name for file of weights assumed to be the same as "name" but with a w
	
	if(separate_path){
		std::string str=name;
		size_t found=str.find_last_of("/\\");
		name_=str.substr(found+1);
		path_=str.substr(0,found+1);  //includes slash
	}else{
		name_=name;
		path_.clear();
	}
	

	//adding name for weights in nodes (6/10/16)
	//std::string str=name;
	//string wname=str.substr(str.find_last_of("/\\")+1);
	//wname="w"+wname;
	//string str1(name);
	//m_wname=str.substr(0,str1.find_last_of("/\\")+1);
	//m_wname+=wname;
}

//template<class T>
//Graph<T>& Graph<T>::operator= (const Graph& g){
///////////////////
//// Sets graph (allocates memory)
//
//	if(this==&g) return *this;		//same graph
//
//	//allocation
//	init(g.NV_);					 //possibly throw exception
//	for(int i=0; i<NV_; i++){
//					adj_[i]=g.adj_[i];
//	}
//	
//	this->name_=g.name_;
//	this->path_=g.path_;	
//				
//return *this;
//}

template<class T>
void Graph<T>::clear (){
	adj_.clear(), name_.clear(), path_.clear();
	NV_=0, NBB_=0, NE_=0;
}

template<class T>
int Graph<T>::init(int size){
///////////////////////////
// Allocates memory for the empty graph (new 18/06/19)
//

//deprecated--
	//deallocates previous
	//clear();
	//	
	//try{
	//	adj_.resize(size);	
	//}catch(...){
	//	cout<<"memory for graph not allocated";
	//	return -1;
	//}

//new simple allocation
	NE_ = 0;
	try{
		adj_.assign(size, T(size));				//old and new bitstrings have different sizes!! (17/01/2023) (1)
	}catch(...){
		NV_ = 0;
		NBB_ = 0;		
		LOG_ERROR("memory for graph not allocated-Graph<T>::init");
		return -1;
	}

	NV_=size;
	NBB_=INDEX_1TO1(NV_);
		
	//zero edges
	for(int i=0; i<NV_; i++){
		adj_[i].init(NV_);					//MUST BE!  (1)
	}


return 0;
}

template<class T>
int Graph<T>::add_vertex(int toADD) {
	///////////////////////////
	// Allocates memory for the enlarged graph |V'|= |V| + toADD
	// with an empty neighborhood
	
	NV_ += toADD;

	try {
		adj_.resize(NV_);
	}
	catch (...) {
		LOG_INFO("memory for graph not allocated-Graph<T>::enlarge(...), exiting");
		exit(-1);
	}

	return 0;
}



template<class T>
Graph<T>& Graph<T>::create_subgraph (int size, Graph<T>& newg)  {
//////////////////////////
// creates new subgraph with subset of size vertices 

	//assert is size required is greater or equal current size
	if(size >= NV_ || size <= 0){
		LOG_ERROR("Graph<T>& Graph<T>::create_subgraph-wrong shrinking size for graph. Remains unchanged");
		return *this;
	}
		
	newg.init(size);
	int bbh=WDIV(size-1);

	
	for(int i=0; i<newg.NV_; i++){
		for(int j=0; j<=bbh; j++){
			newg.adj_[i].get_bitboard(j)=adj_[i].get_bitboard(j);
		}
		//trims last bitblock
		newg.adj_[i].get_bitboard(bbh)&=~Tables::mask_left[WMOD(size-1)];
	}
	
return newg;
}


template<class T>
int Graph<T>::shrink_to_fit(int size){
/////////////////////
// shrinks graph to the size passed (must be less than current size)
	
	LOG_ERROR("Graph<T>::shrink_to_fit-shrinking is only possible in sparse graphs; the graph remains unchanged");

	return -1;
}

template<class T>
int Graph<T>::set_graph (string filename){
	if(read_dimacs(filename)==-1){
		if(read_mtx(filename)==-1){
			if(read_EDGES(filename)==-1){
				LOG_ERROR("Graph<T>::set_graph-unable to read file in any of these formats: DIMACS/MTX/EDGES");
				return -1;
			}
		}
	}
return 0;
}

template<class T>
void Graph<T>::add_edge (int v, int w){
///////////////
// sets v-->w (no self loops allowed)
	if(v!=w){
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
//removes in-out edges from v
	
	//erases all outgoing edges from v
	adj_[v].erase_bit();

	//erases all ingoing edges
	for(int w=0; w<NV_; w++){
		if(w==v) continue;
		adj_[w].erase_bit(v);
	}

	//updates edges
	number_of_edges(false);
}

template<class T>
int Graph<T>::read_dimacs(const string& filename){
/////////////////////////////
// Reads an unweighted simple directed graph in dimacs format
// 'c' comment
// 'p' next numbers in the line are size and number of edges
// 'e' next in the lines is an edge
// 
// RETURN VALUE: 0 if correct, -1 in case of error (no memory allocation)
//
// Extension (9/10/16): Will awways attempt to read weights for nodes from 
//						filename.w file
//
// Extension (6/09/17): Reading weights in DIMACS format (n <V> <W(V)>)
//////////////////////
	
	int size, nEdges, v1, v2, edges=0;
	//_wt wv=0.0;
	/*char token[10];*/ char line [250]; char c;
	
	fstream f(filename.c_str());
	if(!f){
		LOG_ERROR("Graph<T>::read_dimacs-File could not be opened reading DIMACS format");
		clear();
		return -1;
	}

	if(DIMACS_READER::read_dimacs_header(f, size, nEdges)==-1){
		clear(); f.close(); return -1;
	}	
	
	init(size);
	DIMACS_READER::read_empty_lines(f);

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
	//	DIMACS_READER::read_empty_lines(f);
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
	for(int e=0 /*1*/; e<nEdges; e++){
		f>>c;
		if(c!='e'){
			LOGG_ERROR(filename, ":wrong header for edges reading DIMACS format");
			clear();
			f.close();
			return -1;
		}

		//read and add edge
		f>>v1>>v2;
#ifdef DIMACS_REFERENCE_VERTICES_0 
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
			
		f.getline(line, 250);  //remove remaining part of the line
	}
	f.close();
	
	//name (removes path)
	set_name(filename, true);

	//extension for weighted files (9/10/16)
	string str(filename);					//filename contains the full path

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
////////////////////////
// reads 0-1 adjacency matrix (rows) with a first line indicating n
		
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
	set_name(filename, true);
	return 0;
}

template<class T>
int  Graph<T>::read_mtx	(const string& filename){
//////////////////
// reads matrix exchange format (at the moment only MCPS)

	MMI<Graph<T> > myreader(*this);
	return (myreader.read(filename));
}

template<class T>
int  Graph<T>::read_EDGES (const string& filename){
//////////////////
// reads matrix exchange format (at the moment only MCPS)

	EDGES<Graph<T> > myreader(filename, *this);
	return (myreader.read());
}

template<class T>
ostream& Graph<T>::print_data( bool lazy, std::ostream& o, bool endl) {
	o << name_.c_str() << "\t" << number_of_vertices() << "\t" << std::fixed << number_of_edges(lazy) << "\t" << std::setprecision(6) << density(true);
	if (endl) { o << std::endl; }
	return o;
}

template<class T>

ostream& Graph<T>::print_edges (std::ostream& o) const{
	for(int i=0; i<NV_-1; i++){
		for(int j=i+1; j<NV_; j++){
			if(is_edge(i,j)){
				o<<"["<<i<<"]"<<"-->"<<"["<<j<<"]"<<endl;
			}
			if(is_edge(j,i)){
				o<<"["<<j<<"]"<<"-->"<<"["<<i<<"]"<<endl;
			}
		}
	}
	return o;
}

template<class T>
ostream& Graph<T>::print_edges (T& bbsg, ostream& o)const {
/////////////
// TODO-optimize
	for(int i=0; i<NV_-1; i++){
		if(!bbsg.is_bit(i)) continue;
		for(int j=i+1; j<NV_; j++){
			if(!bbsg.is_bit(j)) continue;
			if(is_edge(i,j)){
				o<<"["<<i<<"]"<<"-->"<<"["<<j<<"]"<<endl;
			}
			if(is_edge(j,i)){
				o<<"["<<j<<"]"<<"-->"<<"["<<i<<"]"<<endl;
			}
		}
	}
	return o;
}

template<class T>
BITBOARD Graph<T>::number_of_edges	(const T& bbn) const{
////////////////
// Number of edges in the graph induced by bbn 
// Source: 17/6/10 
// Last Updated: 12/4/12  
	
	BITBOARD edges=0;
	for(int i=0; i<NV_; i++){
		if(bbn.is_bit(i)){
			for(int j=0; j<NV_; j++)
				if(bbn.is_bit(j)){
					if(adj_[i].is_bit(j)) edges++;
				}
		}
	}
return edges;
}

template<class T>
BITBOARD Graph<T>::number_of_edges	(bool lazy) {
////////////
// Computes edges only once and caches its value
//
// REMARKS: Can be an expensive operation
	if (lazy && NE_ != 0) { return NE_; }
	
	BITBOARD  edges=0;
	for(int i=0; i<NV_; i++){
		for(int j=0; j<NV_; j++){
			if(is_edge(i,j)){			//O(log) in sparse graphs (specialize)
				edges++;
			}
		}
	}
	NE_=edges;
	

return NE_;
}



template<class T>
double Graph<T>::density (bool lazy) {
	BITBOARD max_edges=NV_;									//important for very large graphs as (I) is bigger than unsigned int
	max_edges*=(max_edges-1);									//(I)
	return (number_of_edges(lazy)/(double)max_edges);				//n*(n-1) edges
}


template<class T>
double Graph<T>::block_density	()	const{
/////////////////////////
// number of empty bitblocks wrt total amount 
//
// REMARKS: has to be specialized for sparse graphs

	size_t nBB=0;
	for(int v=0; v<NV_; ++v){
		for(int bb=0; bb<NBB_; bb++){
			if(adj_[v].get_bitboard(bb))				
				nBB++;
		}
	}

return (nBB/static_cast<double>(NBB_*NV_));
}


template<>
double Graph<sparse_bitarray>::average_block_density_index()	{
/////////////////////////
// average of density per block
//
	size_t nBB=0; size_t nBBt=0;
	double den=0.0;
	for(int v=0; v<NV_; ++v){
		nBB=adj_[v].number_of_bitblocks();
		nBBt+=nBB;
		int pc=adj_[v].popcn64();
		den+=double(pc)/(BITBOARD(nBB)*WORD_SIZE);
	}
		
	return (den/nBBt);
}

template<class T>
double Graph<T>::density(const T & bbN )  {
	unsigned long long  edges=number_of_edges(bbN);
	if(edges==0) return 0.0;
	unsigned long long  pc=bbN.popcn64();
	return edges/(double)(pc*(pc-1)/2);
}


template<class T>
bool Graph<T>::is_edge	(int i, int j) const{
//////////////////
// true if there is an edge (i,j)
// First created: 14/10/2010

	return(adj_[i].is_bit(j));
}


template<class T>
bool Graph<T>::is_no_self_loops () const{
//////////////////
// true if there is no edge (v,v) 

	for(int i=0; i<NV_; i++)
		if( adj_[i].is_bit(i)) {
			cout<<"Graph with self-loops in:"<<i<<endl;
			return false;
		}
return true;
}


template<class T>
void Graph<T>::remove_vertices (const BitBoardN& bbn, Graph& g){
/////////////////////
// returns in g the induced graph by bbn

	NE_=0;									//resets vertices
	int pc=bbn.popcn64();
	int new_size=NV_-pc;
	if(pc==0){return;}
	else if(new_size==0){ g.clear();}				//empty graph
	
	g.init(new_size);
	g.set_name(this->name_);
	int l=0; int m=0;
	
	//runs through the whole graph
	for(int i=0; i<NV_-1; i++){
		if(bbn.is_bit(i))continue;				//jumps over vertices marked for deletion
		m=l+1;										
		for(int j=i+1; j<NV_; j++){
			if(bbn.is_bit(j)) continue;			//jumps over vertices marked for deletion
			if(adj_[i].is_bit(j)){
				g.add_edge(l,m);
			}
			m++;
		}
		l++;
	}
return ;
}

template<class T>
void Graph<T>::remove_vertices (const BitBoardN& bbn){
///////////////
// Experimental: deletes input list of nodes by creating a temporal graph
//
// OBSERVATIONS:
// 1.Inefficient implementation with double allocation of memory

	Graph<T> g;
	this->remove_vertices(bbn,g);			//allocation 1
	(*this)=g;								//allocation 2	
}


template<class T>
int Graph<T>::degree_out (int v)const{
///////////////////
// outcoming edges from v

	return adj_[v].popcn64();
}


template<class T>
int Graph<T>::degree_in (int v)const{
///////////////////
// incoming edges to v (endpoint in v)
	int res=0;
	for(int i=0; i<NV_; i++){
		if(i==v) continue;
		if(adj_[i].is_bit(v)) 
			res++;
	}
return res;
}

template<class T>
void Graph<T>::make_bidirected (){
///////////////////
// makes all edges symmetrical
	for(int i=0; i<NV_; i++){
		for(int j=0; j<NV_; j++){
			if(is_edge(i,j)) add_edge(j, i);
			if(is_edge(j,i)) add_edge(i, j);
		}
	}
	
	NE_=0;	//resets edges to avoid lazy evaluation later
}

template<class T>
void Graph<T>::write_dimacs (ostream& o) {
/////////////////////////
// writes file in dimacs format 
	
	//timestamp 
	o<<"c File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!name_.empty())
		o<<"\nc "<<name_.c_str()<<endl;

	//tamaño del grafo
	o<<"p edge "<<NV_<<" "<<number_of_edges()<<endl<<endl;

	//write DIMACS nodes n <v> <w>
	//if (is_weighted_v()){
	//	for(int v=0; v<NV_; v++){
	//		o<<"n "<<v+1<<" "<<get_wv(v)<<endl;
	//	}
	//}
	
	//write edges
	for(int v=0; v<NV_; v++){
		for(int w=0; w<NV_; w++){
			if(v==w) continue;
			if(is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<"e "<<v+1<<" "<<w+1<<endl;		//1 based vertex notation dimacs
			
		}
	}
}

template<class T>
void  Graph<T>::write_EDGES	(ostream& o){
/////////////////////////
// writes simple unweighted graphs  in edge list format 
// note: loops are not allowed
		
	//timestamp 
	o<<"% File written by GRAPH:"<<PrecisionTimer::local_timestamp();
	
	//name
	if(!name_.empty())
		o<<"\n%  "<<name_.c_str()<<endl;
	
	//write edges
	for(int v=0; v<NV_; v++){
		for(int w=0; w<NV_; w++){
			if(v==w) continue;
			if(is_edge(v,w) )							//O(log) for sparse graphs: specialize
					o<<v+1<<" "<<w+1<<endl;				//1 based vertex notation dimacs
			
		}
	}
}

////////////////////////
//
// SPECIALIZATIONS
//
////////////////////////

template<>
Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph(int size, Graph<sparse_bitarray>& newg) {
	//////////////////////////
	// creates new subgraph with vertex set V=[1,2,..., size]
	//
	// RETURNS: reference to the new subgraph

		//assert is size required is greater or equal current size
	if (size >= NV_ || size <= 0) {
		LOG_ERROR("Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph-wrong shrinking size for graph. Remains unchanged");
		return *this;
	}

	newg.init(size);

	//copies to the new graph
	for (int i = 0; i < newg.NV_; i++) {
		newg.adj_[i] = adj_[i];
		newg.adj_[i].clear_bit(size, EMPTY_ELEM);		//from size to the end
	}

	return newg;
}

template<>
int Graph<sparse_bitarray>::shrink_to_fit(int size) {
	/////////////////////
	// shrinks graph to the size passed (must be less than current size)
	if (NV_ <= size) {
		LOG_ERROR("Graph<sparse_bitarray>::shrink_to_fit-wrong shrinking size for graph, remains unchanged");
		return -1;
	}

	//trims vertices 
	for (int i = 0; i < size; i++) {
		adj_[i].clear_bit(size, EMPTY_ELEM);		//from size to the end
	}

	//resizes adjacency matrix
	adj_.resize(size);
	NV_ = size;
	NE_ = 0;									//so that when needed will be recomputed

	return 0;
}


template<>
BITBOARD Graph<sparse_bitarray>::number_of_edges(bool lazy) {
	//specialization for sparse graphs (is adjacent runs in O(log)) 
	// 

	if (lazy && NE_ != 0)
		return NE_;

	BITBOARD  edges = 0;
	for (int i = 0; i < NV_; i++) {
		edges += adj_[i].popcn64();
	}
	NE_ = edges;

	return NE_;
}

template<>
double Graph<sparse_bitarray>::block_density()	const {
	/////////////////////////
	// specialization for sparse graphs
	//
	// RESULT: should be a 1.0 ratio, since only non-zero bitblocks are stored

	size_t nBB = 0; size_t nBBt = 0;
	for (int v = 0; v < NV_; ++v) {
		nBBt += adj_[v].number_of_bitblocks();
		for (int bb = 0; bb < adj_[v].number_of_bitblocks(); bb++) {
			if (adj_[v].get_bitboard(bb))
				nBB++;
		}
	}

	return nBB / double(nBBt);
}

template<>
double Graph<sparse_bitarray>::block_density_index() {
	/////////////////////////
	// a measure of sparsity in relation to the number of bitblocks //
	size_t nBB = 0; size_t nBBt = 0;
	for (int v = 0; v < NV_; ++v) {
		nBBt += adj_[v].number_of_bitblocks();
	}

	BITBOARD aux = ceil(NV_ / double(WORD_SIZE));
	BITBOARD maxBlock = NV_ * aux;

	cout << NV_ << ":" << aux << ":" << nBBt << ":" << maxBlock << endl;
	return (double(nBBt)) / maxBlock;

}

template<>
void Graph<sparse_bitarray>::write_dimacs(ostream& o) {
	/////////////////////////
	// writes file in dimacs format 

		//***timestamp 

		//name
	if (!name_.empty())
		o << "\nc " << name_.c_str() << endl;

	//tamaño del grafo
	o << "p edge " << NV_ << " " << number_of_edges() << endl << endl;

	//Escribir nodos
	for (int v = 0; v < NV_; v++) {
		//non destructive scan of each bitstring
		if (adj_[v].init_scan(bbo::NON_DESTRUCTIVE) != EMPTY_ELEM) {
			while (1) {
				int w = adj_[v].next_bit();
				if (w == EMPTY_ELEM)
					break;
				o << "e " << v + 1 << " " << w + 1 << endl;
			}
		}
	}
}


////////////////////////////////////////////
//list of valid types for cpp code generation

template class  Graph<bitarray>;
template class  Graph<sparse_bitarray>;

//sparse method specializations
template Graph<sparse_bitarray>& Graph<sparse_bitarray>::create_subgraph(int, Graph<sparse_bitarray>&);
template int Graph<sparse_bitarray>::shrink_to_fit(int);
template BITBOARD Graph<sparse_bitarray>::number_of_edges(bool);
template double Graph<sparse_bitarray>::block_density()	const;
template double Graph<sparse_bitarray>::block_density_index();
template void Graph<sparse_bitarray>::write_dimacs(ostream& o);

////////////////////////////////////////////






