/*
* @file test_wegraph.cpp
* @brief Unit tests for edge-weighted graphs
* @created 20/7/18
* @last_update 27/01/25
* @author pss
*
* @TODO - ADD TESTS, CHECK DISABLED (08/01/2025)
*/

#include <iostream>
#include "gtest/gtest.h"
#include "graph/algorithms/graph_gen.h"		
#include "graph/simple_graph_ew.h"
#include "utils/logger.h"	
#include "utils/common_paths.h"
#include "utils/common.h"

using namespace std;

//alias
using ugraph_ewi = Graph_EW<ugraph, int>;
using ugraph_ew	 = Graph_EW<ugraph, double>;
using vint = vector<int>;

class UGraphEWTest : public ::testing::Test {
protected:
	void SetUp() override {
		ugew.reset(NV, ugraph_ewi::ZERO_WEIGHT);				//all weights to ZERO_WEIGHT, edges, non-edges and vertices
		ugew.add_edge(0, 1, 1);
		ugew.add_edge(0, 2, 2);
		ugew.add_edge(1, 3, 3);
		ugew.name("toy_weighted");
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 5;
	ugraph_ewi ugew;											//undirected graph with integer weights
};

TEST_F(UGraphEWTest, basic_setup) {
	
	EXPECT_EQ(ugraph_ewi::ZERO_WEIGHT, ugew.weight(1, 4));
	EXPECT_EQ(1, ugew.weight(0, 1));
	EXPECT_EQ(2, ugew.weight(0, 2));
	EXPECT_EQ(3, ugew.weight(1, 3));
	EXPECT_EQ(3, ugew.number_of_edges());
	EXPECT_EQ(5, ugew.size());
	
}

TEST_F(UGraphEWTest, copy_constructor) {
	ugraph_ewi gcopy(ugew);

	EXPECT_EQ(NV, gcopy.number_of_vertices());
	EXPECT_EQ(ugew.number_of_edges(), gcopy.number_of_edges());

	//TODO... add more tests
}

TEST_F(UGraphEWTest, add_edge) {
		
	ugew.add_edge(1, 4, 6);					//(1,4) is a new edge

	EXPECT_TRUE	(ugew.is_edge(1, 4));
	EXPECT_EQ	(6, ugew.weight(1, 4));
	EXPECT_EQ	(4, ugew.number_of_edges());
}

TEST_F(UGraphEWTest, set_edge_weights) {

	ugew.set_weight(1, 3, 7);			//(1,3) is an existing edge	

	EXPECT_EQ(7, ugew.weight(1, 3) );
}

TEST_F(UGraphEWTest, complement_weights) {

	ugew.complement_weights();			

	EXPECT_EQ(-1, ugew.weight(0, 1));
	EXPECT_EQ(-2, ugew.weight(0, 2));
	EXPECT_EQ(-3, ugew.weight(1, 3));
}

TEST_F(UGraphEWTest, make_edge_weighted) {

	//sets vertex weights to NO_WEIGHT
	ugew.make_edge_weighted(false);

	EXPECT_EQ(decltype(ugew)::NO_WEIGHT, ugew.weight(0, 0));			
	EXPECT_EQ(decltype(ugew)::ZERO_WEIGHT, ugew.weight(1, 2));			
	EXPECT_EQ(decltype(ugew)::ZERO_WEIGHT, ugew.weight(1, 4));

	//also sets non-edges to NO_WEIGHT
	ugew.make_edge_weighted(true);

	EXPECT_EQ(decltype(ugew)::NO_WEIGHT, ugew.weight(0, 0));
	EXPECT_EQ(decltype(ugew)::NO_WEIGHT, ugew.weight(1, 2));
	EXPECT_EQ(decltype(ugew)::NO_WEIGHT, ugew.weight(1, 4));
}

TEST_F(UGraphEWTest, create_complement) {

	ugraph_ewi ugcomp;

	///////////////////////////////
	ugew.create_complement(ugcomp);
	///////////////////////////////

	EXPECT_EQ(ugew.size(), ugcomp.size());
	EXPECT_FALSE(ugcomp.is_edge(0,1));
	EXPECT_FALSE(ugcomp.is_edge(0, 2));
	EXPECT_FALSE(ugcomp.is_edge(1, 3));
	EXPECT_TRUE(ugcomp.is_edge(0, 3));
}

TEST_F(UGraphEWTest, transform_weights) {

	//multiply all weigths (vertex and edge-weights) by 5
	ugew.transform_weights([](int a) {return 5 * a;});

	EXPECT_EQ(5,  ugew.weight(0, 1));
	EXPECT_EQ(10, ugew.weight(0, 2));
	EXPECT_EQ(15, ugew.weight(1, 3));
	EXPECT_EQ(decltype(ugew)::ZERO_WEIGHT, ugew.weight(1, 4));		//NO_WEIGHT - non-edge
	

	EXPECT_EQ(decltype(ugew)::ZERO_WEIGHT, ugew.weight(1, 4));		//NO_WEIGHT - non-edge

	//same using the scale functor utility
	ugew.transform_weights(com::scale<decltype(ugew)::_wt>(5));
	EXPECT_EQ(25, ugew.weight(0, 1));
	EXPECT_EQ(50, ugew.weight(0, 2));
	EXPECT_EQ(75, ugew.weight(1, 3));

}

TEST_F(UGraphEWTest, generate_weights) {

	//generate modulus-weights on the edges 
	EdgeWeightGen<ugraph_ewi>::create_weights(	ugew,
												EdgeWeightGen<ugraph_ewi>::WMOD,
												DEFAULT_WEIGHT_MODULUS					);		

	EXPECT_EQ(NV, ugew.graph().number_of_vertices());
	EXPECT_EQ(4, ugew.weight(0, 1));
	EXPECT_EQ(5, ugew.weight(0, 2));
	EXPECT_EQ(7, ugew.weight(1, 3));
	EXPECT_TRUE(ugew.is_consistent());

	//I/O
	//ugew.print_weights()
}

TEST(UGraphEW, constructor_file) {

	ugraph_ewi ugew(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "toy_ew_dimacs.txt");

	EXPECT_EQ(5, ugew.number_of_vertices());
	EXPECT_EQ(5, ugew.number_of_edges());

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

TEST(UGraphEW, create_an_edge_weighted_graph) {

	ugraph_ewi ug (10);	
	ug.set_vertex_weight(ugraph_ewi::NO_WEIGHT);			//sets all vertex weights to NO_WEIGHT
	ug.erase_non_edge_weights();							//sets all non-edge weights to NO_WEIGHT
	
	ug.add_edge(0, 1, 1);
	ug.add_edge(0, 2, 2);

	///////////////////////////////////////////////////
	EXPECT_EQ(ugraph_ewi::NO_WEIGHT, ug.weight(0, 3));
	//...
	///////////////////////////////////////////////////
}

//////////////////
//
// DISABLED TESTS - CHECK
//
//////////////////

TEST_F(UGraphEWTest, DISABLED_printing) {
	//visual test - DISABLE by default

	//I/O
	ugew.print_data();
	ugew.print_vertex(1);	
	ugew.print_edge(0, 1);
	ugew.print_weights(cout, true, decltype(ugew)::EDGE);

	LOG_INFO("push any key to continue...");	
	cin.get();
}

TEST(UGraphEW, DISABLED_gen_random ){
////////////////
// generates a random graph and adds random edge weights
//
// TODO@do real test queries

	ugraph_ewi ugew;
	const int NV = 10;

	RandomGen<ugraph_ewi>::create_graph(ugew, NV, .3);										
	EdgeWeightGen< ugraph_ewi >::create_weights(ugew, EdgeWeightGen<ugraph_ewi>::WMOD);

	//QUERIES....

	//I/O
	/*ugew.print_data();
	ugew.print_weights();*/

//////////////////
//directed graph

	//Graph_EW<graph, int> gew;
	//typedef Graph_EW<graph, int> _gt;
	//
	//RandomGen<_gt>::create_graph(gew, NV, .3);								/* 0.0 edge weights */
	//EdgeWeightGen< _gt >::create_weights(gew, EdgeWeightGen<_gt>::WMOD);

	////QUERIES....

	////I/O
	//gew.print_data();
	//gew.print_weights();

}

TEST(UGraphEW, DISABLED_write_to_file) {

	//ofstream of(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "toy_ew_dimacs_gen.txt");
	////////////////////////////////////
	//ugew.write_dimacs(of);
	////////////////////////////////////
	//of.close();

	//TODO@read and test properly (visually correct);
	//Graph_EW<ugraph, int> ugew_gen(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "toy_ew_dimacs_gen.txt");
	//ugew.print_weights();
}


///////////////
//TODO@Check these tests!!! (17/01/19) Not sure they are well coupled with the current implementation

//TEST(EdgeWeighted, ub_DIMACS_subgraph){
//////////////////
//// date@ 14/08/18
//// tests for draft upper bound on an induced subgraph
////
//// COMMENTS: currently a lower bound for the subgraph is not available
////           so this is pending!
//	
//	LOG_INFO("EdgeWeighted:ub_DIMACS_subgraph---------------------------------------");
//
//	string filename="brock200_1.clq";
////	string filename="brock200_2.clq";
////	string filename="brock200_2.clq";
////	string filename="c-fat200-1.clq";
////	string filename="keller4.clq";
////	string filename="MANN_a9.clq";
////	string filename="p_hat300-3.clq";
////	string filename="p_hat500-3.clq";
////	string filename="johnson8-4-4.clq";
//	
//	string fullname=PATH+filename;
//	ugraph ug(fullname);
//	const int NV=ug.number_of_vertices();
//	
//	//generate weights (only mode)
//	EdgeWeightGen<ugraph>::create_weights(ug, EdgeWeightGen<ugraph>::WMOD, ".we", PATH);
//		
//	const int LB=5000;									//CHECK! (sol is apparently 21230)
//	ubew<ugraph> ubg(&ug);
//	ubg.init();
//
//	//subgraph of first 50 nodes
//	bitarray bbv(NV);
//	bbv.set_bit(0, 49);
//
//	int nb_cand=ubg.driver(LB, bbv, 1);
//
//	//I/O: candidates
//	LOG_INFO("number of candidates: "<<nb_cand);
//	if(nb_cand>0){
//		stringstream sstr;
//		ubg.print_candidate_nodes(true,sstr);			/* MUST BE sorted according to non increasing weights */
//		LOG_INFO("candidates: "<<sstr.str());
//	}
//
//	//TESTS....
//	//EXPECT_TRUE(nb_cand>0);
//	//EXPECT_TRUE(1, ubg.first().get_elem(0));			//first elem
//	//EXPECT_TRUE(1, ubg.last().get_elem(0));			//last elem
//
///////////////////
////sort graph pre-processing
//	//nb_cand=ubg.driver(LB, 0.0, gbbs::MIN_DEG_ABS, gbbs::PLACE_FL);
//	nb_cand=ubg.driver(LB, bbv, 1.0, gbbs::MAX_DEG_ABS, gbbs::PLACE_LF);
//	//int nb_cand=ubg.driver(gbbs::MIN_DEG_DEGEN, gbbs::PLACE_LF, LB);
//
//	//I/O: candidates
//	LOG_INFO("number of candidates: "<<nb_cand);
//	if(nb_cand>0){
//		stringstream sstr;
//		ubg.print_candidate_nodes(true,sstr);			/* MUST BE sorted according to non increasing weights */
//		LOG_INFO("candidates: "<<sstr.str());
//	}
//
//	//TESTS....
//	//EXPECT_TRUE(nb_cand>0);
//	//EXPECT_TRUE(1, ubg.first().get_elem(0));			//first elem
//	//EXPECT_TRUE(1, ubg.last().get_elem(0));			//last elem
//	
//	LOG_INFO("------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE WITH NEXT TEST...");
//	cin.get();
//}
//
//TEST(EdgeWeighted, ub_DIMACS){
//////////////////
//// tests for draft upper bound
//// date of creation: 14/08/18
//
//	LOG_INFO("EdgeWeighted:ub_DIMACS---------------------------------------");
//
////	string filename="brock200_1.clq";
////	string filename="brock200_2.clq";
////	string filename="brock200_2.clq";
////	string filename="c-fat200-1.clq";
////	string filename="keller4.clq";
////	string filename="MANN_a9.clq";
////	string filename="p_hat300-3.clq";
//	string filename="p_hat500-3.clq";
////	string filename="johnson8-4-4.clq";
//	
//	string fullname=PATH+filename;
//	ugraph ug(fullname);
//	const int NV=ug.number_of_vertices();
//	
//	//generate weights (only mode)
//	EdgeWeightGen<ugraph>::create_weights(ug, EdgeWeightGen<ugraph>::WMOD, ".we", PATH);
//			
//	//I/O: edge-weights of first k nodes
//	const int K=10;
//	vint lv(K, 0);
//	for(int i=0; i<K; i++){
//		lv[i]=i;
//	}
//	stringstream sstrw;
//	ug.print_edge_weights(lv, sstrw);
//	LOG_INFO(sstrw.str());
//
//	const int LB=122300;									//CHECK! (sol is apparently 21230)
//	ubew<ugraph> ubg(&ug);
//	ubg.init();
//	int nb_cand=ubg.driver(LB, 1.0);
//
//	//I/O: candidates
//	LOG_INFO("number of candidates: "<<nb_cand);
//	if(nb_cand>0){
//		stringstream sstr;
//		ubg.print_candidate_nodes(true,sstr);			/* MUST BE sorted according to non increasing weights */
//		LOG_INFO("candidates: "<<sstr.str());
//	}
//
//	//TESTS....
//	//EXPECT_TRUE(nb_cand>0);
//	//EXPECT_TRUE(1, ubg.first().get_elem(0));			//first elem
//	//EXPECT_TRUE(1, ubg.last().get_elem(0));			//last elem
//
///////////////////
////sort graph pre-processing
//	//	nb_cand=ubg.driver(LB, 1.0, gbbs::MIN_DEG_ABS, gbbs::PLACE_FL);
//	nb_cand=ubg.driver(LB, 1.0, gbbs::MAX_DEG_ABS, gbbs::PLACE_LF);
//	//int nb_cand=ubg.driver(gbbs::MIN_DEG_DEGEN, gbbs::PLACE_LF, LB);
//
//	//I/O: candidates
//	LOG_INFO("number of candidates: "<<nb_cand);
//	if(nb_cand>0){
//		stringstream sstr;
//		ubg.print_candidate_nodes(true,sstr);			/* MUST BE sorted according to non increasing weights */
//		LOG_INFO("candidates: "<<sstr.str());
//	}
//
//	//TESTS....
//	//EXPECT_TRUE(nb_cand>0);
//	//EXPECT_TRUE(1, ubg.first().get_elem(0));			//first elem
//	//EXPECT_TRUE(1, ubg.last().get_elem(0));			//last elem
//	
//
//	LOG_INFO("------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE WITH NEXT TEST...");
//	cin.get();
//}
//
//TEST(EdgeWeighted, ub_basic_II){
//////////////////
//// tests for draft upper bound
//// date of creation: 14/08/18
//
//	LOG_INFO("EdgeWeighted:ub_basic_II---------------------------------------");
//
//	const int NV=6;
//	ugraph g(NV);					//w(G,we)=11
//	g.add_edge(0,1);	
//	g.add_edge(0,3);
//	g.add_edge(1,2);
//	g.add_edge(1,3);
//	g.add_edge(2,4);
//	g.add_edge(3,4);
//	g.add_edge(3,5);
//	g.add_edge(4,5);	
//		
//	g.init_we();			
//	g.set_edge_weight(0, 1, 7);		
//	g.set_edge_weight(0, 3, 1);
//	g.set_edge_weight(1, 2, 2);
//	g.set_edge_weight(1, 3, 9);
//	g.set_edge_weight(2, 4, 6);
//	g.set_edge_weight(3, 4, 4);		
//	g.set_edge_weight(3, 5, 2);
//	g.set_edge_weight(4, 5, 5);
//
//	//I/O: edge-weights
//	stringstream sstrw;
//	g.print_edge_weights(sstrw);
//	LOG_INFO(sstrw.str());
//
//	const int LB=11;
//	ubew<ugraph> ubg(&g);
//	ubg.init();
//	int nb_cand=ubg.driver(LB, 1.0);
//
//	//I/O: candidates
//	LOG_INFO("number of candidates: "<<nb_cand);
//	if(nb_cand>0){
//		stringstream sstr;
//		ubg.print_candidate_nodes(true,sstr);						/* MUST BE sorted according to non increasing weights */
//		LOG_INFO("candidates: "<<sstr.str());
//	}
//
//	//TESTS....
//	//EXPECT_TRUE(nb_cand>0);
//	//EXPECT_TRUE(1, ubg.first().get_elem(0));		//first elem
//	//EXPECT_TRUE(1, ubg.last().get_elem(0));			//last elem
//
///////////////////
////sort graph pre-processing
//	//nb_cand=ubg.driver(gbbs::MIN_DEG_ABS, gbbs::PLACE_FL, LB);
//	nb_cand=ubg.driver(LB, 1.0, gbbs::MAX_DEG_ABS, gbbs::PLACE_LF);  /* MAX_DEG-ABS is best */
//
//	//I/O: candidates
//	LOG_INFO("number of candidates: "<<nb_cand);
//	if(nb_cand>0){
//		stringstream sstr;
//		ubg.print_candidate_nodes(true,sstr);						/* MUST BE sorted according to non increasing weights */
//		LOG_INFO("candidates: "<<sstr.str());
//	}
//
//	//TESTS....
//	//EXPECT_TRUE(nb_cand>0);
//	//EXPECT_TRUE(1, ubg.first().get_elem(0));		//first elem
//	//EXPECT_TRUE(1, ubg.last().get_elem(0));			//last elem
//	
//
//	LOG_INFO("------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE WITH NEXT TEST...");
//	cin.get();
//}
//
//TEST(EdgeWeighted, ub_basic){
//////////////////
//// tests for draft upper bound
//// date of creation: 14/08/18
//
//	LOG_INFO("EdgeWeighted:ub_basic---------------------------------------");
//
//	const int NV=5;
//	ugraph g(NV);			
//	g.add_edge(0,1);	
//	g.add_edge(0,2);
//	g.add_edge(1,2);
//	g.add_edge(2,3);
//	g.add_edge(3,4);
//	
//	g.init_we();
//	g.set_edge_weight(0, 1, 3);		
//	g.set_edge_weight(0, 2, 8);
//	g.set_edge_weight(1, 2, 7);
//	
//	//I/O: edge-weights
//	stringstream sstrw;
//	g.print_edge_weights(sstrw);
//	LOG_INFO(sstrw.str());
//
//	const int LB=15;
//	ubew<ugraph> ubg(&g);
//	ubg.init();
//	int nb_cand=ubg.driver(LB);
//
//	//I/O: candidates
//	LOG_INFO("number of candidates: "<<nb_cand);
//	if(nb_cand>0){
//		stringstream sstr;
//		ubg.print_candidate_nodes(sstr);						/* MUST BE sorted according to non increasing weights */
//		LOG_INFO("candidates: "<<sstr.str());
//	}
//
//	//TESTS....
//	EXPECT_TRUE(nb_cand>0);
//	EXPECT_TRUE(1, ubg.first().get_elem(0));		//first elem
//	EXPECT_TRUE(1, ubg.last().get_elem(0));			//last elem
//
//	LOG_INFO("------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE WITH NEXT TEST...");
//	cin.get();
//}
//
//TEST(EdgeWeighted, gen_edge_weights_to_file){
//////////////////
//// reads DIMACS file without weights and generates "*.we" file with edge_weights
//// date of creation: 20/07/18
//
//	LOG_INFO("EdgeWeighted:gen_edge_weights_to_file---------------------------------------");
//
//	string filename="brock200_1.clq";
//	string fullname=PATH+filename;
//	ugraph ug(fullname);
//	const int NV=ug.number_of_vertices();
//	
//	//generate weights (only mode)
//	EdgeWeightGen<ugraph>::create_weights(ug, EdgeWeightGen<ugraph>::WMOD, ".we", PATH);
//	
//	//TESTS
//	EXPECT_TRUE(ug.is_edge_weighted());
//	
//	
//	LOG_INFO("----------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE WITH NEXT TEST...");
//	cin.get();
//}
//
//TEST(EdgeWeighted, basic){
//	LOG_INFO("EdgeWeighted:basic---------------------------------------");
//
//	const int NV=5;
//	ugraph g(NV);
//	g.add_edge(0,1);
//	g.add_edge(1,2);
//
//	g.init_we();
//	g.set_edge_weight(0, 1, 3.0);		//weights are truncated to int (ugraph has W=INT)
//	g.set_edge_weight(1, 2, 4);
//	g.set_edge_weight(1, 2, 8);
//	g.set_edge_weight(1, 3, 75);			//no weight added: no edge
//	g.set_edge_weight(2, 2, 75);			//no loops, weight is set to 0.0
//
//
//	g.print_edge_weights(cout);	
//
//	//TESTS
//	EXPECT_TRUE(g.is_edge_weighted());
//	EXPECT_EQ(3, g.edge_weights(0,1));
//	EXPECT_EQ(8, g.edge_weights(1,2));
//	EXPECT_EQ(3, g.edge_weights(1,0));
//	EXPECT_EQ(8, g.edge_weights(2,1));
//	EXPECT_EQ(0, g.edge_weights(1,3));
//	EXPECT_EQ(0, g.edge_weights(2,2));	//no loops, weigh is 0
//	
//	//I/O
//	g.write_edge_weights(FILE_LOG("log.txt",WRITE));
//
//	LOG_INFO("----------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE WITH NEXT TEST...");
//	cin.get();
//}
