/**
 * @file test_graph_ew.cpp
 * @brief Unit tests for the generic edge-weighted graph base class.
 *
 * Contains GoogleTest-based tests for `Base_Graph_EW<GraphT, WeightT>`,
 * covering construction, assignment, graph initialization, edge weights,
 * weight transformations, DIMACS I/O, and graph generation.
 *
 * These tests exercise the generic functionality provided by
 * `Base_Graph_EW` independently of facade-specific specializations.
 *
 * @author Pablo San Segundo (pss)
 *
 * @details Created 09/10/2016, last updated 12/10/2026.
 *
 */

// simple_graph_ew.h is not self-contained: the graph headers must come first.
#include "graph/simple_ugraph.h"
#include "graph/algorithms/graph_gen.h"
#include "graph/simple_graph_ew.h"
#include "gtest/gtest.h"

#include "utils/fixed_stack.h"
#include "graph/algorithms/ordering_map.h"
#include "utils/logger.h"
#include "utils/math_utils.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>


using namespace bitgraph;

namespace {

	using ew_int = Base_Graph_EW<graph, int>;
	using ew_dbl = Base_Graph_EW<graph, double>;
	using matrix_int = ew_int::weight_matrix_t;

	const int N = ew_int::NO_WEIGHT;

	/**
	 * @brief Temporary file (in the working directory) removed on destruction.
	 */
	class TempFile {
	public:
		TempFile(const std::string& name, const std::string& content)
			: path_(name)
		{
			std::ofstream f(path_.c_str());
			f << content;
		}
		~TempFile() {
			std::remove(path_.c_str());
		}
		TempFile(const TempFile&) = delete;
		TempFile& operator=(const TempFile&) = delete;
		const std::string& path() const { return path_; }
	private:
		std::string path_;
	};
}

class BaseGraphEWTest : public ::testing::Test {
protected:
	BaseGraphEWTest() : g(NV) {}

	void SetUp() override {
		g.add_edge(0, 1, 1);
		g.add_edge(0, 2, 2);
		g.add_edge(1, 3, 3);
		g.add_edge(3, 1, 8);						// directed: reverse edge, other weight
		g.set_name("toy_ew");
	}

	const int NV = 5;
	ew_int g;
};

/////////////////////
//
// Construction
//
/////////////////////

TEST(BaseGraphEW, default_constructor) {
	ew_int g;

	EXPECT_EQ(0, g.num_vertices());
	EXPECT_EQ(0u, g.size());
	EXPECT_TRUE(g.weights().empty());
	EXPECT_TRUE(g.is_consistent());
	EXPECT_TRUE(g.is_unit_weighted());
}

TEST(BaseGraphEW, constructor_num_vertices) {
	ew_int g(4);

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_EQ(0u, g.num_edges());
	EXPECT_TRUE(g.is_consistent());
	EXPECT_TRUE(g.is_unit_weighted());			// edgeless graph

	ASSERT_EQ(4u, g.weights().size());
	for (vertex_t v = 0; v < 4; ++v) {
		for (vertex_t w = 0; w < 4; ++w) {
			EXPECT_EQ(ew_int::NO_WEIGHT, g.weight(v, w));
		}
	}
}

