 /**
   * @file simple_graph_ew.cpp
   * @brief implementation for Base_Graph_EW and Graph_EW classes (edge-weighted graphs)
   *
   * @created 16/01/19
   * @last_update 06/01/25
   * @author pss
   *
   * @comments see end of file for valid template types
   *
   * This code is part of the GRAPH 1.0 C++ library
   *
   **/

#include "bitscan/bitscan.h"
#include "graph/formats/dimacs_format.h"
#include "graph/simple_graph_ew.h"	
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/prec_timer.h"
#include <fstream>
#include <iostream>

//works when NDEBUG is undefined - normally when the compiler is working in DEBUG mode
#include <cassert>

using namespace std;	

/////////////////////////////////////////////////
template<class Graph_t, class W>
const W Base_Graph_EW <Graph_t, W >::NOWT = 0;					// or 0x1FFFFFFF (best value for empty weight?)									
/////////////////////////////////////////////////


//template<class Graph_t, class W>
//int Base_Graph_EW<Graph_t, W>::init(int NV, W val, bool reset_name){
//
//	if (g_.reset(NV) == -1) {
//		LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::init");
//		return -1;
//	}
//	
//	try {
//		we_.assign(NV, vector<W>(NV, val));
//	}
//	catch (...) {
//		LOG_ERROR("bad weight assignment - Base_Graph_EW<Graph_t, W>::init");
//		return -1;
//	}
//
//	if (reset_name) {
//		g_.name("");
//		g_.path("");
//	}
//
//return 0;
//}


template<class Graph_t, class W>
 int Base_Graph_EW<Graph_t, W>::reset(std::size_t NV, W val, string name)
{
	if (g_.reset(NV) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_W<T, W>::reset");
		return -1;
	}

	try {
		we_.assign(NV, vector<W>(NV, val));
	}
	catch (...) {
		LOG_ERROR("bad weight assignment - Base_Graph_EW<Graph_t, W>::init");
		return -1;
	}

	g_.name(name);

	return 0;	
}

template<class Graph_t, class W>
 bool Base_Graph_EW<Graph_t, W>::is_consistent(){

	auto NV = number_of_vertices();

	for (std::size_t i = 0; i < NV; ++i) {
		for (std::size_t j = 0; j < NV; ++j) {
			if ( we_[i][j] == NOWT &&  g_.is_edge(i, j) ) {
				LOGG_WARNING ("edge [", i, ", ", j ,")] with def. weight - Base_Graph_EW<Graph_t, W>::is_consistent()");
				return false;
			}
		}
	}
	return true;
}

