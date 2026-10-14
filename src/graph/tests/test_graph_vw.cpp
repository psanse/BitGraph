/**
 * @file test_graph_vw.cpp
 * @brief Unit tests for the generic vertex-weighted graph base class.
 *
 * Contains GoogleTest-based tests for `Base_Graph_VW<GraphT, WeightT>`,
 * covering construction, assignment, graph initialization, vertex weights,
 * weight transformations, DIMACS I/O, and graph generation.
 *
 * These tests exercise the generic functionality provided by
 * `Base_Graph_VW` independently of facade-specific specializations.
 *
 * @author Pablo San Segundo (pss)
 *
 * @details Created 09/10/2016, last updated 12/10/2026.
 *
 * @todo Review the tests at the end of the file; some are currently disabled.
 */

#include "graph/simple_ugraph.h"
#include "graph/algorithms/graph_gen.h"
#include "graph/simple_graph_vw.h"
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

	using vw_int = Base_Graph_VW<ugraph, int>;
	using vw_dbl = Base_Graph_VW<ugraph, double>;

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

class BaseGraphVWTest : public ::testing::Test {
protected:
	BaseGraphVWTest() : g(NV) {}

	void SetUp() override {
		g.set_weights(std::vector<int>{ 5, 7, 7, 2, 4 });
		g.add_edge(0, 1);
		g.add_edge(1, 2);
		g.add_edge(3, 4);
		g.set_name("toy_vw");
	}

	const int NV = 5;
	vw_int g;
};

/////////////////////
//
// Construction
//
/////////////////////

TEST(BaseGraphVW, default_constructor) {
	vw_int g;

	EXPECT_EQ(0, g.num_vertices());
	EXPECT_EQ(0u, g.size());
	EXPECT_TRUE(g.weights().empty());
	EXPECT_TRUE(g.is_unit_weighted());			// empty graph
}

TEST(BaseGraphVW, constructor_num_vertices) {
	vw_int g(4);

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_EQ(0u, g.num_edges());
	ASSERT_EQ(4u, g.weights().size());
	for (vertex_t v = 0; v < 4; ++v) {
		EXPECT_EQ(vw_int::DEFAULT_WEIGHT, g.weight(v));
	}
	EXPECT_TRUE(g.is_unit_weighted());
}

TEST(BaseGraphVW, constructor_num_vertices_and_weight) {
	vw_int g(3, 9);

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_EQ(std::vector<int>(3, 9), g.weights());
	EXPECT_FALSE(g.is_unit_weighted());
}

TEST(BaseGraphVW, constructor_from_weights_copy) {
	const std::vector<int> w = { 3, 1, 4, 1 };
	vw_int g(w);

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_EQ(w, g.weights());
	EXPECT_EQ(0u, g.num_edges());
}

TEST(BaseGraphVW, constructor_from_weights_move) {
	std::vector<int> w = { 3, 1, 4, 1 };
	vw_int g(std::move(w));

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_EQ((std::vector<int>{3, 1, 4, 1}), g.weights());
}

TEST(BaseGraphVW, constructor_from_graph_is_unit_weighted) {
	ugraph ug(4);
	ug.add_edge(0, 1);
	ug.add_edge(2, 3);

	vw_int g(ug);

	EXPECT_EQ(4, g.num_vertices());
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(3, 2));
	EXPECT_FALSE(g.is_edge(0, 2));
	EXPECT_TRUE(g.is_unit_weighted());
}

TEST(BaseGraphVW, constructor_from_graph_and_weights) {
	ugraph ug(3);
	ug.add_edge(0, 2);

	vw_int g(ug, std::vector<int>{ 10, 20, 30 });

	EXPECT_TRUE(g.is_edge(2, 0));
	EXPECT_EQ(10, g.weight(0));
	EXPECT_EQ(20, g.weight(1));
	EXPECT_EQ(30, g.weight(2));
}

TEST(BaseGraphVW, constructor_from_graph_and_weights_mismatch_terminates) {
	ugraph ug(3);

	EXPECT_DEATH(vw_int(ug, std::vector<int>{ 1, 2 }), "");
	EXPECT_DEATH(vw_int(ug, std::vector<int>{ 1, 2, 3, 4 }), "");
}

