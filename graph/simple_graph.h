/* 
 * simple_graph.h file for the class Graph for simple graphs  
 * 
 * @creation_date 17/6/10 
 * @last_update 31/12/24
 /**
   * @file simple_graph.h
   * @brief class Graph for simple graphs 
   *
   * @created 17/6/10
   * @last_update 06/01/25
   * @author pss
   *
   * This code is part of the GRAPH 1.0 C++ library
   * 
   * TODO use SFINAE to filter types (10/01/2025)
   *
   **/

#ifndef __SIMPLE_GRAPH_H__
#define __SIMPLE_GRAPH_H__

#include "filter_graph_encoding_type.h"			//contains #include "bitscan/bitscan.h" - TODO use SFINAE to filter types (10/01/2025)
#include <iostream>
#include <string>
#include <vector>

//////////////////
//
// Generic class Graph<T>
// 
// (T is limited to bitarray and sparse_bitarray types)
// 
//////////////////

template<class T = bitarray>
class Graph: public filterGraphTypeError<T> {
	
	friend class GraphConversion;	
	

public:
		
	using type = Graph<T>;				//own type
	using basic_type = T;				//basic type (a type of bitset)
	using _bbt = basic_type;			//alias for basic type - for backward compatibility

/////////////			
//construction / destruction
	Graph								();															//creates empty graph
explicit Graph							(std::size_t n);											//creates graph with n=|V| and m=0 	
explicit Graph							(std::string filename);										//creates graph from file		
	Graph								(std::size_t n, int* adj[], std::string filename = "");		//old-style adjacency matrix

	//move and copy semantics allowed
	Graph								(const Graph& g)				= default;					
	Graph& operator =					(const Graph& g)				= default;					
	Graph								(Graph&& g)			noexcept	= default;					
	Graph& operator =					(Graph&& g)			noexcept	= default;					
		
virtual	~Graph							()								= default; 

/////////////
// setters and getters

	/**
	* @brief Sets instance name.
	* @param instance name of instance
	* @details: Separates path and instance name internally (if applicable)
	**/
	void name							(std::string instance);
std::string name						()					const	{ return name_; }
	
	void path							(std::string path_name)		{ path_ = std::move(path_name); }
std::string path						()					const	{ return path_;}

	std::size_t number_of_vertices		()					const	{return NV_; }

	/**
	* @brief number of bitblocks in each bitset (row) of the adjacency matriz
	*		 In the case of sparse graphs, the value is the maximum capacity
	*		 of all bitsets.
	**/
	std::size_t number_of_blocks		()					const	{return NBB_;}
	
	/*
	* @brief Counts the number of edges	(includes self loops)
	* 
	*		 @comment: it can be a heavy operation for massive graphs
	* 
	* @param lazy if TRUE (reads value @NE_)
	*			  if FALSE counts and updates @NE_	
	* @returns number of edges 
	*/
	virtual	BITBOARD number_of_edges	(bool lazy = true);
	
	/**
	* @brief Counts the number of edges	in an induced subgraph by a set of vertices
	* @param set input bitset of vertices that induces the subgraph
	* @returns number of edges
	**/
	virtual	BITBOARD number_of_edges	(const T& set )	const;				

const vector<T>& get_adjacency_matrix	()					const		{ return adj_; }
const T& get_neighbors					(int v)				const		{return adj_[v];}
      T& get_neighbors					(int v)							{return adj_[v];}

//////////////////////////
// memory allocation 
public:
	
	/**
	* @brief resets to empty graph given name and number of vertices
	* @param n number of vertices
	* @param name name of the instance 
	* @returns 0 if success, -1 if memory allocation fails
	* @creation 31/12/24
	**/
	int reset							(std::size_t n, string name = "");

	/**
	* @brief sets graph from file in dimacs/MTX/Edges formats (in this order)
	* @param filename file
	* @returns 0 if success, -1 if file cannot be read
	**/
	int reset							(std::string filename);

	/**
	* @brief resets to default values (does not deallocate memory)
	* @details: to deallocate memory -  g = graph()
	* @details: in general, should not be called directly
	**/
	void reset							();											 

