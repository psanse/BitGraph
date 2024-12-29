//test_graph.cpp: tests for Graph and Ugraph objects
//last_update@ 29/12/20
//dev@pss
//////////////////////

#include <iostream>
#include "gtest/gtest.h"
#include "../graph.h"
#include "../graph/algorithms/graph_conversions.h"
#include "utils/common_paths.h"

///////////////
//switches
//#define rlf_sort_logs  -TODO@insert log switch
#define print_graph_logs

using namespace std;

//#define TEST_GRAPH_STEP_BY_STEP				//ON to control manually the start of each test
#define print_test_graph_logs


TEST(Ugraph, operator_equal) {
	LOG_INFO("Ugraph: operator_equal ------------");

	ugraph g(7);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(5, 6);
	g.set_name("toy");

	ugraph g1(3);
	g1.add_edge(0, 1);
	g1.add_edge(0, 2);

	///////////
	g1 = g;
	///////////

	EXPECT_EQ(7, g1.number_of_vertices());
	EXPECT_TRUE(g1.is_edge(0, 1));
	EXPECT_TRUE(g1.is_edge(1, 2));
	EXPECT_TRUE(g1.is_edge(2, 3));
	EXPECT_TRUE(g1.is_edge(0, 3));
	EXPECT_TRUE(g1.is_edge(5, 6));
	EXPECT_FALSE(g1.is_edge(0, 2));
	EXPECT_STREQ("toy", g1.get_name().c_str());

	LOG_INFO("Ugraph: END operator_equal --------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Ugraph, create_induced) {
	///////////////////
	// TESTS induced subgraph functionality
	
	LOG_INFO("Ugraph: create_induced ------------");

	const int NV = 10;

	ugraph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(5, 6);

	ugraph ugi;
	std::vector<int> lv;
	lv.push_back(2);  lv.push_back(3); lv.push_back(5); lv.push_back(6);

	////////////////////////////////
	int status = g.create_induced(ugi, lv);		 //G(lv)
	////////////////////////////////
	ASSERT_EQ(0, status);
	ASSERT_EQ(4, ugi.number_of_vertices());
	ASSERT_EQ(2, ugi.number_of_edges(false));
	ASSERT_TRUE(ugi.is_edge(0,1));					//edge (2,3)
	ASSERT_TRUE(ugi.is_edge(2,3));					//edge (5,6)
	
	
#ifdef print_test_graph_logs
	ugi.print_data();
	ugi.print_edges();
#endif

	////////////////////////////////
	status = g.create_induced(ugi, 0);				//G(N(0), neighbour set indiced
	////////////////////////////////
	ASSERT_EQ(0, status);
	ASSERT_EQ(2, ugi.number_of_vertices());
	ASSERT_EQ(0, ugi.number_of_edges(false));

#ifdef print_test_graph_logs
	ugi.print_data();
	ugi.print_edges();
#endif

	LOG_INFO("Ugraph: END create_induced --------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}


TEST(Ugraph, DISABLED_add_vertex_multiple) {
	///////////////////
	// Extends the graph with a new vertex
	//
	// COMMENTS
	// Currently BUGGY- TEST EXITS (DISABLE)
	LOG_INFO("Ugraph: add_vertex_multiple ------------");

	const int NV = 10;

	ugraph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);


	///////////////////
	//Extends graph according to this neighborhood
	graph::_bbt bbn(NV + 1 /* MUST BE THE NEW SIZE! */);
	bbn.set_bit(0);
	bbn.set_bit(3);

	g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood
	g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood
	g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood

/////////////////
//TESTS
	EXPECT_EQ(g.number_of_vertices(), NV + 3);
	EXPECT_EQ(g.number_of_edges(false), 4 + 6);

	int v = NV;
	EXPECT_TRUE(g.is_edge(v, 0));
	EXPECT_TRUE(g.is_edge(v, 3));
	EXPECT_FALSE(g.is_edge(v, 1));

	v = NV+1;
	EXPECT_TRUE(g.is_edge(v, 0));
	EXPECT_TRUE(g.is_edge(v, 3));
	EXPECT_FALSE(g.is_edge(v, 1));

	v = NV + 2;
	EXPECT_TRUE(g.is_edge(v, 0));
	EXPECT_TRUE(g.is_edge(v, 3));
	EXPECT_FALSE(g.is_edge(v, 1));
	
	LOG_INFO("Ugraph: END add_vertex_multiple --------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Ugraph, DISABLED_add_single_vertex) {
	///////////////////
	// Extends the graph with a new vertex
	//
	// COMMENTS
	// Currently BUGGY- TEST EXITS (DISABLE)
	LOG_INFO("Ugraph: add_single_vertex ------------");

	const int NV = 10;

	ugraph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	  	
///////////////////
//Extends graph according to this neighborhood
	graph::_bbt bbn(NV + 1 /* MUST BE THE NEW SIZE! */);
	bbn.set_bit(0);
	bbn.set_bit(3);

	g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood
	
#ifdef	print_graph_logs
	g.print_data(false, cout, true);
	g.print_edges();
#endif

/////////////////
//TESTS
	int v = NV;
	EXPECT_EQ(g.number_of_vertices(), NV + 1);
	EXPECT_EQ(g.number_of_edges(false), 4 + 2);
	EXPECT_TRUE(g.is_edge(v, 0));
	EXPECT_TRUE(g.is_edge(v, 3));
	EXPECT_FALSE(g.is_edge(v, 1));
	   	
///////////////////
//Extends graph by one vertex with empty neighborhood

	g.add_vertex();	
	EXPECT_EQ(g.number_of_vertices(), NV + 2);
	EXPECT_EQ(g.number_of_edges(false), 4 + 2);
	v = NV + 1;
	EXPECT_FALSE(g.is_edge(v, 0));
	

	LOG_INFO("Ugraph: END add_single_vertex --------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Ugraph, graph_from_adj_mat) {
	///////////////////
	// Undirected graphs read by directed graph class (all edges are non symmetrical)

	LOG_INFO("Ugraph: graph_from_ady_mat ------------");

	//build adyacency matrix
	const int NV = 3;

	int** adj = new int*[NV];
	for (int i = 0; i < NV; i++) {
		adj[i] = new int[NV];
	}
	for (int i = 0; i < NV; i++) {
		for (int j = 0; j < NV; j++) {
			adj[i][j] = 0;
		}
	}
	adj[0][1] = 1; 		//[0] -> [1]
	adj[0][2] = 1; 		//[0] -> [2]

	ugraph ug(3, adj, "mygraph");

	//TESTS
	EXPECT_TRUE(ug.is_edge(0, 1));
	EXPECT_TRUE(ug.is_edge(0, 2));
	EXPECT_TRUE(ug.is_edge(1, 0));
	EXPECT_TRUE(ug.is_edge(2, 0));

	//I/O
#ifdef print_test_graph_logs
	stringstream sstr;
	ug.print_data(true, sstr);
	ug.print_adj(sstr);
	LOGG_INFO(sstr.str());
#endif

	for (int i = 0; i < NV; i++) {
		delete[] adj[i];
	}
	delete[] adj;
	

	LOG_INFO("Ugraph: END graph_from_ady_mat------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, graph_from_adj_mat) {
	///////////////////
	// Undirected graphs read by directed graph class (all edges are non symmetrical)

	LOG_INFO("Graph: graph_from_ady_mat ------------");

	//build adyacency matrix
	const int NV = 3;

	int** adj = new int* [NV];
	for (int i = 0; i < NV; i++) {
		adj[i] = new int[NV];
	}
	for (int i = 0; i < NV; i++) {
		for (int j = 0; j < NV; j++) {
			adj[i][j] = 0;
		}
	}
	adj[0][1] = 1; 		//[0] -> [1]
	adj[0][2] = 1; 		//[0] -> [2]

	graph g(3, adj, "mygraph");

	//TESTS
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(0, 2));
	EXPECT_FALSE(g.is_edge(1, 0));
	EXPECT_FALSE(g.is_edge(2, 0));
	
	//I/O
#ifdef print_test_graph_logs
	stringstream sstr;
	g.print_data(true, sstr);
	g.print_adj(sstr);
	LOGG_INFO(sstr.str());
#endif

	for (int i = 0; i < NV; i++) {
		delete[] adj[i];
	}
	delete[] adj;
	

	
	LOG_INFO("Graph: END graph_from_ady_mat------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Graph, operator_same) {
///////////////
// simple test of the == operator (checks adjacency matrix only, NOT isomorphism!)
// date@19/06/2019

	LOG_INFO("Graph: operator_same --------");
	string path = TEST_GRAPH_PATH_DATA;
	
///////////
//undirected
	ugraph ug(path + "brock200_1.clq");
	ugraph ug1(ug);

	EXPECT_TRUE(ug == ug1);

////////////
//directed

	graph g(path + "brock200_1.clq");
	graph g1(g);

	EXPECT_TRUE(g == g1);
	
	LOG_INFO("Graph: END operator_same------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
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

TEST(Ugraph, complement_graph){

	LOG_INFO("Ugraph: complement_graph ------------------");

	
	ugraph g(4);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(0,2);
	

	ugraph cg;
	g.create_complement(cg);

	EXPECT_TRUE(cg.is_edge(0,3));
	EXPECT_TRUE(cg.is_edge(1,3));
	EXPECT_TRUE(cg.is_edge(2,3));
	EXPECT_EQ(g.number_of_vertices(),cg.number_of_vertices());
	//cg.print_edges();

	LOG_INFO("Ugraph: END complement_graph------");
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

TEST(Ugraph, remove_edges){
	LOG_INFO("Ugraph: remove_edges ------------------------");
	
	ugraph g(4);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(1,3);
	g.add_edge(0,2);

	g.remove_edges(1);
	g.print_edges();
	g.print_data(false);
	EXPECT_TRUE(g.is_edge(0,2));

	LOG_INFO("Ugraph: END remove_edges------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}


TEST(Conversions, sparse2ugraph){
	LOG_INFO("Conversions: sparse2ugraph ------------------------");


	sparse_ugraph sug(4);
	sug.add_edge(0,1);
	sug.add_edge(1,2);
	sug.add_edge(1,3);
	sug.add_edge(0,2);

	ugraph ug;
	GraphConversion::sug2ug(sug, ug);

	EXPECT_EQ(4,ug.number_of_vertices());
	EXPECT_EQ(4,ug.number_of_edges());
	EXPECT_TRUE(ug.is_edge(0,1));
	EXPECT_TRUE(ug.is_edge(1,2));
	EXPECT_TRUE(ug.is_edge(1,3));
	EXPECT_TRUE(ug.is_edge(0,2));


	sparse_ugraph sug1(300);
	sug1.add_edge(0,1);
	sug1.add_edge(1,2);
	sug1.add_edge(1,3);
	sug1.add_edge(0,2);

	GraphConversion::sug2ug(sug1, ug);
	EXPECT_EQ(300,ug.number_of_vertices());
	EXPECT_EQ(4,ug.number_of_edges());
	EXPECT_TRUE(ug.is_edge(0,1));
	EXPECT_TRUE(ug.is_edge(1,2));
	EXPECT_TRUE(ug.is_edge(1,3));
	EXPECT_TRUE(ug.is_edge(0,2));
	ug.print_data(false);
		
	LOG_INFO("Conversions: END sparse2ugraph------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Conversions, ugraph2sparse_ugraph){
	LOG_INFO("Conversions: ugraph2sparse_ugraph ------------------------");
	ugraph ug(4);
	ug.add_edge(0,1);
	ug.add_edge(1,2);
	ug.add_edge(1,3);
	ug.add_edge(0,2);

	sparse_ugraph sug;
	GraphConversion::ug2sug(ug, sug);

	EXPECT_EQ(4,sug.number_of_vertices());
	EXPECT_EQ(4,sug.number_of_edges());
	EXPECT_TRUE(sug.is_edge(0,1));
	EXPECT_TRUE(sug.is_edge(1,2));
	EXPECT_TRUE(sug.is_edge(1,3));
	EXPECT_TRUE(sug.is_edge(0,2));
	sug.print_data(false);

	ugraph ug1(300);
	ug1.add_edge(0,1);
	ug1.add_edge(1,2);
	ug1.add_edge(1,3);
	ug1.add_edge(0,2);

	GraphConversion::ug2sug(ug1, sug);
	EXPECT_EQ(300,sug.number_of_vertices());
	EXPECT_EQ(4,sug.number_of_edges());
	EXPECT_TRUE(sug.is_edge(0,1));
	EXPECT_TRUE(sug.is_edge(1,2));
	EXPECT_TRUE(sug.is_edge(1,3));
	EXPECT_TRUE(sug.is_edge(0,2));
	sug.print_data(false);
		
	LOG_INFO("Conversions: END ugraph2sparse_ugraph------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}


//////////////////
//
// TDOD@REFACTOR for the new Graph types
//
/////////////////


//TEST(Graph_test,average_block_density){
//	const int NUM_BB=4;
//	graph g(64*NUM_BB);
//	g.add_edge(0,1);
//	g.add_edge(1,2);
//	g.add_edge(0,2);
//	
//	int nV=g.number_of_vertices();
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
//	EXPECT_DOUBLE_EQ(3.0/g2.number_of_vertices(), g2.block_density());		
//
//	//spare case
//	sparse_ugraph g3(4);
//	g3.add_edge(0,1);
//	g3.add_edge(1,2);
//	g3.add_edge(0,2);
//
//	EXPECT_DOUBLE_EQ(1.0, g3.block_density());	
//}
//
//TEST(Graph_test, resize){
//	graph g(100);
//	g.add_edge(0,1);
//	g.add_edge(1,2);
//	g.add_edge(2,3);
//	g.add_edge(0,3);
//	g.add_edge(54,55);
//	g.add_edge(88,89);
//	g.add_edge(88,90);
//	g.add_edge(89,90);
//
//	graph g1;
//	g.create_subgraph(50, g1);
//	EXPECT_EQ(50, g1.number_of_vertices());
//	EXPECT_EQ(4, g1.number_of_edges());			//0->1, 1->2, 2->3, 0->3
//	cout<<"----------------------------"<<endl;
//
//	g.create_subgraph(3, g1);
//	EXPECT_EQ(3, g1.number_of_vertices());
//	EXPECT_EQ(2, g1.number_of_edges());			//0->1, 1->2, 2->3
//	cout<<"----------------------------"<<endl;
//}
//
//TEST(Graph_test, shrink_graph){
//	graph g(100);
//	g.add_edge(0,1);
//	g.add_edge(1,2);
//	g.add_edge(2,3);
//	g.add_edge(0,3);
//	g.add_edge(54,55);
//	g.add_edge(88,89);
//	g.add_edge(88,90);
//	g.add_edge(89,90);
//
//	//shrinking not possible in non sparse graphs
//	int r=g.shrink_to_fit(50);
//	EXPECT_EQ(-1,r);
//
//
//	ugraph g1(100);
//	g1.add_edge(0,1);
//
//	//shrinking not possible in non sparse graphs
//	int r1=g1.shrink_to_fit(50);
//	EXPECT_EQ(-1,r);
//}
//
//
//TEST(Ugraph_test,ugraph_from_file){
/////////////////////
//// correct use of ugraph object to read undirected DIMACS graph
//
//	ugraph g1("brock200_1.clq");
//	ugraph g2("brock200_2.clq");
//	ugraph g3("brock200_3.clq");
//	ugraph g4("brock200_4.clq");
//
//	//Number of edges
//	EXPECT_EQ(14834,g1.number_of_edges());
//	EXPECT_EQ(9876,g2.number_of_edges());
//	EXPECT_EQ(12048,g3.number_of_edges());
//	EXPECT_EQ(13089,g4.number_of_edges());
//
//	//is undirected
//	EXPECT_EQ(g1.is_edge(3,2),g1.is_edge(2,3));
//	EXPECT_EQ(g1.is_edge(4,2),g1.is_edge(2,4));
//
//	//Number of vertices
//	EXPECT_EQ(200,g1.number_of_vertices());
//	EXPECT_EQ(200,g2.number_of_vertices());
//	EXPECT_EQ(200,g3.number_of_vertices());
//	EXPECT_EQ(200,g4.number_of_vertices());
//
//	//Name
//	EXPECT_STREQ("brock200_1.clq",g1.get_name().c_str());
//	EXPECT_STREQ("brock200_2.clq",g2.get_name().c_str());
//	EXPECT_STREQ("brock200_3.clq",g3.get_name().c_str());
//	EXPECT_STREQ("brock200_4.clq",g4.get_name().c_str());
//}
//
//
//TEST(Ugraph_test,removing_vertices){
//	ugraph g("sample.clq");
//		
//	BitBoardN bbn(4);
//	bbn.set_bit(0, 3);
//	g.remove_vertices(bbn);
//
//	EXPECT_EQ(3, g.number_of_vertices());		//7-4=3 vertices which form a clique (proper)
//	EXPECT_EQ(3, g.number_of_edges());
//}
//
//TEST(Ugraph_test,numberof_edges){
//	ugraph g("brock200_1.clq");
//		
//	EXPECT_EQ(14834, g.number_of_edges());		//7-4=3 vertices which form a clique (proper)
//}
//
//TEST(Graph_test, block_density_index){
//////////////////////
//// block density: number of blocks / total possible number of blocks 
//	sparse_ugraph sg(100);
//	sg.add_edge(0,1);
//	sg.add_edge(1,2);
//	
//	EXPECT_EQ(3/ceil(10000/64.0),sg.block_density_index());		
//}
//
