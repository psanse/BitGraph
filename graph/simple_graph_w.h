/*
 * simple_graph_w.h file for the classes Base_Graph_W and Graph_W for weighted graphs 
 *
 * @creation_date 16/01/19
 * @last_update 06/01/25
 * @dev pss
 *
 * This code is part of the GRAPH C++ library for bit encoded
 * simple graphs. GRAPH stores the adjacency matrix in full, each
 * row encoded as a bitstring.
 *
 * GRAPH is at the core many state of the art leading exact clique
 * algorithms.
 *
 * TODO - specialization for sparse graphs (06/01/2025)
 */

#ifndef __SIMPLE_GRAPH_WEIGHTED_H__
#define __SIMPLE_GRAPH_WEIGHTED_H__

#include <iostream>
#include <vector>
#include "utils/common.h"
#include "graph/simple_ugraph.h"


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
	static const W NOWT;								//empty weight value for weights (0.0)	
	static const W DEFWT;								//default weight value for weights (1.0)	
	
	//constructors
	Base_Graph_W						() {};																				//No memory allocation
explicit Base_Graph_W					(std::vector<W>& lw);																//creates empty graph with |V|= n with vertex weights
	Base_Graph_W						(_gt& g, vector<W>& lw)		:g_(g), w_(lw) {}										//creates graph with vertex weights	
	Base_Graph_W						(_gt& g)					:g_(g), w_(g.number_of_vertices(), 1) {}				//creates graph with unit weights
explicit Base_Graph_W					(int n, W val = DEFWT)				  { reset(n, val); }							//creates empty graph with |V|= n with unit weights	
	
	/*
	* @brief Reads weighted graph from ASCII file in DIMACS format
	*		
	*		 If unable to read weights generates modulus weights [Pullman 2008]
	* 
	*		 TODO: add support for other formats
	*/
	Base_Graph_W						(std::string filename);															

	//copy constructor, move constructor, copy operator =, move operator =
	Base_Graph_W						(const Base_Graph_W& g) = default;												
	Base_Graph_W						(Base_Graph_W&& g)		= default;												
	Base_Graph_W& operator =			(const Base_Graph_W& g) = default;												
	Base_Graph_W& operator =			(Base_Graph_W&& g)		= default;												


	//destructor
	virtual	~Base_Graph_W()	 = default;		

/////////////
// setters and getters
	
	void set_w							(int v, W val)				{ w_.at(v)=val;}				
	void set_w							(W val = DEFWT)				{ w_.assign(g_.number_of_vertices(), val);}		
	
	/*
	* @brief sets vertex weights to all vertices
	* @param lw vector of weights of size |V|
	* @returns 0 if success, -1 if error
	*/
	int	 set_w							(std::vector<W>& lw);
	
	Graph_t& graph						()							{ return g_;}
const Graph_t& graph					()			const			{ return g_; }

	W get_w								(int v)		const			{ return w_[v];}	
const vector<W>& get_weights			()			const			{ return w_;}	
	vector<W>& get_weights				()							{ return w_;}	
	
	/*
	* @brief Determines weight and vertex of maximum weight
	* @param v ouptut vector of maximum weight
	* @returns weight of vertex v
	*/
	W maximum_weight					(int& v)	const;	
	
const _bbt& get_neighbors				(int v)		const			{ return g_.get_neighbors(v); }
 	_bbt& get_neighbors					(int v)						{ return g_.get_neighbors(v); }
	
	void set_name						(std::string str)			{ g_.set_name(str); }
	std::string get_name				()			const			{ return g_.get_name(); }
	void set_path						(std::string path_name)		{ g_.set_path(path_name); }
	std::string get_path				()			const			{ return g_.get_path(); }

	int number_of_vertices				()			const			{ return g_.number_of_vertices(); }
	int number_of_edges					(bool lazy = true)			{ return g_.number_of_edges(lazy); }


