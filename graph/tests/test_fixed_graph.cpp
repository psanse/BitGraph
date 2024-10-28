//text_fixed_graph: tests for the fixed size graph class
//date of creation 21/03/16

#include <iostream>
#include "gtest/gtest.h"
#include "../graph.h"
#include "../simple_fixed_ugraph.h"

using namespace std;

TEST(UgraphF_test, basic_test){
///////////////////
// Undirected graphs read by directed graph class (all edges are non symmetrical)
	
	UgraphF<100> gf;									//empty graph with 100 vertices
	gf.add_edge(0,1);
	gf.add_edge(0,2);
	gf.add_edge(1,2);
	EXPECT_EQ(3,gf.number_of_edges());

	gf.create_empty_graph();
	EXPECT_EQ(0,gf.number_of_edges(false));				//non-lazy evaluation
	EXPECT_EQ(100,gf.number_of_vertices());
	
}

