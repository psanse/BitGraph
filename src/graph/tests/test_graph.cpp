/*
* @file test_graph.cpp  
* @brief Unit tests for the Graph class for directed graphs
* @created ?
* @updated 29/12/20
* @last_update 31/12/24
* @author pss
* 
* @TODO - ADD TESTS, e.g. operator =, move operator =,...
*/


#include "gtest/gtest.h"
#include "graph/simple_graph.h"
#include <iostream>

//useful alias
using vint = vector<int>;

using graph = bitgraph::Graph<bitgraph::bitarray>;						//simple graph

using namespace std;
using namespace bitgraph;

class GraphTest : public ::testing::Test {
protected:
	void SetUp() override {
		g.reset(NV);
		g.add_edge(1, 2);
		g.add_edge(1, 3);
		g.add_edge(1, 4);
		g.add_edge(3, 5);
	}
	void TearDown() override {}

	//directed graph instance	
	const int NV = 6;
	graph g;
};

TEST_F(GraphTest, constructor) {

	EXPECT_EQ(6, g.num_vertices());
	EXPECT_EQ(4, g.num_edges(false));
	EXPECT_EQ(6, g.num_vertices());
	EXPECT_EQ(1, g.num_blocks());
	EXPECT_TRUE(g.path().empty());
	EXPECT_TRUE(g.name().empty());

	EXPECT_TRUE(g.is_edge(1, 2));
	EXPECT_TRUE(g.is_edge(1, 3));
	EXPECT_TRUE(g.is_edge(1, 4));
	EXPECT_TRUE(g.is_edge(3, 5));
	EXPECT_TRUE(g.is_edge(3, 5));
}

TEST_F(GraphTest, deallocate_memory) {
	
	//clean memory
	g = graph();						//std::swap(graph(), g);

	EXPECT_EQ	(0, g.num_vertices());
	EXPECT_TRUE	(g.name().empty());
	EXPECT_EQ	(0, g.num_edges());
}


TEST(Graph, constructor_adj_mat) {

	const int NV = 3;

	//build adyacency matrix
	int** adj = new int* [NV];
	for (int i = 0; i < NV; i++) {
		adj[i] = new int[NV];
	}
	for (int i = 0; i < NV; i++) {
		for (int j = 0; j < NV; j++) {
			adj[i][j] = 0;
		}
	}
	adj[0][1] = 1;
	adj[0][2] = 1;

	/////////////////////////////////
	graph g(3, adj, "mygraph");
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(0, 2));
	EXPECT_FALSE(g.is_edge(1, 0));
	EXPECT_FALSE(g.is_edge(2, 0));
	EXPECT_STREQ("mygraph", g.name().c_str());
	/////////////////////////////////

	//deallocates memory
	for (int i = 0; i < NV; i++) {
		delete[] adj[i];
	}
	delete[] adj;
}

TEST(Graph, constructor_file) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;

	//Undirected graphs read by directed graph class (all edges are non symmetrical)
	graph g1(path + "brock200_1.clq");
	graph g2(path + "brock200_2.clq");
	graph g3(path + "brock200_3.clq");
	graph g4(path + "brock200_4.clq");

	//test number of edges
	EXPECT_EQ(14834, g1.num_edges());
	EXPECT_EQ(9876, g2.num_edges());
	EXPECT_EQ(12048, g3.num_edges());
	EXPECT_EQ(13089, g4.num_edges());

	//test is directed
	EXPECT_NE(g1.is_edge(3, 2), g1.is_edge(2, 3));
	EXPECT_NE(g1.is_edge(4, 2), g1.is_edge(2, 4));

	//test number of vertices
	EXPECT_EQ(200, g1.num_vertices());
	EXPECT_EQ(200, g2.num_vertices());
	EXPECT_EQ(200, g3.num_vertices());
	EXPECT_EQ(200, g4.num_vertices());

	//make bidirected
	g1.make_bidirected();
	g2.make_bidirected();
	g3.make_bidirected();
	g4.make_bidirected();

	//test number of (bidirected) edges	
	EXPECT_EQ(2 * 14834, g1.num_edges());
	EXPECT_EQ(2 * 9876, g2.num_edges());
	EXPECT_EQ(2 * 12048, g3.num_edges());
	EXPECT_EQ(2 * 13089, g4.num_edges());

	//test names
	EXPECT_STREQ("brock200_1.clq", g1.name().c_str());
	EXPECT_STREQ("brock200_2.clq", g2.name().c_str());
	EXPECT_STREQ("brock200_3.clq", g3.name().c_str());
	EXPECT_STREQ("brock200_4.clq", g4.name().c_str());
}

