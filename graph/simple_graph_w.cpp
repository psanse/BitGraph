/*
 * simple_graph_w.cpp implementation file of the classes BaseGraph_W and Graph_W Ugraph (header simple_graph_w.h)
 *
 * @created 16/01/2019
 * @dev pss
 *
 * @comments see end of file for valid template types
 *
 */

#include <iostream>
#include <fstream>
#include "bitscan/bitscan.h"
#include "formats/dimacs_format.h"
#include "utils/prec_timer.h"
#include "utils/logger.h"
#include "utils/common.h"
#include "simple_graph_w.h"			
				
using namespace std;							


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

	if (g_.reset(lw.size()) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::Base_Graph_W");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}

	try{
		w_ = lw;
	}catch(...){
		LOG_ERROR("error during weigth assignment - Base_Graph_W<T, W>::Base_Graph_W");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}	
}

template<class Graph_t, class W>
inline Base_Graph_W<Graph_t, W>::Base_Graph_W(std::string filename){	
	
	if (read_dimacs(filename) == -1) {
		LOG_ERROR("error reading DIMACS file -Base_Graph_W<Graph_t, W>::Base_Graph_W");
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
int Base_Graph_W<Graph_t, W>::init(std::size_t NV , W val, bool reset_name){
	
	if (g_.reset(NV) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::init");
		return -1;
	}

	try{
		w_.assign(NV, val);
	}
	catch(...){
		LOG_ERROR("bad weight assignment - Base_Graph_W<Graph_t, W>::init");
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
	if (g_.reset(NV) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::reset");
		return -1;
	}

	try {
		w_.assign(NV, val);
	}
	catch (...) {
		LOG_ERROR("bad weight assignment - Base_Graph_W<Graph_t, W>::reset");
		return -1;
	}

	g_.set_name(name);

	return 0;
}

template<class Graph_t, class W>
void Base_Graph_W<Graph_t, W>::clear (){
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

	auto it = std::max_element(w_.begin(), w_.end());
	v = it - w_.begin();
	return *it;
}

///////////////
//
// I/O operations
//
//////////////

template<class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::write_dimacs(ostream& o) {
	/////////////////////////
	// writes file in dimacs format 

	//timestamp 
	o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();

	//name
	if (!g_.get_name().empty())
		o << "\nc " << g_.get_name().c_str() << endl;

	//tamaño del grafo
	const int NV = g_.number_of_vertices();
	o << "p edge " << NV << " " << g_.number_of_edges() << endl << endl;

	//write DIMACS nodes n <v> <w>
	//if (is_weighted_v()){
	for (int v = 0; v << NV; v++) {
		o << "n " << v + 1 << " " << get_w(v) << endl;
	}
	//}

	//write edges
	for (int v = 0; v < NV; v++) {
		for (int w = 0; w < NV; w++) {
			if (v == w) continue;
			if (g_.is_edge(v, w))							//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << endl;			//1 based vertex notation dimacs

		}
	}

	return o;
}

template<class W>
inline
ostream& Graph_W<ugraph, W>::write_dimacs(ostream& o) {
	/////////////////////////
	// writes file in dimacs format 

		//timestamp 
	o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();

	//name
	if (!ptype::g_.get_name().empty())
		o << "\nc " << ptype::g_.get_name().c_str() << endl;

	//tamaño del grafo
	const int NV = ptype::g_.number_of_vertices();
	o << "p edge " << NV << " " << ptype::g_.number_of_edges() << endl << endl;

	//write DIMACS nodes n <v> <w>
	//if (is_weighted_v()){
	for (int v = 0; v < NV; v++) {
		o << "n " << v + 1 << " " << ptype::get_w(v) << endl;
	}
	//}

	//write edges
	for (int v = 0; v < NV - 1; v++) {
		for (int w = v + 1; w < NV; w++) {
			//if(v==w) continue;
			if (ptype::g_.is_edge(v, w))					//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << endl;			//1 based vertex notation dimacs

		}
	}

	return o;
}

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
	if(::gio::dimacs::read_dimacs_header(f, size, nEdges) == -1){
		clear(); f.close(); return -1;
	}	
	
	reset(size);
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
		LOG_DEBUG("read_dimacs-no weights in file, initially assuming unit weights 1.0-Base_Graph_W<Graph_t, W>");
	}

/////////////////////	
//read edges

	//read the first edge to determine the type of input
	f.getline(line, 250); 
	stringstream sstr(line);
	int nw = ::com::counting::count_words(sstr.str());
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
int Base_Graph_W<Graph_t, W>::read_weights(const string& filename) {
	///////////////
	// reads weights from an external file (only weights)
	// 
	// REMARKS
	// I.assumes the format are separated (line, space,...) ordered weights
	// i.e. (5 6 7 ...)->w(1)=5, w(2)=6, w(3)=7,...
	// II. allocates memory for m_wv


	ifstream f(filename.c_str());
	if (f.fail()) {
		LOG_DEBUG("could not open weight file: ", filename);
		return -1;
	}

	LOG_INFO("reading vertex weights from: ", filename.c_str());

	//allocate memory
	const int NV = g_.number_of_vertices();
	w_.assign(NV, 0.0);

	//simple reading: assumes no empty lines and no
	double w = -1.0;
	for (int i = 0; i < NV; i++) {
		f >> w;
		if (f.fail()) {
			LOG_ERROR("bad reading of weights in:", filename);
			return -1;
		}
		w_[i] = w;		/*memory is already allocated for m_w*/
	}
	f.close();

	//check to screen
	/*for(int i=0; i<m_size; i++){
		cout<<m_wv[i]<<" ";
	}
	cout<<endl;*/

	return 0;
}



template<class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_data(bool lazy, std::ostream& o, bool endl) {
	g_.print_data(lazy, o, false);
	o << "\t w";								//adds tag to indicate it is weighted		
	if (endl) { o << std::endl; }
	return o;
}


template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (com::stack_t<int>& lv, ostream& o) const{

	for(std::size_t i=0; i<lv.pt; i++){
		o << "[" << lv.get_elem(i) << "," << w_[lv.get_elem(i)] << "] ";
	}
	o << "(" << lv.pt << ")" <<endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (int* lv, int NV, ostream& o) const{

	for(std::size_t i=0; i< NV; i++){
		o << "[" << lv[i] << "," << w_[lv[i]] << "] ";
	}
	o << "(" << NV << ")" << endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (com::stack_t<int>& lv, const vint& mapping, ostream& o) const{

	for(std::size_t i=0; i<lv.pt; i++){
		o << "[" << mapping[lv.get_elem(i)] << "," << w_[mapping[lv.get_elem(i)]] << "] ";
	}
	o << "(" << lv.pt << ")" << endl;
	return o;
}
			

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (vint& lv, ostream& o) const{
	
	for(std::size_t i=0; i<lv.size(); i++){
		o << "[" << lv[i] << "," << w_[lv[i]] << "] ";
	}
	o << "(" << lv.size() << ")" << endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (_bbt& bbsg, ostream& o) const{
	
	bbsg.init_scan(bbo::NON_DESTRUCTIVE);										/* CHECK sparse graphs */
	while(true){
		int v = bbsg.next_bit();
		if(v == EMPTY_ELEM) break;
		o << "[" << v << "," << w_[v] << "] ";
	}
	o << "(" << bbsg.popcn64() << ")" << endl;
	return o;
}

template <class Graph_t, class W>
inline
ostream& Base_Graph_W<Graph_t, W>::print_weights (ostream& o, bool show_v) const{

	const std::size_t NV = g_.number_of_vertices();

	if(show_v){
		for(std::size_t i = 0; i < NV; ++i){
			o << "[" << i << "," << w_[i] << "] ";
		}
		o << endl;
	}else{
		com::stl::print_collection<vector<W>>(w_, o, true);
	}
	return o;
}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

template class  Base_Graph_W<ugraph, int>;
template class  Base_Graph_W<ugraph, double>;
template class  Graph_W<ugraph, int>;
template class  Graph_W<ugraph, double>;

//other specializations... (sparse_graph)


////////////////////////////////////////////




