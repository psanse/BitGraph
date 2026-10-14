/**
* @file graph_sort.cpp
* @brief Example of GRAPH, part of the BITGRAPH lib.
* @details created 01/2025, last updated 04/07/2025
**/


#include <iostream>
#include "BitGraph/graph/graph.h"							//header file for main types of GRAPH lib
#include "BitGraph/graph/algorithms/graph_fast_sort.h"		//sorting algorithms

using namespace std;
//using namespace bitgraph;								//alternatively, to avoid typing "bitgraph::" before each type


int main() {

	//small undirected graph with 10 vertices
	bitgraph::ugraph ug(5);

	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(3, 4);
	
	//A sorting object	
	bitgraph::GraphFastRootSort<bitgraph::ugraph> gs(ug);

	//A new ordering of the vertices - minimum degree first (degenerate), in [OLD]->[NEW] format
	//In a degenerate ordering, each vertex sorted is removed and the sorting criterion is computed for the remaining vertices
	using sorter_t = bitgraph::GraphFastRootSort<bitgraph::ugraph>;
	auto order = gs.new_order(sorter_t::strategy::min_degeneracy,
							  sorter_t::placement::first_to_last,
							  sorter_t::sort_order::old_to_new);

	//create an isomorphism of the original graph according to the new ordering
	bitgraph::ugraph ug_sorted = sorter_t::reorder(ug, order);


	//I/O
	cout << "***GRAPH SORTING - original graph *********" << endl;

	ug.print_data();
	ug.print_edges();

	cout << "***GRAPH SORTING - sorted graph *********" << endl;
	
	ug_sorted.print_data();
	ug_sorted.print_edges();
}