TEST(Graph, copy_constructor) {
	
	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	graph g	(path + "brock200_2.clq");

	///////////////
	graph gc(g);
	///////////////

	/////////////////////////////////////////////////////////
	EXPECT_EQ	(g.num_edges(), gc.num_edges());
	EXPECT_EQ	(g.num_vertices(), gc.num_vertices());
	EXPECT_EQ	(g.name(), gc.name());
	EXPECT_EQ	("brock200_2.clq", gc.name());
	EXPECT_TRUE	(g == gc);
	/////////////////////////////////////////////////////////
		
	/////////////////////////
	gc.make_bidirected();
	EXPECT_FALSE(g == gc);
	/////////////////////////

}

TEST(Graph, move_constructor) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	graph g(path + "brock200_2.clq");
	const int NV = g.num_vertices();
	const int NE = g.num_edges();

	///////////////
	graph gc(std::move(g));
	///////////////

	/////////////////////////////////////////////
	EXPECT_EQ(NE, gc.num_edges());
	EXPECT_EQ(NV, gc.num_vertices());
	EXPECT_EQ("brock200_2.clq", gc.name());	
	/////////////////////////////////////////////


	//moved-from object is in a valid state (size is not assumed).
	//can be reused with reset().
	g.reset(3);

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_EQ(0, g.num_edges());
}


TEST_F(GraphTest, name) {
	
	g.set_name("test");
	EXPECT_STREQ("test", g.name().c_str());
	EXPECT_TRUE(g.path().empty());

	g.set_name("c:/path/test");
	EXPECT_STREQ("test", g.name().c_str());
	EXPECT_STREQ("c:/path/", g.path().c_str());

	g.set_name("c:\\path\\test");
	EXPECT_STREQ("test", g.name().c_str());
	EXPECT_STREQ("c:\\path\\", g.path().c_str());
}

TEST_F(GraphTest, reset_and_reset) {

	const int NV = 10;
	g.set_name("new_name");

	////////////////////////////////////
	g.reset(NV);							
	EXPECT_EQ(NV, g.num_vertices());
	EXPECT_EQ(0, g.num_edges());
	EXPECT_TRUE(g.name().empty());						//reset(NV, true) clears name
	////////////////////////////////////
	
	///////////////////////////////////
	g.reset(NV + 1, "new_name");
	EXPECT_EQ(NV + 1, g.num_vertices());
	EXPECT_EQ(0, g.num_edges());
	EXPECT_STREQ("new_name", g.name().c_str());			
	////////////////////////////////////
}

TEST_F(GraphTest, equal) {

	graph g1(g);

	///////////////////////////
	EXPECT_TRUE(g1 == g);
	///////////////////////////
}


TEST(Graph, equal_dimacs) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	
	graph g1(path + "brock200_1.clq");
	graph g2(g1);

	///////////////////////////
	EXPECT_TRUE(g1 == g2);
	///////////////////////////
}

TEST(Graph, density) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;

	//reading undirected graph as directed 
	//CHECK: only upper half od adjacency matrix non-empty
	graph g(path + "brock200_1.clq");			

	
	const int NVexp = 200, NEexp = 14834;

	//////////////////////////////////////////
	EXPECT_EQ(NVexp, g.num_vertices());
	EXPECT_EQ(NEexp, g.num_edges());
	//////////////////////////////////////////
	
	BITBOARD aux = NVexp * (NVexp - 1);
	double dexp = NEexp / (double)aux;

	//////////////////////////////////////////
	EXPECT_DOUBLE_EQ(dexp, g.density());
	//////////////////////////////////////////
}

TEST(Graph, neighbors){
	
	const int NV=10;
	
	graph g(NV);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(2,3);
	g.add_edge(0,3);
	
	//compute vertices adjacent to {1}
	BitSet& neigh_to_one =g.neighbors(1);
		
	//////////////////////////////////////
	EXPECT_EQ	(1, neigh_to_one.count());	//{2} is the only neighbor of 1 and has degree 2
	EXPECT_TRUE	( neigh_to_one.is_bit(2) );
	///////////////////////////////////////
}

