/*
* @file test_decode.cpp
* @brief Unit tests for class Decode, which unwinds a composition of orderings
* @date 17/12/2024
* @last_update 27/01/25
* @comments based on previous tests in test_graph_sort.cpp
*/

#include <iostream>
#include "gtest/gtest.h"
#include "graph/algorithms/decode.h"
#include "utils/common.h"
#include "graph/graph.h"
#include "graph/algorithms/graph_fast_sort.h"

using namespace std;
using namespace bitgraph;

TEST(Decode, decodeList) {
	
	ugraph ug(106);
	ug.add_edge(1, 2);
	ug.add_edge(1, 3);
	ug.add_edge(1, 4);
	ug.add_edge(78, 5);	
		
	//computes ordering in format [OLD]->[NEW]
	using  gt = GraphFastRootSort<ugraph>;
	gt sorter(ug);											
	vint vres = sorter.new_order(static_cast<int>(gt::MIN), false /*ltf*/, true /*old to new */);		

	//I/O
	sorter.print(static_cast<int>(gt::PRINT_NODES), std::cout);
	
	//initis decoder
	Decode::reverse_in_place(vres);			//format [OLD]->[NEW]		
	Decode d;
	d.add_ordering(vres);					//first 5 elem of vres {0 6 7 8 9 10}

	//computes the vertex number of a list of vertices of the graph isomorphism in the original graph ug
	vint vlist;
	vlist.push_back(0);
	vlist.push_back(1);
	vlist.push_back(2);
	vlist.push_back(3);
	vlist.push_back(4);
	vlist.push_back(5);

	vint dec = d.decode(vlist);
	EXPECT_EQ(dec[0], 0);
	EXPECT_EQ(dec[1], 6);
	EXPECT_EQ(dec[2], 7);
	EXPECT_EQ(dec[3], 8);
	EXPECT_EQ(dec[4], 9);
	EXPECT_EQ(dec[5], 10);	


}