TEST(BaseGraphEW, constructor_from_graph) {
	graph dg(4);
	dg.add_edge(0, 1);
	dg.add_edge(2, 3);

	ew_int g(dg);

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_FALSE(g.is_edge(1, 0));				// directed

	EXPECT_EQ(ew_int::DEFAULT_WEIGHT, g.weight(0, 1));
	EXPECT_EQ(ew_int::DEFAULT_WEIGHT, g.weight(2, 3));
	EXPECT_EQ(ew_int::NO_WEIGHT, g.weight(1, 0));
	EXPECT_EQ(ew_int::NO_WEIGHT, g.weight(0, 2));
	EXPECT_TRUE(g.is_unit_weighted());
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, constructor_from_graph_and_matrix) {
	graph dg(3);
	dg.add_edge(0, 1);
	dg.add_edge(1, 2);
	dg.add_edge(2, 1);

	matrix_int m = {
		{N, 4, N},
		{N, N, 9},
		{N, 6, N} };

	ew_int g(dg, m);

	EXPECT_EQ(4, g.weight(0, 1));
	EXPECT_EQ(9, g.weight(1, 2));
	EXPECT_EQ(6, g.weight(2, 1));				// asymmetric weights are valid
	EXPECT_EQ(N, g.weight(1, 0));
	EXPECT_EQ(m, g.weights());
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, constructor_invalid_matrix_terminates) {
	graph dg(3);
	dg.add_edge(0, 1);

	// edge without weight
	matrix_int missing = { {N, N, N}, {N, N, N}, {N, N, N} };
	EXPECT_DEATH(ew_int(dg, missing), "");

	// weight on a non-edge
	matrix_int extra = { {N, 4, N}, {N, N, 5}, {N, N, N} };
	EXPECT_DEATH(ew_int(dg, extra), "");

	// weight on the diagonal
	matrix_int diag = { {3, 4, N}, {N, N, N}, {N, N, N} };
	EXPECT_DEATH(ew_int(dg, diag), "");

	// wrong number of rows
	matrix_int rows = { {N, 4}, {N, N} };
	EXPECT_DEATH(ew_int(dg, rows), "");

	// non-square
	matrix_int ragged = { {N, 4, N}, {N, N}, {N, N, N} };
	EXPECT_DEATH(ew_int(dg, ragged), "");
}

TEST(BaseGraphEW, constructor_file) {
	TempFile f("bitgraph_ew_ctor.clq",
		"p edge 3 2\n"
		"e 1 2 5\n"
		"e 3 2 7\n");

	ew_int g(f.path());

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_EQ(2u, g.num_edges(false));
	EXPECT_EQ(5, g.weight(0, 1));
	EXPECT_EQ(7, g.weight(2, 1));
	EXPECT_FALSE(g.is_edge(1, 0));				// directed
	EXPECT_EQ(N, g.weight(1, 0));
}

TEST(BaseGraphEW, constructor_missing_file) {
	EXPECT_DEATH(
		{
			ew_int g("this_file_does_not_exist.clq");
		},
		"Failed to read DIMACS file"
	);
}

TEST_F(BaseGraphEWTest, copy_and_move) {
	ew_int copy(g);
	EXPECT_EQ(g.weights(), copy.weights());
	EXPECT_EQ(g.name(), copy.name());

	// copies are independent
	copy.set_weight(0, 1, 100);
	copy.add_edge(0, 4, 5);
	EXPECT_EQ(1, g.weight(0, 1));
	EXPECT_FALSE(g.is_edge(0, 4));

	ew_int assigned;
	assigned = g;
	EXPECT_EQ(g.weights(), assigned.weights());
	EXPECT_TRUE(assigned.is_edge(3, 1));

	ew_int moved(std::move(copy));
	EXPECT_EQ(100, moved.weight(0, 1));
	EXPECT_EQ(5, moved.weight(0, 4));
	EXPECT_TRUE(moved.is_consistent());
}

TEST_F(BaseGraphEWTest, reset) {
	g.reset(6, "fresh");

	EXPECT_EQ(6, g.num_vertices());
	EXPECT_EQ(0u, g.num_edges(false));
	EXPECT_EQ("fresh", g.name());
	EXPECT_FALSE(g.is_edge(0, 1));
	EXPECT_EQ(ew_int::NO_WEIGHT, g.weight(0, 1));
	EXPECT_TRUE(g.is_consistent());
}

/////////////////////
//
// Accessors
//
/////////////////////

TEST_F(BaseGraphEWTest, accessors) {
	EXPECT_EQ("toy_ew", g.name());
	EXPECT_EQ(NV, g.num_vertices());
	EXPECT_EQ(static_cast<size_t>(NV), g.size());
	EXPECT_EQ(NV, g.graph().num_vertices());
	EXPECT_TRUE(g.graph().is_edge(0, 1));

	g.set_path("some/path");
	EXPECT_EQ("some/path", g.path());

	ASSERT_EQ(static_cast<size_t>(NV), g.weights().size());
	EXPECT_EQ(1, g.weights()[0][1]);
	EXPECT_EQ(2, g.weights()[0][2]);
}

TEST_F(BaseGraphEWTest, is_consistent_detects_graph_modification) {
	EXPECT_TRUE(g.is_consistent());

	// bypassing the wrapper breaks the weight/topology invariant
	g.graph().add_edge(2, 4);
	EXPECT_FALSE(g.is_consistent());
}

