/*
* @file test_func.cpp 
* @brief Unit tests for namespace gfunc functions
* @created 20/7/18
* @update Graph_W type for weighted graphs 23/01/19
* @last_update 27/01/25
* @author pss
*
* TODO - ADD TESTS... (09/01/25)
*/

#include "graph/algorithms/graph_func.h"
#include "gtest/gtest.h"
#include <iostream>


using namespace std;
using vint = std::vector<int>;

class GFuncTest : public ::testing::Test {
protected:
	void SetUp() override {
		ug.reset(NV);
		ug.add_edge(0, 1);
		ug.add_edge(0, 2);
		ug.add_edge(0, 3);
		ug.add_edge(1, 3);
		ug.name("toy");
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 5;
	ugraph ug;											//undirected graph with integer weights
};

class GFuncTestW : public ::testing::Test {
protected:
	void SetUp() override {
		ugw.reset(NV, ugraph_wi::NO_WEIGHT);
		ugw.add_edge(0, 1);
		ugw.add_edge(1, 2);
		ugw.add_edge(0, 2);

		ugw.set_weight(0, 3);
		ugw.set_weight(1, 2);
		ugw.set_weight(2, 1);
		ugw.name("ugraph_VW");
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 5;
	ugraph_wi ugw;											//undirected graph with integer weights
};


TEST_F(GFuncTest, is_triangleFree_subgraph) {
		
	vint triangle;	
	EXPECT_TRUE(gfunc::is_triangleFree_subgraph(ug, ug.neighbors(0), triangle));			

	//adds a triangle
	ug.add_edge(1, 2);
	ug.add_edge(2, 3);
	EXPECT_FALSE(gfunc::is_triangleFree_subgraph(ug, ug.neighbors(0), triangle));
}

TEST_F(GFuncTest, is_edgeFree_subgraph) {
		
	vint edge;
	EXPECT_FALSE(gfunc::is_edgeFree_subgraph(ug, ug.neighbors(0), edge) );				//{1, 3} is in G[{1, 2, 3}]					
	
	//removes the only edge 
	ug.remove_edge(1, 3);
	EXPECT_TRUE(gfunc::is_edgeFree_subgraph(ug, ug.neighbors(0), edge));

}

TEST_F(GFuncTestW, sort_w){
	
	using namespace gfunc::vertexW;	

	//set of vertices
	vint lv; 
	lv.push_back(0);
	lv.push_back(1); 
	lv.push_back(2);

	//sorts the set of vertices according to non-decreasing weight
	sort_w(ugw,lv, true);

	vint lv_exp;		
	lv_exp.push_back(2);
	lv_exp.push_back(1);
	lv_exp.push_back(0);
	EXPECT_EQ(lv_exp, lv);				//{w(2)=1, w(1)=2, w(0)=3

	//sorts the set of vertices according to non-increasing weight
	sort_w(ugw,lv, false);

	lv_exp.clear();
	lv_exp.push_back(0);
	lv_exp.push_back(1);
	lv_exp.push_back(2);
	EXPECT_EQ(lv_exp, lv);

	//sort C-style array
	int lv_C[3];
	copy(lv.begin(), lv.end(), lv_C);

	//sorts the set of vertices according to non-decreasing weight
	sort_w(ugw, lv_C, 3, true);
	
	int lv_Cexp[3];
	lv_Cexp[0]=2; 
	lv_Cexp[1]=1;
	lv_Cexp[2]=0;

	for(int i = 0; i < 3; i++){
		EXPECT_EQ(lv_Cexp[i], lv_C[i]);
	}

}

TEST_F(GFuncTestW, wsum){

	using namespace gfunc::vertexW;
	
	//set of vertices {0, 1}
	vint lv; 
	lv.push_back(0);
	lv.push_back(1);

	//sums the weights of the vertices in lv ={0, 1}
	int w = wsum(ugw,lv);
	EXPECT_EQ(5,w);
		
	bitarray bblv(5);
	bblv.set_bit(0);
	bblv.set_bit(1);

	//sums the weights of the (bit) set of vertices bblv = {0, 1}
	w = wsum (ugw, bblv);
	EXPECT_EQ(5,w);

	//sums the weights of the (bit) set of vertices bblv = {0, 1, 2}
	bblv.set_bit(2);
	w = wsum (ugw, bblv);
	EXPECT_EQ(6,w);

}

TEST(GFunc, neighbors){

	using namespace gfunc;
	
	const int NV = 100;

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
	
	//(bit) set of vertices {0, ..., 99}
	bitarray bbV (100);
	bbV.set_bit(0, NV - 1); 
	
	/////////////
	//neighbors of vertex {3} in bitset bbV = {0, 63, 64}
	vint neigh;
	neighbors<ugraph>(ug, 3, bbV, neigh);

	//expected neighbors
	vint neigh_exp;
	neigh_exp.push_back(0);
	neigh_exp.push_back(63);
	neigh_exp.push_back(64);
	EXPECT_EQ(neigh_exp, neigh);

	//////////////
	//neighbors of {3} after {3} in bitset bbV = {63, 64}
	neighbors_after<ugraph>(ug, 3, bbV, neigh);

	//expected neighbors
	neigh_exp.clear();
	neigh_exp.push_back(63);
	neigh_exp.push_back(64);
	EXPECT_EQ(neigh_exp, neigh);

	//////////////
	//neighbors of {63} after {63} in bitset bbV = {64}
	neighbors_after<ugraph>(ug, 63, bbV, neigh);

	//expected neighbors
	neigh_exp.clear(); 
	neigh_exp.push_back(64);
	EXPECT_EQ(neigh_exp, neigh);

	//////////////
	//neighbors of {0} after {0} in bitset bbV = {1, 2, 3, 63, 64, 65}
	neighbors_after<ugraph>(ug, 0, bbV, neigh);

	//expected neighbors
	neigh_exp.clear(); 
	neigh_exp.push_back(1); 
	neigh_exp.push_back(2); 
	neigh_exp.push_back(3); 
	neigh_exp.push_back(63); 
	neigh_exp.push_back(64);  
	neigh_exp.push_back(65);
	EXPECT_EQ(neigh_exp, neigh);
		
}

// date: 24/10/17
TEST(GFunc, sort){				

	using namespace ::gfunc::sort;

	const int NV = 5;

	//defines undirected graph
	ugraph ug(NV);
	ug.add_edge(0,1);
	ug.add_edge(1,2);
	ug.add_edge(0,2);
	ug.add_edge(0,3);

	//set of vertices
	vector<int> lv;
	lv.push_back(0);
	lv.push_back(1);

	//set of vertices
	vector<int> lref;
	lref.push_back(2);
	lref.push_back(3);
	lref.push_back(4);

	//sorts lv ={0, 1} according to non-increasing degree in lv_ref {2, 3, 4}
	//lv = {0 (2), 1 (1)}
	sort_deg (ug, lv, lref, false);	

	////////////////////////
	EXPECT_EQ(0, lv[0]);
	EXPECT_EQ(1, lv[1]);
	EXPECT_EQ(2, lv.size());
	////////////////////////
		
	lv.clear();
	lv.push_back(0);
	lv.push_back(1);

	//sorts lv={0, 1} according to non-decreasing degree in lv_ref {2, 3, 4}
	sort_deg(ug,lv,lref,true);		//lv = { 1 (1), 0 (2)}
	
	////////////////////////
	EXPECT_EQ(1, lv[0]);
	EXPECT_EQ(0, lv[1]);
	EXPECT_EQ(2, lv.size());
	////////////////////////
	
	bitarray bbref(5);
	bbref.set_bit(2);
	bbref.set_bit(3);
	bbref.set_bit(4);

	//sorts lv={1, 0} according to non-decreasing degree in bbref {2, 3, 4}
	sort_deg(ug,lv,bbref,true);

	////////////////////////
	EXPECT_EQ(1, lv[0]);
	EXPECT_EQ(0, lv[1]);
	EXPECT_EQ(2, lv.size());
	////////////////////////

	//empty reference set of vertices - lv unchanged
	//lv = { 1, 0 }
	lref.clear();

	//sorts with empty reference set of vertices - lv unchanged = {1, 0}
	sort_deg(ug, lv, lref, false);	

	////////////////////////
	EXPECT_EQ(1, lv[0]);
	EXPECT_EQ(0, lv[1]);
	EXPECT_EQ(2, lv.size());
	////////////////////////

	//empty reference (bit)set of vertices 
	//lv = { 1, 0 }
	bbref.erase_bit();

	//sorts with empty reference set of vertices - lv unchanged = {1, 0}
	sort_deg(ug,lv,bbref,false);

	////////////////////////
	EXPECT_EQ(1, lv[0]);
	EXPECT_EQ(0, lv[1]);
	EXPECT_EQ(2, lv.size());
	////////////////////////

}


