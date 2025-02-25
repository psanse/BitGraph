#include <iostream>
#include <graph/graph.h>

int main() {

	//non-sparse graph with 10 vertices
	ugraph g(10);
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.print_data();
	
	
	cout << "****************************" << endl;

	//sparse_graph with 10 million vertices
	sparse_ugraph sg(10000000);
	sg.add_edge(0, 1);
	sg.add_edge(0, 2);
	sg.print_data();
	

	cout << "****************************" << endl;


	return 0;
}