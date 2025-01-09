/*
 * simple_ugraph.h file for the class Ugraph for simple undirected graphs
 *						(no self loops)
 *
 * @creation_date 17/6/10
 * @last_update 03/01/25
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


using vint = std::vector<int>;

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
explicit Ugraph					(std::size_t n) : Graph(n){}						//creates empty graph of size n=|V|	
explicit Ugraph					(std::string filename);								//reads graph from file
	
	/*
	* @brief Creates a graph from an C-style adjacency matrix
	*		 
	*		Reads only the upper triangle of the adjacency matrix
	*/
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
// Basic operations

	/*
	* @brief density of the undirected graph
	* @param lazy reads NE_ cached value if TRUE
	*/
	double density				(bool lazy = true )						override;

	/*
	* @brief Computes complement graph
	* @param g output complement graph
	* @return 0 if success, -1 if error 
	*/
	int create_complement		(Ugraph& g)									const;	


/////////////
// degree-related operations
// 
// TODO implement bitstring conversions according to the templates properly (3/4/18) - CHECK (02/01/2025)
public:

	/*
	* @brief Computes the number of neighbors of v (deg(v))	*		
	*/
	int degree					(int v)									const { return ptype::adj_[v].popcn64(); }
	
	/*
	*  @brief number of neighbors of v in a set of vertices
	* 
	*		  (specialized for sparse graphs)
	* 
	*  @param bbn input non-sparse (bit) set of vertices
	*/
	int degree					(int v, const BitBoardN& bbn)			const;
	
	/*
	*  @brief number of neighbors of v in a sparse encoded set of vertices
	* 
	*		  (sparse graphs ONLY )
	* 
	*  @param bbn input sparse (bit) set of vertices
	*/
	int degree					(int v, const BitBoardS& bbs)			const;

	/*
	*  @brief truncated number of neighbors of v in a set of vertices
	* 
	*		  (specialized for sparse graphs)
	* 
	*  @param bbn input (bit) set of vertices
	*  @returns neighbors of v if <= UB, otherwise UB
	*/
	int degree					(int v, int UB, const BitBoardN& bbn)	const;  //truncated degree (14/2/2016)
		
	/*
	*  @brief truncated number of neighbors of v in a sparse enconded set of vertices
	*
	*		  (ONLY for sparse graphs)
	*
	*  @param bbn input sparse (bit) set of vertices
	*  @returns neighbors of v if <= UB, otherwise UB
	*/
	int degree					(int v, int UB, const BitBoardS& bbs)	const;	//truncated degree  (14/2/2016)
	
	/*
	*  @brief number of neighbors of v in a vertex set with higher index than v
	* 
	*		  (applied as pivotal strategy for clique enumeration)
	* 
	*  @param bbn input (bit) set of vertices
	*  
	*/
	int degree_up				(int v, const BitBoardN& bbn)			const;  //TODO: test (27/4/2016)
	
	/*
	*  @brief returns the maximum degree of the graph, 
	*         i.e., the maximum degree of any of its vertices
	*/
	int max_graph_degree		()										const;
	
	/*
	*  @brief returns the maximum degree of an induced subgraph
	*  @param sg input (bit) set of vertices of the subgraph
 	*/
	template<class bitset_t>
	int max_subgraph_degree		(bitset_t& sg)							const;

	//TODO implement min_degree (03/01/2025)
	
//////////////	
// Modifiers
public:

	/*
	* @brief Adds bidirectional edge {v, w}
	*		 a) no self-loops are added (currently no feedback)
	*		 b) keeps track of the number of edges
	* @param v endpoint
	* @param w endpoint
	*/
	void add_edge				(int v, int w)							override;					
	
	/*
	* @brief Removes bidirectional edge {v, w}
	*		 a) if self_loop (v = w), graph remains unchanged
	*		 b) keeps track of the number of edges
	* @param v endpoint
	* @param w endpoint
	*/
	void remove_edge			(int v, int w)							override;

	/*
	* @brief generates undirected edges with probability p.
	*
	*		 I. (v, v) not allowed.
	*		 II. Valid for directed/undirected graphs (TMP design)
	*
	* @param v input endpoint
	* @param w input endpoint
	* @returns 0 is success, -1 if error
	*/
	void gen_random_edges		(double p)								override;


