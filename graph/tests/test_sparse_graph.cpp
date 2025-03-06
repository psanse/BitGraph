/**
* @file test_sparse_graph.cpp
* @brief tests for sparse directed and undirected graphs
* @created ?
* @updated 29/12/20
* @last_update 27/01/2025
* @author pss
* 
* @TODO - ADD TESTS (05/01/25)
**/

#include "graph/graph.h"
#include "graph/algorithms/graph_gen.h"
#include "gtest/gtest.h"
#include <iostream>
#include <strstream>

using namespace std;



TEST(Graph_sparse, max_subgraph_degree) {
	
	const int NV = 100;

	sparse_ugraph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(1, 3);
	g.add_edge(2, 3);
	g.add_edge(78, 79);
	g.add_edge(79, 80);

	sparse_bitarray sg(g.number_of_vertices());
	
	sg.reset_bit(0, 3);
	////////////////////////////////////////
	EXPECT_EQ(3, g.max_subgraph_degree(sg));	//1(3)
	////////////////////////////////////////

	sg.reset_bit(0, 2);
	////////////////////////////////////////
	EXPECT_EQ(2, g.max_subgraph_degree(sg));	//1(2)
	////////////////////////////////////////
	
	sg.reset_bit(78, 79);						
	////////////////////////////////////////
	EXPECT_EQ(1, g.max_subgraph_degree(sg));	//78(1)
	////////////////////////////////////////
}