template<class Graph_t, class W>
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(mat_t& lwe) : we_ (lwe) {

	if (g_.reset(lwe.size()) == -1) {
		LOG_ERROR("error during memory graph allocation - Base_Graph_EW<T, W>::Base_Graph_EW");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}


template<class Graph_t, class W>
Base_Graph_EW<Graph_t, W>::Base_Graph_EW(string filename){
	read_dimacs	(filename);							
}


template<class Graph_t, class W>
void Base_Graph_EW<Graph_t, W>::add_vertex_weight( W val) {

	auto NV = number_of_vertices();

	for (std::size_t v = 0; v < NV; v++) {
			we_[v][v] = val;
	}	

}

template <class Graph_t, class W>
void Base_Graph_EW<Graph_t, W >::add_edge_weight(int v, int w, W val) {
	
	////////////////
	assert(v != w);
	////////////////

	if (g_.is_edge(v, w)) {
		we_[v][w] = val;
	}
	else {
		LOGG_WARNING("edge-weight cannot be added to a non-edge", "(", v, ",", w, ")", "- Base_Graph_EW<Graph_t,W >::add_edge_weight");
	}

	/*if (v == w || g_.is_edge(v, w)) {		
		we_[v][w] = val;
	}
	else {
		LOGG_WARNING ("edge weight cannot be added to the non-edge" , "(" , v , "," , w , ")" , "- Base_Graph_EW<Graph_t,W >::add_edge_weight");
		LOG_WARNING ("weight not added");
		return -1;
	}	*/

//	return 0;
}

template <class Graph_t, class W>
void Base_Graph_EW< Graph_t, W>::add_edge_weight(W val) {
	   
	auto NV = number_of_vertices();

	//set to empty weight the non-edges
	for (int v = 0; v < NV; v++) {
		for (int w = 0; w < NV; w++) {
			if (g_.is_edge(v, w)) {
				we_[v][w] = val;
			}
			else {
				we_[v][w] = NOWT;
			}
		}
	}
}



template<class Graph_t, class W>
 void Base_Graph_EW<Graph_t, W>::neg_w(){

	auto NV = number_of_vertices();	

	for(int i=0; i<NV; i++)
		for (int j = 0; j < NV; j++) {
			if(we_[i][j] != NOWT)
				we_[i][j] = -we_[i][j];
		}
}


template<class Graph_t, class W>
 void Base_Graph_EW<Graph_t, W>::gen_modulus_weights(int MODULUS) {

	auto NV = number_of_vertices();

	//vertex-weights NOWT
	add_vertex_weight(NOWT);

	for (std::size_t v = 0; v < NV; ++v) {
		for (std::size_t w = 0; w < NV; ++w) {
			
			if (g_.is_edge(v, w)) {

				///////////////////////////////////////
				add_edge_weight(v, w, ( 1 + ( (v + w + 2 /* 0-based index*/) % MODULUS) )   );
				///////////////////////////////////////
			}			
		}
	}
}


template<class Graph_t, class W>
const vecw<W>& Base_Graph_EW<Graph_t, W>::vertex_weights()  const {

	auto NV = number_of_vertices();

	vector<W> res;
	res.reserve(NV);
		for (int v = 0; v < NV; v++) {
			res.emplace_back(we_[v][v]);
		}
	return res;
}

///////////////
//
// I/O
//
//////////////

template<class Graph_t, class W>
int Base_Graph_EW<Graph_t, W>::read_dimacs (string filename){
		
	string line;	
		
	fstream f(filename.c_str());
	if(!f){
		LOGG_ERROR("error when reading file ", filename, " in DIMACS format - Base_Graph_EW<Graph_t, W>");
		reset();
		return -1;
	}

	//read header
	 int nV = -1, nEdges = -1;
	 if (::gio::dimacs::read_dimacs_header (f, nV, nEdges) == -1) {
		 reset();
		 f.close();
		 return -1;
	 }

	 //allocates memory for the graph, assigns default NOWT edge-weights
	 if (reset(nV) == -1) {
		 reset();
		 f.close();
		 return -1;
	 }

	 //skips empty lines
	 ::gio::read_empty_lines(f);

	 //////////////
	//read vertex-weights format <n> <vertex index> <weight> if they exist
	 int v1 = -1, v2 = -1;
	 W wv = NOWT;
	 char c;
	 c = f.peek();
	 switch (c) {
	 case 'n':
	 case 'v':									// 'v' format used by Zavalnij in evil_W benchmark 

		 for (int n = 0; n < nV; ++n) {
			 f >> c >> v1 >> wv;

			 //assert
			 if (f.bad()) {
				 LOG_ERROR("error when reading vertex-weights - Base_Graph_EW<Graph_t, W>::read_dimacs");
				 reset();
				 f.close();
				 return -1;
			 }

			 //non-positive vertex-weight check
			 if (wv <= 0.0) {
				 LOGG_WARNING("non-positive weight read: ", wv, "- Base_Graph_EW<Graph_t, W>::read_dimacs");
			 }

			 //////////////////////////
			 we_[v1 - 1][v1 - 1] = wv;
			 //////////////////////////
			 			 
			 std::getline(f, line);  //remove remaining part of the line
		 }

		 //skip empty lines
		 ::gio::read_empty_lines(f);

		 break;
	 default:
		 LOGG_DEBUG("missing vertex-weights in file ", filename, " setting unit weights - Base_Graph_EW<Graph_t, W>::read_dimacs");
	 }
	 	
	 /////////////////////	
	 //read edges
	 
	 //read the first edge line - 4 tokens expected (e <v> <w> <edge-weight>)
	 c = f.peek();
	 if (c != 'e') {
		 LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<Graph_t, W>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }

	 W we = NOWT;
	 std::getline(f, line);
	 stringstream sstr(line);

	 //determine if edge weights are present
	 int nw = ::com::counting::number_of_words(line);
	 bool edge_weights_found = true;
	 switch (nw) {
	 case 4:
		 edge_weights_found = true;		//expected: e <v> <w> <edge-weight> 
		 break;
	 case 3:
		 edge_weights_found = false;	//expeceted: e <v> <w>  	
		 break;
	 default:
		 LOG_ERROR("Wrong edge format - Base_Graph_EW<Graph_t, W>::read_dimacs");
		 reset();
		 f.close();
		 return -1;
	 }

	 //parse the first edge
	 if (edge_weights_found) {

		 /////////////////////////////
		 sstr >> c >> v1 >> v2 >> we;
		 ////////////////////////////

		 g_.add_edge(v1 - 1, v2 - 1);
		 add_edge_weight (v1 - 1, v2 - 1, we);
	 }
	 else {	

		 ///////////////////////
		 sstr >> c >> v1 >> v2;
		 ///////////////////////

		 g_.add_edge(v1 - 1, v2 - 1);
		 add_edge_weight (v1 - 1, v2 - 1, NOWT);				//no edge-weights in file - set NOWT value
	 }
	 
	 //remaining edges
	 for (int e = 1; e < nEdges; ++e) {
		 f >> c;
		 if (c != 'e' || f.bad()) {
			 LOG_ERROR("Wrong edge format reading edges - Base_Graph_EW<Graph_t, W>::read_dimacs");
			 reset();
			 f.close();
			 return -1;
		 }

		 //add weighted edge	
		 if (edge_weights_found) {

			 /////////////////////
			 f >> v1 >> v2 >> we;
			 /////////////////////

			 g_.add_edge (v1 - 1, v2 - 1);
			 add_edge_weight (v1 - 1, v2 - 1, we);
		 }
		 else {
			 ///////////////
			 f >> v1 >> v2;
			 ///////////////

			 g_.add_edge(v1 - 1, v2 - 1);
			 add_edge_weight (v1 - 1, v2 - 1, NOWT);			//no edge-weights in file - set NOWT value
		 }

		 std::getline(f, line);  //remove remaining part of the line
	 }

	 //name (removes path)
	 g_.name(filename);

	 f.close();	

	 return 0;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::write_dimacs (std::ostream& o) {
	
	auto NV = number_of_vertices();

	//timestamp 
	g_.timestamp_dimacs(o);
	
	//name
	g_.name_dimacs(o);

	//dimacs header - recompute edges
	g_.header_dimacs(o, false);

	//write vertex weights (edge weights of self loops are considered vertex weights)
	for (std::size_t v = 0; v < NV; ++v) {
		if (we_[v][v] != NOWT) {
			o << "n " << v + 1 << " " << we_[v][v] << std::endl;
		}
	}
		
	//write (directed) edges and edge-weights
	for(std::size_t v = 0; v < NV; ++v){
		for(std::size_t w = 0; w < NV; ++w){
			if (v != w && g_.is_edge(v, w)) {										
				o << "e " << v + 1 << " " << w + 1 << we_[v][w] << std::endl;			//1-based vertex notation dimacs format	
			}
		}
	}

	return o;
}


template <class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_weights (ostream& o, bool line_format, bool only_vertex_weights) const{

	auto NV = number_of_vertices();

	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		if (line_format) {
			for (std::size_t i = 0; i < NV; ++i) {
				for (std::size_t j = 0; j < NV; ++j) {
					if (we_[i][j] != Base_Graph_EW<Graph_t, W>::NOWT) {
						o << "[" << i << "->" << j << " (" << we_[i][j] << ")] " << endl;
					}
				}
			}
		}
		else {								//outputs to stream edge-weights in matrix form
			for (std::size_t i = 0; i < NV; ++i) {
				for (std::size_t j = 0; j < NV; ++j) {
					if (we_[i][j] != Base_Graph_EW<Graph_t, W>::NOWT) {
						o << we_[i][j] << '\t';
					}
					else {
						o << "--" << '\t';
					}
				}
				o << endl;
			}
		}
	}
	else {	
		//streams vertex weights (weights in self-loops)
		for (std::size_t v = 0; v < NV; v++) {
			if (we_[v][v] != NOWT) {
				o << "[" << v << " (" << we_[v][v] << ")] " << endl;
			}
		}
	}

	o << "**********************************************************" << endl;

	return o;
}
template <class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_weights (vint& lv, ostream& o, bool only_vertex_weights) const{

	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		for (std::size_t i = 0; i < lv.size(); ++i) {
			for (std::size_t j = 0; j < lv.size(); j++) {
				if (we_[lv[i]][lv[j]] != Base_Graph_EW<Graph_t, W>::NOWT) {
					o << "[" << lv[i] << "->" << lv[j] << " (" << we_[lv[i]][lv[j]] << ")] " << endl;
				}
			}
		}
	}
	else {
		//streams vertex weights (weights in self-loops)
		for (std::size_t i = 0; i < lv.size(); i++) {
			if (we_[lv[i]][lv[i]] != NOWT) {
				o << "[" << lv[i] << " (" << we_[lv[i]][lv[i]] << ")] " << endl;
			}
		}
	}

	o << "**********************************************************" << endl;

	return o;
}

