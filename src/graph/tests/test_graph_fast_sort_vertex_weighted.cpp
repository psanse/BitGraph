/**
 * @file test_graph_fast_sort_vertex_weighted.cpp
 * @brief Unit tests for `GraphFastRootSort_VW`.
 *
 * Tests vertex-ordering and graph-reordering functionality for
 * vertex-weighted graphs.
 *
 * @details Created ?, last updated 08/10/2026.
 */

#include "graph/algorithms/graph_fast_sort.h"
#include "graph/algorithms/graph_fast_sort_vertex_weighted.h"
#include "graph/ugraph_vertex_weighted.h"

#include "gtest/gtest.h"

#include <vector>

using namespace bitgraph;

namespace {

    class GraphFastRootSortVertexWeightedTest : public ::testing::Test {
    protected:

        void SetUp() override {

            ugw.reset(NV);

            ugw.add_edge(1, 2);
            ugw.add_edge(1, 5);
            ugw.add_edge(2, 4);
            ugw.add_edge(2, 5);
            ugw.add_edge(3, 5);

            ugw.set_weight(0, 4.0);
            ugw.set_weight(1, 5.0);
            ugw.set_weight(2, 3.0);

            // Remaining vertices have the default weight 1.0.
        }

        void TearDown() override {}

        /*
         * Degrees:
         *
         * deg(5) = 3
         * deg(2) = 3
         * deg(1) = 2
         * deg(3) = 1
         * deg(4) = 1
         * deg(0) = 0
         */
        const int NV = 6;

        ugraph_w ugw;
    };

}

// ---------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, constructor) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto& gw = sorter.graph();

    // Graph structure.
    EXPECT_EQ(NV, sorter.num_vertices());
    EXPECT_EQ(NV, gw.num_vertices());

    // Vertex weights.
    EXPECT_DOUBLE_EQ(4.0, gw.weight(0));
    EXPECT_DOUBLE_EQ(5.0, gw.weight(1));
    EXPECT_DOUBLE_EQ(3.0, gw.weight(2));
    EXPECT_DOUBLE_EQ(1.0, gw.weight(3));
    EXPECT_DOUBLE_EQ(1.0, gw.weight(4));
    EXPECT_DOUBLE_EQ(1.0, gw.weight(5));
}


// ---------------------------------------------------------------
// Typed unweighted ordering
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, new_order_unweighted) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto mapping = sorter.new_order(
        gt::base_strategy_t::min,
        gt::placement_t::first_to_last,
        gt::sort_order_t::old_to_new);      // [OLD] -> [NEW]

    const gt::vertex_ordering_t expected = {
        0, 3, 4, 1, 2, 5
    };

    EXPECT_EQ(expected, mapping);
}


// ---------------------------------------------------------------
// Weighted ordering: non-increasing
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, new_order_non_increasing_weight_n2o) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto mapping = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::new_to_old);                                 // [NEW] -> [OLD]

    /*
     * Weights:
     *
     * w(1) = 5
     * w(0) = 4
     * w(2) = 3
     * w(3) = 1
     * w(4) = 1
     * w(5) = 1
     *
     * Stable sorting preserves 3,4,5.
     */
    const gt::vertex_ordering_t expected = {
        1, 0, 2, 3, 4, 5
    };

    EXPECT_EQ(expected, mapping);
}


// ---------------------------------------------------------------
// Weighted ordering: non-decreasing
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, new_order_non_decreasing_weight_n2o) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto mapping = sorter.new_order(
        gt::strategy_t::min_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::new_to_old);                                 // [NEW] -> [OLD]

    /*
     * Stable ordering:
     *
     * w(3) = 1
     * w(4) = 1
     * w(5) = 1
     * w(2) = 3
     * w(0) = 4
     * w(1) = 5
     */
    const gt::vertex_ordering_t expected = {
        3, 4, 5, 2, 0, 1
    };

    EXPECT_EQ(expected, mapping);
}


