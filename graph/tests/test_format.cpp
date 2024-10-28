//test_format.cpp: tests for reading graphs in different ASCII formats
//
//update@23/01/19 (checked with new Graph_t types)
//update@29/10/22 (gml format)


#include <iostream>
#include "gtest/gtest.h"
#include "../graph.h"
#include "formats/dimacs_reader.h"
#include "utils/common_paths.h"
#include "graph/formats/graph_inout.h"

using namespace std;

//#define TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME

/*************************
*
*  GML FORMAT 			
*
*  fist_update@30/10/22
*  last_update@30/10/22
*
**************************/


TEST(G2gml, clique) {

	LOG_INFO("G2gml::clique------------------------");
		
	const int NV = 6;
	ugraph ug(NV);
	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(0, 3);
	ug.add_edge(1, 2);
	ug.add_edge(1, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 4);
	ug.add_edge(3, 5);
	ug.add_edge(4, 5);

	vint clq, vpart, vhat_map_ori;
	clq.push_back(0); clq.push_back(1); clq.push_back(2);

	/////////////////////////////////////////////////////////////////////
	int status = gio::yed::graph_to_gml_clique(ug, clq, "gclique");
	/////////////////////////////////////////////////////////////////////

	LOG_INFO("G2gml::clique-----------------------");
#ifdef	TEST_COL_AS_CLQ_ONE_TEST_AT_A_TIME
	LOG_ERROR("press any key to continue");
	cin.get();
#endif

}

TEST(G2gml, vertex_set) {
/////////////////
// colors the vertex set
	LOG_INFO("G2gml::vertex_set------------------------");

	const int NV = 6;
	ugraph ug(NV);
	ug.add_edge(0, 1);
	ug.add_edge(0, 2);
	ug.add_edge(0, 3);
	ug.add_edge(1, 2);
	ug.add_edge(1, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 4);
	ug.add_edge(3, 5);
	ug.add_edge(4, 5);

	vint vset;
	vset.push_back(0); vset.push_back(1); vset.push_back(2);

	/////////////////////////////////////////////////////////////////////
	int status = gio::yed::graph_to_gml(ug, vset, "gset", gio::yed::YELLOW);
	/////////////////////////////////////////////////////////////////////

	LOG_INFO("G2gml::vertex_set-----------------------");
#ifdef	TEST_COL_AS_CLQ_ONE_TEST_AT_A_TIME
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}


TEST(G2gml, normal_graph) {
	LOG_INFO("G2gml:normal_graph-----------------------------------");
	
	ugraph g(6);
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.add_edge(0, 3);
	g.add_edge(4, 5);

	int status = gio::yed::graph_to_gml(g, "test_graph_gml_1");
	ASSERT_EQ(status, 0);
		
	LOG_INFO("-----------------------------------------------------");
#ifdef	TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME
	LOG_INFO("press any key to continue");
	cin.get();
#endif
}

TEST(G2gml, layered_graph) {
	LOG_INFO("G2gml:layered_graph----------------------------------");

	ugraph g(10);
	g.add_edge(0, 1);
	g.add_edge(1, 3);
	g.add_edge(2, 4);
	g.add_edge(2, 5);
	g.add_edge(4, 9);
	g.add_edge(5, 8);

	vector<int> layers;
	layers.push_back(0); layers.push_back(1); layers.push_back(EMPTY_ELEM);
	layers.push_back(2); layers.push_back(3); layers.push_back(EMPTY_ELEM);
	layers.push_back(4); layers.push_back(5); layers.push_back(EMPTY_ELEM);
	layers.push_back(6); layers.push_back(7); layers.push_back(8); layers.push_back(9); layers.push_back(EMPTY_ELEM);
	
	int status = gio::yed::graph_to_gml_layered(g, layers,"test_graph_gml_lay");
	ASSERT_EQ(status, 0);


	LOG_INFO("-----------------------------------------------------");
#ifdef	TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME
	LOG_INFO("press any key to continue");
	cin.get();
#endif
}