template<class Graph_t, class W>
ostream& Base_Graph_EW<Graph_t, W>::print_data (bool lazy, std::ostream& o, bool endl) {
	g_.print_data(lazy, o, false);
	o << " [type:ew]";								//"w" for vertex-weighted graphs in GRAPH lib
	if (endl) { o << std::endl; }
	return o;
}

template<class Graph_t, class W>
std::ostream& Base_Graph_EW<Graph_t, W>::print_edges(std::ostream& o, bool eofl) {

	const int NV = g_.number_of_vertices();
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (is_edge(i, j)) {
				o << "[" << i << "]" << "-(" << edge_weight(i, j) << ")->" << "[" << j << "]" << endl;
			}
			if (is_edge(j, i)) {
				o << "[" << j << "]" << "-(" << edge_weight(i, j) << ")->" <<  "[" << i << "]" << endl;
			}

		}
	}

	if (eofl) {	o << std::endl;	}
	return o;
}


///////////////////////////
//
// Graph_EW implementation
//
///////////////////////////

template<class W>
int Graph_EW<ugraph, W>::create_complement(Graph_EW<ugraph, W>& g) const {

	g.name(this->name());
	g.path(this->path());
	g.edge_weights() = ptype::we_;
	ptype::g_.create_complement(g.graph());

	return 0;
}

