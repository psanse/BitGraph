/**
 * @file test_ugraph_ew.cpp
 * @brief Unit tests for edge-weighted undirected graphs.
 *
 * Contains GoogleTest-based tests for construction, assignment,
 * graph initialization, edge weights, weight transformations,
 * DIMACS I/O, and graph generation for `Graph_EW<ugraph, WeightT>`.
 *
 * @author Pablo San Segundo (pss)
 *
 * @details Created 20/07/2018, last updated 12/10/2026.
 */

#include "gtest/gtest.h"
#include "graph/algorithms/graph_gen.h"	
#include "graph/ugraph_edge_weighted.h"
#include "utils/logger.h"	
#include "utils/math_utils.h"

#include <iostream>
#include <sstream>


using namespace bitgraph;

class UGraphEWTest : public ::testing::Test {
protected:
	UGraphEWTest()
		: ugew(NV)
	{}

	void SetUp() override {
		ugew.add_edge(0, 1, 1);
		ugew.add_edge(0, 2, 2);
		ugew.add_edge(1, 3, 3);
		ugew.set_name("toy_weighted");
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 5;
	ugraph_ewi ugew;											//undirected graph with integer weights
};

TEST_F(UGraphEWTest, basic_setup) {
	
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, ugew.weight(1, 4));
	EXPECT_EQ(1, ugew.weight(0, 1));
	EXPECT_EQ(2, ugew.weight(0, 2));
	EXPECT_EQ(3, ugew.weight(1, 3));
	EXPECT_EQ(3, ugew.num_edges());
	EXPECT_EQ(5, ugew.size());
	
}

TEST_F(UGraphEWTest, copy_constructor) {
	ugraph_ewi gcopy(ugew);

	EXPECT_EQ(NV, gcopy.num_vertices());
	EXPECT_EQ(ugew.num_edges(), gcopy.num_edges());

	//TODO... add more tests
}

TEST_F(UGraphEWTest, add_edge) {
		
	ugew.add_edge(1, 4, 6);					//(1,4) is a new edge

	EXPECT_TRUE	(ugew.is_edge(1, 4));
	EXPECT_EQ	(6, ugew.weight(1, 4));
	EXPECT_EQ	(4, ugew.num_edges());
}

TEST_F(UGraphEWTest, set_edge_weights) {

	ugew.set_weight(1, 3, 7);			//(1,3) is an existing edge	

	EXPECT_EQ(7, ugew.weight(1, 3) );
}

TEST_F(UGraphEWTest, transform_weights) {

	//multiply all weigths (vertex and edge-weights) by 5
	ugew.transform_weights([](int a) {return 5 * a;});

	EXPECT_EQ(5,  ugew.weight(0, 1));
	EXPECT_EQ(10, ugew.weight(0, 2));
	EXPECT_EQ(15, ugew.weight(1, 3));
	EXPECT_EQ(decltype(ugew)::NO_WEIGHT, ugew.weight(1, 4));		//NO_WEIGHT - non-edge
	

	EXPECT_EQ(decltype(ugew)::NO_WEIGHT, ugew.weight(1, 4));		//NO_WEIGHT - non-edge

	//same using the scale functor utility
	ugew.transform_weights(utils::Scale<decltype(ugew)::Weight>(5));
	EXPECT_EQ(25, ugew.weight(0, 1));
	EXPECT_EQ(50, ugew.weight(0, 2));
	EXPECT_EQ(75, ugew.weight(1, 3));

}

TEST_F(UGraphEWTest, generate_weights) {

	//generate modulus-weights on the edges 
	EdgeWeightGen<ugraph_ewi>::create_weights (
		ugew,
		EdgeWeightGen<ugraph_ewi>::WMOD,
		DEFAULT_WEIGHT_MODULUS);

	EXPECT_EQ(NV, ugew.graph().num_vertices());
	EXPECT_EQ(3, ugew.weight(0, 1));
	EXPECT_EQ(4, ugew.weight(0, 2));
	EXPECT_EQ(6, ugew.weight(1, 3));
	EXPECT_TRUE(ugew.is_consistent());  // validate_weight_matrix()
}


