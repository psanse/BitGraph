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

using namespace std;

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
	EXPECT_GE(.33, ug.density());
	EXPECT_LE(.27, ug.density());
	
	//TODO...
}

TEST(Random_Graph, sparse_ugraph){
	
	sparse_ugraph sug;
	RandomGen<sparse_ugraph>::create_graph(sug, 100, .30);
	
	//check if inside the range
	EXPECT_GE(.33, sug.density());
	EXPECT_LE(.27, sug.density());

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





