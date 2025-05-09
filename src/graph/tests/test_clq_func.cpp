/**
* @file  test_clq_func.cpp
* @brief This file contains unit tests for clique common functions (under the namespace qfunc).
* @dev pss
* @details: created 01/14/2023, imported in 05/05/2025
**/

#include "gtest/gtest.h"
//#include "graph/graph_gen.h"
#include "utils/common.h"
#include "utils/logger.h"
#include "graph/graph.h"
#include "graph/algorithms/clique/clq_func.h"
//#include "../clique/algorithms/cover.h"
//#include "../runs/run_CLQ_manager.h"
//#include "../clique/clique_config_paths.h"

using namespace std;

class CliqueCommonTest : public ::testing::Test {
protected:
	void SetUp() override {
		ug.reset(NV);
		ug.add_edge(0, 1);
		ug.add_edge(0, 3);
		ug.add_edge(0, 4);		
		ug.add_edge(2, 5);
		
	}
	void TearDown() override {}

	//directed graph instance	
	const int NV = 6;
	ugraph ug;
};


TEST_F(CliqueCommonTest, find_iset_enlarge_singleton) {

	int v = 1;
	int lv[] = { 0, 1, 2, 3, 4, 5 };
	int posBegin = 3;	
	int posEnd = 6;

	//////////////////////////////////////////////////////
	int nV = qfunc::find_iset (ug, v, lv, posBegin, posEnd);
	//////////////////////////////////////////////////////

	EXPECT_EQ(3, nV); 	//{3, 4, 5} - {1, 3, 4, 5} form an independent set

}

TEST_F(CliqueCommonTest, find_iset_consecutive_vertices) {

	int lv[] = { 0, 1, 2, 3, 4, 5 };
	int posBegin = 1;
	int posEnd = 6;

	//////////////////////////////////////////////////////
	int nV = qfunc::find_iset(ug, lv, posBegin, posEnd);
	//////////////////////////////////////////////////////

	EXPECT_EQ(4, nV); 	//{1, 2, 3, 4} is an iset

}


TEST_F(CliqueCommonTest, find_clique) {

	int lv[] = { 0, 1, 2, 3, 4, 5 };
	int posBegin = 2;
	int posEnd = 6;

	//////////////////////////////////////////////////////
	std::vector<int> clq = { 0 };
	int nV = qfunc::find_clique(ug, clq, lv, posBegin, posEnd);
	//////////////////////////////////////////////////////

	EXPECT_EQ(1, nV); 							//only vertex {3} is added to clq		

	std::vector<int> clq_exp = { 0, 3 };		//{0, 3} is a maximal clique
	EXPECT_EQ(clq_exp, clq);

}

TEST_F(CliqueCommonTest, find_clique_2) {

	typename ugraph::_bbt bb{ (int)ug.size(), {2, 3, 4, 5}};
	std::vector<int> clq;


	//////////////////////////////////////////////////////
	int nV = qfunc::find_clique<decltype(ug), false >(ug, clq, bb);				//false: first-to-last order scanning of bb
	//////////////////////////////////////////////////////

	EXPECT_EQ(2, nV); 							//{2, 5} is the maximal clique

	std::vector<int> clq_exp = { 2, 5 };		
	EXPECT_EQ(clq_exp, clq);

	//empty set case
	bb.erase_bit();

	//////////////////////////////////////////////////////
	nV = qfunc::find_clique(ug, clq, bb);
	//////////////////////////////////////////////////////

	EXPECT_EQ(0, nV);
	EXPECT_TRUE(clq.empty());


	//same but computing only the size of the clique
	bb.set_bit(2);
	bb.set_bit(3);
	bb.set_bit(4);
	bb.set_bit(5);
	nV = qfunc::find_clique_lb(ug, bb);

	EXPECT_EQ(2, nV); 							//{2, 5} is the maximal clique


}

TEST_F(CliqueCommonTest, is_iset) {

	typename ugraph::_bbt bb{ static_cast<int>(ug.size()), { 1, 3, 4} };

	///////////////////
	//bitset of vertices

	/////////////////////////////////////
	EXPECT_TRUE(qfunc::is_iset(ug, bb));
	/////////////////////////////////////
	
	//empty set - not an independent set
	bb.erase_bit();

	/////////////////////////////////////
	EXPECT_FALSE(qfunc::is_iset(ug, bb));
	/////////////////////////////////////

	///////////////////
	//collection of vertices (std::vector)
	vint lv = { 1, 3, 4 };

	/////////////////////////////////////
	EXPECT_TRUE(qfunc::is_iset(ug, lv));
	/////////////////////////////////////

	//empty set - not an independent set
	lv.clear();

	/////////////////////////////////////
	EXPECT_FALSE(qfunc::is_iset(ug, lv));
	/////////////////////////////////////

	///////////////////
	//collection of vertices + {v}
	lv = { 2, 5 };
	int v = 0;

	////////////////////////////////////////
	EXPECT_TRUE(qfunc::is_iset(ug, lv, v));		// {0} is non-adjacent to {2, 5}
	///////////////////////////////////////
	
}