	/**
	* @brief reduces the graph to n vertices 
	*
	*		(currently only for sparse graphs)
	* 
	* @param n number of vertices of the new graph
	* @returns 0 if success, -1 if memory allocation fails
	**/
	int shrink_to_fit					(std::size_t n);
						
//////////////	
// Basic operations	
public:
	/**
	* @brief density of the directed graph
	* @param lazy reads NE_ cached value if TRUE
	**/
	
virtual	double density					(bool lazy=true);	
	
	/**
	* @brief density of the subgraph induced by a set of vertices
	* @param set input (bit) set of vertices 
	**/
	template <class bitset_t = T>
	double density						(const bitset_t& set);
	
	/**
	* @brief number of non-empty bit blocks / total number of bit blocks
	*		
			 Specialized for sparse graphs
	*		 (in the case of sparse graphs, density is expected to be 1.0)
	*		
	**/	
	double block_density				()						const;
	
	/**
	* @brief number of allocated blocks / total possible number of blocks
	*
	*		 I.ONLY for sparse graphs
	**/	
	double block_density_sparse			()						const;					
 	
	/**
	* @brief average measure of block density (averages the density of each sparse bitset)
	*
	*		 I.ONLY for sparse graphs
	**/
	double average_block_density_sparse	()						const;					

	/**
	* @brief number of outgoing edges from v
	* @param v input vertex
	**/
	int degree_out						(int v)					const { return adj_[v].size(); }
	
	/**
	* @brief number edges incident to v
	* @param v input vertex
	**/
	int degree_in						(int v)					const;

//////////////	
// Modifiers

public:
	/**
	* @brief adds edge {v -> w} to the graph,
	*		 no self loops allowed 
	* @param v outgoing endpoint 
	* @param w ingoing endpoint
	* 
	**/
virtual void add_edge					(int v, int w);		

	/**
	* @brief removes edge {v -> w} from the graph
	* @param v outgoing endpoint
	* @param w ingoing endpoint
	**/
virtual void remove_edge				(int v, int w);

	/**
	* @brief removes edges with endpoint in v (outgoing/ingoing) 		 
	* @param v input vertex
	**/
	void remove_edges					(int v);									
	
	/**
	* @brief removes all edges
	**/
	void remove_edges					();


	/**
	* @brief makes all edges bidirected (conversion to undirected graph)
	**/
	void make_bidirected				();	

	//random generation

	/**
	* @brief generates directed edges with probability p.
	*
	*		 I. (v, v) not allowed.
	*
	* @param v input endpoint
	* @param w input endpoint
	* @returns 0 is success, -1 if error
	**/
	virtual void gen_random_edges		(double p);
	
	/**
	* @brief generates edge (v, w) with probability p.
	* 
	*		 I. (v, v) not allowed.
	*		 II. Valid for directed/undirected graphs (TMP design)
	* 
	* @param v input endpoint
	* @param w input endpoint
	* @returns 0 is success, -1 if error 	
	**/
	int gen_random_edge		(int v, int w, double p);

//////////////	
// Induced subgraphs

	/**
	* @brief computes the induced subgraph by the first k vertices in the current graph
	* @param first_k first k vertices to be included in the new graph
	* @param g output new induced subgraph
	* @returns the new induced subgraph (if the operation fails, g remains unchanged)
	**/
virtual	Graph& create_subgraph			(std::size_t first_k, Graph& g) const;

	/**
	* @brief creates the subgraph induced by the vertices NOT in the input set
	* @param set input set of vertices
	* @param g ouptut induced subgraph
	**/
	void remove_vertices				(const BitSet& set, Graph& g);

//////////////	
// deleted - CHECK	
	virtual void remove_vertices		(const BitSet& set) = delete;	//commented out implementation - EXPERIMENTAL
	
/////////////
// Boolean properties
public:

virtual	bool is_edge					(int v, int w)			const { return(adj_[v].is_bit(w)); }
	
	/**
	* @brief returns TRUE if (v, v) edges are present
	**/
	bool is_self_loop					()						const;

////////////////
//Comparisons
public:
	/**
	* @brief determines if two graphs have the same adjacency matrices 
	* @param lhs left hand side graph
	* @param rhs right hand side graph
	* @returns TRUE if lhs.adj_ == rhs.adj_
	**/
	template <class T>
	friend bool operator ==				(const Graph<T>& lhs, const Graph<T>& rhs);  
		
////////////
// Read / write basic operations

public:
	/**
	* @brief reads a simple directed unweighted graph in DIMACS format
	* @returns 0 if correct, -1 in case of error 
	* @details: does not read weights
	**/
	int read_dimacs						(const std::string& filename);	

