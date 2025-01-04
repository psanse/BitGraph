/* 
 * simple_graph.h file for the class Graph for simple graphs  
 * 
 * @creation_date 17/6/10 
 * @last_update 31/12/24
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

#ifndef __SIMPLE_GRAPH_H__
#define __SIMPLE_GRAPH_H__

#include "filter_graph_encoding_type.h"			//contains #include "bitscan/bitscan.h"
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
			
	//constructors
	Graph								();															//creates empty graph
	Graph								(std::size_t n);											//creates graph with n=|V| and m=0 	
	Graph								(std::string filename);										//creates graph from file		
	Graph								(std::size_t n, int* adj[], std::string filename = "");		//old-style adjacency matrix

	
	Graph								(Graph&& g)		= default;									//move constructor
	Graph								(const Graph& g)= default;									//copy constructor
	Graph& operator =					(const Graph& g)= default;									//copy operator =
	Graph& operator =					(Graph&& g)		= default;									//move operator =
	

	//destructor	
virtual	~Graph()										= default; 


/////////////
// setters and getters
	/*
	* @brief sets graph from file in dimacs/MTX/Edges formats (in this order)
	* @param filename file
	* @returns 0 if success, -1 if file cannot be read
	*/
	int set_graph						(std::string filename);

	/*
	* @brief Sets instance name.
	*		 Separates path and instance name internally (if applicable
	* @param instance name of instance
	*/
	void set_name						(std::string instance);
	
	void set_path						(std::string path_name) { path_ = path_name; }
	std::string get_name				()					const {return name_;}
	std::string get_path				()					const {return path_;}
	int get_number_of_edges				()					const { return NE_; }

	std::size_t number_of_vertices		()					const		{return NV_; }
	std::size_t number_of_blocks		()					const		{return NBB_;}
	
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
		

	/*
	* @brief Counts the number of edges	in an induced subgraph by a set of vertices
	* @param set input bitset of vertices that induces the subgraph
	* @returns number of edges
	*/
	virtual	BITBOARD number_of_edges	(const T& set )	const;				

const vector<T>& get_adjacency_matrix	()					const		{ return adj_; }
const T& get_neighbors					(int v)				const		{return adj_[v];}
      T& get_neighbors					(int v)							{return adj_[v];}


//////////////////////////
// memory allocation 
public:
	/*
	* @brief resets to empty graph with |V|= n
	* @param n number of vertices
	* @param reset_name if true, @name_ and @path_ reset to empty
	* @returns 0 if success, -1 if memory allocation fails
	* @updates 18/06/19, 31/12/24
	* @comment_1 preserved for backward compatibility (use reset(...))
	*/
	int init							(std::size_t n, bool reset_name = true);	

	/*
	* @brief resets to empty graph given name and number of vertices
	* @param n number of vertices
	* @param name name of the instance 
	* @returns 0 if success, -1 if memory allocation fails
	* @creation 31/12/24
	*/
	int reset							(std::size_t n, string name = "");

	/*
	* @brief deallocates memory and resets to default values
	*/
	void clear							();											 

	/*
	* @brief reduces the graph to n vertices 
	*
	*		(currently only for sparse graphs)
	* 
	* @param n number of vertices of the new graph
	* @returns 0 if success, -1 if memory allocation fails
	*/
	int shrink_to_fit					(std::size_t n);
						
//////////////	
// Basic operations	
public:
	/*
	* @brief density of the directed graph
	* @param lazy reads NE_ cached value if TRUE
	*/
	
