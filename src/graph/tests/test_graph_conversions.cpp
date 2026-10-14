/**
* @file test_graph_conversions.cpp  
* @brief tests for class GraphConversion to convert between different graph types
* @created 04/01/2025 (from older tests)
* @last_update 27/01/2025
* @author pss
* 
**/

#include "graph/algorithms/graph_conversions.h"			
#include "gtest/gtest.h"

using namespace bitgraph;

TEST(Conversions, sparse2ugraph) {
	
	sparse_ugraph sug(4);
	sug.add_edge(0, 1);
	sug.add_edge(1, 2);
	sug.add_edge(1, 3);
	sug.add_edge(0, 2);

	ugraph ug;
	convert_graph(sug, ug);

	EXPECT_EQ(4, ug.num_vertices());
	EXPECT_EQ(4, ug.num_edges());
	EXPECT_TRUE(ug.is_edge(0, 1));
	EXPECT_TRUE(ug.is_edge(1, 2));
	EXPECT_TRUE(ug.is_edge(1, 3));
	EXPECT_TRUE(ug.is_edge(0, 2));

	sparse_ugraph sug1(300);
	sug1.add_edge(0, 1);
	sug1.add_edge(1, 2);
	sug1.add_edge(1, 3);
	sug1.add_edge(0, 2);

	convert_graph(sug1, ug);

	EXPECT_EQ(300, ug.num_vertices());
	EXPECT_EQ(4, ug.num_edges());
	EXPECT_TRUE(ug.is_edge(0, 1));
	EXPECT_TRUE(ug.is_edge(1, 2));
	EXPECT_TRUE(ug.is_edge(1, 3));
	EXPECT_TRUE(ug.is_edge(0, 2));

}

TEST(Conversions, ugraph2sparse_ugraph) {

	ugraph ug(4);
	ug.add_edge(0, 1);
	ug.add_edge(1, 2);
	ug.add_edge(1, 3);
	ug.add_edge(0, 2);

	sparse_ugraph sug;
	convert_graph(ug, sug);

	EXPECT_EQ(4, sug.num_vertices());
	EXPECT_EQ(4, sug.num_edges());
	EXPECT_TRUE(sug.is_edge(0, 1));
	EXPECT_TRUE(sug.is_edge(1, 2));
	EXPECT_TRUE(sug.is_edge(1, 3));
	EXPECT_TRUE(sug.is_edge(0, 2));

	ugraph ug1(300);
	ug1.add_edge(0, 1);
	ug1.add_edge(1, 2);
	ug1.add_edge(1, 3);
	ug1.add_edge(0, 2);

	convert_graph(ug1, sug);

	EXPECT_EQ(300, sug.num_vertices());
	EXPECT_EQ(4, sug.num_edges());
	EXPECT_TRUE(sug.is_edge(0, 1));
	EXPECT_TRUE(sug.is_edge(1, 2));
	EXPECT_TRUE(sug.is_edge(1, 3));
	EXPECT_TRUE(sug.is_edge(0, 2));

}

TEST(Conversions, sparse_to_dense_preserves_edges_across_blocks_and_metadata) {
	sparse_ugraph source(130);
	source.set_name("source.graph");
	source.set_path("graphs/");
	source.add_edge(0, 63);
	source.add_edge(63, 64);
	source.add_edge(64, 129);
	source.add_edge(0, 129);

	ugraph destination(130);
	destination.add_edge(1, 2);
	convert_graph(source, destination);

	EXPECT_EQ(130, destination.num_vertices());
	EXPECT_EQ(4, destination.num_edges());
	EXPECT_EQ("source.graph", destination.name());
	EXPECT_EQ("graphs/", destination.path());
	EXPECT_TRUE(destination.is_edge(0, 63));
	EXPECT_TRUE(destination.is_edge(63, 64));
	EXPECT_TRUE(destination.is_edge(64, 129));
	EXPECT_TRUE(destination.is_edge(0, 129));
	EXPECT_FALSE(destination.is_edge(1, 2));
}

TEST(Conversions, dense_to_sparse_preserves_edges_across_blocks_and_metadata) {
	ugraph source(130);
	source.set_name("source.graph");
	source.set_path("graphs/");
	source.add_edge(0, 63);
	source.add_edge(63, 64);
	source.add_edge(64, 129);
	source.add_edge(0, 129);

	sparse_ugraph destination(130);
	destination.add_edge(1, 2);
	convert_graph(source, destination);

	EXPECT_EQ(130, destination.num_vertices());
	EXPECT_EQ(4, destination.num_edges());
	EXPECT_EQ("source.graph", destination.name());
	EXPECT_EQ("graphs/", destination.path());
	EXPECT_TRUE(destination.is_edge(0, 63));
	EXPECT_TRUE(destination.is_edge(63, 64));
	EXPECT_TRUE(destination.is_edge(64, 129));
	EXPECT_TRUE(destination.is_edge(0, 129));
	EXPECT_FALSE(destination.is_edge(1, 2));
}

TEST(Conversions, empty_graph_resets_destination) {
	sparse_ugraph empty_sparse;
	ugraph dense(2);
	dense.add_edge(0, 1);
	convert_graph(empty_sparse, dense);
	EXPECT_EQ(0, dense.num_vertices());
	EXPECT_EQ(0, dense.num_edges());

	ugraph empty_dense;
	sparse_ugraph sparse(2);
	sparse.add_edge(0, 1);
	convert_graph(empty_dense, sparse);
	EXPECT_EQ(0, sparse.num_vertices());
	EXPECT_EQ(0, sparse.num_edges());
}