TEST(UGraphEW, constructor_file) {

	ugraph_ewi ugew(BITGRAPH_GRAPH_TEST_DATA_DIR "toy_ew_dimacs.txt");

	EXPECT_EQ(5, ugew.num_vertices());
	EXPECT_EQ(5, ugew.num_edges());

	//all vertex weights 
	EXPECT_EQ(10, ugew.weight(0, 0));
	EXPECT_EQ(20, ugew.weight(1, 1));
	EXPECT_EQ(30, ugew.weight(2, 2));
	EXPECT_EQ(40, ugew.weight(3, 3));
	EXPECT_EQ(50, ugew.weight(4, 4));

	//all (undirected) edge weights
	EXPECT_EQ(27, ugew.weight(0, 1));
	EXPECT_EQ(37, ugew.weight(0, 2));
	EXPECT_EQ(47, ugew.weight(0, 3));
	EXPECT_EQ(57, ugew.weight(2, 4));
	EXPECT_EQ(67, ugew.weight(3, 4));
}

TEST(UGraphEW, print_edges_formats) {
	ugraph_ewi graph(3);
	graph.add_edge(0, 1, 2);
	graph.add_edge(1, 2, 5);

	// Check virtual dispatch through the base as well as the returned stream.
	const Base_Graph_EW<ugraph, int>& base = graph;
	std::ostringstream edge_list;
	EXPECT_EQ(&edge_list, &base.print_edges(edge_list));
	EXPECT_EQ("[0]--(2)--[1]\n[1]--(5)--[2]\n", edge_list.str());

	std::ostringstream matrix;
	EXPECT_EQ(&matrix, &base.print_edges(
		matrix, ugraph_ewi::edge_print_format::edge_matrix, true));
	ASSERT_FALSE(matrix.str().empty());
	EXPECT_EQ('\n', matrix.str().back());

	// The undirected matrix lists each pair once, including missing edges.
	std::istringstream entries(matrix.str());
	std::string entry;
	ASSERT_TRUE(static_cast<bool>(entries >> entry));
	EXPECT_EQ("2", entry);
	ASSERT_TRUE(static_cast<bool>(entries >> entry));
	EXPECT_EQ("--", entry);
	ASSERT_TRUE(static_cast<bool>(entries >> entry));
	EXPECT_EQ("5", entry);
	EXPECT_FALSE(static_cast<bool>(entries >> entry));
}

//////////////////
//
// Graph_EW<ugraph, W> - construction
//
//////////////////

TEST(UGraphEW, default_constructor) {
	ugraph_ewi g;

	EXPECT_EQ(0, g.num_vertices());
	EXPECT_EQ(0u, g.size());
	EXPECT_TRUE(g.weights().empty());
	EXPECT_TRUE(g.is_consistent());
}

TEST(UGraphEW, constructor_num_vertices) {
	ugraph_ewi g(4);

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_EQ(0u, g.num_edges());
	EXPECT_TRUE(g.is_consistent());
	EXPECT_TRUE(g.is_unit_weighted());			// edgeless graph

	ASSERT_EQ(4u, g.weights().size());
	for (vertex_t v = 0; v < 4; ++v) {
		for (vertex_t w = 0; w < 4; ++w) {
			EXPECT_EQ(ugraph_ewi::NO_WEIGHT, g.weight(v, w));
		}
	}
}

TEST(UGraphEW, constructor_from_graph) {
	ugraph ug(4);
	ug.add_edge(0, 1);
	ug.add_edge(2, 3);

	ugraph_ewi g(ug);

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(1, 0));
	EXPECT_FALSE(g.is_edge(0, 2));

	// existing edges get DEFAULT_WEIGHT (symmetric), non-edges NO_WEIGHT
	EXPECT_EQ(ugraph_ewi::DEFAULT_WEIGHT, g.weight(0, 1));
	EXPECT_EQ(ugraph_ewi::DEFAULT_WEIGHT, g.weight(1, 0));
	EXPECT_EQ(ugraph_ewi::DEFAULT_WEIGHT, g.weight(2, 3));
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, g.weight(0, 2));
	EXPECT_TRUE(g.is_unit_weighted());
	EXPECT_TRUE(g.is_consistent());
}

TEST(UGraphEW, constructor_from_graph_and_matrix) {
	ugraph ug(3);
	ug.add_edge(0, 1);
	ug.add_edge(1, 2);

	const int N = ugraph_ewi::NO_WEIGHT;
	ugraph_ewi::weight_matrix_t m = {
		{N, 4, N},
		{4, N, 9},
		{N, 9, N} };

	ugraph_ewi g(ug, m);

	EXPECT_EQ(4, g.weight(0, 1));
	EXPECT_EQ(9, g.weight(2, 1));
	EXPECT_EQ(m, g.weights());
	EXPECT_TRUE(g.is_consistent());
}