TEST(G2gml, circular) {
	LOG_INFO("G2gml:circular----------------------------------");

	ugraph g(10);
	g.add_edge(0, 1);
	g.add_edge(1, 3);
	g.add_edge(2, 4);
	g.add_edge(2, 5);
	g.add_edge(4, 9);
	g.add_edge(5, 8);

	
	int status = gio::yed::graph_to_gml_circular(g, "test_graph_gml_circ");
	ASSERT_EQ(status, 0);


	LOG_INFO("-----------------------------------------------------");
#ifdef	TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME
	LOG_INFO("press any key to continue");
	cin.get();
#endif
}


TEST(GtoDimacs, DISABLED_read_RTN_edge_weighted){
///////////////////
// Conversion of RTN networks to DIMACS () 
//
// date@30/09/18, Paris //TODO@check with new data types!!
//
	LOG_INFO("GtoDimacs:read_RTN---------------------------------------");

	//string instance="daysall0-w.dat";
	string instance="in-SCi-r-t2-peel-f.txt";
	
	//GtoDim::read_dimacs_edge_weights(fullname);
	
	Graph_EW<ugraph, double> gew(TEST_GRAPH_DESKTOP_ETSIDI_I9 + instance);
	gew.print_data();
	EXPECT_EQ(3, gew.get_we(0,8));
	EXPECT_EQ(3, gew.get_we(0,9));
	EXPECT_EQ(4, gew.get_we(737,738));
	
	string clq("in-SCi-r-t2-peel-f.clq");	
	gew.write_dimacs(FILE_LOG(clq.c_str(), WRITE));			

	//I/O
	//gew.print_weights();
	
	LOG_INFO("----------------------------------------------------------------");
#ifdef	TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME
	LOG_INFO("press any key to continue");
	cin.get();
#endif
}


TEST(Graph_test, DISABLED_read_mmx){
///////////////////
// Undirected graphs read by directed graph class (all edges are non symmetrical)
	LOG_INFO("Graph_test:read_mmx---------------------------------------");


	string path= TEST_GRAPH_DESKTOP_ETSIDI_I9;

	ugraph g(path + "bio-yeast.mtx");
	EXPECT_EQ(1948,g.number_of_edges());
	EXPECT_EQ(1458,g.number_of_vertices());
	

	sparse_ugraph sg(path + "bio-yeast.mtx");
	EXPECT_EQ(1948,sg.number_of_edges());
	EXPECT_EQ(1458,sg.number_of_vertices());

	LOG_INFO("----------------------------------------------------------------");
#ifdef	TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME
	LOG_INFO("press any key to continue");
	cin.get();
#endif
}


TEST(Graph_test, DISABLED_read_all){
///////////////////
// Undirected graphs read by directed graph class (all edges are non symmetrical)
	LOG_INFO("Graph_test:read_all---------------------------------------");
	string path= TEST_GRAPH_DESKTOP_ETSIDI_I9;

	//DIMACS
	ugraph g1(path + "brock200_1.clq");
	cout<<"read DIMACS:"; g1.print_data(); 
	EXPECT_EQ(200, g1.number_of_vertices());
		
	//MTX
	ugraph g2(path + "bio-yeast.mtx");
	cout<<"read MTX:"; g2.print_data(); 
	EXPECT_EQ(1458, g2.number_of_vertices());
	EXPECT_EQ(1948, g2.number_of_edges());

	LOG_INFO("----------------------------------------------------------------");
#ifdef	TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME
	LOG_INFO("press any key to continue");
	cin.get();
#endif
}

