/**
  * @file simple_graph_ew.h
  * @brief classes Base_Graph_EW and Graph_EW for edge-weighted graphs, 
  *			where self_loops are considered as vertex weights. Thus IT IS
  *			NOT for both vertex and edge-weighted GRAPHS
  *
  * @created 16/01/19
  * @milestoneA 10/11/2021 - train application
  * @last_update 06/01/25
  * @author pss
  *
  * @comments  edge weights in self-loops are possible, and are considered as vertex weights
  *
  * This code is part of the GRAPH 1.0 C++ library
  *
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
	
	//constants - globals
	static const W NOWT;								//default/no weight value for weights (0.0)	
																				
	//constructors
	Base_Graph_EW				()										{};											//no memory allocation
explicit Base_Graph_EW			(mat_t& lwe);																		//creates empty graph with edge weights
	Base_Graph_EW				(_gt& g, mat_t& lwe) : g_(g), we_(lwe)	{}											//creates graph with edge weights
explicit Base_Graph_EW			(int n, W val = NOWT)					{ reset(n, val); }							//creates empty graph with |V|= n and val weights	
explicit Base_Graph_EW			(std::string filename);																//read weighted ASCII file or generate weights using formula- CHECK! (21/11/2021))
			
	//copy constructor, move constructor, copy operator =, move operator =
	Base_Graph_EW					(const Base_Graph_EW& g)	= default;
	Base_Graph_EW					(Base_Graph_EW&& g)			= default;
	Base_Graph_EW& operator =		(const Base_Graph_EW& g)	= default;
	Base_Graph_EW& operator =		(Base_Graph_EW&& g)			= default;

	//destructor
virtual	~Base_Graph_EW()										= default;

/////////////
// setters and getters	

	/*
	*  @brief sets self-loop edge weight (considered as vertex-weight)
	*/
	void add_vertex_weight			(int v, W val)						{ we_[v][v] = val; }					
	
	/*
	*  @brief sets all self-loop edge weights to the same weight value
	*/
	void add_vertex_weight			( W val = NOWT);

	/*
	*  @brief sets edge weight given a directed edge (v, w)
	*  @param v input vertex
	*  @param w input vertex
	*  @param we input weight value
	
	*  @details: asserts it REALLY is an edge.
	*/
	virtual	void add_edge_weight	(int v, int w,  W val);
	
	/*
	*  @brief sets edge weights in @lw  if there is a corresponding edge.
	*		  Vertex weights are not modified. 
	*  @param lw matrix of edge weights
	*  @param template Erase: if TRUE weights of non-edges are set to NOWT
	*						  (aditional cleaning)
	*  @details: causses an assertion error if @lw.size() != |V|
	*/
	template<bool Erase = false>
	void add_edge_weight		(mat_t& lw);
	
	/*
	*  @brief sets all edge-weights to val
	*  @details: vertex weights are not modified
	*/
	virtual	void add_edge_weight	(W val = NOWT);																	//sets all weights to val 

	
	W edge_weight				(int v, int w)			const		{ return we_[v][w]; }
	W vertex_weight				(int v)					const		{ return we_[v][v]; }									
	
	/*
	*  @brief getter for vertex weights
	*/
	vecw<W> vertex_weights		()						const;
	
	mat_t& edge_weights			()								{ return we_; }	
	const mat_t& edge_weights	()						const	{ return we_; }
	Graph_t& graph				()								{ return g_; }
	const Graph_t& graph		()						const	{ return g_; }

	int number_of_vertices		()					const		{ return g_.number_of_vertices(); }
	int number_of_edges			(bool lazy = true)				{ return g_.number_of_edges(lazy); }


	const _bbt& neighbors		(int v)			const		{ return g_.neighbors(v); }
	_bbt& neighbors				(int v)						{ return g_.neighbors(v); }

	void name					(std::string str)			{ g_.name(str); }
  string name					()				const		{ return g_.name(); }
    void path					(std::string path_name)		{ g_.path(path_name); }
  string path					()				const		{ return g_.path(); }
	

