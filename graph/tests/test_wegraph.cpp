//test_wegraph.cpp: all tests for graphs with edge_weights (for now weights are over undirected edges to all effects; I am not sure yet if directed edges will have impact on weighted edges)
//initial date:20/7/18
//author:pss

#include <iostream>
#include "gtest/gtest.h"
#include "graph.h"										//contains all relevant graph types
#include "utils/logger.h"
#include "utils/file.h"
#include "graph_gen.h"			
#include "utils/common_paths.h"

using namespace std;

//#define TEST_GRAPH_FAST_SORT_STEP_BY_STEP										//[DEF-OFF] ON for one test at a time

TEST(EdgeWeighted_New, read_write_dimacs) {

	LOG_INFO("EdgeWeighted_New:read_write_dimacs------------------------");

	Graph_EW<ugraph, int> ugew(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "toy_ew_dimacs.txt");
	ugew.print_weights();
		
	//vertex weights
	EXPECT_EQ(10, ugew.get_we(0, 0));
	EXPECT_EQ(20, ugew.get_we(1, 1));
	EXPECT_EQ(30, ugew.get_we(2, 2));
	EXPECT_EQ(40, ugew.get_we(3, 3));
	EXPECT_EQ(50, ugew.get_we(4, 4));

	//edge weights
	EXPECT_EQ(27, ugew.get_we(0, 1));
	EXPECT_EQ(37, ugew.get_we(0, 2));
	EXPECT_EQ(47, ugew.get_we(0, 3));
	EXPECT_EQ(57, ugew.get_we(2, 4));
	EXPECT_EQ(67, ugew.get_we(3, 4));

	//write
	ofstream of(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "toy_ew_dimacs_gen.txt");
	//////////////////////////////////
	ugew.write_dimacs(of);
	//////////////////////////////////
	of.close();

	//TODO@read and test properly (visually correct);
	Graph_EW<ugraph, int> ugew_gen(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "toy_ew_dimacs_gen.txt");
	ugew.print_weights();
			
	LOG_INFO("EdgeWeighted_New: END read_write_dimacs-------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}

TEST(EdgeWeighted_New, basic){

	LOG_INFO("EdgeWeighted_New:basic---------------------------------");
	Graph_EW<ugraph, double> ugew(10, 0.0);
	ugew.set_name("test graph");
	
	double NOWT = Graph_EW<ugraph, double>::NOWT;		//empty weights are 0 - CKECK!
	//double NOWT = 0;		

	ugew.graph().add_edge(0,1);
	int status =ugew.set_we(0, 1, 1.0);
	EXPECT_EQ(0, status);
	
	status = ugew.set_we(0, 2, 2.0);										/* adding weight to a non-edge*/
	EXPECT_EQ(-1, status);
	
	EXPECT_DOUBLE_EQ(1.0, ugew.get_we(0,1));
	EXPECT_DOUBLE_EQ(NOWT, ugew.get_we(0,2));

	status = ugew.set_we(0, 0, 1.3);										/* adding vertex weight */
	EXPECT_DOUBLE_EQ(1.3, ugew.get_we(0, 0));
	
	//list of the first 3 nodes
	vector<int> lv;
	lv.push_back(0); lv.push_back(1); lv.push_back(2); 
		
//////////
//I/O 
	ugew.print_data();
	ugew.print_weights(cout, true);									//shows only upper triangle because it is an undirected graph
	
	//weights of only the first 3 nodes
	ugew.print_weights(lv);
	
//////////
//copy construct

	Graph_EW<ugraph, double> ugew_c(2, 0.0);
	//////////////////////////
	ugew_c = ugew;
	///////////////////////////
	EXPECT_EQ(ugew.graph().number_of_vertices(),ugew_c.graph().number_of_vertices());
	EXPECT_DOUBLE_EQ(ugew.get_we(0,1), ugew_c.get_we(0,1));
	EXPECT_DOUBLE_EQ(ugew.get_we(0,2), ugew_c.get_we(0,2));

	//I/O
	ugew_c.print_data();
	ugew_c.print_weights();

//////////
//directed graphs 
	
	//weights of type int
	Graph_EW<graph, int> gew(10, 0.0);
	gew.set_name("test graph 2");
	gew.graph().add_edge(0,1);
	gew.set_we(0, 1, 1);
	gew.set_we(0, 2, 2);							/* non-edge*/
	gew.set_we(2, 2, 3);							/* vertex weight */
			
	EXPECT_EQ(1, gew.get_we(0,1));
	EXPECT_EQ((int)NOWT, gew.get_we(0,2));
	EXPECT_EQ(3, gew.get_we(2, 2));

//////////
//I/O 
	gew.print_data();
	gew.print_weights();							

	LOG_INFO("GraphFastRootSort_W: END basic-------------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}

TEST(EdgeWeighted_New, generate_weights){
////////////////
// reads DIMACS file without weights and generates weights with %200 formula
// date of creation: 15/04/18

	LOG_INFO("EdgeWeighted_New:generate_weights-----------------------");

	string path= TEST_CLIQUE_PATH_DIMACS_ETSIDI_I9;
	Graph_EW<ugraph, int> ugew(path + "brock200_1.clq");
	const int NV=ugew.graph().number_of_vertices();
		
	//generate weights WDEG mode
	EdgeWeightGen< Graph_EW<ugraph, int> >::create_wgraph(ugew, EdgeWeightGen<Graph_EW<ugraph, int>>::WMOD, 200);

	EXPECT_EQ(200, ugew.graph().number_of_vertices());
	EXPECT_EQ(6, ugew.get_we(2, 1));		

	//vint lv; lv.push_back(100); lv.push_back(101); lv.push_back(102);
	/*ugew.print_data();
	ugew.print_weights();*/
	//ugew.print_weights(lv);
			
	LOG_INFO("EdgeWeighted_New: END generate_weights------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}

TEST(EdgeWeighted_New, gen_random){
////////////////
// generates a random graph and adds random edge weights
//
// TODO@do real test queries
	
	LOG_INFO("EdgeWeighted_New:gen_random----------------------------");

	Graph_EW<ugraph, int> ugew;
	typedef Graph_EW<ugraph, int> _ugt;
	const int NV=10;

	RandomGen<_ugt>::create_ugraph(ugew, NV, .3);								/* 0.0 edge weights */
	EdgeWeightGen< _ugt >::create_wgraph(ugew, EdgeWeightGen<_ugt>::WMOD);

	//QUERIES....

	//I/O
	ugew.print_data();
	ugew.print_weights();

//////////////////
//directed graph

	Graph_EW<graph, int> gew;
	typedef Graph_EW<graph, int> _gt;
	
	RandomGen<_gt>::create_graph(gew, NV, .3);								/* 0.0 edge weights */
	EdgeWeightGen< _gt >::create_wgraph(gew, EdgeWeightGen<_gt>::WMOD);

	//QUERIES....

	//I/O
	gew.print_data();
	gew.print_weights();
	
	LOG_INFO("EdgeWeighted_New: END gen_random-----------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
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
//	EdgeWeightGen<ugraph>::create_wgraph(ug, EdgeWeightGen<ugraph>::WMOD, ".we", PATH);
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
//	EdgeWeightGen<ugraph>::create_wgraph(ug, EdgeWeightGen<ugraph>::WMOD, ".we", PATH);
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
//	g.set_we(0, 1, 7);		
//	g.set_we(0, 3, 1);
//	g.set_we(1, 2, 2);
//	g.set_we(1, 3, 9);
//	g.set_we(2, 4, 6);
//	g.set_we(3, 4, 4);		
//	g.set_we(3, 5, 2);
//	g.set_we(4, 5, 5);
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
//	g.set_we(0, 1, 3);		
//	g.set_we(0, 2, 8);
//	g.set_we(1, 2, 7);
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
//	EdgeWeightGen<ugraph>::create_wgraph(ug, EdgeWeightGen<ugraph>::WMOD, ".we", PATH);
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
//	g.set_we(0, 1, 3.0);		//weights are truncated to int (ugraph has W=INT)
//	g.set_we(1, 2, 4);
//	g.set_we(1, 2, 8);
//	g.set_we(1, 3, 75);			//no weight added: no edge
//	g.set_we(2, 2, 75);			//no loops, weight is set to 0.0
//
//
//	g.print_edge_weights(cout);	
//
//	//TESTS
//	EXPECT_TRUE(g.is_edge_weighted());
//	EXPECT_EQ(3, g.get_we(0,1));
//	EXPECT_EQ(8, g.get_we(1,2));
//	EXPECT_EQ(3, g.get_we(1,0));
//	EXPECT_EQ(8, g.get_we(2,1));
//	EXPECT_EQ(0, g.get_we(1,3));
//	EXPECT_EQ(0, g.get_we(2,2));	//no loops, weigh is 0
//	
//	//I/O
//	g.write_edge_weights(FILE_LOG("log.txt",WRITE));
//
//	LOG_INFO("----------------------------------------------------------------");
//	LOG_INFO("PRESS ANY KEY TO CONTINUE WITH NEXT TEST...");
//	cin.get();
//}
