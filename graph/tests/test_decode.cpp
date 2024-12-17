
/*
* @brief Tests for class Decode, which unwinds a composition of orderings
* @date 17/12/2024
* @comments based on previous tests in test_graph_sort.cpp
*/

#include <iostream>
#include "utils/common.h"
#include "utils/result.h"
#include "gtest/gtest.h"
#include "../graph.h"
#include "../graph_gen.h"
#include "../algorithms/graph_sort.h"

using namespace std;

TEST(Graph_sort, Decode) {
	
	ugraph ug(106);
	ug.add_edge(1, 2);
	ug.add_edge(1, 3);
	ug.add_edge(1, 4);
	ug.add_edge(78, 5);	
		
	//computes MIN_DEG_DEGEN ordering in format [OLD_INDEX]=NEW_INDEX
	GraphSort<ugraph> o(ug);
	vint vres = o.new_order_fast(gbbs::MIN_DEG_DEGEN, gbbs::PLACE_LF);
			
	Decode::reverse_in_place(vres);		//format[NEW_INDEX] = OLD_INDEX
	Decode d;
	d.insert_ordering(vres);

	//decodes the position of a list of vertices in the original ordering
	vint vlist;
	vlist.push_back(105);
	vlist.push_back(104);
	vlist.push_back(103);
	vlist.push_back(102);
	vlist.push_back(101);
	vlist.push_back(100);

	vint dec = d.decode_list(vlist);
	EXPECT_EQ(dec[0], 0);
	EXPECT_EQ(dec[1], 6);
	EXPECT_EQ(dec[2], 7);
	EXPECT_EQ(dec[3], 8);
	EXPECT_EQ(dec[4], 9);
	EXPECT_EQ(dec[5], 10);	
}

/*
*	FIXME - does not compile
*/
//TEST(Graph_composite_sort,basic) {
//	////////////
//	// decoding a sequence of orderings
//
//	cout << "Graph_composite_sort: basic------------------------" << endl;
//	ugraph ug(6);
//	ug.add_edge(1, 2);
//	ug.add_edge(1, 3);
//	ug.add_edge(1, 4);
//	ug.add_edge(4, 5);
//	ug.print_data();
//
//
//	GraphSort<ugraph> o(ug);
//	vector<pair<sort_t, place_t> > lo;
//	lo.push_back(pair<sort_t, place_t>(NONE, PLACE_LF));
//	lo.push_back(pair<sort_t, place_t>(NONE, PLACE_LF));
//
//	Decode d;
//
//	//////////////////////////////////////////
//	o.reorder_composite(lo, d, NULL);				//TODO - DOES NOT COMPILE
//	///////////////////////////////////////////
//
//	vector<int> vindex(6);
//	for (int i = 0; i < vindex.size(); i++) {
//		vindex[i] = i;
//	}
//	vector<int> vdec = d.decode_list(vindex);		//same ordering expected since the graph remains the original one
//	EXPECT_EQ(vindex, vdec);
//
//
//	cout << "--------------------------------------" << endl;
//}