// ---------------------------------------------------------------
// Stable sorting
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, stable_weight_sort) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto mapping = sorter.new_order(
        gt::strategy_t::min_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::new_to_old);                                 // [NEW] -> [OLD]

    /*
     * Vertices 3, 4 and 5 all have weight 1.0.
     *
     * Since stable_sort is used, their original relative order
     * must be preserved.
     */
    ASSERT_GE(mapping.size(), 3u);

    EXPECT_EQ(3, mapping[0]);
    EXPECT_EQ(4, mapping[1]);
    EXPECT_EQ(5, mapping[2]);
}


// ---------------------------------------------------------------
// Mapping direction
//
// Use a permutation which is NOT self-inverse so that conversion
// between [NEW]->[OLD] and [OLD]->[NEW] is actually tested.
// ---------------------------------------------------------------

TEST(GraphFastRootSortVertexWeightedMappingTest, mapping_direction) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    ugraph_w graph;
    graph.reset(4);

    graph.set_weight(0, 2.0);
    graph.set_weight(1, 4.0);
    graph.set_weight(2, 1.0);
    graph.set_weight(3, 3.0);

    gt sorter(graph);

    /*
     * Non-increasing weight:
     *
     * NEW -> OLD
     *
     * new 0 -> old 1
     * new 1 -> old 3
     * new 2 -> old 0
     * new 3 -> old 2
     *
     * {1, 3, 0, 2}
     */
    const auto mapping_n2o = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::new_to_old);

    const gt::vertex_ordering_t expected_n2o = {
        1, 3, 0, 2
    };

    EXPECT_EQ(expected_n2o, mapping_n2o);

    /*
     * Inverse mapping:
     *
     * OLD -> NEW
     *
     * old 0 -> new 2
     * old 1 -> new 0
     * old 2 -> new 3
     * old 3 -> new 1
     *
     * {2, 0, 3, 1}
     */
    const auto mapping_o2n = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::old_to_new);

    const gt::vertex_ordering_t expected_o2n = {
        2, 0, 3, 1
    };

    EXPECT_EQ(expected_o2n, mapping_o2n);
}


// ---------------------------------------------------------------
// Placement
// ---------------------------------------------------------------

TEST(GraphFastRootSortVertexWeightedMappingTest, last_to_first) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    ugraph_w graph;
    graph.reset(4);

    graph.set_weight(0, 2.0);
    graph.set_weight(1, 4.0);
    graph.set_weight(2, 1.0);
    graph.set_weight(3, 3.0);

    gt sorter(graph);

    /*
     * Non-increasing first-to-last:
     *
     * {1, 3, 0, 2}
     *
     * Last-to-first reverses this:
     *
     * {2, 0, 3, 1}
     */
    const auto mapping = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::last_to_first,
        gt::sort_order_t::new_to_old);                                 // [NEW] -> [OLD]

    const gt::vertex_ordering_t expected = {
        2, 0, 3, 1
    };

    EXPECT_EQ(expected, mapping);
}


// ---------------------------------------------------------------
// Reorder - member overload
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, reorder) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto order_o2n = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::old_to_new);      // [OLD] -> [NEW]

    ugraph_w reordered = sorter.reorder(order_o2n);

    EXPECT_EQ(ugw.num_vertices(), reordered.num_vertices());
    EXPECT_EQ(ugw.num_edges(), reordered.num_edges());

    // -----------------------------------------------------------
    // Weights
    // -----------------------------------------------------------

    for (vertex_t v = 0; v < NV; ++v) {

        EXPECT_DOUBLE_EQ(
            ugw.weight(v),
            reordered.weight(order_o2n[v]));
    }

    // -----------------------------------------------------------
    // Edges
    // -----------------------------------------------------------

    for (vertex_t u = 0; u < NV - 1; ++u) {

        for (vertex_t v = u + 1; v < NV; ++v) {

            EXPECT_EQ(
                ugw.is_edge(u, v),
                reordered.is_edge(
                    order_o2n[u],
                    order_o2n[v]));
        }
    }
}