/////////////////////
//
// Edges and weights
//
/////////////////////

TEST_F(BaseGraphEWTest, edges_are_directed) {
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_FALSE(g.is_edge(1, 0));
	EXPECT_EQ(1, g.weight(0, 1));
	EXPECT_EQ(N, g.weight(1, 0));

	// both directions of (1,3) are edges with independent weights
	EXPECT_EQ(3, g.weight(1, 3));
	EXPECT_EQ(8, g.weight(3, 1));
	EXPECT_EQ(4u, g.num_edges());
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, add_edge_default_weight) {
	g.add_edge(2, 4);

	EXPECT_TRUE(g.is_edge(2, 4));
	EXPECT_FALSE(g.is_edge(4, 2));
	EXPECT_EQ(ew_int::DEFAULT_WEIGHT, g.weight(2, 4));
	EXPECT_EQ(5u, g.num_edges());
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, add_edge_existing_replaces_weight) {
	g.add_edge(0, 1, 42);

	EXPECT_EQ(42, g.weight(0, 1));
	EXPECT_EQ(4u, g.num_edges());
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, set_weight) {
	g.set_weight(1, 3, 11);

	EXPECT_EQ(11, g.weight(1, 3));
	EXPECT_EQ(8, g.weight(3, 1));				// reverse direction untouched
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, set_weight_invalid_terminates) {
	EXPECT_DEATH(g.set_weight(1, 0, 5), "");					// non-edge (directed)
	EXPECT_DEATH(g.set_weight(1, 4, 5), "");					// non-edge
	EXPECT_DEATH(g.set_weight(2, 2, 5), "");					// self-loop
	EXPECT_DEATH(g.set_weight(0, 1, ew_int::NO_WEIGHT), "");	// NO_WEIGHT
}

TEST_F(BaseGraphEWTest, set_weights_uniform) {
	g.set_weights(8);

	EXPECT_EQ(8, g.weight(0, 1));
	EXPECT_EQ(8, g.weight(3, 1));
	EXPECT_EQ(N, g.weight(1, 0));
	EXPECT_EQ(N, g.weight(1, 4));
	EXPECT_TRUE(g.is_consistent());
	EXPECT_FALSE(g.is_unit_weighted());

	g.set_weights();							// DEFAULT_WEIGHT
	EXPECT_TRUE(g.is_unit_weighted());
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, set_weights_no_weight_terminates) {
	EXPECT_DEATH(g.set_weights(ew_int::NO_WEIGHT), "");
}

TEST_F(BaseGraphEWTest, set_weights_matrix) {
	matrix_int m(NV, std::vector<int>(NV, N));
	m[0][1] = 10;
	m[0][2] = 20;
	m[1][3] = 30;
	m[3][1] = 40;

	g.set_weights(m);

	EXPECT_EQ(m, g.weights());
	EXPECT_EQ(40, g.weight(3, 1));
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, set_weights_invalid_matrix_terminates) {
	matrix_int base(NV, std::vector<int>(NV, N));
	base[0][1] = 10;
	base[0][2] = 20;
	base[1][3] = 30;
	base[3][1] = 40;

	matrix_int missing = base;					// edge without weight
	missing[0][2] = N;
	EXPECT_DEATH(g.set_weights(missing), "");

	matrix_int extra = base;					// weight on a non-edge
	extra[4][0] = 7;
	EXPECT_DEATH(g.set_weights(extra), "");

	matrix_int diag = base;						// weight on the diagonal
	diag[2][2] = 7;
	EXPECT_DEATH(g.set_weights(diag), "");

	matrix_int small(2, std::vector<int>(2, N));	// wrong dimensions
	EXPECT_DEATH(g.set_weights(small), "");
}