template<class W>
std::ostream& Graph_EW<ugraph, W>::print_edges(std::ostream& o, bool eofl) 
{
	// TODO: Insertar una instrucción "return" aquí
	const int NV = g_.number_of_vertices();	
	for (auto i = 0; i < NV - 1; ++i) {
		for (auto j = i + 1; j < NV; ++j) {

			if (is_edge(i, j)) {
				o << "[" << i << "]" << "-" << edge_weight(i, j)  << "->" << "[" << j << "]" << endl;
			}
		}
	}

	if (eofl) { o << std::endl; }
	return o;
}

template <class W>
void Graph_EW< ugraph, W >::add_edge_weight(int v, int w, W val) {

	//////////////////
	assert(v != w);
	//////////////////

	if (ptype::g_.is_edge(v, w)) {
		ptype::we_[v][w] = val;
		ptype::we_[w][v] = val;
	}
	else {
		LOGG_WARNING("edge-weight cannot be added to a non-edge", "(", v, ",", w, ")", "- Graph_EW<Graph_t,W >::add_edge_weight");
	}

	/*if (v == w) {
		ptype::we_[v][v] = val;
	}
	else if (ptype::g_.is_edge(v, w)) {
		ptype::we_[v][w] = val;
		ptype::we_[w][v] = val;
	}
	else {
		LOGG_DEBUG	("bizarre petition to add weight to the non-edge", "(", v, ",", w, ")", " - Graph_EW<Graph_t,W >::add_edge_weight");
		LOG_DEBUG	("weight not added");
		return -1;
	}*/

	//return 0;
}

