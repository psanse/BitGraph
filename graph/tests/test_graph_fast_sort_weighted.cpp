/*
* test_graph_fast_sort_weighted.cpp Unit tests for Class GraphFastRootSort_W for vertex weighted graphs (graph_fast_sort_weighted.h)
* @created 
* @last_update 26/12/24
* TODO - CHECK AND REFACTOR TESTS (26/12/24)	
*/ 

#include "../algorithms/graph_fast_sort.h"
#include "../algorithms/graph_fast_sort_weighted.h"
#include "../graph.h"
#include "gtest/gtest.h"
#include <iostream>
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/file.h"
#include <string>

using namespace std;

class GraphFastRootSortWeightedTest : public ::testing::Test {
protected:
	void SetUp() override {
		ugw.init(NV);
		ugw.add_edge(1, 2);
		ugw.add_edge(1, 5);
		ugw.add_edge(2, 4);
		ugw.add_edge(2, 5);
		ugw.add_edge(3, 5);		

		ugw.set_w(0, 4.0);
		ugw.set_w(1, 5.0);
		ugw.set_w(2, 3.0);

		//rest of vertices have default weight 1.0
	}
	
	//ugraph instance	
	//deg(5)=3,  deg(2)=3, deg(1)=2,  deg(3)=1, deg(4)=1, deg(0)=0
	const int NV = 6;
	ugraph_w ugw;
};

TEST_F(GraphFastRootSortWeightedTest, constructor) {
	using gt = GraphFastRootSort_W<ugraph_w>;
	gt sorter(ugw);

	//unweighted part
	auto gw = sorter.get_graph();
	EXPECT_EQ(sorter.number_of_vertices(), gw.number_of_vertices());
	EXPECT_EQ(NV, gw.number_of_vertices());

	//weighted part
	EXPECT_DOUBLE_EQ(4.0, gw.get_w(0));
	EXPECT_DOUBLE_EQ(5.0, gw.get_w(1));
	EXPECT_DOUBLE_EQ(3.0, gw.get_w(2));
}

TEST_F(GraphFastRootSortWeightedTest, new_order) {
		
	using gt = GraphFastRootSort_W<ugraph_w>;
	gt sorter(ugw);	

	//sort by min degree (non-decreasing)
	vint mapping_deg= sorter.new_order(static_cast<int>(gt::ptype::sort_alg_t::MIN), false /*f2l*/, false /*n2o*/);
	
	////////////////////////////////////////////
	vint mapping_deg_exp = { 0, 3, 4, 1, 2, 5 };
	EXPECT_EQ(mapping_deg, mapping_deg_exp);
	////////////////////////////////////////////

	//order_exp.push_back(0);
	//order_exp.push_back(3);
	//order_exp.push_back(4);
	//order_exp.push_back(1);
	//order_exp.push_back(2);
	//order_exp.push_back(5);
		   	
	//I/O
	//cout << "absolute min degree ordering with format:  first-to-last, new-to-old" << endl;
	//sorter.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);					//Equivalent to com::stl::print_collection(mapping, cout, true);
	
	//sort according to non-increasing weight
	vint mapping_weight = sorter.new_order(static_cast<int>(gt::sort_algw_t::MAX_WEIGHT), false /*f2l*/, false /*n2o*/);

	////////////////////////////////////////////
	vint mapping_weight_exp = { 1, 0, 2, 3, 4, 5 };				//w(1)=5.0,  w(0)=4.0, w(2)=3.0,  w(3)=1.0, w(4)=1.0, w(5)=1.0
	EXPECT_EQ(mapping_weight, mapping_weight_exp);
	///////////////////////////////////////////

	//I/O
	/*cout << "absolute non-incresing weight with format:  first-to-last, new-to-old" << endl;
	com::stl::print_collection(mapping_weight, cout, true);*/
	
	////////////////////
	//creating an isomorphism  (by non-increasing weight)

	ugraph_w ugw_sorted;
	sorter.reorder(mapping_weight, ugw_sorted);					//mapping_weight is in new-to-old format

	////////////////////////////////////////////
	EXPECT_DOUBLE_EQ(5, ugw_sorted.get_w(0));					//w(1)=5.0,  w(0)=4.0, w(2)=3.0,  w(3)=1.0, w(4)=1.0, w(5)=1.0
	EXPECT_DOUBLE_EQ(4, ugw_sorted.get_w(1));
	EXPECT_DOUBLE_EQ(3, ugw_sorted.get_w(2));
	EXPECT_DOUBLE_EQ(1, ugw_sorted.get_w(3));
	EXPECT_DOUBLE_EQ(1, ugw_sorted.get_w(4));
	EXPECT_DOUBLE_EQ(1, ugw_sorted.get_w(5));
	////////////////////////////////////////////

	//I/O
	//ugw_sorted.print_weights();
}

