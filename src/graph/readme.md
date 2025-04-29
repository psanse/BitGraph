
The GRAPH library
===================

GRAPH is a C++ library for graphs. At its core, GRAPH relies on the BITSCAN library (located in the bitscan folder of the repository). GRAPH uses BITSCAN to efficiently represent graphs in memory and to optimize fundamental operations such as, e.g., computing vertex degrees, sorting vertices, and building graph isomorphisms.

Main Features
-----------------------------
Among the features of GRAPH we highlight the following:
* Efficient representation of graphs in memory using bitsets.
* Optimised graph operations using bitmasks
* Specialised types for sparse graphs, vertex-weighted and edge-weighted graphs.
* Specialised operations for graph generation (uniform random graphs, sparse graphs)
* Specialised operations for vertex sorting, k-core analysis and others. 

Getting Started
-----------------------------
The library currently offers a variety of graph types, the most relevant ones being:
* `graph`: for directed graphs
* `ugraph`: for undirected graphs
* `sparse_graph`: for sparse directed graphs
* `sparse_ugraph`: for sparse undirected graphs
* `ugraph_w`: for undirected vertex-weighted graphs
* `ugraph_ew`: for undirected edge-weighted graphs
 
To include the GRAPH library in your project, add the header #include "BitGraph/graph/graph.h". In the following example, a simple undirected graphs with 10 vertices and 3 edges is declared.

``` plaintext
#include "BitGraph/graph/graph.h"
//...
ugraph g(10);

ug.add_edge(0, 1);
ug.add_edge(0, 2);
ug.add_edge(7, 8);
ug.print_data();		//screen output: n:=10 m:=3 p:=0.067
```
Many more examples are provided in the [tests](https://github.com/psanse/BitGraph/tree/master/src/graph/tests) folder. A basic example is shown in the [install](https://github.com/psanse/BitGraph/blob/master/install/src/graph.cpp) folder.

Declaring graphs
-------------------------------
The following example creates a directed graph with 100 vertices and 3 directed edges, and then makes the edges bidirected:
``` plaintext
//...
    
graph g(100);					
g.add_edge(0,1);				
g.add_edge(0,2);
g.add_edge(1,2);

g.make_bidirected()		//creates a 3-clique
```   

A better option for cliques would be to use the type `ugraph` for undirected graphs. Here is the code to define the 3-clique {0, 2, 3} in a graph with 100 vertices:

``` plaintext   
ugraph ug(100);					
ug.add_edge(0,1);				
ug.add_edge(0,2);
ug.add_edge(1,2);
```
For massive graphs, the specialised type `sparse_graph` should be used. In the following example, a graph with 1M vertices and 3 edges is declared.

``` plaintext   
sparse_ugraph sg(1000000);					
sg.add_edge(0,1);				
sg.add_edge(0,2);
sg.add_edge(1,2);
```

Working with graphs
-------------------------------

Edges may be added or deleted in constant time (*remove\_edge* member function) for small and middle-size graphs. In the case of large and massive graphs they are logarithmic operations.
Graphs may also be copied and assigned, and special generation functions are available for special graphs, i.e., uniform random, cycles, cliques, stars and others. 
In the following example, a 10-cycle is created and then reduced to the induced graph by its first 5 vertices:

``` plaintext      
auto cycle = ugraph::make_cycle(10);				 
cycle.shrink_to_fit(5);			//subgraph induced by the first 5 vertices of the 10-cycle
cycle.print_data();			//screen output n:=5 m:=5 p:=0.5
```		
   
External sources
-------------------------------

GRAPH supports DIMACS, Matrix Market Exchange and Edge list text formats. To load a graph from a file just give the name of the file in the declaration:
``` plaintext  
graph g("brock200_1.clq");				
```
    
Others
-------------------------------

An extensive documentation of GRAPH is under development.


