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
#include <cstdio>
#include <fstream>
#include <sstream>
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


TEST(Random_Graph, create_benchmark) {

	// creates the benchmark in the system temporary directory
	const std::string path = ::testing::TempDir();
	random_attr_t r(100, 150, .3, .5, 1, 50, .1);
	RandomGen<ugraph>::create_graph_benchmark(path, r);

	// expected files r<n>_<p>_<k>.txt - same naming loop as create_graph_benchmark
	int num_files = 0;
	for (std::size_t n = r.nLB; n <= r.nUB; n += r.incN) {
		for (std::size_t pi = 0; ; ++pi) {
			const double p = r.pLB + pi * r.incP;
			if (p > r.pUB)
				break;

			for (std::size_t k = 0; k < r.nRep; ++k) {
				std::ostringstream os;
				os << path << "r" << n << "_" << p << "_" << k << ".txt";

				// the file is a valid DIMACS graph with n vertices
				{
					ugraph g(os.str());
					EXPECT_EQ(n, g.num_vertices());
				}

				// cleanup - removing also checks that the file was written
				EXPECT_EQ(0, std::remove(os.str().c_str()));
				++num_files;
			}

			if (p >= r.pUB)
				break;
		}
	}

	EXPECT_EQ(6, num_files);
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
// WEIGHT GENERATION
//
////////////////////

TEST(VertexWeightGen, modulus_dimacs){
		
	//read DIMACS graph from file - no DIMACS weights, sets default weights 
	ugraph_vwi ugw (BITGRAPH_GRAPH_TEST_DATA_DIR  "brock200_1.clq");

	const int NV = ugw.num_vertices();

	EXPECT_EQ(200, NV);
	EXPECT_EQ(ugraph_vwi::DEFAULT_WEIGHT, ugw.weight(0));
	EXPECT_EQ(ugraph_vwi::DEFAULT_WEIGHT, ugw.weight(1));
	EXPECT_EQ(ugraph_vwi::DEFAULT_WEIGHT, ugw.weight(199));

	//generate modulus weights 
	experimental::VertexWeightGen< ugraph_vwi >::create_weights(
		ugw,
		experimental::VertexWeightGen<ugraph_vwi>::weight_type::modulus, 
		200);
		
	EXPECT_EQ(2, ugw.weight(0));
	EXPECT_EQ(3, ugw.weight(1));
	EXPECT_EQ(200, ugw.weight(198));
	EXPECT_EQ(1, ugw.weight(199));			//unit weight - check!
}

TEST(VertexWeightGen, random_graph_modulus) {
	
	const int NV = 50;
	const double density = 0.31;
		
	//creates random graph of size NV with modulus weights
	ugraph_vwi ugw;
	RandomGen<ugraph_vwi>::create_graph(ugw, NV, density);							/* empty vertex weights by default */
	experimental::VertexWeightGen<ugraph_vwi>::create_weights(ugw,
		experimental::VertexWeightGen<ugraph_vwi>::weight_type::modulus);
	
	/////////////////////
	EXPECT_EQ(NV, ugw.num_vertices());
	EXPECT_TRUE(ugw.graph().density() < density + 0.001);
	EXPECT_TRUE(ugw.graph().density() > density - 0.001);
	////////////////////
}

TEST(VertexWeightGen, degree) {

	using gen_t = experimental::VertexWeightGen<ugraph_vwi>;

	ugraph_vwi ugw(6);
	ugw.add_edge(0, 1);
	ugw.add_edge(0, 2);
	ugw.add_edge(0, 3);
	ugw.add_edge(1, 2);					// vertices 4 and 5 are isolated 

	gen_t::create_weights(ugw, gen_t::weight_type::degree);

	EXPECT_EQ(3, ugw.weight(0));
	EXPECT_EQ(2, ugw.weight(1));
	EXPECT_EQ(2, ugw.weight(2));
	EXPECT_EQ(1, ugw.weight(3));
	EXPECT_EQ(0, ugw.weight(4));			// isolated vertices get weight 0 
	EXPECT_EQ(0, ugw.weight(5));
}

TEST(VertexWeightGen, modulus) {

	using gen_t = experimental::VertexWeightGen<ugraph_vwi>;

	ugraph_vwi ugw(7);

	// w(v) = 1 + ((v + 1) % wmod) 
	gen_t::create_weights(ugw, gen_t::weight_type::modulus, 3);

	EXPECT_EQ(2, ugw.weight(0));
	EXPECT_EQ(3, ugw.weight(1));
	EXPECT_EQ(1, ugw.weight(2));
	EXPECT_EQ(2, ugw.weight(3));
	EXPECT_EQ(3, ugw.weight(4));
	EXPECT_EQ(1, ugw.weight(5));
	EXPECT_EQ(2, ugw.weight(6));
}

TEST(VertexWeightGen, to_file) {

	using gen_t = experimental::VertexWeightGen<ugraph_vwi>;

	ugraph_vwi ugw(10);
	ugw.add_edge(0, 1);
	ugw.add_edge(2, 3);
	ugw.set_name("gen_weights_to_file_test");

	// writes <TempDir>/gen_weights_to_file_test.w 
	const std::string path = ::testing::TempDir();
	gen_t::create_weights(ugw, gen_t::weight_type::modulus, 4, ".w", path);

	const std::string filename = path + "gen_weights_to_file_test.w";

	// file contents are the generated weights, as streamed by print_weights 
	{
		std::ifstream f(filename);
		ASSERT_TRUE(f.good());
		std::ostringstream contents, expected;
		contents << f.rdbuf();
		ugw.print_weights(expected, false);
		EXPECT_EQ(expected.str(), contents.str());
	}

	// cleanup - removing also checks that the file was written 
	EXPECT_EQ(0, std::remove(filename.c_str()));
}

TEST(EdgeWeightGen, modulus) {

	const int NV = 5;
	ugraph_ewi ugew(NV);
	ugew.add_edge(0, 1, 1);
	ugew.add_edge(0, 2, 2);
	ugew.add_edge(1, 3, 3);

	//generate modulus-weights on the edges 
	experimental::EdgeWeightGen<ugraph_ewi>::create_weights (
		ugew,
		DEFAULT_WEIGHT_MODULUS);

	EXPECT_EQ(NV, ugew.graph().num_vertices());
	EXPECT_EQ(3, ugew.weight(0, 1));
	EXPECT_EQ(4, ugew.weight(0, 2));
	EXPECT_EQ(6, ugew.weight(1, 3));
	EXPECT_TRUE(ugew.is_consistent());  // validate_weight_matrix()
}

TEST(EdgeWeightGen, modulus_non_default) {

	using gen_t = experimental::EdgeWeightGen<ugraph_ewi>;

	const int NV = 6;
	ugraph_ewi ugew(NV);
	ugew.add_edge(0, 1);
	ugew.add_edge(0, 4);
	ugew.add_edge(2, 5);
	ugew.add_edge(4, 5);

	// w(v, w) = 2 + ((v + w) % (wmod - 1)), weights in [2, wmod] 
	gen_t::create_weights(ugew, 4);

	EXPECT_EQ(3, ugew.weight(0, 1));		// 2 + (1 % 3) 
	EXPECT_EQ(3, ugew.weight(0, 4));		// 2 + (4 % 3) 
	EXPECT_EQ(3, ugew.weight(2, 5));		// 2 + (7 % 3) 
	EXPECT_EQ(2, ugew.weight(4, 5));		// 2 + (9 % 3) 

	// symmetric weights 
	EXPECT_EQ(ugew.weight(0, 1), ugew.weight(1, 0));
	EXPECT_EQ(ugew.weight(4, 5), ugew.weight(5, 4));

	// non-edges have no weight 
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, ugew.weight(0, 2));
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, ugew.weight(3, 5));

	// all weights in [2, wmod] 
	for (int v = 0; v < NV; ++v) {
		for (int w = v + 1; w < NV; ++w) {
			if (ugew.graph().is_edge(v, w)) {
				EXPECT_GE(ugew.weight(v, w), 2);
				EXPECT_LE(ugew.weight(v, w), 4);
			}
		}
	}

	EXPECT_TRUE(ugew.is_consistent());
}

