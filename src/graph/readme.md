
GRAPH DOCUMENTATION UNDER DEVELOPMENT
===================

GRAPH is a C++ library for graphs. At its core, GRAPH relies on the BITSCAN library (located in the bitscan folder of the repository). GRAPH uses BITSCAN to efficiently represent graphs in memory and to optimize fundamental operations such as, e.g., computing vertex degrees, sorting vertices, and building graph isomorphisms.

Main Features
-----------------------------
Among the features of GRAPH we highlight the following:
* Efficient representation of graphs in memory using bitsets.
* Optimised graph operations using bitmasks
* Specialised types for sparse graphs, vertex-weighted and edge-weighted graphs.
* Specialised operations for graph generation (uniform random graphs, sparse graphs)
* Specialised operations for vertex sorting 

Getting Started
-----------------------------

To use GRAPH in your projects, add the following header: 

	#include "graph/graph.h"

To add k-core graph analysis, for example, (see GRAPH ANALYSIS section below) the header is:

	#include "graph/kcore.h"

**Additional documentation is under development, and will be published asap.**

BUILDING GRAPHS
-------------------------------
GRAPH currently distinguishes 4 main types of graph objects:

- `graph`: for directed graphs
- `ugraph`: for undirected graphs
- `sparse_graph`: for sparse directed graphs
- `sparse_ugraph`: for sparse undirected graphs


The following example builds a directed graph with 100 vertices and 3 edges:

    #include "graph/graph.h"
    //...
    
    graph g(100);					//creates an empty (directed) graph with 100 vertices
    g.add_edge(0,1);				//configures the edges
	g.add_edge(0,2);
	g.add_edge(1,2);


To make the edges bidirected call the *make\_bidirected* function:
   
    g.make_bidirected()				//creates a 3-clique

A better option for cliques would be the class Ugraph for undirected graphs. Here is the code for the 3-clique subgraph {0, 2, 3} in a graph with 100 vertices:
   
    ugraph g(100);					//creates an empty (undirected) graph with 100 vertices
	g.add_edge(0,1);				
	g.add_edge(0,2);
	g.add_edge(1,2);

EDITING GRAPHS
-------------------------------

Edges may be added or deleted in constant time (*remove\_edge* member function) for simple graphs (in the case of sparse graphs complexity increases because it takes logarithmic time to find an edge). They may also be generated randomly, as well as uniform random graph benchmarks. Graphs may also be resized and copied. Here is a trivial example of graph editing which involves adding and deleting edges:
   
    graph g(500);							
	g.add_edge(0,1);				
	g.add_edge(0,2);
	g.add_edge(1,2);
	g.remove_edge(0,1);
	g.remove_edge(0,2);
	g.remove_edge(1,2);
    if(g.is_empty()) cout<<"empty graph"<<endl;			

A MORE COMPLEX EXAMPLE
-------------------------------
[K-core analysis](https://en.wikipedia.org/wiki/Degeneracy_(graph_theory)) in GRAPH is extremely fast, which makes it ideal for massive sparse graphs.  

Here is an example:
    
    #include "graph/graph.h" 
    #include "graph/kcore.h" 
   				
    void main(){
	  sparse_ugraph ug("road-belgium-osm");
	  KCore<sparse_ugraph> kc(ug);				//configures KCore class
      kc.kcore();						//k-core analysis for the full graph
	  kc.print_kcore();	

	  sparse_bitarray bbs(ug.number_of_vertices());		//k-core analysis for the subgraph bbs induced by the first 30 vertices
	  bbs.set_bit(0,29);
	  kc.set_subgraph(&bbs);
	 
  	  kc.kcore();    
	  kc.print_kcore();							
    }



    
EXTERNAL SOURCES
-------------------------------

GRAPH supports DIMACS, Matrix Market Exchange and Edge list text formats. A few examples of undirected graphs in different formats may be found in the *data* folder. To load a graph from file pass the appropriate filename to the constructor:

    
    graph g("brock200_1.clq");				//reads "brock200_1.clq" file in DIMACS, MTX, or EDGES format
    
OTHERS
-------------------------------

Basic functionality in GRAPH includes sorting according to different criteria, subgraph degree and support,  random generation, subgraph generation, density, weights on edges and vertices and many others.


COMPATIBILITY
-------------------------------

GRAPH has been tested in Windows 64-bit OS and Linux OS. Note that a 64-bit OS is mandatory because of the BITSCAN dependency.
