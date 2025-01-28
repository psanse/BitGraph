#include <iostream>
#include <graph/graph.h>

int main() {
	ugraph g(10);
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.print_data();
	g.print_edges();


	return 0;
}