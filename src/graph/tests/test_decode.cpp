/*
* @file test_decode.cpp
* @brief Unit tests for class Decode, which unwinds a composition of orderings
* @date 17/12/2024
* @last_update 27/01/25
* @comments based on previous tests in test_graph_sort.cpp
*/

#include <iostream>
#include "utils/common.h"
//#include "utils/result.h"
#include "gtest/gtest.h"
#include "graph/graph.h"
#include "graph/algorithms/graph_gen.h"
#include "graph/algorithms/graph_sort.h"
#include "graph/algorithms/graph_fast_sort.h"

using namespace std;
using namespace bitgraph;

TEST(Decode, GraphFastRootSort) {
	
	ugraph ug(106);
	ug.add_edge(1, 2);
	ug.add_edge(1, 3);
	ug.add_edge(1, 4);
	ug.add_edge(78, 5);	

	
	//computes ordering in format [OLD]->[NEW]
	using  gt = GraphFastRootSort<ugraph>;
	gt sorter(ug);											
	vint vres = sorter.new_order(static_cast<int>(gt::MIN), false /*ltf*/, true /*o2n*/);

	//I/O
	//sorter.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), std::cout);
	
	//initis decoder
	Decode::reverse_in_place(vres);			//format [NEW]->[OLD]
	Decode d;
	d.insert_ordering(vres);

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
	EXPECT_EQ(dec[1], 105);
	EXPECT_EQ(dec[2], 100);
	EXPECT_EQ(dec[3], 101);
	EXPECT_EQ(dec[4], 102);
	EXPECT_EQ(dec[5], 103);	
}
