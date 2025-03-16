 /**
   * @file simple_graph_w.h
   * @brief classes Base_Graph_W and Graph_W for simple weighted graphs 
   *
   * @created 16/01/19
   * @last_update 06/01/25
   * @author pss
   *
   * This code is part of the GRAPH 1.0 C++ library
   *
   **/

#ifndef __SIMPLE_GRAPH_WEIGHTED_H__
#define __SIMPLE_GRAPH_WEIGHTED_H__

#include "utils/common.h"
#include "simple_ugraph.h"
#include <iostream>
#include <vector>
#include <algorithm>


////////////////////////////////////////////////////////////////
extern const int DEFAULT_WEIGHT_MODULUS;								//for modulus weight generation  [Pullman 2008]					
////////////////////////////////////////////////////////////////
	
//alias
using vint = std::vector<int>;
using ugraph = Ugraph<bitarray>;


///////////////////////
//
// Base_Graph_W class 
// 
// (used to be able to specialize functions
// according to the type of graph only)
//
// User type class is Graph_W 
//
////////////////////////

template<class Graph_t, class W>
class Base_Graph_W {	
																					
public:
	enum { Wext = 0, Dext, WWWext, NOext };				//file extensions for weights (used in function read_dimacs)	

	using type = Base_Graph_W<Graph_t, W>;				//own type
	using graph_type = Graph_t;							//graph type	
	using basic_type = typename Graph_t::basic_type;	//bitset type used by graph type 
		
	//alias types for backward compatibility
	using _gt = graph_type;								
	using _bbt = basic_type;							
	using _wt =	 W;	

	//constants - globals
	static const W NO_WEIGHT;							
	static const W ZERO_WEIGHT;
	static const W DEFAULT_WEIGHT;								//default weight value for weights (1.0)	
	

	//constructors
	Base_Graph_W			() {};																			//No memory allocation
explicit Base_Graph_W		(std::vector<W>& lw);															//creates empty graph with |V|= n with vertex weights
	Base_Graph_W			(_gt& g, vector<W>& lw) :g_(g), w_(lw)  {  assert(w_.size() == g_.size()); }	//creates graph with vertex weights	
	Base_Graph_W			(_gt& g)				:g_(g), w_(g.size(), DEFAULT_WEIGHT) {}					//creates graph with DEFAULT_WEIGHTs
explicit Base_Graph_W		(int N, W val = DEFAULT_WEIGHT) { int ret_val = reset(N, val);
																assert(ret_val != -1);		}				//creates empty graph with |V|= N with weight value val	
	
	/*
	* @brief Reads weighted graph from ASCII file in DIMACS format
	*		
	*		 If unable to read weights generates modulus weights [Pullman 2008]
	* 
	*		 TODO: add support for other formats
	*/
explicit Base_Graph_W					(std::string filename);															

	//copy constructor, move constructor, copy operator =, move operator =
	Base_Graph_W						(const Base_Graph_W& g)			= default;												
	Base_Graph_W						(Base_Graph_W&& g)   noexcept	= default;												
	Base_Graph_W& operator =			(const Base_Graph_W& g)			= default;												
	Base_Graph_W& operator =			(Base_Graph_W&& g)	 noexcept	= default;												


	//destructor
	virtual	~Base_Graph_W()	 = default;		

/////////////
// setters and getters
	
	void set_weight						(int v, W val)				{ w_.at(v) = val;}				
	void set_weight						(W val = DEFAULT_WEIGHT)	{ w_.assign(g_.number_of_vertices(), val);}		
	
	/*
	* @brief sets vertex weights to all vertices
	* @param lw vector of weights of size |V|
	* @returns 0 if success, -1 if error
	*/
	int	 set_weight						(std::vector<W>& lw);

	/**
	* @brief generates weights based on modulus operation [Pullan 2008, MODE = 200]
	*
	*			w(v) = (v + 1) % MODE	(v 1-based index)
	*
	*			MODE = 1 -> w(v) = 1	(unweighted graph)
	**/
	int set_modulus_weight				(int MODE = DEFAULT_WEIGHT_MODULUS);

	
	Graph_t& graph						()							{ return g_;}
const Graph_t& graph					()			const			{ return g_; }