TEST(BaseGraphVW, constructor_file) {
	TempFile f("bitgraph_vw_ctor.clq",
		"p edge 3 2\n"
		"n 1 5\nn 2 7\nn 3 9\n"
		"e 1 2\ne 2 3\n");

	vw_int g(f.path());

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_EQ(2u, g.num_edges(false));
	EXPECT_EQ((std::vector<int>{5, 7, 9}), g.weights());
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(2, 1));
}

TEST(BaseGraphVW, constructor_missing_file_terminates) {
	EXPECT_DEATH(vw_int("this_file_does_not_exist.clq"), "");
}

TEST_F(BaseGraphVWTest, copy_and_move) {
	vw_int copy(g);
	EXPECT_EQ(g.weights(), copy.weights());
	EXPECT_EQ(g.name(), copy.name());
	EXPECT_TRUE(copy.is_edge(0, 1));

	// copies are independent
	copy.set_weight(0, 100);
	copy.add_edge(0, 4);
	EXPECT_EQ(5, g.weight(0));
	EXPECT_FALSE(g.is_edge(0, 4));

	vw_int assigned;
	assigned = g;
	EXPECT_EQ(g.weights(), assigned.weights());
	EXPECT_TRUE(assigned.is_edge(3, 4));

	vw_int moved(std::move(copy));
	EXPECT_EQ(100, moved.weight(0));
	EXPECT_TRUE(moved.is_edge(0, 4));
}

TEST_F(BaseGraphVWTest, reset) {
	g.reset(3, 8, "fresh");

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_EQ(0u, g.num_edges(false));
	EXPECT_EQ(std::vector<int>(3, 8), g.weights());
	EXPECT_EQ("fresh", g.name());
	EXPECT_FALSE(g.is_edge(0, 1));
}

/////////////////////
//
// Weights
//
/////////////////////

TEST_F(BaseGraphVWTest, getters) {
	EXPECT_EQ(5, g.weight(0));
	EXPECT_EQ(2, g.weight(3));
	EXPECT_EQ(static_cast<size_t>(NV), g.weights().size());
	EXPECT_EQ(NV, g.num_vertices());
	EXPECT_EQ(static_cast<size_t>(NV), g.size());
	EXPECT_EQ(NV, g.graph().num_vertices());
	EXPECT_EQ("toy_vw", g.name());

	g.set_path("some/path");
	EXPECT_EQ("some/path", g.path());
}

TEST_F(BaseGraphVWTest, set_weight) {
	g.set_weight(2, 42);

	EXPECT_EQ(42, g.weight(2));
	EXPECT_EQ(7, g.weight(1));				// neighbours untouched
}

TEST_F(BaseGraphVWTest, set_weights_uniform) {
	g.set_weights(6);
	EXPECT_EQ(std::vector<int>(NV, 6), g.weights());
	EXPECT_FALSE(g.is_unit_weighted());

	g.set_weights();						// DEFAULT_WEIGHT
	EXPECT_TRUE(g.is_unit_weighted());
	EXPECT_EQ(static_cast<size_t>(NV), g.weights().size());
}

TEST_F(BaseGraphVWTest, set_weights_vector) {
	const std::vector<int> w = { 1, 2, 3, 4, 5 };
	g.set_weights(w);

	EXPECT_EQ(w, g.weights());
}

TEST_F(BaseGraphVWTest, set_weights_vector_move) {
	std::vector<int> w = { 9, 8, 7, 6, 5 };
	g.set_weights(std::move(w));

	EXPECT_EQ((std::vector<int>{9, 8, 7, 6, 5}), g.weights());
}

TEST_F(BaseGraphVWTest, set_weights_size_mismatch_terminates) {
	EXPECT_DEATH(g.set_weights(std::vector<int>{ 1, 2, 3 }), "");
	EXPECT_DEATH(g.set_weights(std::vector<int>(NV + 1, 1)), "");
}

TEST_F(BaseGraphVWTest, set_modulus_weights) {
	g.set_modulus_weights(3);				// w(v) = ((v + 1) % 3) + 1

	EXPECT_EQ((std::vector<int>{2, 3, 1, 2, 3}), g.weights());
}

TEST_F(BaseGraphVWTest, set_modulus_weights_range) {
	const int M = 4;
	g.set_modulus_weights(M);

	for (vertex_t v = 0; v < NV; ++v) {
		EXPECT_GE(g.weight(v), 1);
		EXPECT_LE(g.weight(v), M);
	}
}

TEST_F(BaseGraphVWTest, set_modulus_weights_unit_modulus) {
	g.set_modulus_weights(1);
	EXPECT_TRUE(g.is_unit_weighted());
}

