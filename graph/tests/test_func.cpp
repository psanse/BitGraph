//test_func.cpp: tests for functions in graph_func.h (namespace gfunc)
//
// last_update@:23/01/2019 (adapted for the new Graph_W type) 


#include "../graph.h"
#include "../graph/algorithms/graph_func.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;
using vint = std::vector<int>;


//#define TEST_GRAPH_FUNC_STEP_BY_STEP
//#define print_graph_func_logs


TEST(Graph_func, is_triangleFree_subgraph) {
	
	const int NV = 5;
	ugraph ug(NV);
	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(0, 3);
	ug.add_edge(1, 3);
		
	vint tri;
	//////////////////
	//TEST	
	int tri_free = gfunc::is_triangleFree_subgraph(ug, ug.get_neighbors(0), tri);
	EXPECT_TRUE(tri_free);														//[1]-->[3]
	////////////////////

	//////////////////
	//TEST
	ug.add_edge(1, 2);
	ug.add_edge(1, 3);
	ug.add_edge(2, 3);
	EXPECT_FALSE(gfunc::is_triangleFree_subgraph(ug, ug.get_neighbors(0), tri));
	////////////////////

}

TEST(Graph_func, is_edgeFree_subgraph) {
	
	const int NV = 5;
	ugraph ug(NV);
	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(0, 3);
	ug.add_edge(1, 3);

	//////////////////
	//TEST
	vint edge;
	int edge_free = gfunc::is_edgeFree_subgraph(ug, ug.get_neighbors(0), edge);
	EXPECT_FALSE(edge_free);											//[1]-->[3]
	////////////////////

	//////////////////
	//TEST
	ug.remove_edge(1, 3);
	EXPECT_TRUE(gfunc::is_edgeFree_subgraph(ug, ug.get_neighbors(0), edge));
	////////////////////

}

TEST(Graph_func, sort_by_weights){
	
	using namespace gfunc::vertexW;	

	const int NV=5;
	Graph_W<ugraph, int> ugw(NV, 0.0);		//0.0 vertex weights (unit weights by default)
	ugw.add_edge(0,1);
	ugw.add_edge(1,2);
	ugw.add_edge(0,2);

	ugw.set_w(0,3);
	ugw.set_w(1,2);
	ugw.set_w(2,1);

	//I/O
	ugw.print_weights();

	//sort vector
	vint lv; lv.push_back(0); lv.push_back(1); lv.push_back(2);
	sort_w(ugw,lv, true);
	vint expected;
	expected.push_back(2); expected.push_back(1); expected.push_back(0);
	EXPECT_EQ(expected, lv);

	sort_w(ugw,lv, false);
	expected.clear();
	expected.push_back(0); expected.push_back(1); expected.push_back(2);
	EXPECT_EQ(expected, lv);

	//sort old_vector
	int lv_old[3];
	copy(lv.begin(), lv.end(), lv_old);
	sort_w(ugw,lv_old, 3, true);
	int expected_old[3];
	expected_old[0]=2; expected_old[1]=1; expected_old[2]=0;
	for(int i=0; i<3; i++){
		EXPECT_EQ(expected_old[i], lv_old[i]);
	}
	
}

TEST(Graph_func, sum_of_weights){

	using namespace gfunc::vertexW;
	
	const int NV=5;
	Graph_W<ugraph, int> ugw(NV, 0.0);		//0.0 vertex weights (unit weights by default)
	ugw.add_edge(0,1);
	ugw.add_edge(1,2);
	ugw.add_edge(0,2);

	ugw.set_w(0,1);
	ugw.set_w(1,2);
	ugw.set_w(2,3);

	//I/O
	ugw.print_weights();
	
	vint lv; lv.push_back(0); lv.push_back(1);
	int w = wsum(ugw,lv);
	EXPECT_EQ(3,w);

	bitarray bb(5);
	bb.set_bit(0);  bb.set_bit(1); 
	w=wsum(ugw,bb);
	EXPECT_EQ(3,w);

	bb.set_bit(2);
	w=wsum(ugw,bb);
	EXPECT_EQ(6,w);

}

TEST(Graph_func, neighbors){

	using namespace gfunc;
	
	const int NV=100;
	ugraph ug(NV);
	ug.add_edge(0,1);
	ug.add_edge(0,2);
	ug.add_edge(0,63);
	ug.add_edge(0,64);
	ug.add_edge(0,65);
	ug.add_edge(3,0);
	ug.add_edge(3,63);
	ug.add_edge(3,64);
	ug.add_edge(63,64);
	
	bitarray bbV(100);
	bbV.set_bit(0,NV-1); 
	vint res;
		
	//neighbors
	neighbors<ugraph>(ug, 3, bbV, res);
	vint expected;
	expected.push_back(0); expected.push_back(63); expected.push_back(64);
	EXPECT_EQ(expected, res);
//	com::stl::print_collection(res,cout, true);

	//neighbors_after 
	neighbors_after<ugraph>(ug, 3, bbV, res);
	expected.clear();
	expected.push_back(63); expected.push_back(64);
	EXPECT_EQ(expected, res);
//	com::stl::print_collection(res,cout, true);

	//neighbors_after
	neighbors_after<ugraph>(ug, 63, bbV, res);
	expected.clear(); expected.push_back(64); 
	EXPECT_EQ(expected, res);
//	com::stl::print_collection(res,cout, true);

	//neighbors_after
	neighbors_after<ugraph>(ug, 0, bbV, res);
	expected.clear(); expected.push_back(1); expected.push_back(2); expected.push_back(3); expected.push_back(63); expected.push_back(64);  expected.push_back(65); 
	EXPECT_EQ(expected, res);
//	com::stl::print_collection(res,cout, true);
		
}


TEST(Graph_func, sort){
//////////////
// date: 24/10/17

	using namespace ::gfunc::sort;

	ugraph ug(5);
	ug.add_edge(0,1);
	ug.add_edge(1,2);
	ug.add_edge(0,2);
	ug.add_edge(0,3);

	vector<int> lv;
	lv.push_back(0);
	lv.push_back(1);
	vector<int> lref;
	lref.push_back(2);
	lref.push_back(3);
	lref.push_back(4);

	sort_deg(ug,lv,lref,false);	
	EXPECT_EQ(0,lv.front());

	lv.clear();
	lv.push_back(0);
	lv.push_back(1);
	sort_deg(ug,lv,lref,true);
	EXPECT_EQ(1,lv.front());
	
	bitarray bbref(5);
	bbref.set_bit(2);
	bbref.set_bit(3);
	bbref.set_bit(4);
	sort_deg(ug,lv,bbref,true);
	EXPECT_EQ(1,lv.front());

	//empty ref test
	lref.clear();
	sort_deg(ug,lv,lref,false);	
	EXPECT_EQ(1,lv.front());

	bbref.erase_bit();
	sort_deg(ug,lv,bbref,false);	
	EXPECT_EQ(1,lv.front());
		

}


