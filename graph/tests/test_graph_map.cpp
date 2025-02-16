/*
* @file test_graph_map.cpp  
* @brief Unit tests for GraphMap class which manages a pair of vertex orderings
* @created  14/8/17
* @update 03/01/20 - adapted to the new GraphFastRootSort class
* @last_update 10/01/25
* @author pss
*
* TODO - check deprecated tests (27/01/25)
*/

#include "graph/algorithms/graph_fast_sort.h"
#include "graph/algorithms/graph_map.h"
#include "gtest/gtest.h"
#include "graph/simple_ugraph.h"
#include "utils/common.h"
#include <iostream>

using vint = std::vector<int>;
using  ugraph = Ugraph<bitarray>;

using namespace std;

class GraphMapTest : public ::testing::Test {
protected:
	void SetUp() override {
		ug.reset(NV);
		ug.add_edge(0, 1);
		ug.add_edge(0, 2);
		ug.add_edge(0, 3);
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 4;
	ugraph ug;											
};

TEST_F(GraphMapTest, build_mapping) {
		
	//degrees: {0(3), 1(2), 2(3), 3(2)}
	//l2r={1, 2, 3 ,0}, r2l={3, 0, 1, 2}

	GraphMap gm;
	gm.build_mapping< GraphFastRootSort<ugraph>> (ug, GraphFastRootSort<ugraph>::MAX, ::com::sort::FIRST_TO_LAST,
													  GraphFastRootSort<ugraph>::MIN, ::com::sort::FIRST_TO_LAST, "MAX F2L", "MIN F2L"	);
	//////////////////////////////////												  
	EXPECT_EQ	(NV, gm.size());
	EXPECT_TRUE	(gm.is_consistent());
	//////////////////////////////////
			
	vint l2rexp;
	l2rexp.push_back(1); l2rexp.push_back(2); l2rexp.push_back(3); l2rexp.push_back(0);
	vint r2lexp;
	r2lexp.push_back(3); r2lexp.push_back(0); r2lexp.push_back(1); r2lexp.push_back(2);

	//////////////////////////////////
	EXPECT_EQ	(l2rexp, gm.get_l2r());
	EXPECT_EQ	(r2lexp, gm.get_r2l());
	//////////////////////////////////
	
	//I/O
	/*gm.print_names();
	gm.print_mappings();*/

}

TEST_F(GraphMapTest, build_mapping_single_ordering){
		
	//degrees: {0(3), 1(2), 2(3), 3(2)}
	//l2r={1, 2, 3 ,0}

	GraphMap gm;
	gm.build_mapping< GraphFastRootSort<ugraph> > (ug, GraphFastRootSort<ugraph>::MIN, ::com::sort::FIRST_TO_LAST, "MIN_DEG");
	
	EXPECT_EQ(NV, gm.size());
	
	//check mappings
	vint l2rexp;
	l2rexp.push_back(1); l2rexp.push_back(2); l2rexp.push_back(3); l2rexp.push_back(0);

	EXPECT_EQ(l2rexp, gm.get_l2r());

	//I/O
	/*gm.print_names();
	gm.print_mappings();*/	

}

TEST_F(GraphMapTest, predefined_single_ordering){
	
	//predefined ordering
	GraphFastRootSort<ugraph> gol(ug);
	vint o2n = gol.new_order(GraphFastRootSort<ugraph>::MIN, ::com::sort::FIRST_TO_LAST);

	GraphMap gm;
	gm.build_mapping(o2n, "MIN F2L");			// builds mapping according to the given ordering

	EXPECT_EQ(NV, gm.size());

	//check mappings
	vint l2rexp;
	l2rexp.push_back(1); l2rexp.push_back(2); l2rexp.push_back(3); l2rexp.push_back(0);

	EXPECT_EQ(l2rexp, gm.get_l2r());
	EXPECT_STREQ("MIN F2L", gm.nameL().c_str());

	//I/O
	/*gm.print_names(); 
	gm.print_mappings();*/
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
//	bitarray bbl(ug.number_of_vertices());
//	bitarray bbr(ug.number_of_vertices());
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
