/**
* @file graph.cpp
* @brief Example of how to link to GRAPH lib.
* @details created 01/2025, last updated 26/02/2025
**/
using namespace std;

#include <iostream>
#include "Bitgraph/graph/graph.h"		//the one and only header file for the GRAPH 1.0 lib

int main() {

	//small undirected graph with 10 vertices
	ugraph g(10);

	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.add_edge(7, 8);
	g.print_data();
	g.print_edges();	

	cout << "****************************" << endl;

	g.shrink_to_fit(5);		//graph induced by the first 5 vertices	
	g.print_data();
	g.print_edges();
	
	cout << "****************************" << endl;

	//massive graph with 500000 vertices
	sparse_ugraph sg(500000);

	sg.add_edge(0, 1);
	sg.add_edge(0, 2);
	sg.add_edge(499000, 499001);
	sg.print_data();
	sg.print_edges();	
	
	cout << "****************************" << endl;

	//unit weighted graph with 10 vertices and vertex weights set at  3.0
	ugraph_w gw(10, 3.0);	

	gw.add_edge(0, 1);
	gw.add_edge(0, 2);
	gw.print_data();
	gw.print_weights();
	gw.print_edges();

	cout << "****************************" << endl;

	//edge-weighted graph with 10 vertices, no edges and vertex weights set at 8.0
	ugraph_ew gew(10, 8.0);

	gew.add_edge(0, 1, 4.5);
	gew.add_edge(0, 2, 5.3);
	gew.print_data();
	gew.print_weights();


	return 0;
}