TEST_F(CliqueCommonTest, is_clique) {

	typename ugraph::_bbt bb{ static_cast<int>(ug.size()), { 0, 4 } };

	///////////////////
	//bitset of vertices

	/////////////////////////////////////
	EXPECT_TRUE(qfunc::is_clique(ug, bb));
	/////////////////////////////////////

	//empty set - not a clique
	bb.erase_bit();

	/////////////////////////////////////
	EXPECT_FALSE(qfunc::is_clique(ug, bb));
	/////////////////////////////////////

	///////////////////
	//collection of vertices (std::vector)
	vint lv = { 0, 4 };

	/////////////////////////////////////
	EXPECT_TRUE(qfunc::is_clique(ug, lv));
	/////////////////////////////////////

	//empty set - not an independent set
	lv.clear();

	/////////////////////////////////////
	EXPECT_FALSE(qfunc::is_clique(ug, lv));
	/////////////////////////////////////

	///////////////////
	//collection of vertices + {v}
	lv = { 4 };
	int v = 0;

	////////////////////////////////////////
	EXPECT_TRUE(qfunc::is_clique(ug, lv, v));		// {0} is adjacent to {4}
	///////////////////////////////////////

	///////////////////
	//collection of vertices (C-array)
	int lv_CArrray [] = {0, 4};

	/////////////////////////////////////
	EXPECT_TRUE(qfunc::is_clique(ug, lv_CArrray, 2));
	/////////////////////////////////////

}

TEST(Clique, find_clique_max_deg) {

	const int NV = 6;

	ugraph ug(NV);
	ug.add_edge(0, 1);
	ug.add_edge(0, 3);
	ug.add_edge(0, 4);	
	ug.add_edge(2, 5);	
	ug.add_edge(3, 4);

	std::vector<int> clq;
	typename ugraph::_bbt bbsg{ static_cast<int>(ug.size()), { 0, 1, 2, 3, 4, 5} };

	/////////////////////////////////////////////////////////
	int nV = qfunc::find_clique_max_deg<decltype(ug), false>(ug, clq, bbsg);
	/////////////////////////////////////////////////////////

	EXPECT_EQ(3, nV);		//{0, 3, 4} is the maximum clique

	std::vector<int> clq_exp = { 0, 3, 4};
	EXPECT_EQ(clq_exp, clq);

}

TEST(Clique, find_clique_in_pool) {

	const int NV = 6;

	ugraph ug(NV);
	ug.add_edge(0, 1);
	ug.add_edge(0, 3);
	ug.add_edge(0, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 4);

	std::vector<int> clq;
	typename ugraph::_bbt bbsg{ static_cast<int>(ug.size()), { 0, 1, 2, 3, 4, 5} };

	/////////////////////////////////////////////////////////
	int nV = qfunc::find_clique_from_pool(ug, clq, bbsg);
	/////////////////////////////////////////////////////////

	//pool of cliques: {0, 1, 3, 4} {1} {2 5} {3 4} {4} {5} - clq = {0, 1, 3, 4}
	std::vector<int> clq_exp = { 0, 1, 3, 4 };
	EXPECT_EQ(clq_exp, clq);

}

TEST(Clique, find_max_clique_lb) {

	const int NV = 6;

	ugraph ug(NV);
	ug.add_edge(0, 1);
	ug.add_edge(0, 3);
	ug.add_edge(0, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 4);

	std::vector<int> clq;
	typename ugraph::_bbt bbsg{ static_cast<int>(ug.size()), { 0, 1, 2, 3, 4, 5} };

	/////////////////////////////////////////////////////////
	int nV = qfunc::find_clique_lb(ug);
	/////////////////////////////////////////////////////////

	EXPECT_EQ(2, nV);		//{0, 3, 4} is the maximum clique

}


TEST(Color, SEQ) {


	//TODO...

}


//TEST(qfunc, DISABLED_basic){
//			
//	const int NV=6;
//	ugraph ug(NV);
//	ug.add_edge(0, 1);
//	ug.add_edge(0, 3);
//	ug.add_edge(1, 2);
//	ug.add_edge(1, 3);
//	ug.add_edge(0, 4);
//	ug.add_edge(2, 5);
//	ug.add_edge(3, 4);
//
//	vector<int> clq;
//	clq.push_back(0); clq.push_back(1); clq.push_back(3);				//clique {0,1,3}
//
//	////////////////////////////////////////////////////////////
//	int nb_out_edges = qfunc::number_outgoing_edges(ug, clq);
//	////////////////////////////////////////////////////////////
//
//	EXPECT_EQ(3, nb_out_edges);					//(1, 2) (0, 4) (3, 4)
//		
//
//	//I/O
//	com::stl::print_collection(clq, cout, true);;
//	LOGG_INFO("outgoing edges: ", nb_out_edges);
//
//}