TEST(UGraphEW, constructor_invalid_matrix_terminates) {
	ugraph ug(3);
	ug.add_edge(0, 1);
	const int N = ugraph_ewi::NO_WEIGHT;

	// edge without weight
	ugraph_ewi::weight_matrix_t missing = {
		{N, N, N},
		{N, N, N},
		{N, N, N} };
	EXPECT_DEATH(ugraph_ewi(ug, missing), "");

	// wrong dimensions
	ugraph_ewi::weight_matrix_t small = { {N, 1}, {1, N} };
	EXPECT_DEATH(ugraph_ewi(ug, small), "");
}

TEST(UGraphEW, constructor_asymmetric_matrix_terminates) {
	ugraph ug(3);
	ug.add_edge(0, 1);
	const int N = ugraph_ewi::NO_WEIGHT;

	// asymmetric weights on an undirected edge
	ugraph_ewi::weight_matrix_t asym = {
		{N, 4, N},
		{5, N, N},
		{N, N, N} };
	
	EXPECT_DEATH(ugraph_ewi(ug, asym), "");
}

TEST(UGraphEW, constructor_missing_file) {
	ugraph_ewi g(BITGRAPH_GRAPH_TEST_DATA_DIR "this_file_does_not_exist.txt");
	EXPECT_EQ(0, g.num_vertices());
}

TEST(UGraphEW, copy_and_move) {
	ugraph_ewi g(4);
	g.add_edge(0, 1, 7);
	g.add_edge(2, 3, 9);

	ugraph_ewi copy(g);
	EXPECT_EQ(g.weights(), copy.weights());

	// copies are independent
	copy.set_weight(0, 1, 100);
	EXPECT_EQ(7, g.weight(0, 1));
	EXPECT_EQ(100, copy.weight(1, 0));

	ugraph_ewi assigned;
	assigned = g;
	EXPECT_EQ(g.weights(), assigned.weights());

	ugraph_ewi moved(std::move(copy));
	EXPECT_EQ(100, moved.weight(0, 1));
	EXPECT_TRUE(moved.is_consistent());
}

TEST(UGraphEW, reset) {
	ugraph_ewi g(4);
	g.add_edge(0, 1, 7);

	g.reset(6, "fresh");

	EXPECT_EQ(6, g.num_vertices());
	EXPECT_EQ(0u, g.num_edges());
	EXPECT_EQ("fresh", g.name());
	EXPECT_FALSE(g.is_edge(0, 1));
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, g.weight(0, 1));
	EXPECT_TRUE(g.is_consistent());
}

//////////////////
//
// Graph_EW<ugraph, W> - edges and weights
//
//////////////////

TEST_F(UGraphEWTest, weights_are_symmetric) {
	EXPECT_EQ(1, ugew.weight(1, 0));
	EXPECT_EQ(2, ugew.weight(2, 0));
	EXPECT_EQ(3, ugew.weight(3, 1));
	EXPECT_TRUE(ugew.is_consistent());
}

TEST_F(UGraphEWTest, add_edge_symmetric_and_default_weight) {
	ugew.add_edge(2, 4);							// default weight

	EXPECT_TRUE(ugew.is_edge(4, 2));
	EXPECT_EQ(ugraph_ewi::DEFAULT_WEIGHT, ugew.weight(2, 4));
	EXPECT_EQ(ugraph_ewi::DEFAULT_WEIGHT, ugew.weight(4, 2));
	EXPECT_EQ(4u, ugew.num_edges());
	EXPECT_TRUE(ugew.is_consistent());
}

TEST_F(UGraphEWTest, add_edge_existing_replaces_weight) {
	{
		ugraph u(3);
		u.add_edge(0, 1);
		u.add_edge(1, 2);
		std::cerr << "DBG u after 2 edges: " << u.num_edges() << "\n";
		u.add_edge(0, 1);
		std::cerr << "DBG u after dup: " << u.num_edges() << "\n";
		std::cerr << "DBG ugew graph lazy before: " << ugew.graph().num_edges() << "\n";
		ugew.graph().add_edge(0, 1);
		std::cerr << "DBG ugew graph lazy after dup: " << ugew.graph().num_edges() << "\n";
	}
	ugew.add_edge(0, 1, 42);						// (0,1) already exists
	{
		const std::size_t n = ugew.num_edges();
		std::cerr << "DBG ew lazy after dup: " << n << "\n";
	}

	EXPECT_EQ(3u, ugew.num_edges());
	EXPECT_EQ(42, ugew.weight(0, 1));
	EXPECT_EQ(42, ugew.weight(1, 0));
	EXPECT_TRUE(ugew.is_consistent());
}