TEST(EdgeWeightGen, invalid_modulus_terminates) {

	using gen_t = experimental::EdgeWeightGen<ugraph_ewi>;

	ugraph_ewi ugew(3);
	ugew.add_edge(0, 1);

	EXPECT_DEATH(gen_t::create_weights(ugew, 1), "");
	EXPECT_DEATH(gen_t::create_weights(ugew, 0), "");
	EXPECT_DEATH(gen_t::create_weights(ugew, -5), "");
}

TEST(EdgeWeightGen, to_file) {

	using gen_t = experimental::EdgeWeightGen<ugraph_ewi>;

	ugraph_ewi ugew(5);
	ugew.add_edge(0, 1);
	ugew.add_edge(1, 3);
	ugew.add_edge(2, 4);
	ugew.set_name("edge_weight_gen_to_file_test");

	// writes <TempDir>/edge_weight_gen_to_file_test.ew 
	const std::string path = ::testing::TempDir();
	gen_t::create_weights(ugew, DEFAULT_WEIGHT_MODULUS, ".ew", path);

	const std::string filename = path + "edge_weight_gen_to_file_test.ew";

	// file contents are the edge-weight matrix, as streamed by print_edges 
	{
		std::ifstream f(filename);
		ASSERT_TRUE(f.good());
		std::ostringstream contents, expected;
		contents << f.rdbuf();
		ugew.print_edges(expected, ugraph_ewi::edge_print_format::edge_matrix, false);
		EXPECT_EQ(expected.str(), contents.str());
	}

	// cleanup - removing also checks that the file was written 
	EXPECT_EQ(0, std::remove(filename.c_str()));
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





