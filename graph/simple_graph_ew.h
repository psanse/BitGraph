/**
  * @file simple_graph_ew.h
  * @brief classes Base_Graph_EW and Graph_EW for edge-weighted graphs, 
  *			where self_loops are considered as vertex weights. Thus IT IS
  *			NOT for the most general case of vertex and edge-weighted GRAPHS,
  *		    but could suffice also for vertices in both vertices and egdes
  *			in many applications.
  *
  * @details: created 16/01/19, last_update 08/03/25
  * @details: milestone train application (10/11/2021)
  * @author pss
  *
  * This code is part of the GRAPH 1.0 C++ library
  **/

#ifndef __SIMPLE_GRAPH_EDGE_WEIGHTED_H__
#define __SIMPLE_GRAPH_EDGE_WEIGHTED_H__

#include "utils/common.h"
#include "simple_ugraph.h"
#include <iostream>
#include <vector>

/////////////////////////////////////////////
extern const int DEFAULT_WEIGHT_MODULUS;								//for modulus weight generation  [Pullman 2008]					
///////////////////////////////////////////

//aliases
template<class W>
using vecw = std::vector<W>;	

using  ugraph = Ugraph<bitarray> ;

///////////////////////
//
// class Base_Graph_EW 
// (used to be able to specialize functions according to the type of graph only)
//
// comment: user class is Graph_EW (see below) 
//
///////////////////////

template<class Graph_t, class W>
class Base_Graph_EW {							
public:
	
	using type = Base_Graph_EW<Graph_t, W>;				//own type
	using graph_type = Graph_t;							//graph type	
	using basic_type = typename Graph_t::basic_type;	//bitset type used by graph type 
	using mat_t = vector<vector<W>>;					//matrix type for weights


	//alias types for backward compatibility
	using _gt = graph_type;
	using _bbt = basic_type;
	using _wt = W;																					
	
	//enum to distinguish between vertex and edge weights
	enum { VERTEX, EDGE, BOTH };	

////////////////////////////
//constants / globals
	static const W NO_WEIGHT;								//default/no weight value for weights (0.0)	
	static const W ZERO_WEIGHT;

//////////////////////////
//constructors / destructor

	Base_Graph_EW					()										{};										//no memory allocation
	Base_Graph_EW					(_gt& g, mat_t& lwe) : g_(g), we_(lwe)	{}										//creates graph from a non-weighted graph and a set of weights

	/**
	* @brief creates a graph with |V|= n and val weights as base line.
	* @param n number of vertices
	* @param val weight value
	* @param edge_weighted if TRUE creates an edge-weighted graph 
	*			(vertex weights are set to NO_WEIGHT, instead of val)
	* @details: base line for all weights is set to val, i.e. vertex weights are 
	*			set to val, but edge-weights will be overwritten when edges are added
	**/
	Base_Graph_EW					(int n,  W val = ZERO_WEIGHT, bool edge_weighted = false);						//creates empty graph with |V|= n and val weights	
	
	/**
	* @brief reads graph from file. If ewights are not found it generated them 
	*		 automatically based on the Pullman 2008 formula
	* @details currently only reads dimacs format (with or without weights)
	**/
	explicit Base_Graph_EW			(std::string filename);															//read weighted ASCII file or generate weights using formula- CHECK! (21/11/2021))
			
	//copy constructor, move constructor, copy operator =, move operator =
	Base_Graph_EW					(const Base_Graph_EW& g)			= default;
	Base_Graph_EW					(Base_Graph_EW&& g)		noexcept	= default;
	Base_Graph_EW& operator =		(const Base_Graph_EW& g)			= default;
	Base_Graph_EW& operator =		(Base_Graph_EW&& g)		noexcept	= default;

	//destructor
virtual	~Base_Graph_EW()												= default;

/////////////
// setters and getters	
	
	/**
	* @brief getter for edge-weights
	* @param v, w: edge
	* @details: no check for vertex existence
	**/
	W weight					(int v, int w)			const	{ return we_[v][w]; }
	
	/**
	* @brief getter for vertex-weights
	* @param v: vertex	
	* @details: no check for vertex existence
	**/
	W weight					(int v)					const	{ return we_[v][v]; }									
	