TEST_F(BaseGraphVWTest, set_modulus_weights_default) {
	g.set_modulus_weights();				// DEFAULT_WEIGHT_MODULUS
	EXPECT_EQ(2, g.weight(0));				// (0 + 1) % 200 + 1
	EXPECT_EQ(static_cast<size_t>(NV), g.weights().size());
}

TEST_F(BaseGraphVWTest, set_modulus_weights_invalid_terminates) {
	EXPECT_DEATH(g.set_modulus_weights(0), "");
	EXPECT_DEATH(g.set_modulus_weights(-3), "");
}

TEST_F(BaseGraphVWTest, transform_weights) {
	g.transform_weights([](int w) { return 2 * w; });

	EXPECT_EQ((std::vector<int>{10, 14, 14, 4, 8}), g.weights());
}

TEST_F(BaseGraphVWTest, transform_weights_skips_no_weight) {
	g.set_weight(1, vw_int::NO_WEIGHT);
	g.transform_weights([](int w) { return 10 * w; });

	EXPECT_EQ(vw_int::NO_WEIGHT, g.weight(1));
	EXPECT_EQ(50, g.weight(0));
}

TEST_F(BaseGraphVWTest, maximum_weight) {
	vertex_t v = -1;

	EXPECT_EQ(7, g.maximum_weight(v));
	EXPECT_EQ(1, v);						// first vertex attaining the maximum

	g.set_weight(4, 100);
	EXPECT_EQ(100, g.maximum_weight(v));
	EXPECT_EQ(4, v);
}

TEST(BaseGraphVW, maximum_weight_empty_terminates) {
	vw_int g;
	vertex_t v = -1;
	EXPECT_DEATH(g.maximum_weight(v), "");
}

TEST_F(BaseGraphVWTest, is_unit_weighted) {
	EXPECT_FALSE(g.is_unit_weighted());

	vw_int unit(5);
	EXPECT_TRUE(unit.is_unit_weighted());

	unit.set_weight(3, 2);
	EXPECT_FALSE(unit.is_unit_weighted());
}

TEST(BaseGraphVW, double_weights) {
	vw_dbl g(std::vector<double>{ 0.5, 1.5, 2.5 });

	EXPECT_DOUBLE_EQ(1.5, g.weight(1));
	EXPECT_FALSE(g.is_unit_weighted());

	g.transform_weights([](double w) { return 2 * w; });
	EXPECT_DOUBLE_EQ(1.0, g.weight(0));
	EXPECT_DOUBLE_EQ(5.0, g.weight(2));

	vertex_t v = -1;
	EXPECT_DOUBLE_EQ(5.0, g.maximum_weight(v));
	EXPECT_EQ(2, v);

	g.set_weights();
	EXPECT_TRUE(g.is_unit_weighted());
}

/////////////////////
//
// Topology (weights unaffected)
//
/////////////////////

TEST_F(BaseGraphVWTest, add_edge_and_queries) {
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(1, 0));
	EXPECT_FALSE(g.is_edge(0, 2));
	EXPECT_EQ(3u, g.num_edges(false));

	g.add_edge(0, 4);
	EXPECT_TRUE(g.is_edge(4, 0));
	EXPECT_EQ(4u, g.num_edges(false));
	EXPECT_EQ(5, g.weight(0));				// weights unaffected
}

TEST(BaseGraphVW, gen_random_edges_keeps_weights) {
	vw_int g(std::vector<int>{ 4, 3, 2, 1, 5, 6, 7, 8, 9, 10 });
	const auto before = g.weights();

	g.gen_random_edges(0.5);

	EXPECT_EQ(before, g.weights());
	EXPECT_EQ(10, g.num_vertices());
	EXPECT_EQ(23u, g.num_edges(false));		// round(0.5 * C(10,2)), G(n,m) model
}

/////////////////////
//
// I/O
//
/////////////////////

TEST_F(BaseGraphVWTest, write_dimacs) {
	std::ostringstream os;
	EXPECT_EQ(&os, &g.write_dimacs(os));
	const std::string s = os.str();

	// 1-based vertex weights
	EXPECT_NE(std::string::npos, s.find("n 1 5\n"));
	EXPECT_NE(std::string::npos, s.find("n 2 7\n"));
	EXPECT_NE(std::string::npos, s.find("n 4 2\n"));
	EXPECT_NE(std::string::npos, s.find("n 5 4\n"));

	// edges (1-based)
	EXPECT_NE(std::string::npos, s.find("e 1 2\n"));
	EXPECT_NE(std::string::npos, s.find("e 4 5\n"));
	EXPECT_EQ(std::string::npos, s.find("e 1 3\n"));
}

