/**
 * @file test_decode.cpp
 * @brief Unit tests for the OrderingDecoder class.
 *
 * Tests permutation inversion, single-vertex decoding, composed orderings,
 * collection decoding, in-place decoding, state management, and the
 * backward-compatible Decode API.
 *
 * @author Pablo San Segundo
 * @date Created: 17/12/2024
 * @date Last updated: 26/09/2026
 */


#include "gtest/gtest.h"

#include "graph/graph_unweighted.h"
#include "graph/algorithms/decode.h"
#include "graph/algorithms/graph_fast_sort.h"

#include <utility>

namespace {

    using bitgraph::ugraph;
    using bitgraph::OrderingDecoder;
    using ordering_type = OrderingDecoder::ordering_type;
   
}

/**
 * @test Verifies computation of an inverse vertex permutation.
 */
TEST(OrderingDecoderTest, ComputesInverseOrdering)
{
	// Mapping new indices to old indices.
	const ordering_type ordering{ 2, 0, 3, 1 };

	const ordering_type inverse =
		OrderingDecoder::inverse_ordering(ordering);

	EXPECT_EQ(
		(ordering_type{ 1, 3, 0, 2 }),
		inverse);
}

/**
 * @test Verifies in-place inversion of a vertex permutation.
 */
TEST(OrderingDecoderTest, InvertsOrderingInPlace)
{
	ordering_type ordering{ 2, 0, 3, 1 };

	OrderingDecoder::invert_ordering(ordering);

	EXPECT_EQ(
		(ordering_type{ 1, 3, 0, 2 }),
		ordering);
}

/**
 * @test Verifies that one stored ordering decodes individual vertices.
 */
TEST(OrderingDecoderTest, DecodesIndividualVertices)
{
	OrderingDecoder decoder;
	decoder.add_ordering(
		ordering_type{ 2, 0, 3, 1 });

	EXPECT_EQ(2, decoder.decode(0));
	EXPECT_EQ(0, decoder.decode(1));
	EXPECT_EQ(3, decoder.decode(2));
	EXPECT_EQ(1, decoder.decode(3));
}

/**
 * @test Verifies that stored orderings are composed in reverse insertion
 *       order during decoding.
 */
TEST(OrderingDecoderTest, DecodesComposedOrderings)
{
	OrderingDecoder decoder;

	// Maps first reordered graph to the original graph.
	decoder.add_ordering(
		ordering_type{ 2, 0, 3, 1 });

	// Maps second reordered graph to the first reordered graph.
	decoder.add_ordering(
		ordering_type{ 3, 2, 1, 0 });

	/*
	 * Vertex 0:
	 *   second ordering: 0 -> 3
	 *   first ordering:  3 -> 1
	 */
	EXPECT_EQ(1, decoder.decode(0));

	/*
	 * The complete composed mapping is:
	 *   {1, 3, 0, 2}
	 */
	EXPECT_EQ(3, decoder.decode(1));
	EXPECT_EQ(0, decoder.decode(2));
	EXPECT_EQ(2, decoder.decode(3));
}

/**
 * @test Verifies decoding of a collection of vertices.
 */
TEST(OrderingDecoderTest, DecodesVertexCollection)
{
	OrderingDecoder decoder;

	decoder.add_ordering(
		ordering_type{ 2, 0, 3, 1 });

	decoder.add_ordering(
		ordering_type{ 3, 2, 1, 0 });

	const ordering_type vertices{ 0, 2, 3 };

	const ordering_type decoded =
		decoder.decode(vertices);

	EXPECT_EQ(
		(ordering_type{ 1, 0, 2 }),
		decoded);

	// The input collection is not modified.
	EXPECT_EQ(
		(ordering_type{ 0, 2, 3 }),
		vertices);
}

/**
 * @test Verifies in-place decoding of a vertex collection.
 */
TEST(OrderingDecoderTest, DecodesVertexCollectionInPlace)
{
	OrderingDecoder decoder;

	decoder.add_ordering(
		ordering_type{ 2, 0, 3, 1 });

	ordering_type vertices{ 0, 1, 2, 3 };

	decoder.decode_vertices_in_place(vertices);

	EXPECT_EQ(
		(ordering_type{ 2, 0, 3, 1 }),
		vertices);
}