	/*
	*  @brief getter for the specific subset of vertex-weights
	*/
 vecw<W> vertex_weights			()						const;
	
	/**
	*  @brief getter for the all the weights (vertex and edge-weights)
	**/
	mat_t& weights				()								{ return we_; }	
	const mat_t& weights		()						const	{ return we_; }
	
	/**
	*  @brief getter for the graph (no weight information just edges)
	**/
	Graph_t& graph				()								{ return g_; }
	const Graph_t& graph		()						const	{ return g_; }

	int number_of_vertices		()						const	{ return g_.number_of_vertices(); }
	
	/**
	* @brief alias for the number_of_vertices function
	**/
	int size					()						const	{ return g_.number_of_vertices(); }

	int number_of_edges			(bool lazy = true)				{ return g_.number_of_edges(lazy); }

	/**
	* @brief neighbor set of vertex @v
	* @param v input vertex
	**/
	const _bbt& neighbors		(int v)			const			{ return g_.neighbors(v); }
	_bbt& neighbors				(int v)							{ return g_.neighbors(v); }

	void name					(std::string str)				{ g_.name(str); }
  string name					()				const			{ return g_.name(); }
    void path					(std::string path_name)			{ g_.path(path_name); }
  string path					()				const			{ return g_.path(); }
	
  double density				(bool lazy = true)				{ return g_.density(lazy); }

//////////////////////////
// memory allocation

protected:
	/**
	* @brief clearing without deallocation - internal use only	
	**/
	void reset					()								{ g_.reset(); we_.clear(); }

public:
	/**
	* @brief resets to a graph with |V|= n with weight val in everey vertex and edge
	* @param n number of vertices
	* @param name name of the instance
	* @returns 0 if success, -1 if memory allocation fails
	**/
	template<bool EdgeWeightedGraph = false>
	int reset					(std::size_t n, W val = ZERO_WEIGHT, string name = "");					//before: int init (int n, W val = NO_WEIGHT, bool reset_name = true);
	

/////////////////////////
// basic operations
		
	/**
	* @brief Adds an edge (v, w) with weight val 
	*	      (if the edge exists, sets val)
	**/
	virtual	void add_edge				(int v, int w, W val = ZERO_WEIGHT);

	/**
	*  @brief sets vertex-weight
	*  @details: vertex-weights are stored as self-loop edge-weights
	**/
	void set_weight						(int v, W val)		{ we_[v][v] = val; }

	/**
	*  @brief sets edge-weight to an EXISTNG given directed edge (v, w)
	*		  (in a non-edge it can only set weight to NO_WEIGHT)
	*  @param v input vertex
	*  @param w input vertex
	*  @param val input weight value

	*  @details: asserts it REALLY is an edge (and not a self-loop)
	*  @details: in a non-edge it can only set weight to NO_WEIGHT
	**/
	virtual	void set_weight(int v, int w, W val);

	/**
	*  @brief sets edge weights in @lw  if there is a corresponding edge.
	*		  Vertex weights are not modified.
	*  @param lw matrix of edge weights
	*  @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
	*						  (aditional cleaning)
	*  @details: asserts  @lw.size() == |V|
	**/
	template<bool EraseNonEdges = false>
	void set_weight(mat_t& lw, bool edges_only = true);

	/**
	*  @brief sets all vertex-weights (self-loop edge weights) to the same weight @val
	**/
	void set_vertex_weight				(W val = ZERO_WEIGHT);
		
	/**
	*  @brief sets all edge-weights to val IF there is a corresponding edge.
	*		  Vertex weights are not modified.
	*  @param lw matrix of edge weights
	*  @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
	*						  (aditional cleaning)
	**/
	template<bool EraseNonEdges = false>
	void set_edge_weight				(W val = ZERO_WEIGHT);

