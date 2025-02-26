#include <iostream>
#include <graph/graph.h>

int main() {

	//small graph with 10 vertices
	ugraph g(10);
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.print_data();
	g.print_edges();
	
	cout << "****************************" << endl;

	//massive graph with 50 million vertices
	sparse_ugraph sg(50000000);
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

	return 0;
}