	/**
	* @brief reads a graph matrix exchange format (at the moment only MCPS)
	* @returns 0 if correct, -1 in case of error 
	**/
	int read_mtx						(const std::string& filename);

	/**
	* @brief reads a graph in list of edges format  
	* @returns 0 if correct, -1 in case of error
	**/
	int read_EDGES						(const std::string& filename);

	/**
	* @brief reads 0-1 adjacency matrix (rows) with a first line indicating |V|
	**/
	int read_01							(const std::string& filename);

	/*
	* @brief writes directed graph in dimacs format
	* 
	*		(specialized for sparse graphs)
	* 
	* @param o output stream
	*/
	virtual	void  write_dimacs			(std::ostream& o);

	/**
	* @brief writes timestamp for dimacs format
	**/
	std::ostream& timestamp_dimacs		(std::ostream& o = std::cout) const;
	
	/**
	* @brief writes graph name for dimacs format
	**/
	std::ostream& name_dimacs			(std::ostream& o = std::cout) const;
	
	/**
	* @brief writes graph header for dimacs format
	**/
	std::ostream& header_dimacs			(std::ostream& o = std::cout, bool lazy = true);

	/*
	* @brief writes directed graph in edge list format
	* @param o output stream
	*/
virtual	void  write_EDGES				(std::ostream& o) ;

////////////
// I/O basic operations
	
	/**
	* @brief prints basic data of the graph to the output stream (n, m and density)
	* @param lazy if TRUE, reads the number of edges from the cached value @NE_ to compute density
	* @details Uses the Template Method Pattern (number_of_edges will be overriden in derived classes)
	* @details Density can be a heavy operation to compute, since it requires the number of edges. 
	*		   If  @lazy is TRUE the number of edges is read from the cached value @NE_
	**/
	ostream& print_data					(bool lazy = true, std::ostream& = std::cout, bool eofl = true);
	
	/**
	* @brief Adjacency matrix to the output stream, in a readable 0-1 format
	**/
	ostream& print_adj					(std::ostream& = std::cout, bool eofl = true) const;
	
	/**
	* @brief Edges of the graph to the output stream in format [v]-->[w]
	**/
	virtual ostream& print_edges		(std::ostream& = std::cout) ;

	/*
	* @brief Edges of the subgraph induced by a set of vertices to output stream
	* @param bbsg input (bit) set of vertices
	* @param o output stream
	*/
	template <class bitset_t = T>
	ostream& print_edges				(bitset_t& bbsg, ostream& o = std::cout)	const;	
		
//////////////////////////
// data members
protected:
	std::vector<T> adj_;	//adjacency matrix 
	std::size_t NV_;		//number of vertices
	BITBOARD NE_;			//number of edges (updated on the fly)
	std::size_t NBB_;		//number of bit blocks per row (in the case of sparse graphs this is a maximum value)
	
	//names
	std::string name_;		//name of instance, without path	
	std::string path_;		//path of instance
};

//////////////////////////////////////////
// Necessary implementation of template methods in header file

template <class T>
inline bool operator == (const Graph<T>& lhs, const Graph<T>& rhs) {	return lhs.adj_ == rhs.adj_; }

template<class T>
template <class bitset_t>
inline double Graph<T>::density(const bitset_t& bbN) {
	BITBOARD  edges = number_of_edges(bbN);
	if (edges == 0) { return 0.0; }

	BITBOARD  pc = bbN.popcn64();
	return edges / static_cast<double>(pc * (pc - 1) / 2);
}

template<class T>
template<class bitset_t>
ostream& Graph<T>::print_edges(bitset_t& bbsg, ostream& o) const {
	
	for (int i = 0; i < NV_ - 1; i++) {

		if (!bbsg.is_bit(i)) continue;

		for (int j = i + 1; j < NV_; j++) {

			if (!bbsg.is_bit(j)) continue;

			if (is_edge(i, j)) {
				o << "[" << i << "]" << "-->" << "[" << j << "]" << endl;
			}
			if (is_edge(j, i)) {
				o << "[" << j << "]" << "-->" << "[" << i << "]" << endl;
			}
		}
	}
	return o;
}




#endif