	/**
	* @brief sets new edge-weights based on modulus operation [Pullan 2008, MODULUS = 200]
	*
	*			I. we(v, w) = 1 + ((v + w) % MODULUS)	(1-based index)
	*
	*			II. vertex-weigths are not changed
	* @param template EraseNonEdges:  if TRUE, non-edges are set to NO_WEIGHT
	* @param MODULUS: modulus value [Pullan 2008, DEFAULT_WEIGHT_MODULUS = 200]
	**/
	template <bool EraseNonEdges = false>
	void set_modulus_edge_weight		(int MODULUS = DEFAULT_WEIGHT_MODULUS);


////////////////
// Conversions 

	/**
	* @brief sets all vertex weights to NO_WEIGHT
	* @param erase_non_edges if TRUE, sets non-edges to NO_WEIGHT
	**/
	void make_edge_weighted				(bool erase_non_edges = false);

	/**
	* @brief sets all non-edges to weight NO_WEIGHT.
	**/
	void erase_non_edge_weights			();

/////////////////////////
// boolean properties

	bool is_edge						(int v, int w)			const			{ return g_.is_edge(v, w); }
	
	/*
	* @brief consistency check 
	* 
	*		 I. edges with NO_WEGHT weight values 
	*		
	* @returns true if consistent, false otherwise
	* 
	*  TODO... other checks
	*/
	bool is_consistent					();																				
	
////////////////////////
//weight operations
	
	/**
	* @brief transforms weights (excluding NO_WEIGHT values) using functor f
	* @param f functor	
	* @param type: EDGE (edge-weights), VERTEX (vertex-weights) or BOTH
	**/
	template<class Func>
	void transform_weights				(Func f, int type = BOTH);

	/**
	* @brief specific transformation of weights (excluding NO_WEIGHT values)
	*		 from positive to negative, i.e.,  we(i, j) = - we(i, j)
	* @param type: EDGE, VERTEX or BOTH
	**/
	void complement_weights				(int type = BOTH);	
		
////////////////////////
// other operations
	
	/**
	* @brief created complement graph in @g
	* @param g output graph
	* @returns 0 if success, -1 if error
	**/
	int create_complement				(Base_Graph_EW<Graph_t, W>& g)					const;

	/**
	* @brief generates random edges uniformly with probability p and weight val
	**/
	virtual void gen_random_edges		(double , W val); 

////////////
// I/O 

virtual std::ostream& print_data		(bool lazy = true, std::ostream& o = std::cout, bool endl = true);

	/**
	* @brief prints the edge (v, w) in line format: [v]-(val)->[w], one edge per line
	**/
std::ostream& print_edge				(int v, int w, std::ostream& o = std::cout, bool endl = true) const;
	/**
	* @brief streams the vertex @v in format [v:(val)]	
	**/
std::ostream& print_vertex				(int v, std::ostream& o = std::cout, bool endl = true)		 const;
	/**
	* @brief streams non-empty (excluding NO_WEIGHT value) weight info
	*		 for all directed edges.
	**/
	std::ostream& print_weights			(std::ostream& o = std::cout, bool line_format = true,
											int type = BOTH				)							const;
	/**
	* @brief streams non-empty (excluding NO_WEIGHT value)  weights
	*		 of directed edges induced by the subgraph of vertices in lv
	**/
	std::ostream& print_weights					(vint& lv, std::ostream& o = std::cout, int type = BOTH)	const;

	///////////
	// read/write operations

	/**
	* @brief Reads weighted directed graph from file in DIMACS format
	* 
	*			I. vertex weights are read in lines n <v> <w>, if missing, weights are set to NO_WEIGHT
	* 
	*			II. if the file has edges with no edge weights, weights are set to NO_WEIGHT
	* 
	* @param filename name of the file	 
	* @returns 0 if success, -1 if error
	**/
	int read_dimacs								(std::string filename);

	/**
	* @brief Writes directed graph to stream in dimacs format
	*
	*		 I. weights in self-loops are considered vertex weights
	* 
	*		II. vertex weights with value NO_WEIGHT are not written to file
	* 
	*		III.For every edge, a weight is written (including NO_WEIGHT)
	* 
	* @param o output stream
	* @returns output stream
	**/
	virtual	std::ostream& write_dimacs			(std::ostream& o);

	/////////////////////
	//private I/O

protected:	
	/**
	* @brief prints the edges of the graph in line format [v]-(val)->[w], one edge per line
	**/
	virtual std::ostream& print_edges			(std::ostream& o = std::cout, bool eofl = true)				const;
	
