/*
* test_graph.cpp  tests for class Graph for directed graphs
* @created ?
* @updated 29/12/20
* @last_update 31/12/24
* @dev pss
*/

#include "../graph.h"
#include "../graph/algorithms/graph_conversions.h"
#include "gtest/gtest.h"
#include "utils/common_paths.h"
#include <iostream>

using namespace std;

///////////////
//switches
//#define rlf_sort_logs  -TODO@insert log switch
//#define print_graph_logs

//#define TEST_GRAPH_STEP_BY_STEP				//ON to control manually the start of each test
//#define print_test_graph_logs

class GraphTest : public ::testing::Test {
protected:
	void SetUp() override {
		g.init(NV);
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
	
	EXPECT_EQ(6, g.number_of_vertices());
	EXPECT_EQ(4, g.number_of_edges(false));
	EXPECT_EQ(6, g.number_of_vertices());
	EXPECT_EQ(1, g.number_of_blocks());
	EXPECT_TRUE(g.get_path().empty());
	EXPECT_TRUE(g.get_name().empty());

	EXPECT_TRUE(g.is_edge(1, 2));
	EXPECT_TRUE(g.is_edge(1, 3));
	EXPECT_TRUE(g.is_edge(1, 4));
	EXPECT_TRUE(g.is_edge(3, 5));
	EXPECT_TRUE(g.is_edge(3, 5));
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
	EXPECT_STREQ("mygraph", g.get_name().c_str());
	/////////////////////////////////

	//deallocates memory
	for (int i = 0; i < NV; i++) {
		delete[] adj[i];
	}
	delete[] adj;
}

TEST_F(GraphTest, name) {
	
	g.set_name("test");
	EXPECT_STREQ("test", g.get_name().c_str());
	EXPECT_TRUE(g.get_path().empty());

	g.set_name("c:/path/test");
	EXPECT_STREQ("test", g.get_name().c_str());
	EXPECT_STREQ("c:/path/", g.get_path().c_str());

	g.set_name("c:\\path\\test");
	EXPECT_STREQ("test", g.get_name().c_str());
	EXPECT_STREQ("c:\\path\\", g.get_path().c_str());
}

TEST_F(GraphTest, init_and_reset) {

	const int NV = 10;
	g.set_name("new_name");

	////////////////////////////////////
	g.init(NV, true);							
	EXPECT_EQ(NV, g.number_of_vertices());
	EXPECT_EQ(0, g.number_of_edges());
	EXPECT_TRUE(g.get_name().empty());						//init(NV, true) clears name
	////////////////////////////////////
	
	///////////////////////////////////
	g.reset(NV + 1, "new_name");
	EXPECT_EQ(NV + 1, g.number_of_vertices());
	EXPECT_EQ(0, g.number_of_edges());
	EXPECT_STREQ("new_name", g.get_name().c_str());			
	////////////////////////////////////
}

TEST(Graph, equal) {

	string path = TESTS_GRAPH_DATA_CMAKE;
	
	graph g1(path + "brock200_1.clq");
	graph g2(g1);

	///////////////////////////
	EXPECT_TRUE(g1 == g2);
	///////////////////////////
}

TEST(Graph, density) {
///////////////
// simple test- after removing print_data() for the simple undirected graph
// date@19/06/2019

	LOG_INFO("Graph: density ------------");
	string path = TEST_GRAPH_PATH_DATA;

///////////
//undirected
	ugraph ug(path + "brock200_1.clq");
	
	int NV = 200; int NE = ug.number_of_edges();
	EXPECT_EQ(14834, NE);
	EXPECT_EQ(NV, ug.number_of_vertices());
	BITBOARD aux = NV * (NV - 1);
	double d = (2 * NE) / (double)aux;
	EXPECT_EQ(d, ug.density());

////////////
//directed

	graph g(path + "brock200_1.clq");
	NV = 200;  NE = g.number_of_edges();
	EXPECT_EQ(14834, NE);
	EXPECT_EQ(NV, g.number_of_vertices());
	d = NE / (double)aux;				//half the density of the undirected case
	EXPECT_EQ(d, g.density());
	
	LOG_INFO("Graph: END density------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, equality) {
/////////////////
// Silly testing of the new predefined equality for simple-unweighted graphs(18/06/19) 
	
	LOG_INFO("Graph: equality ------------");
	string path = TEST_GRAPH_PATH_DATA;

	ugraph g(path + "brock200_1.clq");
	ugraph g1;
		
	g1 = g;
	EXPECT_EQ(g1.get_name(), g.get_name());
	EXPECT_EQ(g1.number_of_edges(), g.number_of_edges());
	EXPECT_EQ(g1.is_edge(100,199), g.is_edge(100, 199));

	LOG_INFO("Graph: END equality------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, graph_from_file){
///////////////////
// Undirected graphs read by directed graph class (all edges are non symmetrical)

	LOG_INFO("Graph: graph_from_file ------------");
	string path= TEST_GRAPH_PATH_DATA;

	graph g1(path + "brock200_1.clq");
	graph g2(path + "brock200_2.clq");
	graph g3(path + "brock200_3.clq");
	graph g4(path + "brock200_4.clq");

	//Number of (directed) edges
	EXPECT_EQ(14834,g1.number_of_edges());
	EXPECT_EQ(9876,g2.number_of_edges());
	EXPECT_EQ(12048,g3.number_of_edges());
	EXPECT_EQ(13089,g4.number_of_edges());

	//is directed
	EXPECT_NE(g1.is_edge(3,2),g1.is_edge(2,3));
	EXPECT_NE(g1.is_edge(4,2),g1.is_edge(2,4));

	//Number of vertices
	EXPECT_EQ(200,g1.number_of_vertices());
	EXPECT_EQ(200,g2.number_of_vertices());
	EXPECT_EQ(200,g3.number_of_vertices());
	EXPECT_EQ(200,g4.number_of_vertices());

	g1.make_bidirected();
	g2.make_bidirected();
	g3.make_bidirected();
	g4.make_bidirected();

	//Number of (bidirected) edges	
	EXPECT_EQ(2*14834,g1.number_of_edges());
	EXPECT_EQ(2*9876,g2.number_of_edges());
	EXPECT_EQ(2*12048,g3.number_of_edges());
	EXPECT_EQ(2*13089,g4.number_of_edges());

	//Name
	EXPECT_STREQ("brock200_1.clq",g1.get_name().c_str());
	EXPECT_STREQ("brock200_2.clq",g2.get_name().c_str());
	EXPECT_STREQ("brock200_3.clq",g3.get_name().c_str());
	EXPECT_STREQ("brock200_4.clq",g4.get_name().c_str());

	LOG_INFO("Graph: END graph_from_file------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, neighbors){
	LOG_INFO("Graph: neighbors -------------------");
	const int NV=10;
	
	graph g(NV);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(2,3);
	g.add_edge(0,3);
	
	int v=1;
	BBIntrin& aux=g.get_neighbors(v);
		
	//computes vertex support
	vector<int> vec;
	aux.init_scan(bbo::NON_DESTRUCTIVE);
	while(true){
		int vadj=aux.next_bit();
		if(vadj==EMPTY_ELEM) break;
		vec.push_back(vadj);
	}

	//vertex 2 is the only neighbor of 1 and has degree 2
	EXPECT_EQ(1,vec.size());
	EXPECT_EQ(1,count(vec.begin(), vec.end(), 2));

	LOG_INFO("Graph: END neighbors------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph,graph_copy){
	LOG_INFO("Graph: graph_copy -------------------");
	string path= TEST_GRAPH_PATH_DATA;
	graph g(path + "brock200_2.clq");
	graph g1(g);

	EXPECT_EQ(g.number_of_edges(),g1.number_of_edges());
	EXPECT_EQ(g.number_of_vertices(),g1.number_of_vertices());
	EXPECT_EQ(g.get_name(),g1.get_name());
	EXPECT_EQ("brock200_2.clq",g1.get_name());
	EXPECT_TRUE(g == g1);

	//make bidirected
	g1.make_bidirected();
	EXPECT_FALSE(g==g1);

	LOG_INFO("Graph: END graph_copy------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, is_edge){
	LOG_INFO("Graph: is_edge -------------------");
	string path= TEST_GRAPH_PATH_DATA;
	graph g(path + "sample.clq");
	
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


	g.remove_edge(2, 0);
	EXPECT_FALSE(g.is_edge(2,0));
	
	//makes bidirected and removes the first 4 vertices
	g.make_bidirected();
	BitBoardN bbn(4);
	bbn.set_bit(0, 3);
	g.remove_vertices(bbn);
	EXPECT_EQ(3, g.number_of_vertices());		//7-4=3 vertices which form a clique (but directed)
	EXPECT_EQ(3, g.number_of_edges());

	LOG_INFO("Graph: END is_edge------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, degree_sample){
	LOG_INFO("Graph: degree_sample -------------------");
	string path= TEST_GRAPH_PATH_DATA;
	ugraph g(path + "sample.clq");
	
	/*EXPECT_TRUE(g.is_edge(1,0));
	EXPECT_TRUE(g.is_edge(2,0));
	EXPECT_TRUE(g.is_edge(2,1));
	EXPECT_TRUE(g.is_edge(3,0));
	EXPECT_TRUE(g.is_edge(3,1));
	EXPECT_TRUE(g.is_edge(3,2));
	EXPECT_TRUE(g.is_edge(4,3));
	EXPECT_TRUE(g.is_edge(5,3));
	EXPECT_TRUE(g.is_edge(5,4));
	EXPECT_TRUE(g.is_edge(6,4));
	EXPECT_TRUE(g.is_edge(6,5));*/
	
	EXPECT_EQ (3,g.degree(1));
	
	bitarray bba(g.number_of_vertices());
	bba.set_bit(0); bba.set_bit(1); bba.set_bit(6);

	EXPECT_EQ (1,g.degree(1,bba));

	bba.erase_bit();
	bba.set_bit(0); bba.set_bit(1); 
	EXPECT_EQ (1,g.degree(1,bba));

	LOG_INFO("Graph: END degree_sample------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, degree_brock){
	LOG_INFO("Graph: degree_brock -------------------");
	string path= TEST_GRAPH_PATH_DATA;
	ugraph g(path + "brock200_1.clq");
	

	EXPECT_EQ (200,  g.number_of_vertices());
	EXPECT_EQ (14834,  g.number_of_edges());
	
	bitarray bba(g.number_of_vertices());
	EXPECT_FALSE(g.is_edge(0,1));		//(1)
	
	bba.set_bit(0); bba.set_bit(1); 
	EXPECT_EQ (0,g.degree(1,bba));		//expected because of (1)

	bba.set_bit(0, g.number_of_vertices()-1);
	EXPECT_EQ (g.get_neighbors(1).popcn64(),g.degree(1,bba));

	LOG_INFO("Graph: END degree_brock------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, max_degree_subgraph){
	LOG_INFO("Graph: max_degree_subgraph ------------------------");
	ugraph g(100);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(1,3);
	g.add_edge(2,3);
	g.add_edge(78,79);
	g.add_edge(79,80);
	
	bitarray sg(g.number_of_vertices());
	sg.init_bit(0, 3);
	EXPECT_EQ(3,g.max_degree_of_subgraph(sg));	//1(3)

	sg.init_bit(0, 2);
	EXPECT_EQ(2,g.max_degree_of_subgraph(sg));  //1(2)

	sg.init_bit(78, 79);						//78(1)
	EXPECT_EQ(1,g.max_degree_of_subgraph(sg));

	LOG_INFO("Graph: END max_degree_subgraph------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, max_degree_subgraph_sparse){
	LOG_INFO("Graph: max_degree_subgraph_sparse ------------------");
	
	sparse_ugraph g(100);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(1,3);
	g.add_edge(2,3);
	g.add_edge(78,79);
	g.add_edge(79,80);
	
	sparse_bitarray sg(g.number_of_vertices());
	sg.init_bit(0, 3);
	EXPECT_EQ(3,g.max_degree_of_subgraph(sg));	//1(3)

	sg.init_bit(0, 2);
	EXPECT_EQ(2,g.max_degree_of_subgraph(sg));  //1(2)

	sg.init_bit(78, 79);						//78(1)
	EXPECT_EQ(1,g.max_degree_of_subgraph(sg));

	LOG_INFO("Graph: END max_degree_subgraph_sparse------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, remove_edges){
	LOG_INFO("Graph: remove_edges ------------------------");
	
	graph g(4);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(1,3);
	g.add_edge(0,2);

	g.remove_edges(1);
	g.print_edges();
	g.print_data(false);
	EXPECT_TRUE(g.is_edge(0,2));
	
	LOG_INFO("Graph: END remove_edges------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}





