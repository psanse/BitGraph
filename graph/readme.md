
GRAPH 1.0 library for bit-encoded graphs
===================

GRAPH is a very efficient C++ library for graphs. At the core of GRAPH is the BITSCAN library (see folder *bitscan*, in this repository). 

GETTING STARTED
-------------------------------

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
K-core analysis is wrapped inside the `KCore` class.  It computes core decomposition for the graph as well as for any subgraph. Here is an example:
    
    #include "graph/graph.h" 
    #include "graph/kcore.h" 
   				
    void main(){
	  sparse_ugraph ug("brock200_1.clq");
	  KCore<sparse_ugraph> kc(ug);				//configures KCore class
      kc.kcore();								//computes k-core analysis for the full graph
	  kc.print_kcore();	

	 sparse_bitarray bbs(ug.number_of_vertices());		//computes k-core for the subgraph induced by the first 30 vertices
	 bbs.init_bit(0,29);
	 kc.set_subgraph(&bbs);
	 kc.kcore();					
    
	 kc.print_kcore();							
    }

k-core analysis has been [optimized](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=2&cad=rja&uact=8&ved=0CCgQFjAB&url=http%3A%2F%2Fvlado.fmf.uni-lj.si%2Fpub%2Fnetworks%2Fdoc%2Fcores%2Fcores.pdf&ei=Pe8FVJfZD6PIyAO0-IKIAQ&usg=AFQjCNFNFQZTbvdmsjXTqTSH1BFYf1ACKg&sig2=_leTrcnaQKbfFHpSwiZuKQ&bvm=bv.74115972,d.bGQ) and is extremely fast (i.e. runs in O(|E|). This makes it ideal for very large sparse graphs. Tests on real graphs with millions of vertices have been solved in but a few seconds.

    
EXTERNAL SOURCES
-------------------------------

Currently version GRAPH reads simple undirected unweighted graphs in ASCII DIMACS, Matrix Market Exchange and Edge list formats. A few examples of undirected graphs in different formats may be found in tha *data* folder of the block. To load a graph from file simply pass the appropiate filename to the constructor:

    
    graph g("foo-filename");				//loads "brock200_2.clq" file in DIMACS/MTX/EDGES in g
    
OTHERS
-------------------------------

Other basic functionality includes computing degree, density, comparing graphs etc. Examples can be found in the *tests* folder which uses google *gtest* framework imported from the Biicode Repository.


COMPATIBILITY
-------------------------------

GRAPH has been tested in Windows 64-bit OS and Linux OS. Please note that a 64-bit OS is mandatory becase of the BITSCAN dependency.

Acknowledgements
-------------------------------

This research has been partially funded by the Spanish Ministry of Economy and Competitiveness (MINECO), national grant DPI 2010-21247-C02-01.