TEST_F(BaseGraphEWTest, set_modulus_weights) {
	g.set_modulus_weights(10);					// w(v,w) = 2 + ((v + w) % 9)

	EXPECT_EQ(3, g.weight(0, 1));				// 2 + 1
	EXPECT_EQ(4, g.weight(0, 2));				// 2 + 2
	EXPECT_EQ(6, g.weight(1, 3));				// 2 + 4
	EXPECT_EQ(6, g.weight(3, 1));				// formula is symmetric
	EXPECT_EQ(N, g.weight(1, 0));				// non-edge
	EXPECT_EQ(N, g.weight(1, 4));
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, set_modulus_weights_range) {
	const int M = 4;
	g.add_edge(3, 4, 1);
	g.set_modulus_weights(M);

	for (vertex_t v = 0; v < NV; ++v) {
		for (vertex_t w = 0; w < NV; ++w) {
			if (g.is_edge(v, w)) {
				EXPECT_GE(g.weight(v, w), 2);
				EXPECT_LE(g.weight(v, w), M);
			}
		}
	}
}

TEST_F(BaseGraphEWTest, set_modulus_weights_invalid_terminates) {
	EXPECT_DEATH(g.set_modulus_weights(1), "");
	EXPECT_DEATH(g.set_modulus_weights(0), "");
}

TEST_F(BaseGraphEWTest, transform_weights) {
	g.transform_weights([](int a) { return 5 * a; });

	EXPECT_EQ(5, g.weight(0, 1));
	EXPECT_EQ(10, g.weight(0, 2));
	EXPECT_EQ(15, g.weight(1, 3));
	EXPECT_EQ(40, g.weight(3, 1));
	EXPECT_EQ(N, g.weight(1, 0));				// non-edges untouched
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, transform_weights_no_weight_terminates) {
	EXPECT_DEATH(
		g.transform_weights([](int) { return ew_int::NO_WEIGHT; }), "");
}

TEST_F(BaseGraphEWTest, is_unit_weighted) {
	EXPECT_FALSE(g.is_unit_weighted());			// weights 1, 2, 3, 8

	ew_int unit(3);
	unit.add_edge(0, 1);
	unit.add_edge(1, 2);
	EXPECT_TRUE(unit.is_unit_weighted());

	unit.set_weight(1, 2, 5);
	EXPECT_FALSE(unit.is_unit_weighted());
}

TEST(BaseGraphEW, gen_random_edges) {
	ew_int g(20);

	g.gen_random_edges(0.5, 3);

	EXPECT_TRUE(g.is_consistent());
	const std::size_t NE = g.num_edges(false);
	EXPECT_GT(NE, 0u);
	EXPECT_LT(NE, 20u * 19u);
	for (vertex_t v = 0; v < 20; ++v) {
		for (vertex_t w = 0; w < 20; ++w) {
			if (g.is_edge(v, w)) {
				EXPECT_EQ(3, g.weight(v, w));
			}
			else {
				EXPECT_EQ(N, g.weight(v, w));
			}
		}
	}
}

TEST(BaseGraphEW, double_weights) {
	ew_dbl g(3);
	g.add_edge(0, 1, 2.5);
	g.add_edge(2, 1, 0.5);

	EXPECT_DOUBLE_EQ(2.5, g.weight(0, 1));
	EXPECT_DOUBLE_EQ(0.5, g.weight(2, 1));
	EXPECT_DOUBLE_EQ(ew_dbl::NO_WEIGHT, g.weight(1, 0));

	g.transform_weights([](double x) { return 2 * x; });
	EXPECT_DOUBLE_EQ(5.0, g.weight(0, 1));
	EXPECT_DOUBLE_EQ(1.0, g.weight(2, 1));
	EXPECT_TRUE(g.is_consistent());
}

/////////////////////
//
// I/O
//
/////////////////////

TEST_F(BaseGraphEWTest, write_dimacs) {
	std::ostringstream os;
	EXPECT_EQ(&os, &g.write_dimacs(os));
	const std::string s = os.str();

	// directed edges, 1-based, with weight
	EXPECT_NE(std::string::npos, s.find("e 1 2 1\n"));
	EXPECT_NE(std::string::npos, s.find("e 1 3 2\n"));
	EXPECT_NE(std::string::npos, s.find("e 2 4 3\n"));
	EXPECT_NE(std::string::npos, s.find("e 4 2 8\n"));
	EXPECT_EQ(std::string::npos, s.find("e 2 1 "));
}