	W weight							(int v)		const			{ return w_[v];}	
const vector<W>& weight					()			const			{ return w_;}	
	vector<W>& weight					()							{ return w_;}	
	
	/*
	* @brief Determines weight and vertex of maximum weight
	* @param v ouptut vector of maximum weight
	* @returns weight of vertex v
	*/
	W maximum_weight					(int& v)	const;	
	
const _bbt& neighbors					(int v)		const			{ return g_.neighbors(v); }
 	_bbt& neighbors						(int v)						{ return g_.neighbors(v); }
	
	void name							(std::string str)			{ g_.name(str); }
std::string name						()			const			{ return g_.name(); }
	void path							(std::string path_name)		{ g_.path(path_name); }
std::string path						()			const			{ return g_.path(); }

	int number_of_vertices				()			const			{ return g_.number_of_vertices(); }
	int size							()			const			 { return g_.number_of_vertices(); }

	int number_of_edges					(bool lazy = true)			{ return g_.number_of_edges(lazy); }

//////////////////////////
// memory allocation 

	/**
	* @brief resets to empty graph given name and number of vertices
	* @param n number of vertices
	* @param name name of the instance
	* @returns 0 if success, -1 if memory allocation fails
	**/
	int reset							(std::size_t n, W val = DEFAULT_WEIGHT,  string name = "");

protected:
	/**
	* @brief resets to default values (does not deallocate memory)
	* @details: to deallocate do g = Base_Graph_W<xx>() 
	* @detials: should not be called directly in the general case
	**/
	void reset							()								{ g_.reset(); w_.clear(); }

/////////////////////////
//basic graph operations for simplicity
//(calls directly graph services)

public:
	/**
	* @brief adds edge (no self-loops allowed)
	**/
	void add_edge						(int v, int w)					{ g_.add_edge(v, w); }
	
	double density						(bool lazy = true)				{ return g_.density(lazy); }
	
	/**
	* @brief generates edges with probability p
	* @details: graph operation, no weights involved since
	*			edges are unweighted
	**/
	void gen_random_edges				(double p)						 { g_.gen_random_edges(p); }


/////////////
// boolean properties

	bool is_edge						(int v, int w)		const		{ return g_.is_edge(v, w); }
	
	/**
	* @brief checks if the graph is unit-weighted
	* @returns true if all weights are 1.0, otherwise false
	* @details: a unit-weighted graph is equivalent to an unweighted graph
	*		    from a theoretical perspective
	**/
	bool is_unit_weighted				();


///////////////////////////
//weight operations
	
	/**
	* @brief transforms (vertex) weights (excluding NO_WEIGHT values) using functor f
	* @param f functor
	**/
	template<class Func>
	void transform_weights				(Func f);

	/**
	* @brief specific transformation of weights (excluding NO_WEIGHT values)
	*		 from positive to negative, i.e.,  w(i) = - w(i)
	**/
	void complement_weights				();

	
////////////////////////
// other operations

	/*
	* @brief Complement graph (currently name info of original graph is lost)
	* @param g output graph
	* @returns 0 if success, -1 if error
	*/
	int create_complement				(Base_Graph_W& g)					const;

////////////
// I/O
public:

	/**
	* @brief Writes directed graph to stream in dimacs format
	*
	*		 (self-loops are not considered)
	**/
	virtual ostream& write_dimacs		(std::ostream& o = std::cout);												
	
	/**
	* @brief Reads weighted undirected graph from file in DIMACS format
	* @param filename name of the file
	* @param type extension of additional filename for separate weights (Wext, Dext, WWWext, NOext)
	*		 default NOext - no additional file
	* @returns 0 if success, -1 if error
	**/
	int read_dimacs						(string filename, int type = NOext);
	