//////////////////////////
// memory allocation
 
	/*
	* @brief resets to empty graph with |V|= n and assigns weight val to all vertices and edges
	* @param n number of vertices
	* @param reset_name if true, @name_ and @path_ reset to empty
	* @returns 0 if success, -1 if memory allocation fails
	* 
	* @comment preserved for backward compatibility (use reset(...))
	*/
	//int init			(int n, W val = NOWT, bool reset_name = true);
	
	void reset			()										{ g_.reset(); we_.clear(); }
		
	/*
	* @brief resets to empty graph |V|= n and assigns weight val to all vertices and edges
	* @param n number of vertices
	* @param name name of the instance
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int reset			(std::size_t n, W val = NOWT, string name = "");

/////////////////////////
//basic graph operations

	/*
	*  @brief adds edge, no self-loops allowed
	*/
	void add_edge		(int v, int w)							{ g_.add_edge(v, w); }

	double density		(bool lazy = true)						{ return g_.density(lazy); }

	void gen_random_edges(double p)								{ g_.gen_random_edges(p); }


	

/////////////////////////
// boolean properties

	bool is_edge		(int v, int w)			const			{ return g_.is_edge(v, w); }
	
	/*
	* @brief consistency check 
	* 
	*		 I. edges with default weights 
	*		 TODO...
	* @returns true if consistent, false otherwise
	*/
	bool is_consistent	();																				
	
////////////////////////
//weight operations
	
	/*
	* @brief changes the sign of the weights 
	*		we(i, j) = -we(i, j)
	*		 
	* @returns true if consistent, false otherwise
	*/
	void neg_w();	


	///////////////////////////
	//weight generation

	/*
	* @brief generates weights based on modulus operation [Pullan 2008, MODULUS = 200]
	*
	*			I. we(v, w) = 1 + ((v + w) % MODULUS)	(1-based index)
	* 
	*			II.we(v, v) are set to NOWT
	* 
	*			III. non-edges are not overritten
	*/
	virtual void gen_modulus_weights (int MODULUS = DEFAULT_WEIGHT_MODULUS);

////////////
// I/O 

	virtual std::ostream& print_data (bool lazy = true, std::ostream& o = std::cout, bool endl = true);

	/*
	* @brief Reads weighted directed graph from file in DIMACS format
	* 
	*			I. vertex weights are read in lines n <v> <w>, if missing, weights are set to NOWT
	* 
	*			II. if the file has edges with no edge weights, weights are set to NOWT
	* 
	* @param filename name of the file	 
	* @returns 0 if success, -1 if error
	*/
	int read_dimacs						(std::string filename);

	/*
	* @brief Writes directed graph to stream in dimacs format
	*
	*		 I. weights in self-loops are considered vertex weights
	* 
	*		II. vertex weights with value NOWT are not written to file
	* 
	*		III.For every edge, a weight is written (including NOWT)
	* 
	* @param o output stream
	* @returns output stream
	*/
	virtual	std::ostream& write_dimacs	(std::ostream& o);
		
	/*
	* @brief streams non-empty (weight value not NOWT) weights
	*		 of all directed edges. 
	*/
	virtual	std::ostream& print_weights	(std::ostream& o = std::cout, bool line_format = true,
											bool only_vertex_weights = false		)					const;
	
	/*
	* @brief streams non-empty (weight value not NOWT) weights 
	*		 of directed edges with endpoints in vertices in lv
	*/
	virtual	std::ostream& print_weights	(vint& lv, std::ostream& o = std::cout, 
											bool only_vertex_weights = false		)					const;

///////////////////
//data members

protected:
	Graph_t g_;
	mat_t   we_;			//extended for vertex and edge weights (20_/11/21))																	
};

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

