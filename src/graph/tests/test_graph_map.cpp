/*
* @file test_graph_map.cpp  
* @brief Unit tests for GraphMap class which manages a pair of vertex orderings
* @date: created  14/8/17, update  GraphFastRootSort 03/01/20, imported from prior COPT (10/01/25), last update 30/11/25
* @dev pss
*
* TODO - check deprecated tests (27/01/25)
*/

#include "gtest/gtest.h"
#include "graph/algorithms/graph_map.h"
#include "graph/algorithms/graph_fast_sort.h"
#include "graph/simple_ugraph.h"
#include "utils/common.h"
#include <iostream>

using vint = std::vector<int>;


namespace bitgraph {
	using  ugraph =	 Ugraph<bitarray>;
	using GraphSort = GraphFastRootSort<ugraph>;
}

using namespace std;
using namespace bitgraph;

class GraphMapTest : public ::testing::Test {
protected:
	void SetUp() override {
		ug.reset(NV);
		ug.add_edge(0, 1);		
		ug.add_edge(0, 2);
		ug.add_edge(0, 3);
		ug.add_edge(1, 2);
		ug.add_edge(2, 3);
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 4;
	ugraph ug;											
};

TEST_F(GraphMapTest, build_mapping_2_orderings) {
		
	//degrees: {0(3), 1(2), 2(3), 3(2)}
	 
	GraphMap gm;
	gm.build_mapping< GraphSort> (ug, GraphSort::MAX, GraphSort::FIRST_TO_LAST,
									  GraphSort::MIN, GraphSort::FIRST_TO_LAST, "MAX F2L", "MIN F2L"	);
		
	//////////////////////////////////												  
	EXPECT_EQ	(NV, gm.size());
	EXPECT_TRUE	(gm.is_consistent());
	//////////////////////////////////
		
	//N2O_L: 0 2 1 3	[4]
	//O2N_L: 0 2 1 3	[4]
	//N2O_R: 1 3 0 2	[4]
	//O2N_R: 2 0 3 1	[4]
	
	//l2r={2, 3, 0 ,1}, r2l={2, 3, 0, 1}

	vint l2rexp = { 2, 3, 0, 1 };
	vint r2lexp = { 2, 3, 0, 1 };
	
	//////////////////////////////////
	EXPECT_EQ	(l2rexp, gm.get_l2r());
	EXPECT_EQ	(r2lexp, gm.get_r2l());
	//////////////////////////////////

	
	//left index to right index
	EXPECT_EQ(gm.map_l2r(0), 2); 
	EXPECT_EQ(gm.map_l2r(1), 3);
	EXPECT_EQ(gm.map_l2r(2), 0);
	EXPECT_EQ(gm.map_l2r(3), 1);

	//right index to left index
	EXPECT_EQ(gm.map_r2l(0), 2);
	EXPECT_EQ(gm.map_r2l(1), 3);
	EXPECT_EQ(gm.map_r2l(2), 0);
	EXPECT_EQ(gm.map_r2l(3), 1);

	//I/O
	//gm.print_names();
	//gm.print_mappings();

}

TEST_F(GraphMapTest, build_mapping_single_ordering){
		
	//degrees: {0(3), 1(2), 2(3), 3(2)}

	GraphMap gm;
	gm.build_mapping< GraphSort > (ug, GraphSort::MIN, GraphSort::FIRST_TO_LAST, "MIN_DEG");
	
	EXPECT_EQ(NV, gm.size());

	//l2r ={ 2, 0, 3, 1 };  - [OLD / original index] to [NEW]
	//r2l ={ 1, 3, 0, 2 }  -  [NEW] to [OLD / original index]
	
	VertexMapping l2rexp = { 2, 0, 3, 1 };
	VertexMapping r2lexp = { 1, 3, 0, 2 };
	
	/////////////////////////////////
	EXPECT_EQ(l2rexp, gm.get_l2r());
	////////////////////////////////

	//original index to new index
	EXPECT_EQ(gm.map_l2r(0), 2);							
	EXPECT_EQ(gm.map_l2r(1), 0);
	EXPECT_EQ(gm.map_l2r(2), 3);
	EXPECT_EQ(gm.map_l2r(3), 1);

	//new index to original index
	EXPECT_EQ(gm.map_r2l(0), 1);
	EXPECT_EQ(gm.map_r2l(1), 3);
	EXPECT_EQ(gm.map_r2l(2), 0);
	EXPECT_EQ(gm.map_r2l(3), 2);


	EXPECT_STREQ("ORIGINAL GRAPH", gm.nameL().c_str());		//left ordering is the original graph in single ordering use
	EXPECT_STREQ("MIN_DEG", gm.nameR().c_str());			//right ordering is the new graph in single ordering use

	//I/O
	/*gm.print_names();
	gm.print_mappings();	*/
	
}

TEST_F(GraphMapTest, predefined_single_ordering){
	
	//degrees: {0(3), 1(2), 2(3), 3(2)}

	//predefined ordering
	GraphSort gol(ug); 
	vint n2o = gol.new_order(GraphSort::MIN, GraphSort::FIRST_TO_LAST, false);			 //n2o = {1,3,0,2}			

	GraphMap gm;
	gm.build_mapping(n2o, "MIN F2L");			// builds mapping according to the given ordering

	EXPECT_EQ(NV, gm.size());

	//check mappings
	VertexMapping r2lexp = { 1, 3, 0, 2 };
	
	EXPECT_EQ(r2lexp, gm.get_r2l());						//original index to new index is identity
	EXPECT_STREQ("MIN F2L", gm.nameR().c_str());
	EXPECT_STREQ("ORIGINAL GRAPH", gm.nameL().c_str());


	//user code - map vertices from the original to the new ordering
	EXPECT_EQ(gm.map_l2r(0), 2);
	EXPECT_EQ(gm.map_l2r(1), 0);
	EXPECT_EQ(gm.map_l2r(2), 3);
	EXPECT_EQ(gm.map_l2r(3), 1);

	//user code - map vertices from the new ordering to the original
	EXPECT_EQ(gm.map_r2l(0), 1);
	EXPECT_EQ(gm.map_r2l(1), 3);
	EXPECT_EQ(gm.map_r2l(2), 0);
	EXPECT_EQ(gm.map_r2l(3), 2);

	//I/O
	/*gm.print_names(); 
	gm.print_mappings();*/
}

TEST_F(GraphMapTest, mapBetweenBitsets_2orderings) {

	//degrees: {0(3), 1(2), 2(3), 3(2)}
	//l2r = {2, 3, 0 ,1}, r2l = {2, 3, 0, 1}

	GraphMap gm;
	gm.build_mapping< GraphSort>(ug, GraphSort::MAX, GraphSort::FIRST_TO_LAST,
									 GraphSort::MIN, GraphSort::FIRST_TO_LAST, "MAX F2L", "MIN F2L");

	auto NV = ug.num_vertices();

			
	ugraph::VertexBitset bbl(static_cast<int>(NV));			//left bitset
	ugraph::VertexBitset bbr(static_cast<int>(NV));			//right bitset

	//set bits in left bitset
	bbl.set_bit(1);
	bbl.set_bit(3);

	////////////////////////////////////////////
	gm.map_l2r(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbl was erased before
	////////////////////////////////////////////

	//check right bitset
	EXPECT_TRUE(bbr.is_bit(3));
	EXPECT_TRUE(bbr.is_bit(1));
	EXPECT_EQ(2, bbr.count());

	//set bits in right bitset
	bbr.erase_bit();
	bbr.set_bit(1);
	bbr.set_bit(3);
	gm.map_r2l(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbr was erased before

	//check left bitset
	EXPECT_TRUE(bbl.is_bit(3));
	EXPECT_TRUE(bbl.is_bit(1));
	EXPECT_EQ(2, bbl.count());
}


TEST_F(GraphMapTest, mapBetweenBitsets_single_ordering) {

	//degrees: {0(3), 1(2), 2(3), 3(2)}	
	//l2r ={ 2, 0, 3, 1 };  - [OLD / original index] to [NEW]
	//r2l ={ 1, 3, 0, 2 }  -  [NEW] to [OLD / original index]

	GraphMap gm;
	gm.build_mapping< GraphSort >(ug, GraphSort::MIN, GraphSort::FIRST_TO_LAST, "MIN_DEG");

	auto NV = ug.num_vertices();

	ugraph::VertexBitset bbl(static_cast<int>(NV));			//left bitset
	ugraph::VertexBitset bbr(static_cast<int>(NV));			//right bitset

	//set bits in left bitset
	bbl.set_bit(1);
	bbl.set_bit(3);

	////////////////////////////////////////////
	gm.map_l2r(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbl was erased before
	////////////////////////////////////////////

	//check right bitset
	EXPECT_TRUE(bbr.is_bit(0));
	EXPECT_TRUE(bbr.is_bit(1));
	EXPECT_EQ(2, bbr.count());

	//set bits in right bitset
	bbr.erase_bit();
	bbr.set_bit(1);
	bbr.set_bit(3);
	gm.map_r2l(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbr was erased before

	//check left bitset
	EXPECT_TRUE(bbl.is_bit(3));
	EXPECT_TRUE(bbl.is_bit(2));
	EXPECT_EQ(2, bbl.count());
}


///////////////
//
// DEPRECATED TESTS - CHECK
//
//////////////////

////////////////
// Weighted graphs: TODO-change to new format!
///////////////
//
//TEST(Graph_map, build_mapping) {
//	LOG_INFO("Graph_map-build_mapping()---------------");
//
//	const int SIZE = 10;
//	ugraph ug(SIZE);
//	ug.add_edge(0, 1);
//	ug.add_edge(1, 2);
//	ug.add_edge(2, 3);
//	ug.add_edge(0, 3);		/* cycle 0-3 */
//
//	ug.init_wv();
//	ug.set_wv(0, 10);
//	ug.set_wv(1, 11);
//	ug.set_wv(2, 12);
//	ug.set_wv(3, 13);
//
//	GraphMap gm;
//	gm.build_mapping(ug, gbbs::sort_t::MIN_DEG_DEGEN, gbbs::place_t::PLACE_FL,
//		gbbs::sort_t::MAX_WEIGHT, gbbs::place_t::PLACE_FL, "MIN_DEG", "MAX_W");
//
//	gm.print_names(); cout << endl;
//	gm.print_mappings();
//	vector<int> sol_r2l;
//	sol_r2l.push_back(9); sol_r2l.push_back(8); sol_r2l.push_back(7); sol_r2l.push_back(6);
//	sol_r2l.push_back(0); sol_r2l.push_back(1); sol_r2l.push_back(2); sol_r2l.push_back(3);
//	sol_r2l.push_back(4); sol_r2l.push_back(5);
//
//	EXPECT_EQ(sol_r2l, gm.get_r2l());
//
//	vector<int> sol_l2r;
//	sol_l2r.push_back(4); sol_l2r.push_back(5); sol_l2r.push_back(6); sol_l2r.push_back(7);
//	sol_l2r.push_back(8); sol_l2r.push_back(9); sol_l2r.push_back(3); sol_l2r.push_back(2);
//	sol_l2r.push_back(1); sol_l2r.push_back(0);
//
//	EXPECT_EQ(sol_l2r, gm.get_l2r());
//
//	//check values
//	int vl = 5;
//	int vr = gm.map_l2r(vl);
//	EXPECT_EQ(vl, gm.map_r2l(vr));
//	LOG_INFO("-----------------------------");
//}
//
//TEST(Graph_map, mapping_functions) {
//	LOG_INFO("Graph_map-mapping_functions()---------------");
//
//	const int SIZE = 10;
//	ugraph ug(SIZE);
//	ug.add_edge(0, 1);
//	ug.add_edge(1, 2);
//	ug.add_edge(2, 3);
//	ug.add_edge(0, 3);		/* cycle 0-3 */
//
//	ug.init_wv();
//	ug.set_wv(0, 10);
//	ug.set_wv(1, 11);
//	ug.set_wv(2, 12);
//	ug.set_wv(3, 13);
//
//	GraphMap gm;
//	gm.build_mapping(ug, gbbs::sort_t::MIN_DEG_DEGEN, gbbs::place_t::PLACE_FL,
//		gbbs::sort_t::MAX_WEIGHT, gbbs::place_t::PLACE_FL, "MIN_DEG", "MAX_W");
//
//
//	bitarray bbl(ug.num_vertices());
//	bitarray bbr(ug.num_vertices());
//	bbl.set_bit(0); bbl.set_bit(9);
//
//	//l2r-bitstring
//	gm.map_l2r(bbl, bbr);
//	EXPECT_TRUE(bbr.is_bit(4));
//	EXPECT_TRUE(bbr.is_bit(0));
//	EXPECT_EQ(2, bbr.popcn64());
//
//	//r2l-bitstring
//	bbr.erase_bit();
//	bbr.set_bit(0); bbr.set_bit(9);
//	gm.map_r2l(bbl, bbr);
//	EXPECT_TRUE(bbl.is_bit(9));
//	EXPECT_TRUE(bbl.is_bit(5));
//	EXPECT_EQ(2, bbl.popcn64());
//
//	//l2r-bba_t
//	bba_t<bitarray> bbal;
//	bba_t<bitarray> bbar;
//	bbal.init(2, 10); bbar.init(2, 10);
//	bbal.set_bit(0, 0); bbal.set_bit(0, 9);
//	bbal.set_bit(1, 0); bbal.set_bit(1, 9);
//	gm.map_l2r(bbal, bbar, 0, 1);
//	for (int pos = 0; pos < 2; pos++) {
//		EXPECT_TRUE(bbar.pbb[pos].is_bit(0));
//		EXPECT_TRUE(bbar.pbb[pos].is_bit(4));
//		EXPECT_EQ(2, bbar.pbb[pos].popcn64());
//	}
//
//	//r2l-bba_t
//	bbar.erase_bit();
//	bbar.set_bit(0, 0); bbar.set_bit(0, 9);
//	bbar.set_bit(1, 0); bbar.set_bit(1, 9);
//	gm.map_r2l(bbal, bbar, 0, 1);
//	for (int pos = 0; pos < 2; pos++) {
//		EXPECT_TRUE(bbal.pbb[pos].is_bit(9));
//		EXPECT_TRUE(bbal.pbb[pos].is_bit(5));
//		EXPECT_EQ(2, bbal.pbb[pos].popcn64());
//	}
//}