	/**
	* @brief prints edges in different formats
	* @param line_format: if TRUE, prints one edge per line, otherwise in matrix format
	**/
	virtual	std::ostream& print_edge_weights	(std::ostream& o = std::cout, bool line_format = true)		const;
			std::ostream& print_vertex_weights  (std::ostream& o = std::cout)								const;
	
	/**
	* @brief prints the edges of the graph induced by the vertices of lv in line format
	* @param lv: input set of vertices of the induced subgraph
	**/
	virtual	std::ostream& print_edge_weights	(vint& lv, std::ostream& o = std::cout )					const;
	
	/**
	* @brief prints the vertices of the vertices of lv 
	* @param lv: input set of vertices 
	**/
	std::ostream& print_vertex_weights			(vint& lv, std::ostream& o = std::cout)						const;

///////////////////
//data members

protected:
	Graph_t g_;
	mat_t   we_;								//matrix of vertex and edge-weights 																

}; //end of class Base_Graph_EW

////////////////////////
//
// Graph_WE class 
// 
// I.  user generic class to specialize for different types of graphs
// II. currently only defined for ugraph type
//
///////////////////////

template <class Graph_t, class W>
class Graph_EW : public Base_Graph_EW<Graph_t, W> {};

////////////////////////////////////
// 
// Specialization for undirected graphs
//
////////////////////////////////////

template <class W>
class Graph_EW<ugraph, W> : public Base_Graph_EW<ugraph, W> {
public:

	using type = Graph_EW<ugraph, W>;					//own type
	using ptype = Base_Graph_EW<ugraph, W>;				//parent type
	using graph_type = ugraph;							//graph type
	using basic_type = typename graph_type::basic_type;	//bitset type used by graph type 
	using mat_t = typename ptype::mat_t;				//matrix type for weights

	//alias types for backward compatibility
	using _wt = W;										//weight number type for backward compatibility
	using _gt = graph_type;
	
	//constructors (inherited)
	using Base_Graph_EW<ugraph, W>::Base_Graph_EW;


/////////////////////////
// basic operations
	/**
	* @ brief adds an edge (v, w) with weight val
	**/
	void add_edge					(int v, int w, W val = ptype::ZERO_WEIGHT)	override;
		
	/**
	*  @brief sets edge-weight val to the undirected edge {v, w} if the edge exists
	*		  (in a non-edge it can only set the weight to NO_WEIGHT)
	*  @param v input vertex
	*  @param w input vertex
	*  @param we input weight value
	*
	*	(weights in self-loops are always added - considered vertex weights)
	*
	*  @details: asserts v!=w
	*  @details: in a non-edge it can only set weight to NO_WEIGHT
	**/
	void set_weight					(int v, int w, W val)					override;

	/**
	*  @brief sets edge-weights in lw consistently to the graph
	*
	*		I. NO_WEIGHT is set as weight value to non-edges in lw
	*		II. Weights in self-loops (vertices) are NOT modified
	*
	*  @param lw matrix of edge weights
	*  @param template EraseNonEdges: if TRUE weights of non-edges are set to NO_WEIGHT
	*  @details: asserts lw.size() == |V|
	**/
	template<bool EraseNonEdges = false>
	void set_weight					(mat_t& lw, bool edges_only = true);

	/**
	*  @brief sets edge-weight val to existing edges
	* 			
	*		I. Weights in self-loops (vertices) are NOT modified
	* 
	* @param template EraseNonEdges: if TRUE weights of non-edges are set to NO_WEIGHT
	* @param val weight value
	**/
	template <bool EraseNonEdges = false>
	void set_edge_weight			(W val = ptype::ZERO_WEIGHT);	

	/**
	* @brief sets new edge-weights based on modulus operation [Pullan 2008, MODULUS = 200]
	*
	*			I.  we(v, w) = 1 + ((v + w) % MODULUS)	(1-based index)
	*			II. Non-edges and vertex-weights are not modified 
	* @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
	* @param MODULUS: modulus value [Pullan 2008, DEFAULT_WEIGHT_MODULUS = 200]
	**/
	template<bool EraseNonEdges = false>
	void set_modulus_edge_weight	(int MODULUS = DEFAULT_WEIGHT_MODULUS);
			

/////////////
//useful interface for graph operations (no weights)
	