/////////////
//setters and getters

	/*
	*  @brief sets edge weight given an undirected edge {v, w}
	*  @param v input vertex
	*  @param w input vertex
	*  @param we input weight value
	*
	*	(weights in self-loops are always added - considered vertex weights)
	*
	*  @details: asserts it is a real edge 
	*/
	void add_edge_weight	(int v, int w, W we)	override;

	/**
	*  @brief sets edge-weight val to all vertices (_we[v, v]) and edge weights
	* 
	*		I. NOWT is set as weight value to non-edges
	*		II. Weights in self-loops (vertices) are NOT modified
	* 
	* @param val weight value
	**/
	void add_edge_weight	(W val = 0.0)			override;
	
	/*
	*  @brief sets edge-weights in lw consistently to the graph
	*
	*		I. NOWT is set as weight value to non-edges in lw
	*		II. Weights in self-loops (vertices) are NOT modified
	* 
	*  @param lw matrix of edge weights
	*  @param template Erase: if TRUE weights of non-edges are set to NOWT
	*  @details: causses an assertion error if @lw.size() != |V|
	*/
	template<bool Erase = false>
	void add_edge_weight	(mat_t& lw);

/////////////
// weight operations

	/*
	* @brief generates weights based on modulus operation [Pullan 2008, MODULUS = 200]
	*
	*			I. we(v, w) = 1 + ((v + w) % MODULUS)	(1-based index)
	* 
	*			II.we(v, v) and non-edge weights are set to NOWT
	*		
	*			III. non-edges are not overritten
	*/
	void gen_modulus_weights(int MODULUS = DEFAULT_WEIGHT_MODULUS)  override;

/////////////
//useful framework-specific interface for undirected weighted graphs
	
	int max_graph_degree	()														{ return ptype::g_.max_graph_degree(); }
	int degree				(int v)										const		{ return ptype::g_.degree(v); }
	int degree				(int v, const typename ptype::_bbt & bbn)	const		{ return ptype::g_.degree(v, bbn); }

	/*
	* @brief Complement graph (currently name info of original graph is lost)
	* @param g output graph
	* @returns 0 if success, -1 if error
	*/
	int create_complement	(Graph_EW<ugraph, W>& g)					const;				
	
	/*
	* @brief Complement undirected graph (weights are lost)
	* @param g output complement undirected unweighted graph
	* @returns 0 if success, -1 if error
	*/
	int create_complement	(ugraph& g)									const		{ return ptype::g_.create_complement(g); }

/////////////
// I/O operations

	/*
	* @brief streams non-empty (weight value not NOWT) weights
	*		 of all undirected edges.
	*/
	virtual	std::ostream& print_weights	(std::ostream& o = std::cout, bool line_format = true,
											bool only_vertex_weights = false					)	const override;
	
	/*
	* @brief streams non-empty (weight value not NOWT) weights
	*		 of undirected edges with endpoints in vertices in lv
	*/
	virtual	std::ostream& print_weights	(vint& lv, std::ostream& o = std::cout,
												bool only_vertex_weights = false				)	const override;

	/*
	* @brief Writes undirected graph to stream in dimacs format
	*
	*		 I. weights in self-loops are considered vertex weights
	*
	*		II. vertex weights with value NOWT are not written to file
	*
	*		III.for every edge, a weight is written (including NOWT)
	*
	* @param o output stream
	* @returns output stream
	*/
	virtual	std::ostream& write_dimacs	(std::ostream& o);
};

template <class Graph_t, class W>
template <bool Erase>
void Base_Graph_EW< Graph_t, W>::add_edge_weight (mat_t& lw) {

	auto NV = number_of_vertices();

	/////////////////////////
	assert(lw.size() == NV);
	/////////////////////////

	/*if (lw.size() != NV) {
		LOG_ERROR("bizarre matrix of weights-Base_Graph_EW<Graph_t,W >::add_edge_weight(mat_t...)");
		return -1;
	}*/

	//set to empty wv and non-edges
	for (auto v = 0; v < NV; ++v) {
		for (auto w = 0; w < NV; ++w) {
			if (g_.is_edge(v, w)) {
				we_[v][w] = lw[v][w];
			}
			else {
				//cleans non-edge weights if required
				if (Erase) { we_[v][w] = NOWT; }
			}
		}
	}

}

#endif