//TEST(qfunc, DISABLED_outgoing_edges) {
//	LOG_INFO("qfunc:outgoing_edges---------------------------");
//
//	const int NV = 6;
//	ugraph ug(NV);
//	ug.add_edge(0, 1);
//	ug.add_edge(0, 3);
//	ug.add_edge(1, 2);
//	ug.add_edge(1, 3);
//	ug.add_edge(0, 4);
//	ug.add_edge(2, 5);
//	ug.add_edge(3, 4);
//
//	cover_t <vector<int>> part;
//	vector<int> clq;
//	clq.push_back(0); clq.push_back(1); clq.push_back(3);				//clique {0,1,3}
//	part.cv.push_back(clq);
//	clq.clear();
//	clq.push_back(2); clq.push_back(5); 
//	part.cv.push_back(clq);
//	
//	////////////////////////////////////////////////////////////
//	int POINTER_MAX, POINTER_MIN;
//	int minOUT = qfunc::min_number_outgoing_edges(ug, part.cv, POINTER_MIN);
//	int maxOUT = qfunc::max_number_outgoing_edges(ug, part.cv, POINTER_MAX);
//	////////////////////////////////////////////////////////////
//		
//	EXPECT_EQ(1, minOUT);		//{2, 5}
//	EXPECT_EQ(3, maxOUT);		//{0, 1, 3}
//	
//#ifdef print_clique_func_logs
//	part.print(cout);	
//	LOG_INFO("************************");
//	LOG_INFO("min outgoing edges: " << minOUT);
//	com::stl::print_collection(part.cv[POINTER_MIN], cout, true);
//	LOG_INFO("************************");
//	LOG_INFO("max outgoing edges: " << maxOUT);
//	com::stl::print_collection(part.cv[POINTER_MAX], cout, true);
//	LOG_INFO("************************");
//#endif
//
//	LOG_INFO("END qfunc:outgoing_edges-------------------");
//#ifdef TEST_CLIQUE_FUNC_STEP_BY_STEP
//	LOG_ERROR("press any key to continue");
//	cin.get();
//#endif
//}
//
//TEST(qfunc, outgoing_edges_DIMACS) {
//	/////////////////////
//	// Test specific for 1-FullIns_3.col.dimacs (do not change)
//
//	LOG_INFO("qfunc:outgoing_edges_DIMACS---------------------------");
//	
//	string name = "1-FullIns_3.col.dimacs";
//	string fullname = TEST_CLIQUE_PATH_DATA + name;
//	cout << fullname << endl;
//	ugraph ug(fullname);
//	const int NV = ug.number_of_vertices();
//	
//	ugraph ugCOPY = ug;
//	////////////////////////////////////////////////////////////////////////////////////////////
//	int status = runCLQ::cliqueEnum(ugCOPY, 2, 2, 100, 3, 100,  all_infoCLQ_Enum, "", true);				//(1)
//	////////////////////////////////////////////////////////////////////////////////////////////
//	if (status == BaseRunCLQ::ERR) { LOG_ERROR("error runCLQ::cliqueEnum,exiting..."); }
//		
//	cover_t <vector<int>> part;
//	part.cv = all_infoCLQ_Enum.sol;	
//	
//	/////////////////////////////////////////////////////
//	for (int i = 0; i < part.cv.size(); i++) {
//		EXPECT_TRUE(qfunc::is_clique(ug, part.cv[i]));
//	}
//	/////////////////////////////////////////////////////
//	
//	int POINTER_MAX, POINTER_MIN;
//
//	/////////////////////////////////////////////////////////////
//	int minOUT = qfunc::min_number_outgoing_edges(ug, part.cv, POINTER_MIN);
//	int maxOUT = qfunc::max_number_outgoing_edges(ug, part.cv, POINTER_MAX);
//	//////////////////////////////////////////////////////////////
//
//	////////////////////////
//	EXPECT_EQ(16, minOUT);			//exactly this call (1)
//	EXPECT_EQ(27, maxOUT);			//exactly this call (1)
//	////////////////////////
//
//#ifdef print_clique_func_logs
//	part.print(cout);
//	LOG_INFO("************************");
//	LOG_INFO("min outgoing edges: " << minOUT);
//	com::stl::print_collection(part.cv[POINTER_MIN], cout, true);
//	LOG_INFO("************************");
//	LOG_INFO("max outgoing edges: " << maxOUT);
//	com::stl::print_collection(part.cv[POINTER_MAX], cout, true);
//	LOG_INFO("************************");
//#endif
//
//	LOG_INFO("END qfunc:outgoing_edges_DIMACS-------------------");
//#ifdef TEST_CLIQUE_FUNC_STEP_BY_STEP
//	LOG_ERROR("press any key to continue");
//	cin.get();
//#endif
//}