template <class W>
void Graph_EW< ugraph, W >::add_edge_weight(W val) {

	auto NV = ptype::number_of_vertices();

	//set to empty wv and non-edges UPPER-T
	for (auto v = 0; v < NV - 1; v++) {
		for (auto w = v + 1; w < NV; w++) {
			if (ptype::g_.is_edge(v, w)) {
				ptype::we_[v][w] = val;
				ptype::we_[w][v] = val;
			}
			else {
				ptype::we_[v][w] = ptype::NOWT;
				ptype::we_[w][v] = ptype::NOWT;
			}
		}
	}

	//vertex weights
	/*for (int v = 0; v < NV; v++) {
		ptype::we_[v][v] = val;
	}*/
}

template <class W>
template<bool Erase>
void Graph_EW< ugraph, W >::add_edge_weight(typename Graph_EW<ugraph, W>::mat_t& lw) {

	auto NV = ptype::number_of_vertices();

	/////////////////////////
	assert(lw.size() == NV);
	/////////////////////////

	////assert
	//if (lw.size() != NV) {
	//	LOG_ERROR("bizarre matrix of weights - Graph_EW< ugraph, W >::add_edge_weight");
	//	LOG_ERROR("weights remain unchanged");
	//	return -1;
	//}

	//sets edge-weights
	for (auto v = 0; v < NV - 1; ++v) {
		for (auto w = v + 1; w < NV; ++w) {
			if (ptype::g_.is_edge(v, w)) {
				ptype::we_[v][w] = lw[v][w];
				ptype::we_[w][v] = lw[w][v];
			}
			else {
				if (Erase) {
					ptype::we_[v][w] = ptype::NOWT;
					ptype::we_[w][v] = ptype::NOWT;
				}				
			}
		}
	}

	//vertex weights
	/*for (int v = 0; v < NV; v++) {
		ptype::we_[v][v] = lw[v][v];
	}*/

}

template<class W>
void Graph_EW<ugraph, W>::gen_modulus_weights(int MODULUS)
{
	auto NV = number_of_vertices();

	//vertex-weights NOWT
	add_vertex_weight(this->NOWT);

	//sets weights of undirected edges
	for (std::size_t v = 0; v < NV - 1; ++v) {
		for (std::size_t w = v + 1; w < NV; ++w) {
			if (ptype::g_.is_edge(v, w)) {

				///////////////////////////////////////
				add_edge_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
				///////////////////////////////////////

			}			
		}
	}
}

