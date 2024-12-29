/*  
 * graph.h file from the GRAPH library, a C++ library for bit encoded 
 * simple graphs. GRAPH stores the adjacency matrix in full, each row encoded as a bitstring. 
 * GRAPH is at the core many state of the art leading exact clique algorithms. 
 * (see license file (legal.txt) for references)
 *
 * Copyright (C)
 * Main developper: Pablo San Segundo
 * Intelligent Control Research Group CAR(UPM-CSIC) 
 *
 * Permission to use, modify and distribute this software is
 * granted provided that this copyright notice appears in all 
 * copies, in source code or in binaries. For precise terms 
 * see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any 
 * kind, express or implied, and with no claim as to its
 * suitability for any purpose.
 *
 */

#ifndef __SIMPLE_GRAPH_H__
#define __SIMPLE_GRAPH_H__

//#include "bitscan/bitscan.h"
#include "filter_graph_encoding_type.h"
#include <iostream>
#include <string>
#include <vector>

//////////////////
//switches 
//#define DIMACS_REFERENCE_VERTICES_0			//DEFAULT OFF! (real DIMACS format)

//////////////////
//
// generic class Graph
// 
// Basic types limited to bitarrays and sparse bitarrays
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
	Graph								(int nV);												//creates empty graph with size vertices	
	Graph								(std::string filename);	
	Graph								(int nV, int* adj[], std::string filename = "");					//old-style adjacency matrix
virtual	~Graph() = default; 

/////////////
// setters and getters
	int set_graph						(std::string filename);
		
	std::string get_name				() const {return name_;}
	std::string get_path				() const {return path_;}
	void set_name						(std::string graph_name, bool separate_path=false);		//clears path by DEFAULT (separate_path = false)
	void set_path						(std::string path_name) {path_ = path_name;}
const vector<T>& get_graph				()	const;	
virtual void add_edge					(int v, int w);								//v->w	(no self_loops allowed)
virtual void remove_edge				(int v, int w);	
		void remove_edges				(int v);									//removes in-out edges from v

	//bitstring encoding
	int number_of_vertices				()					const		{return NV_; }
	int number_of_blocks				()					const		{return NBB_;}
virtual	BITBOARD number_of_edges		(bool lazy=true);
virtual	BITBOARD number_of_edges		(const T& bbsg )	const;								//on induced subgraph
const T& get_neighbors					(int v)				const		{return adj_[v];}
      T& get_neighbors					(int v)							{return adj_[v];}
	
//////////////////////////
// memory allocation 
	int init							(int nV);									//allocates memory for n vertices
	void clear							();											//deallocates memory if required
	Graph& create_subgraph				(int size, Graph& newh)  ;
	int shrink_to_fit					(int size);									//reduces the graph to size (currently only for sparse graphs)

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

	void make_bidirected				();											//make all edges symmetrical

/////////////
// Boolean
virtual	bool is_edge					(int i, int j)			const;		
		
private:
	bool is_no_self_loops				()						const;

////////////////
//Comparisons
	template <class U>
	friend bool operator ==				(const Graph<U>& lhs, const Graph<U>& rhs);				//EXPERIMENTAL! only compares the adj. matrix	

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
	ostream& print_adj					(std::ostream& = std::cout, bool add_endl=true);
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
inline
bool operator == (const Graph<T>& lhs, const Graph<T>& rhs) {

	return lhs.adj_ == rhs.adj_;
	/*for(int i=0; i<lhs.NV_; i++){
		if(!(lhs.get_neighbors(i)==rhs.get_neighbors(i)))
			return false;
	}
	return true;*/
}

#endif