TEST_F(BaseGraphEWTest, write_read_dimacs_round_trip) {
	std::ostringstream os;
	g.write_dimacs(os);
	TempFile f("bitgraph_ew_roundtrip.clq", os.str());

	ew_int h;
	ASSERT_EQ(0, h.read_dimacs(f.path()));

	EXPECT_EQ(g.num_vertices(), h.num_vertices());
	EXPECT_EQ(g.weights(), h.weights());
	EXPECT_TRUE(h.is_consistent());
}

TEST(BaseGraphEW, read_dimacs_edge_weights) {
	TempFile f("bitgraph_ew_read.clq",
		"c a comment\n"
		"p edge 4 3\n"
		"e 1 2 5\n"
		"e 2 3 6\n"
		"e 4 1 7\n");

	ew_int g;
	ASSERT_EQ(0, g.read_dimacs(f.path()));

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_EQ(5, g.weight(0, 1));
	EXPECT_EQ(6, g.weight(1, 2));
	EXPECT_EQ(7, g.weight(3, 0));
	EXPECT_EQ(f.path(), g.name());
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, read_dimacs_invalid_input_fails) {
	ew_int g;

	// missing file
	EXPECT_EQ(-1, g.read_dimacs("this_file_does_not_exist.clq"));
	EXPECT_EQ(0, g.num_vertices());

	// not a DIMACS header
	TempFile header("bitgraph_ew_header.clq", "e 1 2 3\n");
	EXPECT_EQ(-1, g.read_dimacs(header.path()));

	// unreadable edge weight
	TempFile bad_weight("bitgraph_ew_badw.clq", "p edge 3 1\ne 1 2 abc\n");
	EXPECT_EQ(-1, g.read_dimacs(bad_weight.path()));
	EXPECT_EQ(0, g.num_vertices());

	// edge weight equal to NO_WEIGHT
	TempFile no_weight("bitgraph_ew_now.clq", "p edge 3 1\ne 1 2 -1\n");
	EXPECT_EQ(-1, g.read_dimacs(no_weight.path()));

	// vertex out of range
	TempFile range("bitgraph_ew_range.clq", "p edge 3 1\ne 1 4 2\n");
	EXPECT_EQ(-1, g.read_dimacs(range.path()));

	// self-loop
	TempFile loop("bitgraph_ew_loop.clq", "p edge 3 1\ne 2 2 2\n");
	EXPECT_EQ(-1, g.read_dimacs(loop.path()));

	// fewer / more edges than announced in the header
	TempFile few("bitgraph_ew_few.clq", "p edge 3 2\ne 1 2 2\n");
	EXPECT_EQ(-1, g.read_dimacs(few.path()));

	TempFile many("bitgraph_ew_many.clq", "p edge 3 1\ne 1 2 2\ne 2 3 3\n");
	EXPECT_EQ(-1, g.read_dimacs(many.path()));

	// unknown tag
	TempFile tag("bitgraph_ew_tag.clq", "p edge 3 1\nx 1 2\n");
	EXPECT_EQ(-1, g.read_dimacs(tag.path()));

	// every failure leaves an empty, consistent graph
	EXPECT_EQ(0, g.num_vertices());
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, read_dimacs_ignores_vertex_weights) {
	TempFile f("bitgraph_ew_vw.clq",
		"p edge 3 2\n"
		"n 1 10\nn 2 20\nn 3 30\n"
		"e 1 2 5\ne 2 3 6\n");

	ew_int g;
	ASSERT_EQ(0, g.read_dimacs(f.path()));

	for (vertex_t v = 0; v < 3; ++v) {
		EXPECT_EQ(N, g.weight(v, v));				// diagonal untouched
	}
	EXPECT_EQ(5, g.weight(0, 1));
	EXPECT_EQ(6, g.weight(1, 2));
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, read_dimacs_no_weights_generates_modulus_weights) {
	TempFile f("bitgraph_ew_mod.clq",
		"p edge 4 3\ne 1 2\ne 2 3\ne 4 1\n");

	ew_int g;
	ASSERT_EQ(0, g.read_dimacs(f.path()));

	// w(v,w) = 2 + ((v + w) % (DEFAULT_WEIGHT_MODULUS - 1)), 0-based
	EXPECT_EQ(3, g.weight(0, 1));				// 2 + 1
	EXPECT_EQ(5, g.weight(1, 2));				// 2 + 3
	EXPECT_EQ(5, g.weight(3, 0));				// 2 + 3
	EXPECT_EQ(N, g.weight(1, 0));				// directed: no reverse edge
	EXPECT_FALSE(g.is_unit_weighted());
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, read_dimacs_no_edges_is_consistent) {
	TempFile f("bitgraph_ew_noedges.clq", "p edge 3 0\n");

	ew_int g;
	ASSERT_EQ(0, g.read_dimacs(f.path()));

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_EQ(0u, g.num_edges(false));
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, read_dimacs_edges_without_weight) {
	TempFile f("bitgraph_ew_unw.clq",
		"p edge 3 2\ne 1 2\ne 2 3 7\n");

	ew_int g;
	ASSERT_EQ(0, g.read_dimacs(f.path()));

	EXPECT_EQ(ew_int::DEFAULT_WEIGHT, g.weight(0, 1));	// missing weight -> default
	EXPECT_EQ(7, g.weight(1, 2));
	EXPECT_TRUE(g.is_consistent());
}