template <class W>
ostream& Graph_EW<ugraph, W>::print_weights (ostream& o, bool line_format, bool only_vertex_weights) const{

	auto NV = ptype::number_of_vertices();
	
	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		if (line_format) {
			for (int v = 0; v < NV; v++) {
				if (ptype::we_[v][v] != ptype::NOWT) {
					o << "[" << v << " (" << ptype::we_[v][v] << ")] " << endl;
				}
			}

			for (int i = 0; i < NV - 1; i++) {
				for (int j = i + 1; j < NV; j++) {
					if (ptype::we_[i][j] != ptype::NOWT) {
						o << "[" << i << "-" << j << " (" << ptype::we_[i][j] << ")] " << endl;
					}
				}
			}
		}
		else {																			//outputs to stream edge-weights in matrix form
			for (int i = 0; i < NV; i++) {
				for (int j = i; j < NV; j++) {
					if (ptype::we_[i][j] != ptype::NOWT) {
						o << "[" << i << "-" << j << " (" << ptype::we_[i][j] << ")] ";
					}
					else {
						o << "--" << " ";
					}
				}
				o << endl;
			}
		}
	}else{
		//streams vertex weights (weights in self-loops)
		for (int v = 0; v < NV; v++) {
			if (ptype::we_[v][v] != ptype::NOWT) {
				o << "[" << v << " (" << ptype::we_[v][v] << ")] " << endl;
			}
		}
	}

	o << "**********************************************************" << endl;

	return o;
}

template <class W>
ostream& Graph_EW<ugraph, W>::print_weights (vint& ln, ostream& o, bool only_vertex_weights) const{
	   
	o << "\n**********non-empty edge-weights*************************" << endl;

	if (only_vertex_weights == false) {

		//streams edge-weights 
		for (int i = 0; i < ln.size(); i++) {
			if (ptype::we_[ln[i]][ln[i]] != ptype::NOWT) {
				o << "[" << ln[i] << " (" << ptype::we_[ln[i]][ln[i]] << ")] " << endl;
			}
		}

		for (int i = 0; i < ln.size() - 1; i++) {
			for (int j = i + 1; j < ln.size(); j++) {
				if (ptype::we_[ln[i]][ln[j]] != ptype::NOWT) {
					o << "[" << ln[i] << "-" << ln[j] << " (" << ptype::we_[ln[i]][ln[j]] << ")] " << endl;
				}
			}
		}
	}
	else {
		//streams vertex weights (weights in self-loops)
		for (int i = 0; i < ln.size(); i++) {
			if (ptype::we_[ln[i]][ln[i]] != ptype::NOWT) {
				o << "[" << ln[i] << " (" << ptype::we_[ln[i]][ln[i]] << ")] " << endl;
			}
		}
	}
	o << "**********************************************************" << endl;

	return o;
}

template<class W>
ostream& Graph_EW<ugraph, W>::write_dimacs (ostream& o) {
	
	auto NV = ptype::number_of_vertices();

	//timestamp 
	ptype::g_.timestamp_dimacs(o);
	
	//name
	ptype::g_.name_dimacs(o);

	//dimacs header - recompute edges
	ptype::g_.header_dimacs(o, false);
		
	//write vertex weights
	for (std::size_t v = 0; v < NV; ++v) {
		if (ptype::we_[v][v] != ptype::NOWT) {
			o << "n " << v + 1 << " " << ptype::we_[v][v] << endl;
		}
	}

	//write undirected edges and edge weights
	for(std::size_t v = 0; v < NV - 1; ++v){
		for(std::size_t w = v + 1; w < NV; ++w){
			if (ptype::g_.is_edge(v, w)) {														//O(log) for sparse graphs: specialize
				o << "e " << v + 1 << " " << w + 1 << " " << ptype::we_[v][w] << endl;			//1-based vertex notation dimacs
			}
		}
	}

	return o;
}


////////////////////////////////////////////
//list of valid types to allow generic code in *.cpp files 

template class  Base_Graph_EW<ugraph, int>;
template class  Base_Graph_EW<ugraph, double>;
template class  Graph_EW<ugraph, int>;
template class  Graph_EW<ugraph, double>;

//other specializations... (sparse_graph)


////////////////////////////////////////////
