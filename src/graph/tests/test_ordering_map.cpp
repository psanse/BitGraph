/*
* @file test_ordering_map.cpp  
* @brief Unit tests for OrderingMap class which manages a pair of vertex orderings
* @date: created  14/8/17, update  GraphFastRootSort 03/01/20, imported from prior COPT (10/01/25), last update 30/11/25
* @author pss
*/

#include "gtest/gtest.h"
#include "graph/algorithms/ordering_map.h"
#include "graph/algorithms/graph_fast_sort.h"
#include "graph/simple_ugraph.h"
#include <iostream>

namespace bitgraph {
	using  ugraph =	 Ugraph<bitarray>;
	using GraphSort = GraphFastRootSort<ugraph>;
}

using namespace std;
using namespace bitgraph;

namespace {

	class OrderingMapTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			ug.reset(NV);
			ug.add_edge(0, 1);
			ug.add_edge(0, 2);
			ug.add_edge(0, 3);
			ug.add_edge(1, 2);
			ug.add_edge(2, 3);
		}
		void TearDown() override {}

		// undirected graph instance
		const int NV = 4;
		ugraph ug;
	};
}

TEST_F(OrderingMapTest, build_mapping_2_orderings) {
		
	//degrees: {0(3), 1(2), 2(3), 3(2)}
	 
	OrderingMap gm;
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

	VertexMapping l2rexp = { 2, 3, 0, 1 };
	VertexMapping r2lexp = { 2, 3, 0, 1 };
	
	//////////////////////////////////
	EXPECT_EQ	(l2rexp, gm.left_to_right());
	EXPECT_EQ	(r2lexp, gm.right_to_left());
	//////////////////////////////////

	
	//left index to right index
	EXPECT_EQ(gm.map_left_to_right(0), 2); 
	EXPECT_EQ(gm.map_left_to_right(1), 3);
	EXPECT_EQ(gm.map_left_to_right(2), 0);
	EXPECT_EQ(gm.map_left_to_right(3), 1);

	//right index to left index
	EXPECT_EQ(gm.map_right_to_left(0), 2);
	EXPECT_EQ(gm.map_right_to_left(1), 3);
	EXPECT_EQ(gm.map_right_to_left(2), 0);
	EXPECT_EQ(gm.map_right_to_left(3), 1);

	//I/O
	//gm.print_names();
	//gm.print_mappings();

}

TEST_F(OrderingMapTest, build_mapping_enum_overload) {

	OrderingMap gm;
	gm.build_mapping<GraphSort>(
		ug,
		GraphSort::strategy::max,
		GraphSort::placement::first_to_last,
		GraphSort::strategy::min,
		GraphSort::placement::first_to_last,
		"MAX F2L",
		"MIN F2L");

	VertexMapping l2rexp = { 2, 3, 0, 1 };
	VertexMapping r2lexp = { 2, 3, 0, 1 };

	EXPECT_TRUE(gm.is_consistent());
	EXPECT_EQ(l2rexp, gm.left_to_right());
	EXPECT_EQ(r2lexp, gm.right_to_left());
}

TEST_F(OrderingMapTest, build_mapping_from_known_o2n_orderings) {

	// same orderings as build_mapping_2_orderings, given explicitly in [OLD]->[NEW] format
	GraphSort gs(ug);
	VertexMapping lhs_o2n = gs.new_order(GraphSort::MAX, GraphSort::FIRST_TO_LAST, true);
	VertexMapping rhs_o2n = gs.new_order(GraphSort::MIN, GraphSort::FIRST_TO_LAST, true);

	OrderingMap gm;
	gm.build_mapping(lhs_o2n, rhs_o2n, "MAX F2L", "MIN F2L");

	OrderingMap gm_ref;
	gm_ref.build_mapping<GraphSort>(ug, GraphSort::MAX, GraphSort::FIRST_TO_LAST,
									GraphSort::MIN, GraphSort::FIRST_TO_LAST);

	EXPECT_EQ(NV, gm.size());
	EXPECT_TRUE(gm.is_consistent());
	EXPECT_EQ(gm_ref.left_to_right(), gm.left_to_right());
	EXPECT_EQ(gm_ref.right_to_left(), gm.right_to_left());
	EXPECT_STREQ("MAX F2L", gm.left_name().c_str());
	EXPECT_STREQ("MIN F2L", gm.right_name().c_str());

	// identical orderings give the identity mapping
	OrderingMap gm_id;
	gm_id.build_mapping(lhs_o2n, lhs_o2n);
	VertexMapping id = { 0, 1, 2, 3 };
	EXPECT_EQ(id, gm_id.left_to_right());
	EXPECT_EQ(id, gm_id.right_to_left());
}

TEST_F(OrderingMapTest, build_mapping_single_ordering){
		
	//degrees: {0(3), 1(2), 2(3), 3(2)}

	OrderingMap gm;
	gm.build_mapping< GraphSort > (ug, GraphSort::MIN, GraphSort::FIRST_TO_LAST, "MIN_DEG");
	
	EXPECT_EQ(NV, gm.size());

	//l2r ={ 2, 0, 3, 1 };  - [OLD / original index] to [NEW]
	//r2l ={ 1, 3, 0, 2 }  -  [NEW] to [OLD / original index]
	
	VertexMapping l2rexp = { 2, 0, 3, 1 };
	VertexMapping r2lexp = { 1, 3, 0, 2 };
	
	/////////////////////////////////
	EXPECT_EQ(l2rexp, gm.left_to_right());
	////////////////////////////////

	//original index to new index
	EXPECT_EQ(gm.map_left_to_right(0), 2);							
	EXPECT_EQ(gm.map_left_to_right(1), 0);
	EXPECT_EQ(gm.map_left_to_right(2), 3);
	EXPECT_EQ(gm.map_left_to_right(3), 1);

	//new index to original index
	EXPECT_EQ(gm.map_right_to_left(0), 1);
	EXPECT_EQ(gm.map_right_to_left(1), 3);
	EXPECT_EQ(gm.map_right_to_left(2), 0);
	EXPECT_EQ(gm.map_right_to_left(3), 2);


	EXPECT_STREQ("ORIGINAL GRAPH", gm.left_name().c_str());		//left ordering is the original graph in single ordering use
	EXPECT_STREQ("MIN_DEG", gm.right_name().c_str());			//right ordering is the new graph in single ordering use

	//I/O
	/*gm.print_names();
	gm.print_mappings();	*/
	
}

