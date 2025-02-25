#include <iostream>
#include <graph/graph.h>

int main() {

	//non-sparse graph with 10 vertices
	ugraph g(10);
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.print_data();
	g.print_edges();
	
	cout << "****************************" << endl;

	//sparse_graph with 1M vertices
	sparse_ugraph sg(1000000);
	sg.add_edge(0, 1);
	sg.add_edge(0, 2);
	sg.print_data();
	sg.print_edges();

	cout << "****************************" << endl;


	return 0;
}