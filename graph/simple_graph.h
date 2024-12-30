/*  
 * graph.h file for the class Graph for simple graphs  
 *
 * This code is part of the GRAPH C++ library for bit encoded 
 * simple graphs. GRAPH stores the adjacency matrix in full, each row encoded as a bitstring. 
 * GRAPH is at the core many state of the art leading exact clique algorithms. 
 * 
 * Copyright (C)
 * Main developper: Pablo San Segundo
 * Intelligent Control Research Group CAR(UPM-CSIC) 
 *
 */

#ifndef __SIMPLE_GRAPH_H__
#define __SIMPLE_GRAPH_H__

#include "filter_graph_encoding_type.h"			//contains #include "bitscan/bitscan.h"
#include <iostream>
#include <string>
#include <vector>

//////////////////
//switches 
//#define DIMACS_REFERENCE_VERTICES_0			//DEFAULT OFF! (real DIMACS format)

//////////////////
//
// Generic class Graph<T>
// 
// (T is limited to bitarray and sparse_bitarray types)
// 
//////////////////

template<class T>
class Graph: public filterGraphTypeError<T>{
	friend class GraphConversion;

//	template<typename G, typename W>
//	friend class Graph_W;				/* all specializations are friends */

public:
		
	using type = Graph<T>;				//own type
	using basic_type = T;				//basic type (a type of bitset)
	using _bbt = basic_type;			//alias for basic type - for backward compatibility
			
	//constructors
	Graph								();														//does not allocate memory
	Graph								(std::size_t n);												//creates empty graph with size vertices	
	Graph								(std::string filename);	
	Graph								(std::size_t n, int* adj[], std::string filename = "");		//old-style adjacency matrix
virtual	~Graph() = default; 

/////////////
// setters and getters
	int set_graph						(std::string filename);
		
	std::string get_name				() const {return name_;}
	std::string get_path				() const {return path_;}
	void set_name						(std::string graph_name, bool separate_path=false);		//clears path by DEFAULT (separate_path = false)
	void set_path						(std::string path_name) {path_ = path_name;}

	//bitstring encoding
	int number_of_vertices				()					const		{return NV_; }
	int number_of_blocks				()					const		{return NBB_;}
virtual	BITBOARD number_of_edges		(bool lazy = true);
virtual	BITBOARD number_of_edges		(const T& bbsg )	const;								//on induced subgraph
const T& get_neighbors					(int v)				const		{return adj_[v];}
      T& get_neighbors					(int v)							{return adj_[v];}
const vector<T>& get_adjacency_matrix	()					const		{return adj_;}

//////////////////////////
// memory allocation 
	int init							(std::size_t n);							//allocates memory for n vertices
	void clear							();											//deallocates memory 
	Graph& create_subgraph				(std::size_t n, Graph& g)  ;
	int shrink_to_fit					(std::size_t n);							//reduces the graph to size (currently only for sparse graphs)

	int add_vertex						(int toADD);								//enlarges the graph with @toADD new isolanies 
					
//////////////	
// Basic operations		
virtual	double density					(bool lazy=true)		;	
	double density						(const T& )				;					//on induced graph
	double block_density				()						const;
	double block_density_index			()						;					//returns number of blocks/total possible number of blocks
 	double average_block_density_index	()						;					//average density per block

	int degree_out						(int v)					const;				//outgoing edges from v
	int degree_in						(int v)					const;				//edges incident to v

//////////////	
// Modifiers
virtual void add_edge					(int v, int w);								//v->w	(no self_loops allowed)
virtual void remove_edge				(int v, int w);
	void remove_edges					(int v);									//removes edges with endpoint in v
	void make_bidirected				();											//make all existing edges symmetrical

/////////////
// Boolean
virtual	bool is_edge					(int i, int j)			const;		
		
private:
	bool is_no_self_loops				()						const;

////////////////
//Comparisons

	/*
	* @brief determines if two graphs have the same adjacency matrices 
	* @param lhs left hand side graph
	* @param rhs right hand side graph
	* @returns TRUE if lhs.adj_ == rhs.adj_
	*/
	template <class T>
	friend bool operator ==				(const Graph<T>& lhs, const Graph<T>& rhs);  

/////////////
// Update operations
public:
	void remove_vertices				(const BitBoardN&, Graph&);				
virtual void remove_vertices			(const BitBoardN&);				

////////////
// I/O
public:
	//TODO@place in a graph_io class
	int read_dimacs						(const std::string& filename);	
	int read_mtx						(const std::string& filename);
	int read_EDGES						(const std::string& filename);
	int read_01							(const std::string& filename);
virtual	void  write_dimacs				(std::ostream& o);	
virtual	void  write_EDGES				(std::ostream& o);

virtual	ostream& print_data				(bool lazy=true, std::ostream& = std::cout, bool endl=true);
	ostream& print_adj					(std::ostream& = std::cout, bool endl=true);
	virtual ostream& print_edges		(std::ostream& = std::cout) const;
	virtual ostream& print_edges		(T& bbsg, ostream& = std::cout) const;	/* edges of subgraph*/
		
//////////////////////////
// data members
protected:
	std::vector<T> adj_;	//adjacency matrix 
	std::size_t NV_;		//number of vertices
	BITBOARD NE_;			//number of edges (updated on the fly)
	int NBB_;				//number of bit blocks per row (in the case of sparse graphs this is a maximum value)
	
	//names
	std::string name_;		//name of instance, without path	
	std::string path_;		//path of instance
};

template <class T>
inline bool operator == (const Graph<T>& lhs, const Graph<T>& rhs) {	return lhs.adj_ == rhs.adj_; }

#endif