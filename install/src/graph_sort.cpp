/**
* @file graph_sort.cpp
* @brief GRAPH sorting example - linking to GRAPH lib.
* @details created 01/2025, last updated 29/04/2025
**/
using namespace std;

#include <iostream>
#include "BitGraph/graph/graph.h"							//header file for main types of GRAPH lib
#include "BitGraph/graph/algorithms/graph_fast_sort.h"		//sorting 


int main() {

	//small undirected graph with 10 vertices
	ugraph ug(10);

	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(7, 8);
	

	//creates an isomorphism with vertices sorted according to (non-decreasing) degenerate minimum degree
	ugraph ug_sorted;
	GraphFastRootSort<ugraph> gs(ug);
	gs.reorder(gs.new_order(GraphFastRootSort<ugraph>::MIN_DEGEN, false), ug_sorted);


	//I/O
	cout << "***GRAPH SORTING - original graph *********" << endl;

	ug.print_data();
	ug.print_edges();

	cout << "***GRAPH SORTING - sorted graph *********" << endl;
	
	ug_sorted.print_data();
	ug_sorted.print_edges();
}