virtual	double density					(bool lazy=true);	
	
	/*
	* @brief density of the subgraph induced by a set of vertices
	* @param set input (bit) set of vertices 
	*/
	template <class bitset_t = T>
	double density						(const bitset_t& set);
	
	/*
	* @brief number of non-empty bit blocks / total number of bit blocks
	*		
			 Specialized for sparse graphs
	*		 (in the case of sparse graphs, density is expected to be 1.0)
	*		
	*/	
	double block_density				()						const;
	
	/*
	* @brief number of allocated blocks / total possible number of blocks
	*
	*		 ONLY for sparse graphs
	*/	
	double block_density_sparse			()						const;					
 	
	/*
	* @brief average measure of block density (averages the density of each sparse bitset)
	*
	*		 ONLY for sparse graphs
	*/
	double average_block_density_sparse	()						const;					

	/*
	* @brief number of outgoing edges from v
	* @param v input vertex
	*/
	int degree_out						(int v)					const { return adj_[v].popcn64(); }
	
	/*
	* @brief number edges incident to v
	* @param v input vertex
	*/
	int degree_in						(int v)					const;

//////////////	
// Modifiers

public:
	/*
	* @brief adds edge {v -> w} to the graph,
	*		 no self loops allowed 
	* @param v outgoing endpoint 
	* @param w ingoing endpoint
	* 
	*/
virtual void add_edge					(int v, int w);		

	/*
	* @brief removes edge {v -> w} from the graph
	* @param v outgoing endpoint
	* @param w ingoing endpoint
	*/
virtual void remove_edge				(int v, int w);

	/*
	* @brief removes edges with endpoint in v (outgoing/ingoing) 		 
	* @param v input vertex
	*/
	void remove_edges					(int v);									
	
	/*
	* @brief makes all edges bidirected (conversion to undirected graph)
	*/
	void make_bidirected				();	

//////////////	
// Induced subgraphs

	/*
	* @brief computes the induced subgraph by the first k vertices in the current graph
	* @param first_k first k vertices to be included in the new graph
	* @param g output new induced subgraph
	* @returns the new induced subgraph (if the operation fails, g remains unchanged)
	*/
virtual	Graph& create_subgraph			(std::size_t first_k, Graph& g) const;

	/*
	* @brief creates the subgraph induced by the vertices NOT in the input set
	* @param set input set of vertices
	* @param g ouptut induced subgraph
	*/
	void remove_vertices				(const BitBoardN& set, Graph& g);

//////////////	
// deleted - CHECK	
	virtual void remove_vertices		(const BitBoardN& set) = delete;	//commented out implementation - EXPERIMENTAL
	
/////////////
// Boolean properties
public:

virtual	bool is_edge					(int v, int w)			const { return(adj_[v].is_bit(w)); }
	
	/*
	* @brief returns TRUE if (v, v) edges are present
	*/
	bool is_self_loop					()						const;

////////////////
//Comparisons
public:
	/*
	* @brief determines if two graphs have the same adjacency matrices 
	* @param lhs left hand side graph
	* @param rhs right hand side graph
	* @returns TRUE if lhs.adj_ == rhs.adj_
	*/
	template <class T>
	friend bool operator ==				(const Graph<T>& lhs, const Graph<T>& rhs);  
		
////////////
// Read / write basic operations

public:

	int read_dimacs						(const std::string& filename);	
	int read_mtx						(const std::string& filename);
	int read_EDGES						(const std::string& filename);
	int read_01							(const std::string& filename);

	/*
	* @brief writes directed graph in dimacs format
	* 
	*		(specialized for sparse graphs)
	* 
	* @param o output stream
	*/
	virtual	void  write_dimacs			(std::ostream& o);

	std::ostream& timestamp_dimacs		(std::ostream& o = std::cout);
	std::ostream& name_dimacs			(std::ostream& o = std::cout);
	std::ostream& header_dimacs			(std::ostream& o = std::cout, bool lazy = true);

	/*
	* @brief writes directed graph in edge list format
	* @param o output stream
	*/
virtual	void  write_EDGES				(std::ostream& o);

////////////
// I/O basic operations
	ostream& print_data					(bool lazy = true, std::ostream& = std::cout, bool endl = true);
	ostream& print_adj					(std::ostream& = std::cout, bool endl = true);
	
	virtual ostream& print_edges		(std::ostream& = std::cout) const;

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
	/////////////
	// TODO-optimize
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