TEST_F(BaseGraphVWTest, DISABLED_write_read_dimacs_round_trip) {
	std::ostringstream os;
	g.write_dimacs(os);
	TempFile f("bitgraph_vw_roundtrip.clq", os.str());

	vw_int h;
	ASSERT_TRUE(h.read_dimacs(f.path()));

	EXPECT_EQ(g.weights(), h.weights());
	EXPECT_EQ(g.num_vertices(), h.num_vertices());
	for (vertex_t v = 0; v < NV; ++v) {
		for (vertex_t w = 0; w < NV; ++w) {
			EXPECT_EQ(g.is_edge(v, w), h.is_edge(v, w)) << v << "," << w;
		}
	}
}

TEST(BaseGraphVW, read_dimacs_inline_weights) {
	TempFile f("bitgraph_vw_inline.clq",
		"c a comment\n"
		"p edge 3 2\n"
		"n 1 5\nn 2 7\nn 3 9\n"
		"e 1 2\ne 2 3\n");

	vw_int g;
	ASSERT_TRUE(g.read_dimacs(f.path()));

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_EQ((std::vector<int>{5, 7, 9}), g.weights());
	EXPECT_TRUE(g.is_edge(0, 1));
	EXPECT_TRUE(g.is_edge(1, 2));
	EXPECT_FALSE(g.is_edge(0, 2));
}

TEST(BaseGraphVW, read_dimacs_without_weights_is_unit_weighted) {
	TempFile f("bitgraph_vw_noweights.clq",
		"p edge 3 2\ne 1 2\ne 2 3\n");

	vw_int g;
	ASSERT_TRUE(g.read_dimacs(f.path()));

	EXPECT_EQ(3, g.num_vertices());
	EXPECT_TRUE(g.is_unit_weighted());
}

TEST(BaseGraphVW, read_dimacs_separate_weight_file) {
	TempFile f("bitgraph_vw_sep.clq",
		"p edge 3 1\ne 1 2\n");
	TempFile w("bitgraph_vw_sep.clq.w", "4 5 6\n");

	vw_int g;
	ASSERT_TRUE(g.read_dimacs(f.path(), vw_int::weight_file_extension::w));

	EXPECT_EQ((std::vector<int>{4, 5, 6}), g.weights());
}

TEST(BaseGraphVW, read_dimacs_missing_weight_file_fails) {
	TempFile f("bitgraph_vw_sep2.clq",
		"p edge 3 1\ne 1 2\n");

	vw_int g;
	EXPECT_FALSE(g.read_dimacs(f.path(), vw_int::weight_file_extension::w));
	EXPECT_EQ(0, g.num_vertices());			// reset on failure
}

TEST(BaseGraphVW, read_dimacs_inline_weights_ignore_weight_file) {
	TempFile f("bitgraph_vw_sep3.clq",
		"p edge 2 1\nn 1 3\nn 2 4\ne 1 2\n");
	TempFile w("bitgraph_vw_sep3.clq.w", "9 9\n");

	vw_int g;
	ASSERT_TRUE(g.read_dimacs(f.path(), vw_int::weight_file_extension::w));

	EXPECT_EQ((std::vector<int>{3, 4}), g.weights());
}

TEST(BaseGraphVW, read_dimacs_invalid_input_fails) {
	vw_int g;

	// missing file
	EXPECT_FALSE(g.read_dimacs("this_file_does_not_exist.clq"));
	EXPECT_EQ(0, g.num_vertices());

	// wrong number of edges
	TempFile few("bitgraph_vw_few.clq", "p edge 3 2\ne 1 2\n");
	EXPECT_FALSE(g.read_dimacs(few.path()));
	EXPECT_EQ(0, g.num_vertices());

	TempFile many("bitgraph_vw_many.clq", "p edge 3 1\ne 1 2\ne 2 3\n");
	EXPECT_FALSE(g.read_dimacs(many.path()));

	// vertex out of range
	TempFile range("bitgraph_vw_range.clq", "p edge 3 1\ne 1 4\n");
	EXPECT_FALSE(g.read_dimacs(range.path()));

	// self-loop
	TempFile loop("bitgraph_vw_loop.clq", "p edge 3 1\ne 2 2\n");
	EXPECT_FALSE(g.read_dimacs(loop.path()));

	// bad vertex-weight record
	TempFile badw("bitgraph_vw_badw.clq", "p edge 3 1\nn 7 1\ne 1 2\n");
	EXPECT_FALSE(g.read_dimacs(badw.path()));

	// unknown tag
	TempFile tag("bitgraph_vw_tag.clq", "p edge 3 1\nx 1 2\n");
	EXPECT_FALSE(g.read_dimacs(tag.path()));

	// not a DIMACS header
	TempFile header("bitgraph_vw_header.clq", "e 1 2\n");
	EXPECT_FALSE(g.read_dimacs(header.path()));
}

