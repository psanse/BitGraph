/*
* test_graph_fast_sort_weighted.cpp Unit tests for Class GraphFastRootSort_W for vertex weighted graphs (graph_fast_sort_weighted.h)
* @created 
* @last_update 26/12/24
* @logger_level determined by CMake
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

//#define PATH_GFSW_TESTS_DATA	 PATH_GRAPH_TESTS_CMAKE_SRC_CODE		//fixed in CMakeLists.txt

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
	
//////////////////
//SORT BY DEGREE
//////////////////
	vint order= sorter.new_order((int)gt::ptype::sort_alg_t::MIN, false /*f2l*/, false /*n2o*/);
	
	//deg(5)=3,  deg(2)=3, deg(1)=2,  deg(3)=1, deg(4)=1, deg(0)=0
	vint order_exp;
	order_exp.push_back(0);
	order_exp.push_back(3);
	order_exp.push_back(4);
	order_exp.push_back(1);
	order_exp.push_back(2);
	order_exp.push_back(5);

	//////////////////
	//TEST
	EXPECT_EQ(order_exp, order);

	   	
#ifdef  print_graph_fast_sort_logs
	com::stl::print_collection(order, cout, true);
#endif

//////////////////
//SORT BY WEIGHT (non-increasing)
//////////////////
	//w(1)=5.0,  w(0)=4.0, w(2)=3.0,  w(3)=1.0, w(4)=1.0, w(5)=1.0
	order = sorter.new_order((int)gt::sort_algw_t::MAX_WEIGHT, false /*f2l*/, false /*n2o*/);
	order_exp.clear();
	order_exp.push_back(1);
	order_exp.push_back(0);
	order_exp.push_back(2);
	order_exp.push_back(3);
	order_exp.push_back(4);
	order_exp.push_back(5);

	//////////////////
	//TEST
	EXPECT_EQ(order_exp, order);

#ifdef  print_graph_fast_sort_logs
	com::stl::print_collection(order, cout, true);
#endif		  
	
////////////////////
//REORDER  (by non-increasing weight)
//////////////////
	ugraph_w ugw_sorted;
	sorter.reorder(order, ugw_sorted);

	//////////////////
	//TEST
	EXPECT_TRUE(5, ugw_sorted.get_w(0));
	EXPECT_TRUE(4, ugw_sorted.get_w(1));
	EXPECT_TRUE(3, ugw_sorted.get_w(2));
	EXPECT_TRUE(1, ugw_sorted.get_w(3));
	EXPECT_TRUE(1, ugw_sorted.get_w(4));
	EXPECT_TRUE(1, ugw_sorted.get_w(5));

#ifdef  print_graph_fast_sort_logs
	ugw_sorted.print_weights();
#endif


	LOG_INFO("GraphFastRootSort_W: END basic-------------------");
#ifdef	TEST_GFS_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}

