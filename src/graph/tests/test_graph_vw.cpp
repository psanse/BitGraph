/**
 * @file test_graph_vw.cpp
 * @brief Unit tests for vertex-weighted graph classes.
 *
 * Contains tests for construction, assignment, graph initialization,
 * vertex weights, weight transformations, DIMACS I/O, and graph generation
 * for vertex-weighted graph types.
 *
 * @author pss
 *
 * @details Created 09/10/2016, last updated 10/10/2026.
 * 
 * TODO: check tests at the end of the file (some are disabled) 
 */

#include "graph/ugraph_vertex_weighted.h"
#include "graph/algorithms/graph_gen.h"
#include "gtest/gtest.h"

#include "utils/logger.h"
#include "utils/math_utils.h"
#include <iostream>
#include <cstdio>
#include <fstream>


using namespace std;
using namespace bitgraph;

class UGraphWTest : public ::testing::Test {
protected:
	void SetUp() override {
		gw.reset(NV, static_cast<ugraph_wi::Weight>(0.0));
		gw.add_edge(0, 1);
		gw.add_edge(0, 2);
		gw.set_weight(0, 1);
		gw.set_weight(1, 2);
		gw.set_weight(2, 3);
		gw.set_name("toy_weighted");
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 5;
	ugraph_wi gw;											//undirected graph with integer weights
};

TEST_F(UGraphWTest, scale_transformation) {

	//scale transformation (mul by *5 all the weights)
	gw.transform_weights(utils::Scale<decltype(gw)::Weight>(5));

	EXPECT_EQ(5,  gw.weight(0));
	EXPECT_EQ(10, gw.weight(1));	
}

TEST_F(UGraphWTest, contruction) {

	EXPECT_EQ	(NV, gw.num_vertices());
	EXPECT_EQ	(2,  gw.num_edges());

	//vertex weights
	EXPECT_EQ	(1, gw.weight(0));
	EXPECT_EQ	(2, gw.weight(1));
	EXPECT_EQ	(3, gw.weight(2));

	//edges
	EXPECT_TRUE	(gw.is_edge(0, 1));
	EXPECT_TRUE	(gw.is_edge(0, 2));
	EXPECT_FALSE(gw.is_edge(1, 2));

	EXPECT_STREQ("toy_weighted", gw.name().c_str());
}

TEST_F(UGraphWTest, copy_constructor) {

	ugraph_wi gw_copy(gw);

	//////////////////////////////////
	EXPECT_EQ	(NV, gw_copy.num_vertices());
	EXPECT_EQ	(2, gw_copy.num_edges());

	//vertex weights
	EXPECT_EQ	(1, gw_copy.weight(0));
	EXPECT_EQ	(2, gw_copy.weight(1));
	EXPECT_EQ	(3, gw_copy.weight(2));

	//edges
	EXPECT_TRUE	(gw_copy.is_edge(0, 1));
	EXPECT_TRUE	(gw_copy.is_edge(0, 2));
	EXPECT_FALSE(gw_copy.is_edge(1, 2));

	EXPECT_STREQ("toy_weighted", gw_copy.name().c_str());
	//////////////////////////////////

}

TEST_F(UGraphWTest, assignment) {

	ugraph_wi gw1;
	gw1 = gw;

	//////////////////////////////////
	EXPECT_EQ(NV, gw1.num_vertices());
	EXPECT_EQ(2, gw1.num_edges());

	//vertex weights
	EXPECT_EQ(1, gw1.weight(0));
	EXPECT_EQ(2, gw1.weight(1));
	EXPECT_EQ(3, gw1.weight(2));

	//edges
	EXPECT_TRUE(gw1.is_edge(0, 1));
	EXPECT_TRUE(gw1.is_edge(0, 2));
	EXPECT_FALSE(gw1.is_edge(1, 2));

	EXPECT_STREQ("toy_weighted", gw1.name().c_str());
	//////////////////////////////////

}

TEST_F(UGraphWTest, init) {

	//inits graph - unit weights and clears old name
	gw.reset(3);

	///////////////////////////////
	EXPECT_EQ	(1, gw.weight(0));
	EXPECT_EQ	(1, gw.weight(1));
	EXPECT_EQ	(1, gw.weight(2));
	EXPECT_TRUE	(gw.name().empty());
	/////////////////////////////////
}

TEST_F(UGraphWTest, reset) {

	//inits graph - unit weights and clears old name
	gw.reset(3, 1, "toy");

	///////////////////////////////
	EXPECT_EQ	(1, gw.weight(0));
	EXPECT_EQ	(1, gw.weight(1));
	EXPECT_EQ	(1, gw.weight(2));
	EXPECT_STREQ("toy", gw.name().c_str());
	/////////////////////////////////
}

TEST_F(UGraphWTest, reset_clears_graph)
{
	ASSERT_EQ(2, gw.num_edges());

	gw.reset(3, 2, "reset_graph");

	EXPECT_EQ(3, gw.num_vertices());
	EXPECT_EQ(0, gw.num_edges());

	EXPECT_EQ(2, gw.weight(0));
	EXPECT_EQ(2, gw.weight(1));
	EXPECT_EQ(2, gw.weight(2));

	EXPECT_STREQ("reset_graph", gw.name().c_str());
}

TEST_F(UGraphWTest, DISABLED_printing) {
//visual test - default disabled

	gw.print_data();
	gw.print_edges();
	gw.print_weights();
	LOG_INFO("press any key to continue");	
	cin.get();

}

TEST_F(UGraphWTest, unit_weighted)
{
	gw.set_weight(1);

	EXPECT_TRUE(gw.is_unit_weighted());

	gw.set_weight(2, 3);

	EXPECT_FALSE(gw.is_unit_weighted());
}

TEST_F(UGraphWTest, maximum_weight)
{
	vertex_t v = BBObject::noBit;

	const auto max_weight = gw.maximum_weight(v);

	EXPECT_EQ(3, max_weight);
	EXPECT_EQ(2, v);
}

TEST(UGraphW, modulus_weights)
{
	ugraph_wi gw;
	gw.reset(201);

	gw.set_modulus_weights(200);


	EXPECT_EQ(2, gw.weight(0));
	EXPECT_EQ(3, gw.weight(1));
	EXPECT_EQ(200, gw.weight(198));
	EXPECT_EQ(1, gw.weight(199));
	EXPECT_EQ(2, gw.weight(200));
}

TEST_F(UGraphWTest, transform_weights_ignores_no_weight)
{
	gw.set_weight(1, ugraph_wi::NO_WEIGHT);

	gw.transform_weights(
		utils::Scale<ugraph_wi::weight_t>(5));

	EXPECT_EQ(5, gw.weight(0));
	EXPECT_EQ(ugraph_wi::NO_WEIGHT, gw.weight(1));
	EXPECT_EQ(15, gw.weight(2));
}

TEST(UGraphW, read_dimacs_inline_weights)
{
	const std::string filename = "test_vw_inline.clq";

	{
		std::ofstream f(filename);

		f << "c weighted test graph\n";
		f << "p edge 3 2\n";
		f << "n 1 10\n";
		f << "n 2 20\n";
		f << "n 3 30\n";
		f << "e 1 2\n";
		f << "e 2 3\n";
	}

	ugraph_wi gw;

	ASSERT_TRUE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_EQ(3, gw.num_vertices());
	EXPECT_EQ(2, gw.num_edges());

	EXPECT_EQ(10, gw.weight(0));
	EXPECT_EQ(20, gw.weight(1));
	EXPECT_EQ(30, gw.weight(2));

	EXPECT_TRUE(gw.is_edge(0, 1));
	EXPECT_TRUE(gw.is_edge(1, 2));
	EXPECT_FALSE(gw.is_edge(0, 2));

	std::remove(filename.c_str());
}

TEST(UGraphW, read_dimacs_v_weight_format)
{
	const std::string filename = "test_vw_v_format.clq";

	{
		std::ofstream f(filename);

		f << "p edge 2 1\n";
		f << "v 1 7\n";
		f << "v 2 11\n";
		f << "e 1 2\n";
	}

	ugraph_wi gw;

	ASSERT_TRUE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_EQ(7, gw.weight(0));
	EXPECT_EQ(11, gw.weight(1));
	EXPECT_TRUE(gw.is_edge(0, 1));

	std::remove(filename.c_str());
}

TEST(UGraphW, read_dimacs_default_unit_weights)
{
	const std::string filename = "test_vw_unit.clq";

	{
		std::ofstream f(filename);

		f << "p edge 3 2\n";
		f << "e 1 2\n";
		f << "e 2 3\n";
	}

	ugraph_wi gw;

	ASSERT_TRUE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_TRUE(gw.is_unit_weighted());

	EXPECT_EQ(1, gw.weight(0));
	EXPECT_EQ(1, gw.weight(1));
	EXPECT_EQ(1, gw.weight(2));

	std::remove(filename.c_str());
}

TEST(UGraphW, read_dimacs_rejects_self_loop)
{
	const std::string filename = "test_vw_self_loop.clq";

	{
		std::ofstream f(filename);

		f << "p edge 3 1\n";
		f << "e 2 2\n";
	}

	ugraph_wi gw;

	EXPECT_FALSE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	// Failure leaves the graph empty.
	EXPECT_EQ(0, gw.num_vertices());

	std::remove(filename.c_str());
}

TEST(UGraphW, read_dimacs_rejects_invalid_vertex)
{
	const std::string filename = "test_vw_bad_vertex.clq";

	{
		std::ofstream f(filename);

		f << "p edge 3 1\n";
		f << "e 1 4\n";
	}

	ugraph_wi gw;

	EXPECT_FALSE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_EQ(0, gw.num_vertices());

	std::remove(filename.c_str());
}

TEST(UGraphW, read_dimacs_rejects_malformed_edge)
{
	const std::string filename = "test_vw_bad_edge.clq";

	{
		std::ofstream f(filename);

		f << "p edge 3 1\n";
		f << "e 1 x\n";
	}

	ugraph_wi gw;

	EXPECT_FALSE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_EQ(0, gw.num_vertices());

	std::remove(filename.c_str());
}

TEST(UGraphW, read_dimacs_rejects_missing_edges)
{
	const std::string filename = "test_vw_missing_edges.clq";

	{
		std::ofstream f(filename);

		f << "p edge 3 2\n";
		f << "e 1 2\n";
	}

	ugraph_wi gw;

	EXPECT_FALSE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_EQ(0, gw.num_vertices());

	std::remove(filename.c_str());
}


TEST(UGraphW, read_dimacs_rejects_extra_edges)
{
	const std::string filename = "test_vw_extra_edges.clq";

	{
		std::ofstream f(filename);

		f << "p edge 3 1\n";
		f << "e 1 2\n";
		f << "e 2 3\n";
	}

	ugraph_wi gw;

	EXPECT_FALSE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_EQ(0, gw.num_vertices());

	std::remove(filename.c_str());
}

TEST_F(UGraphWTest, dimacs_round_trip)
{
	const std::string filename = "test_vw_roundtrip.clq";

	{
		std::ofstream f(filename);
		ASSERT_TRUE(f.good());

		gw.write_dimacs(f);
	}

	ugraph_wi copy;

	ASSERT_TRUE(copy.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	ASSERT_EQ(gw.num_vertices(), copy.num_vertices());
	ASSERT_EQ(gw.num_edges(), copy.num_edges());

	for (vertex_t v = 0; v < gw.num_vertices(); ++v) {
		EXPECT_EQ(gw.weight(v), copy.weight(v));
	}

	EXPECT_TRUE(copy.is_edge(0, 1));
	EXPECT_TRUE(copy.is_edge(0, 2));
	EXPECT_FALSE(copy.is_edge(1, 2));

	std::remove(filename.c_str());
}

TEST(UGraphW, read_dimacs_edgeless_graph)
{
	const std::string filename = "test_vw_edgeless.clq";

	{
		std::ofstream f(filename);
		f << "p edge 4 0\n";
	}

	ugraph_wi gw;

	ASSERT_TRUE(gw.read_dimacs(
		filename,
		ugraph_wi::weight_file_extension::none));

	EXPECT_EQ(4, gw.num_vertices());
	EXPECT_EQ(0, gw.num_edges());
	EXPECT_TRUE(gw.is_unit_weighted());

	std::remove(filename.c_str());
}

TEST(UGraphW, constructor_from_file) {

	//read DIMACS graph from file - no DIMACS weights, sets default weights 
	ugraph_wi ugw(BITGRAPH_GRAPH_TEST_DATA_DIR  "sample.clq");

	const int NV = ugw.num_vertices();

	EXPECT_EQ(7, NV);
	EXPECT_EQ(1, ugw.weight(0));
	EXPECT_EQ(1, ugw.weight(1));
	EXPECT_EQ(1, ugw.weight(6));

}



TEST(UGraphW, gen_weights_dimacs){
		
	//read DIMACS graph from file - no DIMACS weights, sets default weights 
	ugraph_wi ugw (BITGRAPH_GRAPH_TEST_DATA_DIR  "brock200_1.clq");

	const int NV = ugw.num_vertices();

	EXPECT_EQ(200, NV);
	EXPECT_EQ(ugraph_wi::DEFAULT_WEIGHT, ugw.weight(0));
	EXPECT_EQ(ugraph_wi::DEFAULT_WEIGHT, ugw.weight(1));
	EXPECT_EQ(ugraph_wi::DEFAULT_WEIGHT, ugw.weight(199));

	//generate modulus weights 
	WeightGen< ugraph_wi >::create_weights(ugw, WeightGen<ugraph_wi>::WMOD, 200);
		
	EXPECT_EQ(2, ugw.weight(0));
	EXPECT_EQ(3, ugw.weight(1));
	EXPECT_EQ(200, ugw.weight(198));
	EXPECT_EQ(1, ugw.weight(199));			//unit weight - check!
}

TEST(UGraphW, gen_random) {
	
	const int NV = 50;
	const double density = 0.31;
		
	//creates random graph of size NV with modulus weights
	ugraph_wi ugw;
	RandomGen<ugraph_wi>::create_graph(ugw, NV, density);							/* empty vertex weights by default */
	WeightGen<ugraph_wi>::create_weights(ugw, WeightGen<ugraph_wi>::WMOD);
	
	/////////////////////
	EXPECT_EQ(NV, ugw.num_vertices());
	EXPECT_TRUE(ugw.density()< density + 0.001);
	EXPECT_TRUE(ugw.density()> density - 0.001);
	////////////////////
}

///////////////
//
// DISABLED TESTS
//
///////////////

//TEST(GraphW, DISABLED_gen_weights_to_file){
//////////////////
//// reads DIMACS file without weights and generates "*.d" file with weights
//// date of creation: 15/04/18
////
//// TODO@define more tests
//
//	LOG_INFO("GraphW:gen_weights_to_file---------------------------------------");
//
//	string path= BITGRAPH_GRAPH_TEST_DATA_DIR;
//	Graph_W<ugraph, int> ugw(path + "brock200_1.clq");
//	const int NV=ugw.graph().num_vertices();
//	
//	//generate weights WDEG mode
//	WeightGen< Graph_W<ugraph, int> >::create_weights(ugw, 
//													WeightGen<Graph_W<ugraph, int>>::WMOD, 
//													DEFAULT_WEIGHT_MODULUS,
//													".d", 
//													TEST_GRAPH_PATH_LOG			
//													);
//		
//
//	/////////////////////
//	//TESTS
//	EXPECT_EQ(NV, ugw.graph().num_vertices());
//	
//	LOG_INFO("GraphW: END gen_weights_to_file --------");
//#ifdef TEST_GRAPH_WEIGHTED_STEP_BY_STEP
//	LOG_ERROR("press any key to continue");
//	cin.get();
//#endif;
//}



//////////////////
//
// TODO@REFACTOR these tests with the new weighted / edge_weighted graph objects
//
/////////////////

//
//TEST(Extended_Weighted_Type, sort_by_weight){
//	LOG_INFO("Extended_Weighted_Type:sort_by_weight---------------------------------------");
//	const int NV=5;
//	ugraph_w ug(NV);
//	ug.name("my_wgraph");
//	
//	//set edges and cliques
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(0,2);
//	ug.init_wv();
//	ug.set_wv(0,1.75); ug.set_wv(1,3.5); ug.set_wv(2,2.27);						/* note: unit weights by default */
////	ug.print_weights();
//
//	GraphSort<ugraph_w> o(ug);
//	vint lv=o.new_order(gbbs::sort_t::MAX_WEIGHT, gbbs::place_t::PLACE_FL);
//	//com::stl::print_collection(lv, cout);
//	Decode d;
//	o.reorder(lv, d);
//	EXPECT_EQ(3.5, ug.get_wv(0));
//	EXPECT_EQ(2.27, ug.get_wv(1));
//	EXPECT_EQ(1.75, ug.get_wv(2));
//	EXPECT_EQ(1.0, ug.get_wv(3));
//	EXPECT_EQ(1.0, ug.get_wv(4));
//	ug.print_weights();
//	
//	//reverse ordering
//	lv=o.new_order(gbbs::sort_t::MAX_WEIGHT, gbbs::place_t::PLACE_LF);
//	o.reorder(lv, d);
//	EXPECT_EQ(3.5, ug.get_wv(4));
//	EXPECT_EQ(2.27, ug.get_wv(3));
//	EXPECT_EQ(1.75, ug.get_wv(2));
//	EXPECT_EQ(1.0, ug.get_wv(1));
//	EXPECT_EQ(1.0, ug.get_wv(0));
//			
//	LOG_INFO("----------------------------------------------------------------");
//	/*LOG_INFO("PRESS ANY KEY TO CONTINUE...");
//	cin.get();*/
//}
//
//TEST(Weighted, generate_and_write_weights){
//////////////////
//// test for weight generation 
//// date of creation: 1/3/18
//
//	LOG_INFO("Weighted:generate_and_write_weights---------------------------------------");
//	const int NV=5;
//	ugraph ug(NV);
//	ug.name("my_wgraph");
//	
//	//set edges and cliques
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(0,2);
//	
//	//generate weights WDEG mode
//	WeightGen<ugraph>::create_wgraph(ug, WeightGen<ugraph>::WDEG, ".d");
//	ug.print_weights();
//	EXPECT_EQ(ug.degree(0), ug.get_wv(0));
//	EXPECT_EQ(ug.degree(1), ug.get_wv(1));
//	EXPECT_EQ(ug.degree(2), ug.get_wv(2));
//	EXPECT_EQ(ug.degree(3), ug.get_wv(3));
//	EXPECT_EQ(ug.degree(4), ug.get_wv(4));
//	EXPECT_TRUE(ug.is_weighted_v());
//
//	//generate weights WMOD mode
//	WeightGen<ugraph>::create_wgraph(ug, WeightGen<ugraph>::WMOD, ".d");
//	ug.print_weights();
//	EXPECT_EQ(1, ug.get_wv(0));
//	EXPECT_EQ(2, ug.get_wv(1));
//	EXPECT_EQ(3, ug.get_wv(2));
//	EXPECT_EQ(4, ug.get_wv(3));
//	EXPECT_EQ(5, ug.get_wv(4));
//
//	//generate weights WMOD mode, param 2
//	WeightGen<ugraph>::create_wgraph(ug, WeightGen<ugraph>::WMOD, ".r","", 2);
//	ug.print_weights();
//	EXPECT_EQ(1, ug.get_wv(0));
//	EXPECT_EQ(2, ug.get_wv(1));
//	EXPECT_EQ(1, ug.get_wv(2));
//	EXPECT_EQ(2, ug.get_wv(3));
//	EXPECT_EQ(1, ug.get_wv(4));
//	
//	LOG_INFO("----------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE...");
//	cin.get();
//}
//
//TEST(Weighted, read_graph_from_dimacs_file){
//	LOG_INFO("Weighted:read_graph_from_dimacs_file-INIT--------------------------------");
//
//	ugraph ug("in101.wclq");
//	ug.print_data();
//	ug.print_weights();
//	EXPECT_EQ(10241, ug.get_wv(0));
//	EXPECT_EQ(3684, ug.get_wv(999));
//
//	ugraph ug1("brock200_1.clq");
//	ug1.print_data();
//	ug1.print_weights();					/* impossible */
//	EXPECT_FALSE(ug1.is_weighted_v());
//		
//	LOG_INFO("Weighted:read_graph_from_dimacs_file-END---------------------------------");
//	LOG_INFO("PRESS ANY KEY");
//	cin.get();
//}
//
//TEST(Weighted, max_weight){
//	LOG_INFO("Weighted:max_weight-INIT--------------------------------");
//	
//	const int NV=3;
//	ugraph ug(NV);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(0,2);
//	ug.init_wv();											//obligatory for manual introduction of weights
//
//	ug.set_wv(0,1); ug.set_wv(1,2); ug.set_wv(2,3);
//	int node;
//	EXPECT_EQ(3,ug.maximum_weight(node));
//	EXPECT_EQ(2,node);
//	
//	ug.set_wv(0,8); ug.set_wv(1,30); ug.set_wv(2,57);
//	EXPECT_EQ(57,ug.maximum_weight(node));
//	EXPECT_EQ(2,node);
//
//	LOG_INFO("Weighted:max_weight-END---------------------------------");
//	cin.get();
//}
//
//TEST(Weighted, graph_from_file){
/////////////////////
//// Undirected graphs read by directed graph class (all edges are non symmetrical)
////
//// Note: Currently reading two extensions *.w (weights according to degree) and *.d(weights 1-200 loop).
////	The actual choice depends on a switch in simple_graph.h file
//
//	LOG_INFO("Weighted:graph from file---------------------------------------");
//	graph g1("DSJC125_1g.col");
//	g1.print_data();
//	if(g1.is_weighted_v()){
//			g1.print_weights();
//	}
//
//	//Number of (directed) edges
//	EXPECT_EQ(125,g1.num_vertices());
//	EXPECT_EQ(736,g1.num_edges());
//	
//	int w1=g1.get_wv(0);
//	int w125=g1.get_wv(124);
//	EXPECT_EQ(5,w1);
//	EXPECT_EQ(1,w125);
//
//	//change weights
//	g1.set_wv(100,100);
//	EXPECT_EQ(100, g1.get_wv(100));
//	g1.print_weights(); 
//
//	//reading with no weights
//	graph g2("brock200_1.clq");
//	g2.print_data();
//	EXPECT_FALSE(g2.is_weighted_v());
//
//	//undirected graph
//	ugraph ug("DSJC125_1g.col");
//	ug.print_data();
//	w1=ug.get_wv(0);
//	w125=ug.get_wv(124);
//	EXPECT_EQ(5,w1);
//	EXPECT_EQ(1,w125);
//
////new graph: building from scratch 
//	ug.init(5);
//	ug.name("mygraph");
//	EXPECT_FALSE(ug.is_weighted_v());
//
//	//set edges and cliques
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(0,2);
//	ug.init_wv();											//obligatory for weighted graphs introduced manually (size of graphs must be known)
//	ug.set_wv(0,1); ug.set_wv(1,2); ug.set_wv(2,3);
//
//	EXPECT_EQ(1,ug.get_wv(0));
//	EXPECT_EQ(2,ug.get_wv(1));
//	EXPECT_EQ(3,ug.get_wv(2));
//	ug.print_data();
//	ug.print_weights();										//1, 2, 3, 1(default), 1(default)
//
//	LOG_INFO("--------------------------------------------------------------");
//}
//
//TEST(Weighted, copy){
//	LOG_INFO("Weighted:copy---------------------------------------");
//	graph g1("DSJC125_1g.col");
//
//	//copy constructor
//	graph g2=g1;
//	g2.print_data();
//	EXPECT_EQ(5,g2.get_wv(0));
//	EXPECT_EQ(1,g2.get_wv(124));
//
//	//assignment operation
//	graph g3;
//	g3=g1;
//	EXPECT_EQ(5,g3.get_wv(0));
//	EXPECT_EQ(1,g3.get_wv(124));
//			
//	LOG_INFO("--------------------------------------------------------------");
//}
//
//TEST(Weighted, sort_by_weight){
//	LOG_INFO("Weighted:sort_by_weight---------------------------------------");
//	const int NV=5;
//	ugraph ug(NV);
//	ug.name("my_wgraph");
//	
//	//set edges and cliques
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(0,2);
//	ug.init_wv();
//	ug.set_wv(0,1); ug.set_wv(1,3); ug.set_wv(2,2);
//	ug.print_weights();
//
//	GraphSort<ugraph> o(ug);
//	vint lv=o.new_order(gbbs::sort_t::MAX_WEIGHT, gbbs::place_t::PLACE_FL);
//	//com::stl::print_collection(lv, cout);
//	Decode d;
//	o.reorder(lv, d);
//	EXPECT_EQ(3, ug.get_wv(0));
//	EXPECT_EQ(2, ug.get_wv(1));
//	EXPECT_EQ(1, ug.get_wv(2));
//	EXPECT_EQ(1, ug.get_wv(3));
//	EXPECT_EQ(1, ug.get_wv(4));
//	ug.print_weights();
//	
//	//reverse ordering
//	lv=o.new_order(gbbs::sort_t::MAX_WEIGHT, gbbs::place_t::PLACE_LF);
//	o.reorder(lv, d);
//	EXPECT_EQ(3, ug.get_wv(4));
//	EXPECT_EQ(2, ug.get_wv(3));
//	EXPECT_EQ(1, ug.get_wv(2));
//	EXPECT_EQ(1, ug.get_wv(1));
//	EXPECT_EQ(1, ug.get_wv(0));
//			
//	LOG_INFO("----------------------------------------------------------------");
//}
//
//TEST(Weighted, generate_weights){
//////////////////
//// test for weight generation 
//// date of creation: 5/8/17
//
//	LOG_INFO("Weighted:generate_weights---------------------------------------");
//	const int NV=5;
//	ugraph ug(NV);
//	ug.name("my_wgraph");
//	
//	//set edges and cliques
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(0,2);
//	
//	//generate weights WDEG mode
//	WeightGen<ugraph>::create_wgraph(ug, WeightGen<ugraph>::WDEG);
//	ug.print_weights();
//	EXPECT_EQ(ug.degree(0), ug.get_wv(0));
//	EXPECT_EQ(ug.degree(1), ug.get_wv(1));
//	EXPECT_EQ(ug.degree(2), ug.get_wv(2));
//	EXPECT_EQ(ug.degree(3), ug.get_wv(3));
//	EXPECT_EQ(ug.degree(4), ug.get_wv(4));
//	EXPECT_TRUE(ug.is_weighted_v());
//
//	//generate weights WMOD mode
//	WeightGen<ugraph>::create_wgraph(ug, WeightGen<ugraph>::WMOD);
//	ug.print_weights();
//	EXPECT_EQ(1, ug.get_wv(0));
//	EXPECT_EQ(2, ug.get_wv(1));
//	EXPECT_EQ(3, ug.get_wv(2));
//	EXPECT_EQ(4, ug.get_wv(3));
//	EXPECT_EQ(5, ug.get_wv(4));
//
//	//generate weights WMOD mode, param 2
//	WeightGen<ugraph>::create_wgraph(ug, WeightGen<ugraph>::WMOD,2);
//	ug.print_weights();
//	EXPECT_EQ(1, ug.get_wv(0));
//	EXPECT_EQ(2, ug.get_wv(1));
//	EXPECT_EQ(1, ug.get_wv(2));
//	EXPECT_EQ(2, ug.get_wv(3));
//	EXPECT_EQ(1, ug.get_wv(4));
//	
//	LOG_INFO("----------------------------------------------------------------");
//}