TEST_F(UGraphEWTest, set_weight_symmetric) {
	ugew.set_weight(3, 1, 11);						// reversed endpoints

	EXPECT_EQ(11, ugew.weight(1, 3));
	EXPECT_EQ(11, ugew.weight(3, 1));
	EXPECT_TRUE(ugew.is_consistent());
}

TEST_F(UGraphEWTest, set_weight_invalid_terminates) {
	EXPECT_DEATH(ugew.set_weight(1, 4, 5), "");					// non-edge
	EXPECT_DEATH(ugew.set_weight(0, 0, 5), "");					// self-loop
	EXPECT_DEATH(ugew.set_weight(0, 1, ugraph_ewi::NO_WEIGHT), "");	// NO_WEIGHT
}

TEST_F(UGraphEWTest, set_weights_uniform) {
	ugew.set_weights(8);

	EXPECT_EQ(8, ugew.weight(0, 1));
	EXPECT_EQ(8, ugew.weight(2, 0));
	EXPECT_EQ(8, ugew.weight(1, 3));
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, ugew.weight(1, 4));
	EXPECT_TRUE(ugew.is_consistent());
	EXPECT_FALSE(ugew.is_unit_weighted());

	ugew.set_weights();								// DEFAULT_WEIGHT
	EXPECT_TRUE(ugew.is_unit_weighted());
}

TEST_F(UGraphEWTest, set_weights_no_weight_terminates) {
	EXPECT_DEATH(ugew.set_weights(ugraph_ewi::NO_WEIGHT), "");
}

TEST_F(UGraphEWTest, set_weights_matrix) {
	const int N = ugraph_ewi::NO_WEIGHT;
	ugraph_ewi::weight_matrix_t m(NV, std::vector<int>(NV, N));
	m[0][1] = m[1][0] = 10;
	m[0][2] = m[2][0] = 20;
	m[1][3] = m[3][1] = 30;

	ugew.set_weights(m);

	EXPECT_EQ(m, ugew.weights());
	EXPECT_EQ(30, ugew.weight(3, 1));
	EXPECT_TRUE(ugew.is_consistent());
}

TEST_F(UGraphEWTest, set_weights_invalid_matrix_terminates) {
	const int N = ugraph_ewi::NO_WEIGHT;
	ugraph_ewi::weight_matrix_t m(NV, std::vector<int>(NV, N));
	m[0][1] = m[1][0] = 10;
	m[0][2] = m[2][0] = 20;
	m[1][3] = 30;									// asymmetric: m[3][1] missing

	EXPECT_DEATH(ugew.set_weights(m), "");

	// wrong dimensions
	ugraph_ewi::weight_matrix_t small(2, std::vector<int>(2, N));
	EXPECT_DEATH(ugew.set_weights(small), "");
}

TEST_F(UGraphEWTest, set_modulus_weights) {
	ugew.set_modulus_weights(10);					// w(v,w) = 2 + ((v + w) % 9)

	EXPECT_EQ(3, ugew.weight(0, 1));				// 2 + 1
	EXPECT_EQ(4, ugew.weight(0, 2));				// 2 + 2
	EXPECT_EQ(6, ugew.weight(1, 3));				// 2 + 4
	EXPECT_EQ(ugew.weight(0, 1), ugew.weight(1, 0));
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, ugew.weight(1, 4));
	EXPECT_TRUE(ugew.is_consistent());
}

TEST_F(UGraphEWTest, set_modulus_weights_range) {
	const int M = 4;
	ugew.add_edge(3, 4, 1);
	ugew.set_modulus_weights(M);

	for (vertex_t v = 0; v < NV; ++v) {
		for (vertex_t w = 0; w < NV; ++w) {
			if (ugew.is_edge(v, w)) {
				EXPECT_GE(ugew.weight(v, w), 2);
				EXPECT_LE(ugew.weight(v, w), M);
			}
		}
	}
}

TEST_F(UGraphEWTest, set_modulus_weights_invalid_terminates) {
	EXPECT_DEATH(ugew.set_modulus_weights(1), "");
}

TEST_F(UGraphEWTest, transform_weights_no_weight_terminates) {
	EXPECT_DEATH(
		ugew.transform_weights([](int) { return ugraph_ewi::NO_WEIGHT; }), "");
}

TEST_F(UGraphEWTest, is_unit_weighted) {
	EXPECT_FALSE(ugew.is_unit_weighted());			// weights 1, 2, 3

	ugraph_ewi unit(3);
	unit.add_edge(0, 1);
	unit.add_edge(1, 2);
	EXPECT_TRUE(unit.is_unit_weighted());

	unit.set_weight(1, 2, 5);
	EXPECT_FALSE(unit.is_unit_weighted());
}

