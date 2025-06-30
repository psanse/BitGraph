/*
* @file test_graph_fast_sort_weighted.cpp 
* @biref Unit tests for Class GraphFastRootSort_W for vertex-weighted graphs (header graph_fast_sort_weighted.h)
* @created ?
* @last_update 27/01/25
*/ 

#include "graph/algorithms/graph_fast_sort.h"
#include "graph/algorithms/graph_fast_sort_weighted.h"
#include "graph/simple_graph_w.h"							//MUST BE AFTER ugraph type 
#include "gtest/gtest.h"
#include <iostream>
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/file.h"
#include <string>



//useful alias
using graph = bitgraph::Graph<bitarray>;					//simple graph
using ugraph = bitgraph::Ugraph<bitarray>;					//simple undirected graph
using ugraph_w = bitgraph::Graph_W < ugraph, double>;		//simple vertex weighted graph with double weights
using ugraph_wi = bitgraph::Graph_W < ugraph, int>;			//simple vertex weighted graph with integer weights

using namespace std;

class GraphFastRootSortWeightedTest : public ::testing::Test {
protected:
	void SetUp() override {
		ugw.reset(NV);
		ugw.add_edge(1, 2);
		ugw.add_edge(1, 5);
		ugw.add_edge(2, 4);
		ugw.add_edge(2, 5);
		ugw.add_edge(3, 5);		

		ugw.set_weight(0, 4.0);
		ugw.set_weight(1, 5.0);
		ugw.set_weight(2, 3.0);

		//rest of vertices have default weight 1.0
	}

	void TearDown() override {}
	
	//ugraph instance	
	//deg(5)=3,  deg(2)=3, deg(1)=2,  deg(3)=1, deg(4)=1, deg(0)=0
	const int NV = 6;
	ugraph_w ugw;
};

TEST_F(GraphFastRootSortWeightedTest, constructor) {
	using gt = GraphFastRootSort_W<ugraph_w>;
	gt sorter(ugw);

	//test unweighted part of the graph
	auto gw = sorter.graph();
	////////////////////////////////////////////
	EXPECT_EQ(sorter.number_of_vertices(), gw.number_of_vertices());
	EXPECT_EQ(NV, gw.number_of_vertices());
	////////////////////////////////////////////

	//test weights
	////////////////////////////////////////////
	EXPECT_DOUBLE_EQ(4.0, gw.weight(0));
	EXPECT_DOUBLE_EQ(5.0, gw.weight(1));
	EXPECT_DOUBLE_EQ(3.0, gw.weight(2));
	EXPECT_DOUBLE_EQ(1.0, gw.weight(3));
	EXPECT_DOUBLE_EQ(1.0, gw.weight(4));
	EXPECT_DOUBLE_EQ(1.0, gw.weight(5));
	////////////////////////////////////////////
}

TEST_F(GraphFastRootSortWeightedTest, new_order) {
		
	using gt = GraphFastRootSort_W<ugraph_w>;
	gt sorter(ugw);	

	//sort by min degree (non-decreasing)
	vint mapping_deg= sorter.new_order(gt::ptype::MIN, false /*f2l*/, true /*o2n*/);
	
	////////////////////////////////////////////
	vint mapping_deg_exp = { 0, 3, 4, 1, 2, 5 };
	EXPECT_EQ(mapping_deg, mapping_deg_exp);
	////////////////////////////////////////////
		   	
	//I/O
	//cout << "absolute min degree ordering with format:  first-to-last, new-to-old" << endl;
	//sorter.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);					
	
	//sort according to non-increasing weight
	vint mapping_dec_weight = sorter.new_order(gt::MAX_WEIGHT, false /*f2l*/, true /*o2n*/);

	////////////////////////////////////////////
	vint mapping_dec_weight_exp = { 1, 0, 2, 3, 4, 5 };				//w(1)=5.0,  w(0)=4.0, w(2)=3.0,  w(3)=1.0, w(4)=1.0, w(5)=1.0
	EXPECT_EQ(mapping_dec_weight, mapping_dec_weight_exp);
	///////////////////////////////////////////

	//sort according to non-decreasing weight
	vint mapping_inc_weight = sorter.new_order(gt::MIN_WEIGHT, false /*f2l*/, true /*o2n*/);

	////////////////////////////////////////////
	vint mapping_inc_weight_exp = { 3, 4, 5, 2, 0, 1 };				//w(3)=1.0,  w(4)=1.0, w(5)=1.0,  w(2)=3.0, w(0)=4.0, w(1)=5.0
	EXPECT_EQ(mapping_inc_weight, mapping_inc_weight_exp);
	///////////////////////////////////////////


	//I/O
	/*cout << "absolute non-incresing weight with format:  first-to-last, old-to-new" << endl;
	com::stl::print_collection(mapping_weight, cout, true);*/
		
}

TEST_F(GraphFastRootSortWeightedTest, reorder) {

	using gt = GraphFastRootSort_W<ugraph_w>;
	gt sorter(ugw);
	vint mapping_weight = sorter.new_order(gt::MAX_WEIGHT, false /*f2l*/, true /*o2n*/);

	
	////////////////////
	//creating an isomorphism  (by non-increasing weight)

	ugraph_w ugw_sorted;
	sorter.reorder(mapping_weight, ugw_sorted);						//mapping_weight MUST BE in old-to-new format

	////////////////////////////////////////////
	EXPECT_DOUBLE_EQ(5, ugw_sorted.weight(0));						//w(1)=5.0,  w(0)=4.0, w(2)=3.0,  w(3)=1.0, w(4)=1.0, w(5)=1.0
	EXPECT_DOUBLE_EQ(4, ugw_sorted.weight(1));
	EXPECT_DOUBLE_EQ(3, ugw_sorted.weight(2));
	EXPECT_DOUBLE_EQ(1, ugw_sorted.weight(3));
	EXPECT_DOUBLE_EQ(1, ugw_sorted.weight(4));
	EXPECT_DOUBLE_EQ(1, ugw_sorted.weight(5));
	////////////////////////////////////////////

	//I/O
	//ugw_sorted.print_weights();
}