TEST(Graph, is_edge){
	
	string path= PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	graph g(path + "sample.clq");

	EXPECT_EQ(7, g.num_vertices());
	EXPECT_EQ(11, g.num_edges());
	
	EXPECT_TRUE(g.is_edge(1,0));
	EXPECT_TRUE(g.is_edge(2,0));
	EXPECT_TRUE(g.is_edge(2,1));
	EXPECT_TRUE(g.is_edge(3,0));
	EXPECT_TRUE(g.is_edge(3,1));
	EXPECT_TRUE(g.is_edge(3,2));
	EXPECT_TRUE(g.is_edge(4,3));
	EXPECT_TRUE(g.is_edge(5,3));
	EXPECT_TRUE(g.is_edge(5,4));
	EXPECT_TRUE(g.is_edge(6,4));
	EXPECT_TRUE(g.is_edge(6,5));

	EXPECT_FALSE(g.is_edge(0,1));
	EXPECT_FALSE(g.is_edge(0,2));
	EXPECT_FALSE(g.is_edge(1,2));
	EXPECT_FALSE(g.is_edge(0,3));
	EXPECT_FALSE(g.is_edge(1,3));
	EXPECT_FALSE(g.is_edge(2,3));
	EXPECT_FALSE(g.is_edge(3,4));
	EXPECT_FALSE(g.is_edge(3,5));
	EXPECT_FALSE(g.is_edge(4,5));
	EXPECT_FALSE(g.is_edge(4,6));
	EXPECT_FALSE(g.is_edge(5,6));

	g.remove_edge (2, 0);
	EXPECT_FALSE  ( g.is_edge(2,0) );		
}

TEST(Graph, remove_edges) {
	
	graph g(4);

	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(1, 3);
	g.add_edge(0, 2);

	//removes all edges with endpoint in 1
	g.remove_edges(1);				
	EXPECT_TRUE(g.is_edge(0, 2));			//only edge left
	EXPECT_EQ(1, g.num_edges());

}

TEST(Graph, shrink_to_fit) {

	graph g(100);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(54, 55);
	g.add_edge(88, 89);
	g.add_edge(88, 90);
	g.add_edge(89, 90);

	//shrinking to first 50 vertices
	int status = g.shrink_to_fit(50);

	////////////////////////////
	ASSERT_EQ(0, status);
	EXPECT_EQ(50, g.num_vertices());
	EXPECT_EQ(4, g.num_edges());			//0->1, 1->2, 2->3, 0->3
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(1, 2));
	EXPECT_TRUE(g.is_edge(2, 3));
	EXPECT_TRUE(g.is_edge(0, 3));
	////////////////////////////

}

TEST(Graph, shrink_to_fit_only_capacity) {

	graph g(100);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(54, 55);
	g.add_edge(88, 89);
	g.add_edge(88, 90);
	g.add_edge(89, 90);

	//shrinking capacity if required
	g.shrink_to_fit();

	////////////////////////////
	EXPECT_EQ(100, g.num_vertices());
	EXPECT_EQ(8, g.num_edges());			//all of them
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(1, 2));
	EXPECT_TRUE(g.is_edge(2, 3));
	EXPECT_TRUE(g.is_edge(0, 3));
	////////////////////////////

}

TEST(Graph, create_subgraph){

	const int NV = 100;

	graph g(NV);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(2,3);
	g.add_edge(0,3);
	g.add_edge(54,55);
	g.add_edge(88,89);
	g.add_edge(88,90);
	g.add_edge(89,90);

	//creates subgraph with first 50 vertices
	graph g1;
	g.create_subgraph(50, g1);

	////////////////////////////////////////
	EXPECT_EQ(50, g1.num_vertices());
	EXPECT_EQ(4, g1.num_edges());			//0->1, 1->2, 2->3, 0->3
	////////////////////////////////////////

	//creates subgraph with first 3 vertices
	g.create_subgraph(3, g1);
	EXPECT_EQ(3, g1.num_vertices());
	EXPECT_EQ(2, g1.num_edges());			//0->1, 1->2 
	
}

//////////////////
//
// CHECK THE FOLLOWING TESTS
//
/////////////////

//TEST(Graph_test,average_block_density){
//	const int NUM_BB=4;
//	graph g(64*NUM_BB);
//	g.add_edge(0,1);
//	g.add_edge(1,2);
//	g.add_edge(0,2);
//	
//	int nV=g.num_vertices();
//	EXPECT_DOUBLE_EQ(2.0/(nV*NUM_BB) ,g.block_density());		//around 1/500 of useful bitblocks
//	
//	sparse_graph g1(64*NUM_BB);
//	g1.add_edge(0,1);
//	g1.add_edge(1,2);
//	g1.add_edge(0,2);
//
//	EXPECT_DOUBLE_EQ(1.0, g1.block_density());					//all bitblocks are supposed to be non empty
//	
//	//almost full density
//	ugraph g2(4);
//	g2.add_edge(0,1);
//	g2.add_edge(1,2);
//	g2.add_edge(0,2);
//
//	EXPECT_DOUBLE_EQ(3.0/g2.num_vertices(), g2.block_density());		
//
//	//spare case
//	sparse_ugraph g3(4);
//	g3.add_edge(0,1);
//	g3.add_edge(1,2);
//	g3.add_edge(0,2);
//
//	EXPECT_DOUBLE_EQ(1.0, g3.block_density());	
//}


