// ---------------------------------------------------------------
// Reorder - explicit weight checking
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, reorder_weights) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto order_o2n = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::old_to_new);

    ugraph_w reordered = sorter.reorder(order_o2n);

    /*
     * Sorted by non-increasing weight:
     *
     * old vertex : 1   0   2   3   4   5
     * weight     : 5   4   3   1   1   1
     */
    EXPECT_DOUBLE_EQ(5.0, reordered.weight(0));
    EXPECT_DOUBLE_EQ(4.0, reordered.weight(1));
    EXPECT_DOUBLE_EQ(3.0, reordered.weight(2));
    EXPECT_DOUBLE_EQ(1.0, reordered.weight(3));
    EXPECT_DOUBLE_EQ(1.0, reordered.weight(4));
    EXPECT_DOUBLE_EQ(1.0, reordered.weight(5));
}


// ---------------------------------------------------------------
// Reorder - static overload
// ---------------------------------------------------------------

TEST_F(GraphFastRootSortVertexWeightedTest, reorder_static) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    gt sorter(ugw);

    const auto order_o2n = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::old_to_new);

    ugraph_w reordered = gt::reorder(
        ugw,
        order_o2n,
        nullptr);

    EXPECT_EQ(ugw.num_vertices(), reordered.num_vertices());
    EXPECT_EQ(ugw.num_edges(), reordered.num_edges());

    // Check vertex weights.
    for (vertex_t v = 0; v < NV; ++v) {

        EXPECT_DOUBLE_EQ(
            ugw.weight(v),
            reordered.weight(order_o2n[v]));
    }

    // Check graph isomorphism under the permutation.
    for (vertex_t u = 0; u < NV - 1; ++u) {

        for (vertex_t v = u + 1; v < NV; ++v) {

            EXPECT_EQ(
                ugw.is_edge(u, v),
                reordered.is_edge(
                    order_o2n[u],
                    order_o2n[v]));
        }
    }
}


// ---------------------------------------------------------------
// Reorder with a non-self-inverse permutation
// ---------------------------------------------------------------

TEST(GraphFastRootSortVertexWeightedMappingTest, reorder_non_self_inverse_mapping) {

    using gt = GraphFastRootSort_VW<ugraph_w>;

    ugraph_w graph;
    graph.reset(4);

    graph.add_edge(0, 1);
    graph.add_edge(0, 2);
    graph.add_edge(2, 3);

    graph.set_weight(0, 2.0);
    graph.set_weight(1, 4.0);
    graph.set_weight(2, 1.0);
    graph.set_weight(3, 3.0);

    gt sorter(graph);

    /*
     * N2O = {1, 3, 0, 2}
     *
     * therefore
     *
     * O2N = {2, 0, 3, 1}
     */
    const auto order_o2n = sorter.new_order(
        gt::strategy_t::max_weight,
        gt::placement_t::first_to_last,
        gt::sort_order_t::old_to_new);

    const gt::vertex_ordering_t expected_o2n = {
        2, 0, 3, 1
    };

    ASSERT_EQ(expected_o2n, order_o2n);

    ugraph_w reordered = sorter.reorder(order_o2n);

    // Weights must follow the relabeling.
    for (vertex_t v = 0; v < graph.num_vertices(); ++v) {

        EXPECT_DOUBLE_EQ(
            graph.weight(v),
            reordered.weight(order_o2n[v]));
    }

    // Edges must follow exactly the same relabeling.
    for (vertex_t u = 0; u < graph.num_vertices() - 1; ++u) {

        for (vertex_t v = u + 1; v < graph.num_vertices(); ++v) {

            EXPECT_EQ(
                graph.is_edge(u, v),
                reordered.is_edge(
                    order_o2n[u],
                    order_o2n[v]));
        }
    }
}