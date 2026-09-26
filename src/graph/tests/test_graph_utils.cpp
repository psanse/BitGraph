/**
 * @file test_graph_utils.cpp
 * @brief Unit tests for the general-purpose graph utility functions.
 *
 * Tests operations provided by the `bitgraph::graph_utils` namespace,
 * including neighborhood extraction, degree-based vertex sorting, graph
 * construction, and structural graph-property checks.
 *
 * @author Pablo San Segundo
 * @date Created: 20/07/2018
 * @date Last updated: 27/09/2026
 */

#include "graph/algorithms/graph_utils.h"
#include "gtest/gtest.h"
#include "graph/graph.h"				//	facade types
#include <iostream>

using namespace bitgraph;

namespace {

	class GraphUtilsTest : public ::testing::Test {
	protected:
		void SetUp() override {
			ug.reset(NV);
			ug.add_edge(0, 1);
			ug.add_edge(0, 2);
			ug.add_edge(0, 3);
			ug.add_edge(1, 3);
			ug.set_name("toy");
		}
		void TearDown() override {}

		//undirected graph instance	
		const int NV = 5;
		ugraph ug;											//undirected graph with integer weights
	};


	class GraphUtilsTestW : public ::testing::Test {
	protected:
		void SetUp() override {
			ugw.reset(NV, ugraph_wi::NO_WEIGHT);
			ugw.add_edge(0, 1);
			ugw.add_edge(1, 2);
			ugw.add_edge(0, 2);

			ugw.set_weight(0, 3);
			ugw.set_weight(1, 2);
			ugw.set_weight(2, 1);
			ugw.set_name("ugraph_VW");
		}
		void TearDown() override {}

		//undirected graph instance	
		const int NV = 5;
		ugraph_wi ugw;											//undirected graph with integer weights
	};


	class GraphNeighborhoodUtilsTest :
		public ::testing::Test {
	protected:
		enum : int {
			vertex_count = 100
		};

		GraphNeighborhoodUtilsTest()
			: graph_(vertex_count),
			candidates_(vertex_count)
		{}

		void SetUp() override
		{
			graph_.add_edge(0, 1);
			graph_.add_edge(0, 2);
			graph_.add_edge(0, 63);
			graph_.add_edge(0, 64);
			graph_.add_edge(0, 65);

			graph_.add_edge(3, 0);
			graph_.add_edge(3, 63);
			graph_.add_edge(3, 64);

			graph_.add_edge(63, 64);

			// Candidate vertex set {0,...,99}.
			candidates_.set_bit(0, vertex_count - 1);
		}

		bitgraph::ugraph graph_;
		bitgraph::bitarray candidates_;
	};

	class GraphDegreeSortUtilsTest :
		public ::testing::Test {
	protected:
		enum : int {
			vertex_count = 5
		};

		GraphDegreeSortUtilsTest()
			: graph_(vertex_count)
		{}

		void SetUp() override
		{
			graph_.add_edge(0, 1);
			graph_.add_edge(1, 2);
			graph_.add_edge(0, 2);
			graph_.add_edge(0, 3);
		}

		bitgraph::ugraph graph_;
	};

} // end anonymous namespace

TEST(CirculantGraphUtilsTest, circulant) {

	const int NV = 5;
	auto ug = ugraph::make_cycle(NV);

	std::vector<int> step_set;
	auto is_circ = graph_utils::is_circulant(ug, &step_set);			//cycle is circulant with jump list {1, 4}	

	EXPECT_TRUE(is_circ);

	std::vector<int> step_set_exp{ 1, 4 };
	EXPECT_EQ(step_set_exp, step_set);
}

TEST_F(GraphUtilsTest, is_triangleFree_subgraph) {

	Vertices triangle;
	EXPECT_TRUE(graph_utils::is_triangleFree_subgraph(ug, ug.neighbors(0), triangle));

	//adds a triangle
	ug.add_edge(1, 2);
	ug.add_edge(2, 3);
	EXPECT_FALSE(graph_utils::is_triangleFree_subgraph(ug, ug.neighbors(0), triangle));
}