TEST_F(UGraphEWTest, is_consistent_detects_graph_modification) {
	EXPECT_TRUE(ugew.is_consistent());

	// bypassing the wrapper breaks the weight/topology invariant
	ugew.graph().add_edge(2, 4);
	EXPECT_FALSE(ugew.is_consistent());
}

TEST_F(UGraphEWTest, accessors) {
	EXPECT_EQ("toy_weighted", ugew.name());
	EXPECT_EQ(NV, ugew.graph().num_vertices());
	EXPECT_TRUE(ugew.graph().is_edge(0, 1));

	ugew.set_path("some/path");
	EXPECT_EQ("some/path", ugew.path());

	ASSERT_EQ(static_cast<size_t>(NV), ugew.weights().size());
	EXPECT_EQ(1, ugew.weights()[0][1]);
}

TEST(UGraphEW, gen_random_edges) {
	ugraph_ewi g(20);

	g.gen_random_edges(0.5, 3);

	// G(n,m) model: m = round(p * C(n,2))
	EXPECT_EQ(95u, g.num_edges(false));
	EXPECT_TRUE(g.is_consistent());
	for (vertex_t v = 0; v < 20; ++v) {
		for (vertex_t w = v + 1; w < 20; ++w) {
			if (g.is_edge(v, w)) {
				EXPECT_EQ(3, g.weight(v, w));
				EXPECT_EQ(3, g.weight(w, v));
			}
		}
	}
}

TEST(UGraphEW, double_weights) {
	ugraph_ew g(3);
	g.add_edge(0, 1, 2.5);
	g.add_edge(1, 2, 0.5);

	EXPECT_DOUBLE_EQ(2.5, g.weight(1, 0));
	EXPECT_DOUBLE_EQ(0.5, g.weight(2, 1));
	EXPECT_DOUBLE_EQ(ugraph_ew::NO_WEIGHT, g.weight(0, 2));

	g.transform_weights([](double x) { return 2 * x; });
	EXPECT_DOUBLE_EQ(5.0, g.weight(0, 1));
	EXPECT_DOUBLE_EQ(1.0, g.weight(1, 2));
	EXPECT_TRUE(g.is_consistent());
}

//////////////////
//
// Graph_EW<ugraph, W> - I/O
//
//////////////////

TEST_F(UGraphEWTest, write_dimacs_undirected_edges_once) {
	std::ostringstream os;
	ugew.write_dimacs(os);
	const std::string s = os.str();

	// each undirected edge appears once, 1-based, with its weight
	EXPECT_NE(std::string::npos, s.find("e 1 2 1\n"));
	EXPECT_NE(std::string::npos, s.find("e 1 3 2\n"));
	EXPECT_NE(std::string::npos, s.find("e 2 4 3\n"));
	EXPECT_EQ(std::string::npos, s.find("e 2 1 "));
	EXPECT_EQ(std::string::npos, s.find("e 3 1 "));
	EXPECT_EQ(std::string::npos, s.find("e 4 2 "));
}

TEST(UGraphEW, print_edge_formats) {
	ugraph_ewi g(3);
	g.add_edge(0, 1, 2);

	std::ostringstream os;
	g.print_edge(0, 1, os, false);
	EXPECT_EQ("[0]--(2)--[1]", os.str());

	std::ostringstream os_none;
	g.print_edge(0, 2, os_none, true);
	EXPECT_EQ("[0]--(NO_EDGE)--[2]\n", os_none.str());
}

TEST(UGraphEW, print_data_tag) {
	ugraph_ewi g(3);
	g.add_edge(0, 1, 2);

	std::ostringstream os;
	EXPECT_EQ(&os, &g.print_data(true, os, true));
	EXPECT_NE(std::string::npos, os.str().find("[type: ew]"));
	EXPECT_EQ('\n', os.str().back());
}

TEST(UGraphEW, print_edges_induced) {
	ugraph_ewi g(4);
	g.add_edge(0, 1, 2);
	g.add_edge(1, 2, 5);
	g.add_edge(2, 3, 7);

	// subgraph overload lives in the base class (hidden by the derived overrides)
	Base_Graph_EW<ugraph, int>& base = g;
	vertex_set_t vs = { 0, 1, 2 };
	std::ostringstream os;
	base.print_edges(vs, os);

	const std::string s = os.str();
	EXPECT_NE(std::string::npos, s.find("(2)"));
	EXPECT_NE(std::string::npos, s.find("(5)"));
	EXPECT_EQ(std::string::npos, s.find("(7)"));		// edge (2,3) leaves the set
}