TEST_F(OrderingMapTest, predefined_single_ordering){
	
	//degrees: {0(3), 1(2), 2(3), 3(2)}

	//predefined ordering
	GraphSort gol(ug); 
	VertexOrdering n2o = gol.new_order(GraphSort::MIN, GraphSort::FIRST_TO_LAST, false);			 //n2o = {1,3,0,2}			

	OrderingMap gm;
	gm.build_mapping(n2o, "MIN F2L");			// builds mapping according to the given ordering

	EXPECT_EQ(NV, gm.size());

	//check mappings
	VertexMapping r2lexp = { 1, 3, 0, 2 };
	
	EXPECT_EQ(r2lexp, gm.right_to_left());						//original index to new index is identity
	EXPECT_STREQ("MIN F2L", gm.right_name().c_str());
	EXPECT_STREQ("ORIGINAL GRAPH", gm.left_name().c_str());


	//user code - map vertices from the original to the new ordering
	EXPECT_EQ(gm.map_left_to_right(0), 2);
	EXPECT_EQ(gm.map_left_to_right(1), 0);
	EXPECT_EQ(gm.map_left_to_right(2), 3);
	EXPECT_EQ(gm.map_left_to_right(3), 1);

	//user code - map vertices from the new ordering to the original
	EXPECT_EQ(gm.map_right_to_left(0), 1);
	EXPECT_EQ(gm.map_right_to_left(1), 3);
	EXPECT_EQ(gm.map_right_to_left(2), 0);
	EXPECT_EQ(gm.map_right_to_left(3), 2);

	//I/O
	/*gm.print_names(); 
	gm.print_mappings();*/
}

TEST_F(OrderingMapTest, mapBetweenBitsets_2orderings) {

	//degrees: {0(3), 1(2), 2(3), 3(2)}
	//l2r = {2, 3, 0 ,1}, r2l = {2, 3, 0, 1}

	OrderingMap gm;
	gm.build_mapping< GraphSort>(ug, GraphSort::MAX, GraphSort::FIRST_TO_LAST,
									 GraphSort::MIN, GraphSort::FIRST_TO_LAST, "MAX F2L", "MIN F2L");

	auto NV = ug.num_vertices();

			
	ugraph::vertex_bitset_t bbl(static_cast<int>(NV));			//left bitset
	ugraph::vertex_bitset_t bbr(static_cast<int>(NV));			//right bitset

	//set bits in left bitset
	bbl.set_bit(1);
	bbl.set_bit(3);

	////////////////////////////////////////////
	gm.map_left_to_right(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbl was erased before
	////////////////////////////////////////////

	//check right bitset
	EXPECT_TRUE(bbr.is_bit(3));
	EXPECT_TRUE(bbr.is_bit(1));
	EXPECT_EQ(2, bbr.count());

	//set bits in right bitset
	bbr.erase_bit();
	bbr.set_bit(1);
	bbr.set_bit(3);
	gm.map_right_to_left(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbr was erased before

	//check left bitset
	EXPECT_TRUE(bbl.is_bit(3));
	EXPECT_TRUE(bbl.is_bit(1));
	EXPECT_EQ(2, bbl.count());
}


TEST_F(OrderingMapTest, mapBetweenBitsets_single_ordering) {

	//degrees: {0(3), 1(2), 2(3), 3(2)}	
	//l2r ={ 2, 0, 3, 1 };  - [OLD / original index] to [NEW]
	//r2l ={ 1, 3, 0, 2 }  -  [NEW] to [OLD / original index]

	OrderingMap gm;
	gm.build_mapping< GraphSort >(ug, GraphSort::MIN, GraphSort::FIRST_TO_LAST, "MIN_DEG");

	auto NV = ug.num_vertices();

	ugraph::vertex_bitset_t bbl(static_cast<int>(NV));			//left bitset
	ugraph::vertex_bitset_t bbr(static_cast<int>(NV));			//right bitset

	//set bits in left bitset
	bbl.set_bit(1);
	bbl.set_bit(3);

	////////////////////////////////////////////
	gm.map_left_to_right(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbl was erased before
	////////////////////////////////////////////

	//check right bitset
	EXPECT_TRUE(bbr.is_bit(0));
	EXPECT_TRUE(bbr.is_bit(1));
	EXPECT_EQ(2, bbr.count());

	//set bits in right bitset
	bbr.erase_bit();
	bbr.set_bit(1);
	bbr.set_bit(3);
	gm.map_right_to_left(bbl, bbr, true /* overwrite */);		//overwrite is not necessary here since bbr was erased before

	//check left bitset
	EXPECT_TRUE(bbl.is_bit(3));
	EXPECT_TRUE(bbl.is_bit(2));
	EXPECT_EQ(2, bbl.count());
}


