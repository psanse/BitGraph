/**
* @file graph_sort.cpp
* @brief GRAPH sorting example - linking to GRAPH lib.
* @details created 01/2025, last updated 02/05/2025
**/
using namespace std;

#include <iostream>
#include "BitGraph/graph/graph.h"							//header file for main types of GRAPH lib
#include "BitGraph/graph/algorithms/graph_fast_sort.h"		//sorting algorithms


int main() {

	//small undirected graph with 10 vertices
	bitgraph::ugraph ug(5);

	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(3, 4);
	
	//A sorting object	
	bitgraph::GraphFastRootSort<bitgraph::ugraph> gs(ug);

	//A new ordering of the vertices - minimum degree first (degenerate) {1->0, 0->1, 2->2, 3->3, 4->4}	
	//In a degenerate ordering, each vertex sorted is removed and the sorting criterion is computed for the remaining vertices
	auto order = gs.new_order(bitgraph::GraphFastRootSort<bitgraph::ugraph>::MIN_DEGEN, false);
	
	//create an isomorphism of the original graph according to the new ordering
	bitgraph::ugraph ug_sorted;
	gs.reorder(order, ug_sorted);


	//I/O
	cout << "***GRAPH SORTING - original graph *********" << endl;

	ug.print_data();
	ug.print_edges();

	cout << "***GRAPH SORTING - sorted graph *********" << endl;
	
	ug_sorted.print_data();
	ug_sorted.print_edges();
}