	int max_graph_degree	()														{ return ptype::g_.max_graph_degree(); }
	int degree				(int v)										const		{ return ptype::g_.degree(v); }
	int degree				(int v, const typename ptype::_bbt & bbn)	const		{ return ptype::g_.degree(v, bbn); }
	
/////////////
//other operations
	
	/**
	* @brief generates random edges uniformly with probability p and weight val
	**/
	 void gen_random_edges				(double, W val = ptype::ZERO_WEIGHT)						override;

/////////////
// I/O operations
protected:
	/**
	* @brief prints the edges of the graph in line format [v]-(val)->[w], one edge per line
	**/
	std::ostream& print_edges			(std::ostream& o = std::cout, bool eofl = false)					const override;

	std::ostream& print_edge_weights	(std::ostream& o = std::cout, bool line_format = true)				const override;
	
	std::ostream& print_edge_weights	(vint& lv, std::ostream& o = std::cout)								const override;

public:	
	/**
	* @brief Writes undirected graph to stream in dimacs format
	*
	*		 I. weights in self-loops are considered vertex weights
	*
	*		II. vertex weights with value NO_WEIGHT are not written to file
	*
	*		III.for every edge, a weight is written (including NO_WEIGHT)
	*
	* @param o output stream
	* @returns output stream
	**/
	virtual	std::ostream& write_dimacs	(std::ostream& o);

}; //end of Graph_EW class


//////////////////////////////////////////////////////////////////
//  
// Necessary implementations in header file

template <class Graph_t, class W>
template <bool EraseNonEdges>
inline
void Base_Graph_EW< Graph_t, W>::set_weight (mat_t& lw, bool edges_only) {

	auto NV = number_of_vertices();

	/////////////////////////
	assert(lw.size() == NV);
	/////////////////////////

	/*if (lw.size() != NV) {
		LOG_ERROR("bizarre matrix of weights-Base_Graph_EW<Graph_t,W >::set_edge_weight(mat_t...)");
		return -1;
	}*/

	//set to empty wv and non-edges
	for (auto v = 0; v < NV; ++v) {
		for (auto w = 0; w < NV; ++w) {
			
			if (edges_only && v == w) continue;			//skips vertex weights if required

			if (g_.is_edge(v, w)) {
				we_[v][w] = lw[v][w];
			}
			else {
				//cleans non-edge weights if required
				if (EraseNonEdges) { we_[v][w] = NO_WEIGHT; }
			}
		}
	}
}

template<class Graph_t, class W>
template<class Func>
inline
void Base_Graph_EW<Graph_t, W>::transform_weights(Func f, int type)
{
	auto NV = number_of_vertices();
		
	switch (type) {
		//edge-weights
	case EDGE:
		for (auto i = 0; i < NV - 1; ++i) {
			for (auto j = i + 1; j < NV; ++j) {
				if (we_[i][j] != NO_WEIGHT) {
					we_[i][j] = f(we_[i][j]);
				}
				if (we_[j][i] != NO_WEIGHT) {
					we_[j][i] = f(we_[j][i]);
				}
			}
		}
		break;
		//vertex-weights
	case VERTEX:
		for (auto i = 0; i < NV; ++i) {
			if (we_[i][i] != NO_WEIGHT) {
				we_[i][i] = f(we_[i][i]);	
			}	
		}
		break;
	case BOTH:
		//vertex and edge-weights
		for (auto i = 0; i < NV ; ++i) {
			for (auto j = 0; j < NV; ++j) {
				if (we_[i][j] != NO_WEIGHT) {
					we_[i][j] = f(we_[i][j]);
				}			
			}
		}
		break;
	default:	
		//should not happen	
		LOG_ERROR("unknown type -  Base_Graph_EW<Graph_t, W>::transform_weights");
		LOG_ERROR("exiting");
		std::exit(-1);
	}
}