//////////////	
// Induced subgraphs

//TODO	Graph& create_subgraph	(std::size_t first_k, Graph& g) const  override;

	/*
	*  @brief Computes the subgraph induced by a set of vertices
	*  @param lv input set of vertices (std::vector<int>)
	*  @returns 0 if success, -1 if error
	*/
	int create_subgraph			(Ugraph& g, vint& lv)					const;

	/*
	*  @brief Computes the subgraph induced by the neighborhood of a  vertex (29/08/21)
	*		 
	*		 Calls create_subgraph(Ugraph& g, vint& lv)
	* 
	*  @param v input vertex which determines the neighborhood
	*  @returns 0 if success, -1 if error
	*/
	int create_subgraph			(Ugraph& g, int v)						const;			
		
		
////////////
// Write basic operations
// Note: Read operations are inherited from the base class Graph (using the Template Pattern)
public:
	/*
	* @brief writes undirected graph in dimacs format
	* @param o output stream
	*/
	void write_dimacs			(std::ostream& filename)				override;
	
	/*
	* @brief writes undirected graph in edge list format
	* @param o output stream
	*/
	void write_EDGES			(std::ostream& filename)				override;
	
	/*
	* @brief writes undirected graph in MMX (Matrix Exchange) format
	* @param o output stream
	*/
	void write_mtx				(std::ostream& filename);				//MTX format only for Ugraph? (03/01/2025)


/////////////////	
//	I/O basic operations
public:

	ostream& print_degrees		(std::ostream& = std::cout)				const;
	ostream& print_edges		(std::ostream& = std::cout)				const override;
	
	template<class bitset_t = T>
	ostream& print_edges		(bitset_t& bbsg, std::ostream& = std::cout)	const ;
	ostream& print_matrix		(std::ostream& = std::cout)				const;

//////////////	
// deleted - CHECK	
	virtual	void remove_vertices(const BitBoardN& bbn) = delete;				//commented out implementation - EXPERIMENTAL

	/*
	*  @brief enlarges the graph with a new vertex (provided its neighborhood)
	*		  TODO - code removed, BUGGY (should not be called , unit tests DISABLED)
	*/
	int add_vertex(_bbt* neigh = NULL) = delete;

};


/////////////////////////////////////////////////////////////
// Necessary implementation of template methods in header file

template<class T>
template<class bitset_t>
inline
int Ugraph<T>::max_subgraph_degree (bitset_t& sg) const {
	
	int max_degree = 0, temp = 0;

	int v = EMPTY_ELEM;
	if (sg.init_scan(bbo::NON_DESTRUCTIVE) != EMPTY_ELEM) {
		while (true) {
			v = sg.next_bit();
			if (v == EMPTY_ELEM) break;

			//compute max_degree circumscribed to subgraph
			temp = degree (v, sg);
			if (temp > max_degree)
				max_degree = temp;
		}
	}

	return max_degree;
}

template<class T>
template<class bitset_t>
ostream& Ugraph<T>::print_edges(bitset_t& bbsg, std::ostream& o) const
{
	for (std::size_t i = 0; i < ptype::NV_ - 1; ++i) {
		if (!bbsg.is_bit(i)) continue;
		for (std::size_t j = i + 1; j < ptype::NV_; ++j) {
			if (!bbsg.is_bit(j)) continue;
			if (ptype::is_edge(i, j)) {
				o << "[" << i << "]" << "--" << "[" << j << "]" << endl;
			}
		}
	}

	return o;
}

#endif