/**
 * @test Verifies that an empty ordering composition acts as the identity.
 */
TEST(OrderingDecoderTest, EmptyDecoderActsAsIdentity)
{
	const OrderingDecoder decoder;

	EXPECT_EQ(7, decoder.decode(7));

	const ordering_type vertices{ 1, 4, 8 };
	EXPECT_EQ(vertices, decoder.decode(vertices));
}

/**
 * @test Verifies addition, inspection, and removal of stored orderings.
 */
TEST(OrderingDecoderTest, ManagesStoredOrderings)
{
	OrderingDecoder decoder;

	EXPECT_TRUE(decoder.empty());
	EXPECT_EQ(0u, decoder.size());

	const ordering_type first{ 2, 0, 1 };
	decoder.add_ordering(first);

	EXPECT_FALSE(decoder.empty());
	EXPECT_EQ(1u, decoder.size());
	EXPECT_EQ(first, decoder.first_ordering());
	EXPECT_EQ(first, decoder.orderings().front());

	decoder.clear();

	EXPECT_TRUE(decoder.empty());
	EXPECT_EQ(0u, decoder.size());
}

/**
 * @test Verifies that rvalue orderings can be moved into the decoder.
 */
TEST(OrderingDecoderTest, AcceptsMovedOrdering)
{
	OrderingDecoder decoder;
	ordering_type ordering{ 2, 0, 1 };

	decoder.add_ordering(std::move(ordering));

	EXPECT_EQ(
		(ordering_type{ 2, 0, 1 }),
		decoder.first_ordering());
}

/**
 * @test Verifies the backward-compatible Decode type and reversal functions.
 */
TEST(OrderingDecoderCompatibilityTest, SupportsLegacyApi)
{
	bitgraph::Decode decoder;

	ordering_type ordering{ 2, 0, 1 };

	bitgraph::Decode::reverse_in_place(ordering);

	EXPECT_EQ(
		(ordering_type{ 1, 2, 0 }),
		ordering);

	decoder.add_ordering(ordering);

	ordering_type vertices{ 0, 1, 2 };

	EXPECT_EQ(0, decoder.decode_in_place(vertices));

	EXPECT_EQ(
		(ordering_type{ 1, 2, 0 }),
		vertices);
}

/**
 * @test Verifies the historical empty-input result of decode_in_place().
 */
TEST(OrderingDecoderCompatibilityTest, LegacyInPlaceDecodeRejectsEmptyInput)
{
	const bitgraph::Decode decoder;
	ordering_type vertices;

	EXPECT_EQ(-1, decoder.decode_in_place(vertices));
	EXPECT_TRUE(vertices.empty());
}

/**
 * @test Verifies that an ordering produced by GraphFastRootSort can be
 *       inverted and used to recover original graph vertices.
 */
TEST(
	OrderingDecoderIntegrationTest,
	DecodesGraphFastSortOrdering)
{
	bitgraph::ugraph graph(106);

	graph.add_edge(1, 2);
	graph.add_edge(1, 3);
	graph.add_edge(1, 4);
	graph.add_edge(78, 5);

	using sorter_type =
		bitgraph::GraphFastRootSort<bitgraph::ugraph>;

	sorter_type sorter(graph);

	/*
	 * GraphFastRootSort produces an ordering in old-to-new format:
	 *
	 *     ordering[old_vertex] = new_vertex
	 */
	const auto old_to_new_ordering =
		sorter.new_order(
			static_cast<int>(sorter_type::MIN),
			false,  // Lowest-to-first disabled.
			true);  // Request old-to-new format.

	/*
	 * OrderingDecoder expects new-to-old mappings:
	 *
	 *     ordering[new_vertex] = old_vertex
	 */
	auto new_to_old_ordering =
		OrderingDecoder::inverse_ordering(
			old_to_new_ordering);

	ASSERT_GE(new_to_old_ordering.size(), 6u);

	OrderingDecoder decoder;
	decoder.add_ordering(
		std::move(new_to_old_ordering));

	const bitgraph::vertices reordered_vertices{
		0, 1, 2, 3, 4, 5
	};

	const bitgraph::vertices decoded_vertices =
		decoder.decode(reordered_vertices);

	EXPECT_EQ(
		(bitgraph::vertices{ 0, 6, 7, 8, 9, 10 }),
		decoded_vertices);
}