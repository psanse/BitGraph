/*
 * simple_ugraph.h file for the class Ugraph for simple undirected graphs
 *						(no self loops)
 *
 * @creation_date 17/6/10
 * @last_update 01/01/25
 * @dev pss
 *
 * This code is part of the GRAPH C++ library for bit encoded
 * simple graphs. GRAPH stores the adjacency matrix in full, each
 * row encoded as a bitstring.
 *
 * GRAPH is at the core many state of the art leading exact clique
 * algorithms.
 *
 */

#ifndef __SIMPLE_UGRAPH_H__
#define __SIMPLE_UGRAPH_H__

#include "simple_graph.h"

 //////////////////
 //
 // Generic class Ugraph<T>
 // 
 // (T is limited to bitarray and sparse_bitarray types)
 // 
 //////////////////

template<class T = bitarray>
class Ugraph : public Graph<T> {

	friend class GraphConversion;		//TODO-CHECK

public:

	using type = Ugraph<T>;				//own type
	using ptype = Graph<T>;				//parent type
	using basic_type = T;				//basic type (a type of bitset)
	
	using _bbt = basic_type;			//alias for backward compatibility
	using _mypt = ptype;				//alias for backward compatibility

	//constructors - cannot all be inherited	
	Ugraph						() : Graph() {}										//creates empty graph
	Ugraph						(std::size_t n) : Graph(n){}						//creates empty graph of size n=|V|	
	Ugraph						(std::string filename);								//reads graph from file
	Ugraph						(std::size_t n, int* adj[], string name);			//old-style adjacency matrix
   	
	//TODO copy constructor, move constructor, copy operator =, move operator = (1/1/2025)

	//destructor
	~Ugraph						() = default;										
		
/////////////
// setters and getters
public:

	/*
	* @brief Counts the number of edges	(includes self loops)
	* @param lazy if TRUE (reads value @NE_)
	*			  if FALSE counts and updates @NE_
	*/
	BITBOARD number_of_edges	(bool lazy = true)						  override;
	
	/*
	* @brief Counts the number of edges	in an induced subgraph by a set of vertices
	*/
	BITBOARD number_of_edges	(const T&) 								const override;			

/////////////
// basic operations

//TODO implement bitstring conversions according to the templates properly (3/4/18) - CHECK (02/01/2025)
public:

	/*
	* @brief computes the number of neighbors of v (deg(v))
	*/
	int degree					(int v)									const { return ptype::adj_[v].popcn64(); }
	
	/*
	*  @brief number of neighbors of v in a set of vertices
	*/
	int degree					(int v, const BitBoardN& bbn)			const;
	int degree					(int v, const BitBoardS& bbs)			const;

	int degree(int v, int UB, const BitBoardN& bbn)						const;  //truncated degree (14/2/2016)
	int degree(int v, int UB, const BitBoardS& bbs)						const;	//truncated degree  (14/2/2016)
	int degree_up(int v, const BitBoardN& bbn)							const;  //TODO: test (27/4/2016)
	int max_degree_of_graph()											const;
	int max_degree_of_subgraph(T& sg)									const;



virtual	void add_edge			(int v, int w);									//sets bidirected edge
virtual	void remove_edge		(int v, int w);
virtual	void remove_vertices	(const BitBoardN& bbn) = delete;				//commented out implementation - EXPERIMENTAL
				
	


	//graph creation	
	int create_complement		(Ugraph& g)								const;			//returns complement graph in g
	int create_induced			(Ugraph& g, int v)						const;			//returns induced subgraph on g by the neighborhood of v (29/08/2021)
	int create_induced			(Ugraph& g, std::vector<int>& lv)		const;			//returns induced subgraph on g by the set of vertices lv

	int add_vertex				(_bbt* neigh = NULL);	//***BUGGY!-***enlarges the graph with a new vertex (provided its neighborhood)

	//properties
	double density				(bool lazy=true);
	
	//I/O
	ostream& print_degrees			(std::ostream& = std::cout)			const;
virtual	ostream& print_edges		(std::ostream& = std::cout)			const;
virtual ostream& print_edges		(T& bbsg, std::ostream& = std::cout)	const;
virtual	ostream& print_matrix		(std::ostream& = std::cout)			const;

		
virtual	void write_dimacs			(std::ostream & filename);
virtual	void write_EDGES			(std::ostream & filename);
		void write_mtx				(std::ostream & filename);
};

#endif






