/*
* test_ugraph.cpp  tests for class Ugraph for undirected graphs
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
#define print_graph_logs


//#define TEST_GRAPH_STEP_BY_STEP				//ON to control manually the start of each test
#define print_test_graph_logs

TEST(Ugraph, equal_1) {

	string path = TESTS_GRAPH_DATA_CMAKE;

	ugraph ug1(path + "brock200_1.clq");
	ugraph ug2(ug1);

	///////////////////////////
	EXPECT_TRUE(ug1 == ug2);
	///////////////////////////

}

TEST(Ugraph, equal_2) {
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

TEST(Ugraph, degree) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	ugraph g(path + "sample.clq");

	EXPECT_EQ(7, g.number_of_vertices());
	EXPECT_EQ(11, g.number_of_edges());

	//deg(1)=3
	g.print_degrees();
	EXPECT_EQ(3, g.degree(1));
	

	bitarray bba(g.number_of_vertices());
	bba.set_bit(0);
	bba.set_bit(1);
	bba.set_bit(6);
	EXPECT_EQ(1, g.degree(1, bba));

	bba.erase_bit();
	bba.set_bit(0); bba.set_bit(1);
	EXPECT_EQ(1, g.degree(1, bba));

}

TEST(Ugraph, degree_brock) {
	LOG_INFO("Graph: degree_brock -------------------");
	string path = TEST_GRAPH_PATH_DATA;
	ugraph g(path + "brock200_1.clq");


	EXPECT_EQ(200, g.number_of_vertices());
	EXPECT_EQ(14834, g.number_of_edges());

	bitarray bba(g.number_of_vertices());
	EXPECT_FALSE(g.is_edge(0, 1));		//(1)

	bba.set_bit(0); bba.set_bit(1);
	EXPECT_EQ(0, g.degree(1, bba));		//expected because of (1)

	bba.set_bit(0, g.number_of_vertices() - 1);
	EXPECT_EQ(g.get_neighbors(1).popcn64(), g.degree(1, bba));

	LOG_INFO("Graph: END degree_brock------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}


TEST(Ugraph, max_degree_subgraph) {
	LOG_INFO("Graph: max_degree_subgraph ------------------------");
	ugraph g(100);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(1, 3);
	g.add_edge(2, 3);
	g.add_edge(78, 79);
	g.add_edge(79, 80);

	bitarray sg(g.number_of_vertices());
	sg.init_bit(0, 3);
	EXPECT_EQ(3, g.max_degree_of_subgraph(sg));	//1(3)

	sg.init_bit(0, 2);
	EXPECT_EQ(2, g.max_degree_of_subgraph(sg));  //1(2)

	sg.init_bit(78, 79);						//78(1)
	EXPECT_EQ(1, g.max_degree_of_subgraph(sg));

	LOG_INFO("Graph: END max_degree_subgraph------");
#ifdef TEST_GRAPH_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif;
}

TEST(Ugraph, density) {
	
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

	

	LOG_INFO("Graph: END density------");
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

TEST(Ugraph, equality) {
	/////////////////
	// Silly testing of the new predefined equality for simple-unweighted graphs(18/06/19) 

	LOG_INFO("Graph: equality ------------");
	string path = TEST_GRAPH_PATH_DATA;

	ugraph g(path + "brock200_1.clq");
	ugraph g1;

	g1 = g;
	EXPECT_EQ(g1.get_name(), g.get_name());
	EXPECT_EQ(g1.number_of_edges(), g.number_of_edges());
	EXPECT_EQ(g1.is_edge(100, 199), g.is_edge(100, 199));

	LOG_INFO("Graph: END equality------");
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