TEST(Graph_test, DISABLED_read_edgelist){
	LOG_INFO("Graph_test:read_edgelist---------------------------------------");
	string path= TEST_GRAPH_DESKTOP_ETSIDI_I9;

	//EDGES
	/*string filename(path + "bio-yeast-protein-inter.edges");
	cout<<"READING: "<<filename<<"-----------------------"<<endl;*/
	ugraph g1(path + "bio-yeast-protein-inter.edges");
	g1.print_data();

	EXPECT_EQ(2114, g1.number_of_vertices());
	EXPECT_EQ(4406, g1.number_of_edges());
	EXPECT_TRUE(g1.is_edge(2111, 2110));
	EXPECT_TRUE(g1.is_edge(2110, 2111));
	EXPECT_TRUE(g1.is_edge(1, 35));

	//cout<<"FINISHED READING"<<"-----------------------"<<endl;

	//filename="ia-southernwomen.edges";
	//cout<<"READING: "<<filename<<"-----------------------"<<endl;
	ugraph g2(path + "ia-southernwomen.edges");
	g2.print_data();

	EXPECT_EQ(18, g2.number_of_vertices());
	EXPECT_EQ(75, g2.number_of_edges());
	EXPECT_TRUE(g2.is_edge(0, 1));
	EXPECT_TRUE(g2.is_edge(17, 13));
	

	//cout<<"FINISHED READING"<<"-----------------------"<<endl;


	/*filename="bio-MUTAG_g1.edges";
	cout<<"READING: "<<filename<<"-----------------------"<<endl;*/
	ugraph g3(path + "bio-MUTAG_g1.edges");
	g3.print_data();

	EXPECT_EQ(23, g3.number_of_vertices());
	EXPECT_EQ(54, g3.number_of_edges());
	EXPECT_TRUE(g3.is_edge(0, 1));
	EXPECT_TRUE(g3.is_edge(20, 22));
	

	LOG_INFO("----------------------------------------------------------------");
#ifdef	TEST_GRAPH_FORMAT_ONE_TEST_AT_A_TIME
	LOG_INFO("press any key to continue");
	cin.get();
#endif
}


//TEST(Graph_test, write_edgelist){
////////////////////
//// reads as undirected graph a file with edges repeated
//// writes it correctly 
//	LOG_INFO("Graph_test:write_edgelist---------------------------------------");
//	string path=TEST_GRAPH_PATH_TEST_ETSIDI;
//
//	/*string filename="bio-MUTAG_g1.edges";					
//	cout<<"READING: "<<filename<<"-----------------------"<<endl;*/
//	ugraph g1(path + "bio-MUTAG_g1.edges");
//	g1.print_data();
//	EXPECT_EQ(54,g1.number_of_edges());
//
//	ofstream f("bio-MUTAG_g1_u.edges");
//	g1.write_EDGES(f);
//	f.close();
//
//	g1.set_graph(path + "bio-MUTAG_g1_u.edges");
//	g1.print_data();
//	EXPECT_EQ(27,g1.number_of_edges());
//
//
//	////////////////////////
//	/*filename="ia-southernwomen.edges";	
//	cout<<"READING: "<<filename<<"-----------------------"<<endl;*/
//	g1.set_graph(path +  "ia-southernwomen.edges");
//	g1.print_data();
//	EXPECT_EQ(75,g1.number_of_edges());
//
//	f.open("ia-southernwomen_u.edges");
//	g1.write_EDGES(f);
//	f.close();
//
//	g1.set_graph(path + "ia-southernwomen_u.edges");
//	g1.print_data();
//	EXPECT_EQ(64,g1.number_of_edges());
//
//	LOG_INFO("----------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE...");
//	cin.get();
//}

//
//
//TEST(Graph_test, write_edgelist_I){
////////////////////
//// reads as undirected graph a file with edges repeated
//// writes it correctly 
//
//	string filename="aff-digg.mtx";					
//	cout<<"READING: "<<filename<<"-----------------------"<<endl;
//	ugraph g1(filename);
//	g1.print_data();
//	//EXPECT_EQ(54,g1.number_of_edges());
//
//	ofstream f("aff-digg_u.edges");
//	g1.write_EDGES(f);
//	f.close();
//
//	g1.set_graph("aff-digg_u.edge");
//	g1.print_data();
//	EXPECT_EQ(27,g1.number_of_edges());
//
//}








