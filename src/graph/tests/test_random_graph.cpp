/*
* @file test_random_graph.cpp
* @brief Unit tests for random graph and weight generators
* @created ?
* @updated @23/01/19 
* @last_update 27/01/25
* @author pss
* 
* TODO - ADD TESTS, weights...(09/01/2025)
*/

#include "graph/graph.h"
#include "graph/algorithms/graph_gen.h"
#include "gtest/gtest.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace bitgraph;	

TEST(Random_Graph, random_attr_t){
	
	random_attr_t r1(100, 150, .3, .5, 1, 50, .1);
		
	EXPECT_EQ(100, r1.nLB);
	EXPECT_EQ(150, r1.nUB);
	EXPECT_EQ(.3, r1.pLB);
	EXPECT_EQ(.5, r1.pUB);
	EXPECT_EQ(1, r1.nRep);	
	EXPECT_EQ(50, r1.incN);
	EXPECT_EQ(.1, r1.incP);

}

TEST(Random_Graph, ugraph){
	
	ugraph ug;
	RandomGen<ugraph>::create_graph(ug, 100, .30);
	
	//check if inside the range
	EXPECT_GE(.301, ug.density());
	EXPECT_LE(.290, ug.density());
	
	//TODO...
}

TEST(Random_Graph, sparse_ugraph){
	
	sparse_ugraph sug;
	RandomGen<sparse_ugraph>::create_graph(sug, 100, .30);
	
	//check if inside the range
	EXPECT_GE(.301, sug.density());
	EXPECT_LE(.290, sug.density());

	//TODO...
	
}

////////////////////
//
// DSIABLED TESTS - CHECK
//
////////////////////


TEST(Random_Graph, DISABLE_create_benchmark) {

	//will crate a benchmark in default (project) directory
	random_attr_t r(100, 150, .3, .5, 1, 50, .1);
	RandomGen<ugraph>::create_graph_benchmark(".", r);
	//RandomGen<ugraph>::create_graph_benchmark(".", r);
}

TEST(Random_Graph, DISABLE_create_sparse_benchmark) {

	////will crate a benchmark in default (project) directory
	//random_attr_t r(100, 150, .3, .5, 1, 50, .1); 
	//RandomGen<sparse_ugraph>::create_graph_benchmark(".", r);
	////RandomGen<sparse_ugraph>::create_graph_benchmark(".", r);
}

TEST(Sparse_Random_Graph, edge_cases) {

	sparse_ugraph g;

	// single vertex - no edges 
	experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g, 1, 0.5);
	EXPECT_EQ(1, g.num_vertices());
	EXPECT_EQ(0, g.num_edges(false));

	// p = 0 - empty graph 
	experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g, 100, 0.0);
	EXPECT_EQ(100, g.num_vertices());
	EXPECT_EQ(0, g.num_edges(false));

	// p = 1 - complete graph 
	const std::size_t NV = 60;
	experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g, NV, 1.0);
	EXPECT_EQ(NV * (NV - 1) / 2, g.num_edges(false));
	for (std::size_t i = 0; i < NV; ++i) {
		EXPECT_FALSE(g.is_edge((int)i, (int)i));
	}
}

TEST(Sparse_Random_Graph, seed) {

	sparse_ugraph g1, g2, g3;

	experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g1, 1000, 0.01, 7);
	experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g2, 1000, 0.01, 7);
	experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g3, 1000, 0.01, 8);

	auto same_edges = [](sparse_ugraph& ga, sparse_ugraph& gb) {
		if (ga.num_vertices() != gb.num_vertices()) return false;
		for (int i = 0; i < (int)ga.num_vertices(); ++i)
			for (int j = i + 1; j < (int)ga.num_vertices(); ++j)
				if (ga.is_edge(i, j) != gb.is_edge(i, j)) return false;
		return true;
	};

	// same seed, same graph 
	EXPECT_EQ(g1.num_edges(false), g2.num_edges(false));
	EXPECT_TRUE(same_edges(g1, g2));

	// different seed, different graph (with overwhelming probability) 
	EXPECT_FALSE(same_edges(g1, g3));
}

TEST(Sparse_Random_Graph, density) {

	const std::size_t NV = 20000;
	const double p = 1e-4;
	const double m = NV * (NV - 1) / 2.0;
	const int NREP = 10;

	std::uint64_t num_edges = 0;
	std::uint64_t num_bad = 0;			// edges (i, j) with i >= j in the upper triangle (lower is symmetric)
	std::uint64_t num_loops = 0;

	for (int s = 1; s <= NREP; ++s) {

		sparse_ugraph g;
		experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g, NV, p, s);
		ASSERT_EQ(NV, g.num_vertices());

		for (std::size_t i = 0; i < NV; ++i) {
			num_loops += g.is_edge((int)i, (int)i);
			int j = static_cast<int>(i);
			while ((j = g.neighbors((int)i).next_bit(j)) != BBObject::noBit) {
				num_bad += (static_cast<std::size_t>(j) <= i);
				++num_edges;
			}
		}
	}

	EXPECT_EQ(0, num_bad);
	EXPECT_EQ(0, num_loops);

	// mean number of edges within 5 standard deviations of the binomial mean 
	const double mean = m * p;
	const double sd = std::sqrt(m * p * (1 - p) / NREP);
	EXPECT_NEAR(mean, static_cast<double>(num_edges) / NREP, 5 * sd);
}

TEST(Sparse_Random_Graph, first_edge) {

	// regression: the first candidate edge (0, 1) must be reachable,
	// with probability p 
	const int NREP = 2000;
	const double p = 0.3;
	int count = 0;

	for (int s = 1; s <= NREP; ++s) {
		sparse_ugraph g;
		experimental::SparseRandomGen<sparse_ugraph>::create_ugraph(g, 50, p, s);
		count += g.is_edge(0, 1);
	}

	const double sd = std::sqrt(p * (1 - p) / NREP);
	EXPECT_NEAR(p, static_cast<double>(count) / NREP, 5 * sd);
}

////////////////////
//
// COMMENTED OUT TESTS - CHECK
//
////////////////////

//TEST(Sparse_Random_Graph, write_to_file){
//	sparse_ugraph ug;
//	SparseRandomGen<> spgen;
//	spgen.create_ugraph(ug,10000,.02);
//	ug.print_data();
//	
//	EXPECT_GE(10000, ug.number_of_vertices());
//	EXPECT_GE(.025, ug.density());
//
//	//will create a log file
//	ofstream f("log.txt");
//	ug.write_dimacs(f);
//	f.close();
//	ugraph ugnsp("log.txt");
//	
//	EXPECT_EQ(ug.number_of_vertices(), ugnsp.number_of_vertices());
//	EXPECT_EQ(ug.number_of_edges(), ugnsp.number_of_edges());
//	
//	//*** remove log file
//
//}
//
//TEST(Sparse_Random_Graph, ugraph){
//	sparse_ugraph ug;
//	SparseRandomGen<> spgen;
//	spgen.create_ugraph(ug,100000,.002);
//	ug.print_data();
//	
//	EXPECT_GE(100000, ug.number_of_vertices());
//	EXPECT_GE(.025, ug.density());
//}

//1.000.000 graph generation test
//TEST(Sparse_Random_Graph, very_large_ugraph){
//	sparse_ugraph ug;
//	SparseRandomGen<> spgen;
//	spgen.create_ugraph(ug,1000000,.0002);
//	ug.print_data();
//	
//	EXPECT_GE(1000000, ug.number_of_vertices());
//	EXPECT_GE(.00025, ug.density());
//}





