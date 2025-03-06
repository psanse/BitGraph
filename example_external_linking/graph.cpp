/**
* @file graph.cpp
* @brief Example of how to link to GRAPH lib.
* @details created 01/2025, last updated 26/02/2025
**/

#include <iostream>
#include "graph/graph.h"		//the one and only header file for the GRAPH 1.0 lib

int main() {

	//small graph with 10 vertices
	ugraph g(10);
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.print_data();
	g.print_edges();
	
	cout << "****************************" << endl;

	//massive graph with 10 million vertices
	sparse_ugraph sg(10000000);
	sg.add_edge(0, 1);
	sg.add_edge(0, 2);
	sg.print_data();
	sg.print_edges();
	
	cout << "****************************" << endl;

	//make a copy of the massive graph
	sparse_ugraph sgCOPY(sg);
	sgCOPY.print_data();
	sgCOPY.print_edges();

	cout << "****************************" << endl;

	//weighted graph with 10 vertices
	ugraph_w gw(10);	
	gw.add_edge(0, 1);
	gw.add_edge(0, 2);
	gw.print_data();
	gw.print_edges();

	return 0;
}