TEST_F(GraphUtilsTest, is_edgeFree_subgraph) {

	std::pair<vertex_t, vertex_t> edge;
	EXPECT_FALSE(graph_utils::is_edgeFree_subgraph(ug, ug.neighbors(0), edge));				//{1, 3} is in G[{1, 2, 3}]					

	//removes the only edge 
	ug.remove_edge(1, 3);
	EXPECT_TRUE(graph_utils::is_edgeFree_subgraph(ug, ug.neighbors(0), edge));
}

TEST_F(GraphUtilsTestW, sort_w) {

	//set of vertices
	Vertices lv;
	lv.push_back(0);
	lv.push_back(1);
	lv.push_back(2);

	//sorts the set of vertices according to non-decreasing weight
	graph_utils::sort_w(ugw, lv, true);

	Vertices lv_exp;
	lv_exp.push_back(2);
	lv_exp.push_back(1);
	lv_exp.push_back(0);
	EXPECT_EQ(lv_exp, lv);				//{w(2)=1, w(1)=2, w(0)=3

	//sorts the set of vertices according to non-increasing weight
	graph_utils::sort_w(ugw, lv, false);

	lv_exp.clear();
	lv_exp.push_back(0);
	lv_exp.push_back(1);
	lv_exp.push_back(2);
	EXPECT_EQ(lv_exp, lv);

	//sort C-style array
	int lv_C[3];
	copy(lv.begin(), lv.end(), lv_C);

	//sorts the set of vertices according to non-decreasing weight
	graph_utils::sort_w(ugw, lv_C, 3, true);

	int lv_Cexp[3];
	lv_Cexp[0] = 2;
	lv_Cexp[1] = 1;
	lv_Cexp[2] = 0;

	for (int i = 0; i < 3; i++) {
		EXPECT_EQ(lv_Cexp[i], lv_C[i]);
	}
}

TEST_F(GraphUtilsTestW, wsum) {

	//using namespace graph_utils::vertexW;

	//set of vertices {0, 1}
	vertices lv;
	lv.push_back(0);
	lv.push_back(1);

	//sums the weights of the vertices in lv ={0, 1}
	int w = graph_utils::wsum(ugw, lv);
	EXPECT_EQ(5, w);

	bitarray bblv(5);
	bblv.set_bit(0);
	bblv.set_bit(1);

	//sums the weights of the (bit) set of vertices bblv = {0, 1}
	w = graph_utils::wsum(ugw, bblv);
	EXPECT_EQ(5, w);

	//sums the weights of the (bit) set of vertices bblv = {0, 1, 2}
	bblv.set_bit(2);
	w = graph_utils::wsum(ugw, bblv);
	EXPECT_EQ(6, w);
}

/**
 * @test Verifies that neighbors() returns the neighbors of a vertex that
 *       belong to the candidate set.
 */
TEST_F(
	GraphNeighborhoodUtilsTest,
	ReturnsExpectedNeighborsWithinCandidateSet)
{
	bitgraph::Vertices neighbors;

	bitgraph::graph_utils::neighbors(
		graph_,
		3,
		candidates_,
		neighbors);

	EXPECT_EQ(
		(bitgraph::Vertices{ 0, 63, 64 }),
		neighbors);
}

/**
 * @test Verifies that neighbors() filters out vertices not contained in the
 *       candidate set.
 */
TEST_F(
	GraphNeighborhoodUtilsTest,
	FiltersNeighborsByCandidateSet)
{
	bitgraph::bitarray restricted_candidates(vertex_count);
	restricted_candidates.set_bit(0);
	restricted_candidates.set_bit(64);

	bitgraph::Vertices neighbors;

	bitgraph::graph_utils::neighbors(
		graph_,
		3,
		restricted_candidates,
		neighbors);

	EXPECT_EQ(
		(bitgraph::Vertices{ 0, 64 }),
		neighbors);
}

/**
 * @test Verifies that neighbors_after() returns only neighbors whose index is
 *       greater than the specified vertex.
 */
TEST_F(
	GraphNeighborhoodUtilsTest,
	ReturnsOnlyHigherIndexedNeighbors)
{
	bitgraph::Vertices neighbors;

	bitgraph::graph_utils::neighbors_after(
		graph_,
		3,
		candidates_,
		neighbors);

	EXPECT_EQ(
		(bitgraph::Vertices{ 63, 64 }),
		neighbors);

	bitgraph::graph_utils::neighbors_after(
		graph_,
		63,
		candidates_,
		neighbors);

	EXPECT_EQ(
		(bitgraph::Vertices{ 64 }),
		neighbors);

	bitgraph::graph_utils::neighbors_after(
		graph_,
		0,
		candidates_,
		neighbors);

	EXPECT_EQ(
		(bitgraph::Vertices{ 1, 2, 3, 63, 64, 65 }),
		neighbors);
}


