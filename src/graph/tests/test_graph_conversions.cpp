/**
* @file test_graph_conversions.cpp  
* @brief tests for class GraphConversion to convert between different graph types
* @created 04/01/2025 (from older tests)
* @last_update 27/01/2025
* @author pss
* 
**/

#include "graph/algorithms/graph_conversions.h"			
#include "gtest/gtest.h"
#include <iostream>

using namespace std;
using namespace bitgraph;


TEST(Conversions, sparse2ugraph) {
	
	sparse_ugraph sug(4);
	sug.add_edge(0, 1);
	sug.add_edge(1, 2);
	sug.add_edge(1, 3);
	sug.add_edge(0, 2);

	ugraph ug;
	GraphConversion::sug2ug(sug, ug);

	EXPECT_EQ(4, ug.number_of_vertices());
	EXPECT_EQ(4, ug.number_of_edges());
	EXPECT_TRUE(ug.is_edge(0, 1));
	EXPECT_TRUE(ug.is_edge(1, 2));
	EXPECT_TRUE(ug.is_edge(1, 3));
	EXPECT_TRUE(ug.is_edge(0, 2));

	sparse_ugraph sug1(300);
	sug1.add_edge(0, 1);
	sug1.add_edge(1, 2);
	sug1.add_edge(1, 3);
	sug1.add_edge(0, 2);

	GraphConversion::sug2ug(sug1, ug);
	EXPECT_EQ(300, ug.number_of_vertices());
	EXPECT_EQ(4, ug.number_of_edges());
	EXPECT_TRUE(ug.is_edge(0, 1));
	EXPECT_TRUE(ug.is_edge(1, 2));
	EXPECT_TRUE(ug.is_edge(1, 3));
	EXPECT_TRUE(ug.is_edge(0, 2));

}

TEST(Conversions, ugraph2sparse_ugraph) {

	ugraph ug(4);
	ug.add_edge(0, 1);
	ug.add_edge(1, 2);
	ug.add_edge(1, 3);
	ug.add_edge(0, 2);

	sparse_ugraph sug;
	GraphConversion::ug2sug(ug, sug);

	EXPECT_EQ(4, sug.number_of_vertices());
	EXPECT_EQ(4, sug.number_of_edges());
	EXPECT_TRUE(sug.is_edge(0, 1));
	EXPECT_TRUE(sug.is_edge(1, 2));
	EXPECT_TRUE(sug.is_edge(1, 3));
	EXPECT_TRUE(sug.is_edge(0, 2));

	ugraph ug1(300);
	ug1.add_edge(0, 1);
	ug1.add_edge(1, 2);
	ug1.add_edge(1, 3);
	ug1.add_edge(0, 2);

	GraphConversion::ug2sug(ug1, sug);
	EXPECT_EQ(300, sug.number_of_vertices());
	EXPECT_EQ(4, sug.number_of_edges());
	EXPECT_TRUE(sug.is_edge(0, 1));
	EXPECT_TRUE(sug.is_edge(1, 2));
	EXPECT_TRUE(sug.is_edge(1, 3));
	EXPECT_TRUE(sug.is_edge(0, 2));

}