//////////////////////////
// memory allocation 

	/*
	* @brief resets to empty graph with |V|= n and assigns weight val to all vertices
	* @param n number of vertices
	* @param reset_name if true, @name_ and @path_ reset to empty
	* @returns 0 if success, -1 if memory allocation fails
	* @comment preserved for backward compatibility (use reset(...))
	*/
	int init							(std::size_t n, W val = DEFWT, bool reset_name = true);
	

	/*
	* @brief resets to empty graph given name and number of vertices
	* @param n number of vertices
	* @param name name of the instance
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int reset							(std::size_t n, W val = DEFWT,  string name = "");

	/*
	* @brief deallocates memory and resets to default values
	*/
	void clear							()								{ g_.clear(); w_.clear(); }


/////////////////////////
//basic graph operations

	/*
	*  @brief adds edge, no self-loops allowed
	*/
	void add_edge						(int v, int w)					{ g_.add_edge(v, w); }
	
	double density						(bool lazy = true)				{ return g_.density(lazy); }
	
	void gen_random_edges				(double p)						{ g_.gen_random_edges(p); }


/////////////
// boolean properties

	bool is_edge						(int v, int w)		const		{ return g_.is_edge(v, w); }
	bool is_unit_weighted				();


///////////////////////////
//weight generation
	
	/*
	* @brief generates weights based on modulus operation [Pullan 2008, MODE = 200]
	* 
	*			w(v) = (v + 1) % MODE	(v 1-based index)
	*			
	*			MODE = 1 -> w(v) = 1	(unweighted graph)
	*/
	int gen_modulus_weights				(int MODE = DEFAULT_WEIGHT_MODULUS);
	
////////////
// I/O
public:
		
	/*
	* @brief Writes directed graph to stream in dimacs format
	*
	*		 (self-loops are not considered)
	*/
	virtual ostream& write_dimacs		(std::ostream& o = std::cout);												
	
	/*
	* @brief Reads weighted undirected graph from file in DIMACS format
	* @param filename name of the file
	* @param type extension of additional filename for separate weights (Wext, Dext, WWWext, NOext)
	*		 default NOext - no additional file
	* @returns 0 if success, -1 if error
	*/
	int read_dimacs						(string filename, int type = NOext);
	
	/*
	* @brief Reads weights from an external file (only weights) 
	* 
	*		 Format: numbers ordered and separated (line, space) 
	*				  i.e. {5 6 7 ...} -> w(1)=5, w(2)=6, w(3)=7,...
	*			
	*		 Weights not assigned in the file are set to 0.0
	* 
	* @param filename name of the file
	* @returns 0 if success, -1 if error (empty vector of weights)
	*/
	int read_weights					(string filename);												
		
	ostream& print_data					(bool lazy = true, std::ostream& o = std::cout, bool endl = true);	
	
	ostream& print_weights				(std::ostream& o= std::cout, bool show_v=true)								const;
	ostream& print_weights				(_bbt & bbsg, std::ostream& o= std::cout)									const;
	ostream& print_weights				(vint& lnodes, std::ostream& o= std::cout)									const;
	ostream& print_weights				(com::stack_t<int>& lv, ostream& o= std::cout)								const;
	ostream& print_weights				(com::stack_t<int>& lv, const vint& mapping, std::ostream& o= std::cout)	const;
	ostream& print_weights				(int* lv, int n, std::ostream& o= std::cout)								const;

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
	int degree				(int v, const BitBoardN& bbn)	const				{ return ptype::g_.degree(v, bbn); }

	/*
	* @brief Complement graph (currently name info of original graph is lost)
	* @param g output graph
	* @returns 0 if success, -1 if error
	*/
	int create_complement	(Graph_W& g)					const;	

	/*
	* @brief Complement undirected graph (weights are lost)
	* @param g output complement undirected unweighted graph
	* @returns 0 if success, -1 if error
	*/
	int create_complement	(ugraph& g)					    const				{ return ptype::g_.create_complement(g);}		 

///////////
//I/O operations
	
	/*
	* @brief Writes undirected graph to stream in dimacs format
	* 
	*		 (self-loops are not considered)
	*/
	ostream& write_dimacs	(ostream& o = std::cout)		override;								
};


#endif