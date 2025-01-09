//test_graph_map.cpp: tests for GraphMap class for mapping between two isomorphisms
//date@ 14/8/17
//update@ 03/01/20-updated tests for new graph data structures (GraphFastRootSort)- currently only for unit weighted graphs

#include <iostream>
#include "gtest/gtest.h"
#include "utils/common.h"
#include "../algorithms/graph_map.h"

//#include "../algorithms/graph_sort.h"
#include "../algorithms/graph_fast_sort.h"

using namespace std;

TEST(Graph_map_single, build_mapping){
	
	const int SIZE=10;
	ugraph ug(SIZE);
	ug.add_edge(0,1);
	ug.add_edge(1,2);
	ug.add_edge(2,3);
	ug.add_edge(0,3);		/* cycle 0-3 */

	
	GraphMapSingle gm;
	gm.build_mapping< ugraph, GraphFastRootSort<ugraph>> (ug,   GraphFastRootSort<ugraph>::MIN_DEGEN, ::com::sort::FIRST_TO_LAST,
																GraphFastRootSort<ugraph>::NONE,	     ::com::sort::FIRST_TO_LAST, "MIN_DEG", "");
		
	gm.print_names(); cout<<endl;
	gm.print_mappings();
	
	//check values
	int vl=5;
	int vr=gm.map_l2r(vl);
	EXPECT_EQ(vl,gm.map_r2l(vr));

}

TEST(Graph_map_single, build_mapping_II){
		
	const int SIZE=10;
	ugraph ug(SIZE);
	ug.add_edge(0,1);
	ug.add_edge(1,2);
	ug.add_edge(2,3);
	ug.add_edge(0,3);		/* cycle 0-3 */

	//predefined ordering
	GraphFastRootSort<ugraph> gol(ug);
	vint o2n = gol.new_order(GraphFastRootSort<ugraph>::MIN_DEGEN, ::com::sort::LAST_TO_FIRST);

	GraphMapSingle gm;
	gm.build_mapping(o2n);		/* builds mapping*/

	//I/O
	gm.print_names(); cout<<endl;
	gm.print_mappings();
	
	//checking some values
	EXPECT_EQ(9,gm.map_l2r(4));
	EXPECT_EQ(8,gm.map_l2r(5));
	EXPECT_EQ(4,gm.map_r2l(9));
	EXPECT_EQ(5,gm.map_r2l(8));
		
	//check consistency
	int vl=5;
	int vr=gm.map_l2r(vl);
	EXPECT_EQ(vl,gm.map_r2l(vr));

}

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