TEST_F(BaseGraphVWTest, read_dimacs_sets_name) {
	TempFile f("bitgraph_vw_name.clq", "p edge 2 1\ne 1 2\n");

	ASSERT_TRUE(g.read_dimacs(f.path()));
	EXPECT_EQ(f.path(), g.name());
}

TEST_F(BaseGraphVWTest, read_weights) {
	TempFile f("bitgraph_vw_weights.w", "1 2\n3 4 5\n");

	ASSERT_TRUE(g.read_weights(f.path()));
	EXPECT_EQ((std::vector<int>{1, 2, 3, 4, 5}), g.weights());
}

TEST_F(BaseGraphVWTest, read_weights_failure) {
	EXPECT_FALSE(g.read_weights("this_file_does_not_exist.w"));

	TempFile shortf("bitgraph_vw_short.w", "1 2 3\n");		// 5 vertices
	EXPECT_FALSE(g.read_weights(shortf.path()));
	EXPECT_TRUE(g.weights().empty());
}

TEST_F(BaseGraphVWTest, print_data_tag) {
	std::ostringstream os;
	EXPECT_EQ(&os, &g.print_data(true, os, true));

	EXPECT_NE(std::string::npos, os.str().find("[type: vw]"));
	EXPECT_EQ('\n', os.str().back());

	std::ostringstream no_nl;
	g.print_data(true, no_nl, false);
	EXPECT_NE('\n', no_nl.str().back());
}

TEST_F(BaseGraphVWTest, print_weights_with_vertices) {
	std::ostringstream os;
	EXPECT_EQ(&os, &g.print_weights(os));

	EXPECT_EQ("[0:(5)] [1:(7)] [2:(7)] [3:(2)] [4:(4)] \n", os.str());
}

TEST_F(BaseGraphVWTest, print_weights_only) {
	std::ostringstream os;
	g.print_weights(os, false);

	// weights in vertex order followed by the element count; no vertex ids
	EXPECT_EQ("5 7 7 2 4  [5]\n", os.str());
}

TEST_F(BaseGraphVWTest, print_weights_vertex_set) {
	vertex_set_t vs = { 0, 3 };
	std::ostringstream os;
	g.print_weights(vs, os);

	const std::string s = os.str();
	EXPECT_NE(std::string::npos, s.find("[0:(5)]"));
	EXPECT_NE(std::string::npos, s.find("[3:(2)]"));
	EXPECT_NE(std::string::npos, s.find("(2)\n"));
	EXPECT_EQ(std::string::npos, s.find("[1:"));
}

TEST_F(BaseGraphVWTest, print_weights_c_array) {
	const vertex_t vs[] = { 4, 1 };
	std::ostringstream os;
	g.print_weights(vs, 2, os);

	EXPECT_EQ("[4:(4)] [1:(7)] (2)\n", os.str());
}

TEST_F(BaseGraphVWTest, print_weights_bitset) {
	vw_int::vertex_bitset_t bs(NV);
	bs.set_bit(1);
	bs.set_bit(4);

	std::ostringstream os;
	g.print_weights(bs, os);

	EXPECT_EQ("[1:(7)] [4:(4)] (2)\n", os.str());
	EXPECT_EQ(2, bs.count());					// bitset not modified
}

TEST_F(BaseGraphVWTest, create_complement) {
	const vw_int c = g.create_complement();

	EXPECT_EQ(g.weights(), c.weights());
	EXPECT_EQ("", c.name());
	for (vertex_t v = 0; v < NV; ++v) {
		for (vertex_t w = 0; w < NV; ++w) {
			if (v != w) {
				EXPECT_NE(g.is_edge(v, w), c.is_edge(v, w)) << v << "," << w;
			}
		}
	}
}