TEST(BaseGraphEW, read_dimacs_replaces_previous_content) {
	TempFile f("bitgraph_ew_replace.clq", "p edge 2 1\ne 1 2 9\n");

	ew_int g(5);
	g.add_edge(3, 4, 1);

	ASSERT_EQ(0, g.read_dimacs(f.path()));

	EXPECT_EQ(2, g.num_vertices());
	EXPECT_EQ(1u, g.num_edges(false));
	EXPECT_EQ(9, g.weight(0, 1));
	EXPECT_TRUE(g.is_consistent());
}

TEST_F(BaseGraphEWTest, print_data_tag) {
	std::ostringstream os;
	EXPECT_EQ(&os, &g.print_data(true, os, true));

	EXPECT_NE(std::string::npos, os.str().find("[type: ew]"));
	EXPECT_EQ('\n', os.str().back());

	std::ostringstream no_nl;
	g.print_data(true, no_nl, false);
	EXPECT_NE('\n', no_nl.str().back());
}

TEST_F(BaseGraphEWTest, print_edge) {
	std::ostringstream edge;
	EXPECT_EQ(&edge, &g.print_edge(0, 1, edge, false));
	EXPECT_EQ("[0]--(1)-->[1]", edge.str());

	std::ostringstream reverse;
	g.print_edge(1, 0, reverse, true);
	EXPECT_EQ("[1]--(NO_EDGE)-->[0]\n", reverse.str());
}

TEST_F(BaseGraphEWTest, print_edges_list) {
	std::ostringstream os;
	EXPECT_EQ(&os, &g.print_edges(os));

	// directed edges in row-major order
	EXPECT_EQ(
		"[0]--(1)-->[1]\n"
		"[0]--(2)-->[2]\n"
		"[1]--(3)-->[3]\n"
		"[3]--(8)-->[1]\n",
		os.str());
}

TEST_F(BaseGraphEWTest, print_edges_list_trailing_newline) {
	std::ostringstream plain, trailing;
	g.print_edges(plain);
	g.print_edges(trailing, ew_int::edge_print_format::edge_list, true);

	EXPECT_EQ(plain.str() + "\n", trailing.str());
}

TEST(BaseGraphEW, print_edges_matrix) {
	ew_int g(3);
	g.add_edge(0, 1, 2);
	g.add_edge(2, 0, 5);

	std::ostringstream os;
	g.print_edges(os, ew_int::edge_print_format::edge_matrix, true);

	// full N x N matrix, one tab-terminated row per line, plus the trailing newline
	EXPECT_EQ(
		"--\t2\t--\t\n"
		"--\t--\t--\t\n"
		"5\t--\t--\t\n"
		"\n",
		os.str());
}

TEST_F(BaseGraphEWTest, print_edges_induced) {
	vertex_set_t vs = { 0, 1, 3 };
	std::ostringstream os;
	EXPECT_EQ(&os, &g.print_edges(vs, os));

	const std::string s = os.str();
	EXPECT_NE(std::string::npos, s.find("[0]--(1)-->[1]"));
	EXPECT_NE(std::string::npos, s.find("[1]--(3)-->[3]"));
	EXPECT_NE(std::string::npos, s.find("[3]--(8)-->[1]"));
	EXPECT_EQ(std::string::npos, s.find("(2)"));		// edge (0,2) leaves the set
}