	/**
	* @brief Reads weights from an external file (only weights) 
	* 
	*		 Format: numbers ordered and separated (line, space) 
	*				  i.e. {5 6 7 ...} -> w(1)=5, w(2)=6, w(3)=7,...
	*			
	*		 Weights not assigned in the file are set to 0.0
	* 
	* @param filename name of the file
	* @returns 0 if success, -1 if error (empty vector of weights)
	**/
	int read_weights					(string filename);												
		
	std::ostream& print_data			(bool lazy = true, std::ostream& o = std::cout, bool endl = true);	
	std::ostream& print_edges			(std::ostream& o = std::cout, bool eofl = false)							{ g_.print_edges(o, eofl); return o; }
	
	/**
	* @brief streams vertex-weights in the format [v:(val)]	
	* @param o output stream
	* @param show_vert: if FALSE does not show vertex info but the collection of weights
	*					in order 
	**/
	std::ostream& print_weights			(std::ostream& o = std::cout, bool show_vert = true)						const;
	
	/**
	* @brief streams vertex-weights in the subset of vertices  bbsg
	**/
	std::ostream& print_weights			(_bbt & bbsg, std::ostream& o = std::cout)									const;
	
	/**
	* @brief prints the weights of the vertices in the stack lv
	* @param lv: a set of vertices with a stack interface
	**/
	std::ostream& print_weights			(com::stack_t<int>& lv, ostream& o = std::cout)								const;
	
	/**
	* @brief prints the weights of the vertices in the stack lv
	*		 given a mapping of the vertices
	* @param mapping: input mapping of the vertices with at least the same size as
	*				  the stack lv
	* @param lv: a set of vertices with a stack interface
	**/
	std::ostream& print_weights			(com::stack_t<int>& lv, const vint& mapping, std::ostream& o = std::cout)	const;

	/**
	* @brief prints the weights of the vertices in lv
	* @supports C-arrays
	**/
	std::ostream& print_weights			(vint& lv, std::ostream& o = std::cout)										const;
	std::ostream& print_weights			(int* lv, int n, std::ostream& o = std::cout)								const;

/////////////////////////////////////
// data members

protected:
	Graph_t g_;							//graph
	vector<W> w_;						//vector of weights 

};

///////////////////////
//
// Graph_W class 
// (user generic class to specialize for different types of graphs) 
//
///////////////////////

template<class Graph_t, class W>
class Graph_W : public Base_Graph_W <Graph_t, W> {};

////////////////////////////////////
// 
// Specialization for undirected graphs
//
////////////////////////////////////

template<class W>
class Graph_W<ugraph, W>: public Base_Graph_W<ugraph, W> {
public:

	using type = Graph_W<ugraph, W>;					//own type
	using ptype = Base_Graph_W<ugraph, W>;				//parent type
	using graph_type = ugraph;							//graph type
	using basic_type = typename graph_type::basic_type;	//bitset type used by graph type 

	//alias types for backward compatibility
	using _wt = W;										//weight number type for backward compatibility
	using _gt = graph_type;								

	//constructors (inherited)
	using Base_Graph_W<ugraph, W>::Base_Graph_W;		

/////////////
//useful interface-specific for undirected weighted graphs
	
	int max_graph_degree	()													{ return ptype::g_.max_graph_degree(); }
	int degree				(int v)							const				{ return ptype::g_.degree(v); }
	int degree				(int v, const BitSet& bbn)		const				{ return ptype::g_.degree(v, bbn); }
			

///////////
//I/O operations
	
	/*
	* @brief Writes undirected graph to stream in dimacs format
	* 
	*		 (self-loops are not considered)
	*/
	ostream& write_dimacs	(ostream& o = std::cout)		override;								
};



/////////////////////////////////////////////
//
// Necessary implementations in header file	

template<class Graph_t, class W>
template<class Func>
inline 
void Base_Graph_W<Graph_t, W>::transform_weights(Func f)
{
	auto NV = number_of_vertices();

	///////////////////////////////////////////////////////
	std::transform(w_.begin(), w_.end(), w_.begin(), f);
	///////////////////////////////////////////////////////

}


#endif