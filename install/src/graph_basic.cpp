/**
* @file graph_basic.cpp
* @brief GRAPH basics - linking to GRAPH lib.
* @details created 01/2025, last updated 02/05/2025
**/


#include <iostream>
#include "BitGraph/graph/graph.h"		//header file for main types of GRAPH lib


using namespace std;

int main() {

	//small undirected graph with 10 vertices
	bitgraph::ugraph ug(10);

	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(7, 8);
	ug.print_data();
	ug.print_edges();	

	cout << "****************************" << endl;

	ug.shrink_to_fit(5);		//graph induced by the first 5 vertices	
	ug.print_data();
	ug.print_edges();
	
	cout << "****************************" << endl;

	//massive graph with 500000 vertices
	bitgraph::sparse_ugraph sg(500000);

	sg.add_edge(0, 1);
	sg.add_edge(0, 2);
	sg.add_edge(499000, 499001);
	sg.print_data();
	sg.print_edges();	
	
	cout << "****************************" << endl;

	//unit weighted graph with 10 vertices and vertex weights set at  3.0
	bitgraph::ugraph_w ugw(10, 3.0);

	ugw.add_edge(0, 1);
	ugw.add_edge(0, 2);
	ugw.print_data();
	ugw.print_weights();
	ugw.print_edges();

	cout << "****************************" << endl;

	//edge-weighted graph with 10 vertices, no edges and vertex weights set at 8.0
	bitgraph::ugraph_ew ugew(10, 8.0);

	ugew.add_edge(0, 1, 4.5);
	ugew.add_edge(0, 2, 5.3);
	ugew.print_data();
	ugew.print_weights();

	cout << "****************************" << endl;

	//directed graphs 
	bitgraph::graph g(100);
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.add_edge(1, 2);

	g.make_bidirected();	//makes all edges bidrected


	//editing
	bitgraph::ugraph ug1(100);
	ug1.add_edge(0, 1);
	ug1.add_edge(0, 2);
	ug1.add_edge(1, 2);

	bitgraph::ugraph ug2(ug1);
	ug2.add_edge(0, 3);
	ug2.remove_edge(0, 2);
	ug2.print_data();

	cout << "****************************" << endl;


	//generation
	auto cycle = bitgraph::ugraph::make_cycle(10);				//creates a 10-cycle 
	cycle.shrink_to_fit(5);								//subgraph induced by the first 5 vertices of the 10-cycle
	cycle.print_data();


}