TEST(Graph_sparse,constructor_file){

	//undirected graphs read by directed graph class (all edges are non symmetrical)
	sparse_graph g1(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_1.clq");
	sparse_graph g2(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_2.clq");
	sparse_graph g3(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_3.clq");
	sparse_graph g4(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_4.clq");

	//number of vertices
	EXPECT_EQ	(200, g1.number_of_vertices());
	EXPECT_EQ	(200, g2.number_of_vertices());
	EXPECT_EQ	(200, g3.number_of_vertices());
	EXPECT_EQ	(200, g4.number_of_vertices());

	//number of (directed) edges
	EXPECT_EQ	(14834,g1.number_of_edges());
	EXPECT_EQ	(9876,g2.number_of_edges());
	EXPECT_EQ	(12048,g3.number_of_edges());
	EXPECT_EQ	(13089,g4.number_of_edges());

	//name
	EXPECT_STREQ("brock200_1.clq", g1.name().c_str());
	EXPECT_STREQ("brock200_2.clq", g2.name().c_str());
	EXPECT_STREQ("brock200_3.clq", g3.name().c_str());
	EXPECT_STREQ("brock200_4.clq", g4.name().c_str());

	//is directed
	EXPECT_NE	(g1.is_edge(3,2),g1.is_edge(2,3));
	EXPECT_NE	(g1.is_edge(4,2),g1.is_edge(2,4));

	//make graphs bidirected
	g1.make_bidirected();
	g2.make_bidirected();
	g3.make_bidirected();
	g4.make_bidirected();

	//number of (bidirected) edges
	EXPECT_EQ	(2*14834,g1.number_of_edges());
	EXPECT_EQ	(2*9876,g2.number_of_edges());
	EXPECT_EQ	(2*12048,g3.number_of_edges());
	EXPECT_EQ	(2*13089,g4.number_of_edges());
	
}

TEST(Graph_sparse,constructor_copy){

	sparse_graph g	(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_2.clq");
	sparse_graph g1	(g);

	////////////////////////////////////////////////////////////
	EXPECT_EQ	(g.number_of_edges(),g1.number_of_edges());
	EXPECT_EQ	(g.number_of_vertices(),g1.number_of_vertices());
	EXPECT_EQ	(g.name(),g1.name());
	EXPECT_EQ	("brock200_2.clq",g1.name());
	EXPECT_TRUE	(g == g1);
	////////////////////////////////////////////////////////////

	//make bidirected
	g1.make_bidirected();

	////////////////////////
	EXPECT_FALSE(g == g1);		
	///////////////////////
	
}

TEST(Graph_sparse_test,is_edge){
	
	sparse_graph g(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "sample.clq");
	
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
	
}

TEST(Graph_sparse, create_subgraph) {
	
	const int NV = 100;

	sparse_graph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(54, 55);
	g.add_edge(88, 89);
	g.add_edge(88, 90);
	g.add_edge(89, 90);

	//create graph with first 50 vertices
	sparse_graph g1;
	g.create_subgraph(50, g1);

	/////////////////////////////////////////////
	EXPECT_EQ	(50, g1.number_of_vertices());
	EXPECT_EQ	(4, g1.number_of_edges());			//0->1, 1->2, 2->3, 0->3
	/////////////////////////////////////////////

	//create graph with first 3 vertices
	g.create_subgraph(3, g1);
	
	/////////////////////////////////////////
	EXPECT_EQ	(3, g1.number_of_vertices());
	EXPECT_EQ	(2, g1.number_of_edges());			//0->1, 1->2, 2->3
	/////////////////////////////////////////

	//creating a wrong size subgraph - g1 remains unchanged
	EXPECT_EQ	(g1.number_of_vertices(), (g.create_subgraph(0, g1)).number_of_vertices());

}

TEST(Graph_sparse, shrink_to_fit) {

	const int NV = 100;

	sparse_graph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(2, 3);
	g.add_edge(0, 3);
	g.add_edge(54, 55);
	g.add_edge(88, 89);
	g.add_edge(88, 90);
	g.add_edge(89, 90);

	//reduces graph to 50 vertices
	g.shrink_to_fit(50);

	/////////////////////////////////////////
	EXPECT_EQ	(50, g.number_of_vertices());
	EXPECT_EQ	(4, g.number_of_edges());			//0->1, 1->2, 2->3, 0->3
	/////////////////////////////////////////
	
	//reduces graph to 3 vertices
	g.shrink_to_fit(3);
	
	/////////////////////////////////////////
	EXPECT_EQ	(3, g.number_of_vertices());
	EXPECT_EQ	(2, g.number_of_edges());			//0->1, 1->2
	EXPECT_TRUE	(g.is_edge(0, 1));
	EXPECT_TRUE	(g.is_edge(1, 2));
	EXPECT_FALSE(g.is_edge(2, 3));
	/////////////////////////////////////////
}

/////////////
//
// Ugraph_sparse TESTS
//
/////////////

TEST(Ugraph_sparse, number_of_edges) {

	const int NV = 140;

	sparse_ugraph g(NV);
	g.add_edge(1, 4);
	g.add_edge(0, 4);
	g.add_edge(5, 7);
	g.add_edge(0, 5);
	g.add_edge(7, 128);

	EXPECT_EQ(5, g.number_of_edges(true));			//uses current value of number of edges	
	EXPECT_EQ(5, g.number_of_edges(true));			//uses current value of number of edges	
	EXPECT_EQ(5, g.number_of_edges(false));			//recomputes number of edges
	
}

TEST(Ugraph_sparse, degree_induced){
	
	sparse_ugraph g(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "sample.clq");
	
	////////////////////////////
	EXPECT_EQ (3,g.degree(1));
	////////////////////////////
	
	//set of vertices {0, 1, 6}
	sparse_bitarray bbset(g.number_of_vertices());
	bbset.set_bit(0);
	bbset.set_bit(1);
	bbset.set_bit(6);

	/////////////////////////////////
	EXPECT_EQ (1, g.degree(1, bbset) );
	/////////////////////////////////

	//set of vertices {0, 1}
	bbset.erase_bit();
	bbset.set_bit(0); 
	bbset.set_bit(1);

	/////////////////////////////////
	EXPECT_EQ (1, g.degree(1, bbset) );
	/////////////////////////////////
}

TEST(Ugraph_sparse, degree_brock){
	
	sparse_ugraph g(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_1.clq");
	
	/////////////////////////////////
	EXPECT_FALSE(g.is_edge(0, 1));		//(1)
	/////////////////////////////////

	//set of vertices {0, 1}
	sparse_bitarray bbset(g.number_of_vertices());
	bbset.set_bit(0);
	bbset.set_bit(1);
	
	/////////////////////////////////
	EXPECT_EQ (0,g.degree(1, bbset));		//expected because of (1)
	/////////////////////////////////

	//set of all the vertices 
	bbset.set_bit(0, g.number_of_vertices()-1);
	
	///////////////////////////////////////////////////////////
	EXPECT_EQ (g.neighbors(1).size(),g.degree(1, bbset));
	///////////////////////////////////////////////////////////
}

TEST(Ugraph_sparse, degree_up) {

	const int NV = 40;
	sparse_ugraph sug(NV);

	sug.add_edge(2, 3);
	sug.add_edge(2, 5);
	sug.add_edge(27, 30);
	sug.add_edge(38, 39);

	///////////////////////////////
	EXPECT_EQ(2, sug.degree_up(2));
	EXPECT_EQ(0, sug.degree_up(10));			
	EXPECT_EQ(1, sug.degree_up(38));
	EXPECT_EQ(1, sug.degree_up(27));
	////////////////////////////////

}

TEST(Ugraph_sparse,ugraph_from_file){

	sparse_ugraph g1(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_1.clq");
	sparse_ugraph g2(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_2.clq");
	sparse_ugraph g3(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_3.clq");
	sparse_ugraph g4(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_4.clq");

	//number of vertices
	EXPECT_EQ(200, g1.number_of_vertices());
	EXPECT_EQ(200, g2.number_of_vertices());
	EXPECT_EQ(200, g3.number_of_vertices());
	EXPECT_EQ(200, g4.number_of_vertices());

	//number of edges
	EXPECT_EQ	(14834,g1.number_of_edges());
	EXPECT_EQ	(9876,g2.number_of_edges());
	EXPECT_EQ	(12048,g3.number_of_edges());
	EXPECT_EQ	(13089,g4.number_of_edges());

	//name
	EXPECT_STREQ("brock200_1.clq", g1.name().c_str());
	EXPECT_STREQ("brock200_2.clq", g2.name().c_str());
	EXPECT_STREQ("brock200_3.clq", g3.name().c_str());
	EXPECT_STREQ("brock200_4.clq", g4.name().c_str());

	//is undirected
	EXPECT_EQ	(g1.is_edge(3,2),g1.is_edge(2,3));
	EXPECT_EQ	(g1.is_edge(4,2),g1.is_edge(2,4));

}


///////////////////////////
// 
// DISABLED TESTS -
// ¡TO BE CHECKED!
// 
///////////////////////////

TEST(Graph_sparse, DISABLED_write_to_file) {
	sparse_graph sg(1000);
	sg.add_edge(1, 3);
	sg.add_edge(5, 7);
	sg.add_edge(100, 999);
	sg.add_edge(0, 10);
	ofstream f("log.txt");
	sg.write_dimacs(f);
	f.close();
}

TEST(Ugraph_sparse, DISABLED_write_to_file){
	sparse_ugraph sug(1000);
	sug.add_edge(1,3);
	sug.add_edge(5,7);
	sug.add_edge(100,999);
	sug.add_edge(0,10);
	ofstream f("logsug.txt");
	sug.write_dimacs(f);
	f.close();

}

TEST(Ugraph_sparse, DISABLED_adjacency_matrix){
	
	const int NV_INF=100, NV_SUP=1000, INC_SIZE=100, REP_MAX=10;
    const double DEN_INF=.1,DEN_SUP=.9, INC_DENSITY=.1;

    for(int N=NV_INF; N<NV_SUP; N+=INC_SIZE)  {
        for(double P=DEN_INF; P<DEN_SUP;P+=INC_DENSITY){
            for(int rep=0;rep<REP_MAX;rep++){
				 cout<<"--------------------------------------------------------"<<endl;
				sparse_ugraph ug;
				SparseRandomGen<sparse_ugraph>().create_ugraph(ug,(std::size_t)N,P);
				sparse_ugraph ug2(ug);
			

				//deletes higher triangle
				for(int i=0; i<N; i++){
					ug.neighbors(i).erase_bit(i, N-1);
				}

				//expected number of edges is 0 because it is only reading upper triangle
				ASSERT_EQ(0, ug.number_of_edges(false));
				
				//deletes lower triangle
				BITBOARD E=ug2.number_of_edges(false);
				for(int i=0; i<N; i++){
					ug2.neighbors(i).erase_bit(0, i);
				}

				//expected number of edges is the same as before (only reading upper triangle)
				ASSERT_EQ(E, ug2.number_of_edges(false));
												
				cout<<"[N:"<<N<<" p:"<<P<<" r:"<<rep<<"]"<<endl;
			}
		}
	}
}
TEST(Graph_sparse, print_edges) {
	//not formulated as a real test, just visual inspection
	const int NV = 100;

	sparse_ugraph g(NV);
	g.add_edge(0, 1);
	g.add_edge(1, 2);
	g.add_edge(1, 3);
	g.add_edge(2, 3);
	g.add_edge(78, 79);
	g.add_edge(79, 80);

	//I/O 
	stringstream sstr;
	g.print_edges(sstr);

	EXPECT_FALSE(sstr.str().empty());

	//visual inspection
	//cout << sstr.str() << endl;
}