/**
* @test Verifies sorting by non-increasing degree within a vertex reference
*       set.
*/
TEST_F(
	GraphDegreeSortUtilsTest,
	SortsByNonIncreasingDegreeInVertexReferenceSet)
{
	/*
	 * Degrees into {2,3,4}:
	 *   vertex 0 -> {2,3}: degree 2
	 *   vertex 1 -> {2}:   degree 1
	 */
	const bitgraph::vertices reference_vertices{ 2, 3, 4 };

	bitgraph::vertices candidates{ 1, 0 };

	bitgraph::graph_utils::sort_by_degree(
		graph_,
		candidates,
		reference_vertices,
		bitgraph::graph_utils::degree_order::nonincreasing);		

	EXPECT_EQ(
		(bitgraph::vertices{ 0, 1 }),
		candidates);
}

TEST_F(
	GraphDegreeSortUtilsTest,
	LegacySortDegSortsByNonIncreasingDegree)
{

	/*
	 * Degrees into {2,3,4}:
	 *   vertex 0 -> {2,3}: degree 2
	 *   vertex 1 -> {2}:   degree 1
	 */
	const bitgraph::vertices reference_vertices{ 2, 3, 4 };
	bitgraph::vertices candidates{ 1, 0 };

	bitgraph::graph_utils::sort_deg(
		graph_,
		candidates,
		reference_vertices,
		false);							// min_sort = false (non-increasing)

	EXPECT_EQ(
		(bitgraph::vertices{ 0, 1 }),
		candidates);
}

/**
 * @test Verifies sorting by non-decreasing degree within a vertex reference
 *       set.
 */
TEST_F(
	GraphDegreeSortUtilsTest,
	SortsByNonDecreasingDegreeInVertexReferenceSet)
{
	bitgraph::Vertices vertices{ 0, 1 };
	const bitgraph::Vertices reference{ 2, 3, 4 };

	constexpr bool non_decreasing = true;

	bitgraph::graph_utils::sort_deg(
		graph_,
		vertices,
		reference,
		non_decreasing);

	EXPECT_EQ(
		(bitgraph::Vertices{ 1, 0 }),
		vertices);
}

/**
 * @test Verifies sorting by degree within a bitset reference set.
 */
TEST_F(
	GraphDegreeSortUtilsTest,
	SortsByDegreeInBitsetReferenceSet)
{
	bitgraph::Vertices vertices{ 0, 1 };

	bitgraph::bitarray reference(vertex_count);
	reference.set_bit(2);
	reference.set_bit(3);
	reference.set_bit(4);

	constexpr bool non_decreasing = true;

	bitgraph::graph_utils::sort_deg(
		graph_,
		vertices,
		reference,
		non_decreasing);

	EXPECT_EQ(
		(bitgraph::Vertices{ 1, 0 }),
		vertices);
}

/**
 * @test Verifies that an empty vertex reference set preserves the input
 *       ordering.
 */
TEST_F(
	GraphDegreeSortUtilsTest,
	EmptyVertexReferenceSetPreservesOrdering)
{
	bitgraph::Vertices vertices{ 1, 0 };
	const bitgraph::Vertices empty_reference;

	constexpr bool non_decreasing = false;

	bitgraph::graph_utils::sort_deg(
		graph_,
		vertices,
		empty_reference,
		non_decreasing);

	EXPECT_EQ(
		(bitgraph::Vertices{ 1, 0 }),
		vertices);
}

/**
 * @test Verifies that an empty bitset reference set preserves the input
 *       ordering.
 */
TEST_F(
	GraphDegreeSortUtilsTest,
	EmptyBitsetReferenceSetPreservesOrdering)
{
	bitgraph::vertices vertices{ 1, 0 };
	bitgraph::bitarray empty_reference(vertex_count);

	constexpr bool non_decreasing = false;

	bitgraph::graph_utils::sort_deg(
		graph_,
		vertices,
		empty_reference,
		non_decreasing);

	EXPECT_EQ(
		(bitgraph::Vertices{ 1, 0 }),
		vertices);
}