template <class Graph_t, class W>
template<bool EraseNonEdges>
inline
void Base_Graph_EW< Graph_t, W>::set_edge_weight(W val) {

	auto NV = number_of_vertices();

	//set to empty weight the non-edges
	for (int v = 0; v < NV; v++) {
		for (int w = 0; w < NV; w++) {
			if (g_.is_edge(v, w)) {
				we_[v][w] = val;
			}
			else {
				if (EraseNonEdges) { we_[v][w] = NO_WEIGHT; }
			}
		}
	}
}

template<class Graph_t, class W>
template <bool EraseNonEdges>
inline
void Base_Graph_EW<Graph_t, W>::set_modulus_edge_weight(int MODULUS) {

	auto NV = number_of_vertices();

	//vertex-weights NO_WEIGHT
	//set_vertex_weight(NO_WEIGHT);

	for (auto v = 0; v < NV; ++v) {
		for (auto w = 0; w < NV; ++w) {

			if (g_.is_edge(v, w)) {

				///////////////////////////////////////
				set_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
				///////////////////////////////////////
			}
			else {
				if (EraseNonEdges) {
					set_edge_weight(v, w, NO_WEIGHT);
				}
			}
		}
	}
}

template <class W>
template <bool EraseNonEdges>
inline
void Graph_EW< ugraph, W >::set_edge_weight(W val) {

	auto NV = ptype::number_of_vertices();

	//set to empty wv and non-edges UPPER-T
	for (auto v = 0; v < NV - 1; v++) {
		for (auto w = v + 1; w < NV; w++) {
			if (ptype::g_.is_edge(v, w)) {
				ptype::we_[v][w] = val;
				ptype::we_[w][v] = val;
			}
			else {
				if (EraseNonEdges) {
					ptype::we_[v][w] = ptype::NO_WEIGHT;
					ptype::we_[w][v] = ptype::NO_WEIGHT;
				}
			}
		}
	}

	//vertex weights
	/*for (int v = 0; v < NV; v++) {
		ptype::we_[v][v] = val;
	}*/
}

template <class W>
template<bool EraseNonEdges>
inline
void Graph_EW< ugraph, W >::set_weight(typename Graph_EW<ugraph, W>::mat_t& lw, bool edges_only) {

	auto NV = ptype::number_of_vertices();

	/////////////////////////
	assert(lw.size() == NV);
	/////////////////////////

	////assert
	//if (lw.size() != NV) {
	//	LOG_ERROR("bizarre matrix of weights - Graph_EW< ugraph, W >::set_edge_weight");
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
				if (EraseNonEdges) {
					ptype::we_[v][w] = ptype::NO_WEIGHT;
					ptype::we_[w][v] = ptype::NO_WEIGHT;
				}
			}
		}
	}

	//vertex weights
	if (!edges_only) {
		for (auto v = 0; v < NV; v++) {
			ptype::we_[v][v] = lw[v][v];
		}
	}	
}

template<class W>
template<bool EraseNonEdges>
inline
void Graph_EW<ugraph, W>::set_modulus_edge_weight(int MODULUS) {

	auto NV = ptype::g_.number_of_vertices();

	//vertex-weights NO_WEIGHT
	//set_vertex_weight(ptype::NO_WEIGHT);

	//sets weights of undirected edges
	for (auto v = 0; v < NV - 1; ++v) {
		for (auto w = v + 1; w < NV; ++w) {
			if (ptype::g_.is_edge(v, w)) {

				///////////////////////////////////////
				set_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
				///////////////////////////////////////

			}
			else {
				if (EraseNonEdges) { set_weight(v, w, ptype::NO_WEIGHT); }
			}
		}
	}
}

template<class Graph_t, class W>
template<bool EdgeWeightedGraph>
inline
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
		LOG_ERROR("bad weight assignment - Base_Graph_EW<Graph_t, W>::reset");
		return -1;
	}

	//set vertex weights to NO_WEIGHT if required
	if (EdgeWeightedGraph) {
		for (auto v = 0; v < NV; ++v) {
			we_[v][v] = NO_WEIGHT;
		}
	}

	g_.name(name);

	return 0;
}


#endif