/**
* @file test_ugraph.cpp
* @brief Unit tests for the Ugraph class for undirected graphs
* @created ?
* @updated 29/12/20
* @last_update 27/01/25
* @author pss
**/


#include "graph/simple_ugraph.h"
#include "gtest/gtest.h"
#include <iostream>

//alias
using graph = Graph<bitarray>;						//simple graph
using ugraph = Ugraph<bitarray>;					//simple undirected graph	

using namespace std;

TEST(Ugraph_test, constructor_file){

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;

	ugraph g1(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_1.clq");
	ugraph g2(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_2.clq");
	ugraph g3(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_3.clq");
	ugraph g4(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_4.clq");

	//test number of vertices
	//////////////////////////////////////////
	EXPECT_EQ(200, g1.number_of_vertices());
	EXPECT_EQ(200, g2.number_of_vertices());
	EXPECT_EQ(200, g3.number_of_vertices());
	EXPECT_EQ(200, g4.number_of_vertices());
	//////////////////////////////////////////

	//test number of edges
	//////////////////////////////////////////
	EXPECT_EQ(14834,g1.number_of_edges());
	EXPECT_EQ(9876,g2.number_of_edges());
	EXPECT_EQ(12048,g3.number_of_edges());
	EXPECT_EQ(13089,g4.number_of_edges());
	//////////////////////////////////////////
		
	//test names 
	/////////////////////////////////////////////////////
	EXPECT_STREQ("brock200_1.clq",g1.name().c_str());
	EXPECT_STREQ("brock200_2.clq",g2.name().c_str());
	EXPECT_STREQ("brock200_3.clq",g3.name().c_str());
	EXPECT_STREQ("brock200_4.clq",g4.name().c_str());
	/////////////////////////////////////////////////////

}

TEST(Ugraph, constructor_adj_matrix) {
	
	//build adjacency matrix
	const int NV = 3;

	int** adj = new int* [NV];
	for (int i = 0; i < NV; i++) {
		adj[i] = new int[NV];
	}

	//initialize adjacency matrix to zero
	for (std::size_t i = 0; i < NV; i++) {
		for (std::size_t j = 0; j < NV; j++) {
			adj[i][j] = 0;
		}
	}

	//adds directed edges
	adj[0][1] = 1; 		//[0] -> [1]
	adj[0][2] = 1; 		//[0] -> [2]

	////////////////////////////////
	ugraph ug(3, adj, "mygraph");				//undirected graph constructed from directed graph - only upper half of the adjacency matrix is read
	////////////////////////////////

	////////////////////////////////
	EXPECT_TRUE(ug.is_edge(0, 1));
	EXPECT_TRUE(ug.is_edge(0, 2));
	EXPECT_TRUE(ug.is_edge(1, 0));
	EXPECT_TRUE(ug.is_edge(2, 0));
	EXPECT_STREQ("mygraph", ug.name().c_str());
	////////////////////////////////

	//deallcoate memory
	for (std::size_t i = 0; i < NV; i++) {
		delete[] adj[i];
	}
	delete[] adj;
}


TEST(Ugraph, equal_brock) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;

	ugraph ug1(path + "brock200_1.clq");
	ugraph ug2(ug1);

	///////////////////////////
	EXPECT_TRUE(ug1 == ug2);
	///////////////////////////

}

TEST(Ugraph, equal_toy) {
	
	ugraph g(7);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(5, 6);
	g.name("toy");

	ugraph g1(3);
	g1.add_edge(0, 1);
	g1.add_edge(0, 2);

	///////////
	g1 = g;
	//////////

	//////////////////////////////////////////////
	EXPECT_EQ	(7, g1.number_of_vertices());
	EXPECT_TRUE	(g1.is_edge(0, 1));
	EXPECT_TRUE	(g1.is_edge(1, 2));
	EXPECT_TRUE	(g1.is_edge(2, 3));
	EXPECT_TRUE	(g1.is_edge(0, 3));
	EXPECT_TRUE	(g1.is_edge(5, 6));
	EXPECT_FALSE(g1.is_edge(0, 2));
	EXPECT_STREQ("toy", g1.name().c_str());
	//////////////////////////////////////////////

}

TEST(Ugraph, degree) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	ugraph g(path + "sample.clq");

	EXPECT_EQ	(7, g.number_of_vertices());
	EXPECT_EQ	(11, g.number_of_edges());
	
	/////////////////////////////
	EXPECT_EQ	( 3, g.degree(1) );
	/////////////////////////////
	
	//define a subset of vertices
	bitarray bbset	(g.number_of_vertices());
	bbset.set_bit	(0);
	bbset.set_bit	(1);
	bbset.set_bit	(6);

	//////////////////////////////////
	EXPECT_EQ	(1, g.degree(1, bbset) );		//degree of 1 in the subset {0,1,6}
	/////////////////////////////////

	//remove vertex {6} from the subset
	bbset.erase_bit(6);
	
	//////////////////////////////////
	EXPECT_EQ	(1, g.degree(1, bbset) );		//same result, the negihbor is vertex {0} not {6}
	//////////////////////////////////

}

TEST(Ugraph, degree_up) {

	const int NV = 40;
	ugraph ug(NV);

	ug.add_edge(2, 3);
	ug.add_edge(2, 5);
	ug.add_edge(27, 30);
	ug.add_edge(38, 39);

	///////////////////////////////
	EXPECT_EQ(2, ug.degree_up(2));
	EXPECT_EQ(0, ug.degree_up(10));
	EXPECT_EQ(1, ug.degree_up(38));
	EXPECT_EQ(1, ug.degree_up(27));
	////////////////////////////////

}



TEST(Ugraph, degree_dimacs) {
	
	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	ugraph g(path + "brock200_1.clq");


	EXPECT_EQ		(200, g.number_of_vertices());
	EXPECT_EQ		(14834, g.number_of_edges());

	///////////////////////////////////////////
	EXPECT_FALSE(g.is_edge(0, 1));								//(1)	
	///////////////////////////////////////////

	//define a subset of vertices
	bitarray bbset (g.number_of_vertices());
	bbset.set_bit(0);
	bbset.set_bit(1);

	///////////////////////////////////////////
	EXPECT_EQ	(0, g.degree(1, bbset) );						//expected because of (1)
	///////////////////////////////////////////

	//set of all vertices
	bbset.set_bit(0, g.number_of_vertices() - 1);

	//////////////////////////////////////////////
	EXPECT_EQ(g.degree(1), g.degree(1, bbset));
	//////////////////////////////////////////////

}

//TODO - CONTINUE REFACTORIZING THE CODE (04/01/2025)

TEST(Ugraph, max_subgraph_degree) {
	
	ugraph g(100);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(1, 3);
	g.add_edge(2, 3);
	g.add_edge(78, 79);
	g.add_edge(79, 80);

	//induced subgraph by {0, 1, 2, 3}
	bitarray bbset(g.number_of_vertices());
	bbset.set_bit(0, 3);
	EXPECT_EQ(3, g.max_subgraph_degree(bbset));		

	//induced subgraph by {0, 1, 2}
	bbset.erase_bit();
	bbset.set_bit(0, 2);									//clears the rest of bits
	EXPECT_EQ(2, g.max_subgraph_degree(bbset));		

	//induced subgraph by {78, 79}
	bbset.erase_bit();
	bbset.set_bit(78, 79);									//clears the rest of bits				
	EXPECT_EQ(1, g.max_subgraph_degree(bbset));

}

TEST(Ugraph, density_sample) {

	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	ugraph ug(path + "sample.clq");

	const int NVexp = 7;
	const int NEexp = 11;

	auto NV = ug.number_of_vertices();
	auto NE = ug.number_of_edges();

	//////////////////////
	EXPECT_EQ(NEexp, NE);
	EXPECT_EQ(NVexp, NV);
	//////////////////////
	
	//expected density
	BITBOARD aux = NVexp * (NVexp - 1);
	double dexp = (2 * NEexp) / static_cast<double> (aux);

	////////////////////////////////
	EXPECT_EQ(dexp, ug.density());
	/////////////////////////////////
}

TEST(Ugraph, density_brock) {
		
	string path = PATH_GRAPH_TESTS_CMAKE_SRC_CODE;
	ugraph ug(path + "brock200_1.clq");

	const int NVexp = 200;
	const int NEexp = 14834;
	
	auto NV = ug.number_of_vertices();
	auto NE = ug.number_of_edges();
	
	//////////////////////
	EXPECT_EQ(NEexp, NE);
	EXPECT_EQ(NVexp, NV);
	//////////////////////

	//expected density
	BITBOARD aux = NVexp * (NVexp - 1);
	double dexp = (2 * NEexp) / static_cast<double> ( aux );
	
	/////////////////////////////////
	EXPECT_EQ(dexp, ug.density());
	/////////////////////////////////

}

TEST(Ugraph, create_subgraph) {
	
	const int NV = 10;

	ugraph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(5, 6);

	//set of vertices {2, 3, 5, 6}
	std::vector<int> lv;
	lv.push_back(2);  lv.push_back(3); lv.push_back(5); lv.push_back(6);

	////////////////////////////////
	ugraph ugi;
	int status = g.create_subgraph(ugi, lv);		 //G[set of vertices]
	////////////////////////////////

	////////////////////////////////
	ASSERT_EQ	(0, status);
	EXPECT_EQ	(4, ugi.number_of_vertices());
	EXPECT_EQ	(2, ugi.number_of_edges(true));
	EXPECT_TRUE	(ugi.is_edge(0,1));					//edge (2,3)
	EXPECT_TRUE	(ugi.is_edge(2,3));					//edge (5,6)
	////////////////////////////////

	////////////////////////////////
	status = g.create_subgraph(ugi, 0);				//G[N(0)] 
	////////////////////////////////

	////////////////////////////////
	ASSERT_EQ	(0, status);
	EXPECT_EQ	(2, ugi.number_of_vertices());
	EXPECT_EQ	(0, ugi.number_of_edges(true));
	////////////////////////////////

}


TEST(Ugraph, complement_graph){
		
	ugraph g(4);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(0,2);
	
	///////////////////////////////
	ugraph cg;
	g.create_complement(cg);
	///////////////////////////////

	////////////////////////////////
	EXPECT_TRUE	(cg.is_edge(0,3));
	EXPECT_TRUE	(cg.is_edge(1,3));
	EXPECT_TRUE	(cg.is_edge(2,3));
	EXPECT_FALSE(cg.is_edge(0, 1));
	EXPECT_FALSE(cg.is_edge(1, 2));
	EXPECT_EQ	(g.number_of_vertices(), cg.number_of_vertices());
	////////////////////////////////

}

TEST(Ugraph, remove_edges){
		
	ugraph g(4);
	g.add_edge(0,1);
	g.add_edge(1,2);
	g.add_edge(1,3);
	g.add_edge(0,2);

	//removes all edges with endpoint in {1}
	g.remove_edges(1);
	
	////////////////////////////////
	EXPECT_TRUE	( g.is_edge(0, 2) );
	EXPECT_FALSE( g.is_edge(1, 2) );
	EXPECT_FALSE( g.is_edge(1, 3) );
	EXPECT_FALSE( g.is_edge(0, 1) );
	EXPECT_EQ	(1, g.number_of_edges());		//edge {0, 2}
	////////////////////////////////

}

/////////////////////////////
//
// DISABLED TESTS
//
/////////////////////////////

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

	//g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood
	//g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood
	//g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood

/////////////////
//TESTS
	EXPECT_EQ(g.number_of_vertices(), NV + 3);
	EXPECT_EQ(g.number_of_edges(false), 4 + 6);

	int v = NV;
	EXPECT_TRUE(g.is_edge(v, 0));
	EXPECT_TRUE(g.is_edge(v, 3));
	EXPECT_FALSE(g.is_edge(v, 1));

	v = NV + 1;
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

	//g.add_vertex(&bbn);			//extends graph according to the @bbn neighborhood

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

		//g.add_vertex();	
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
