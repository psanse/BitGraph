/**
* @file test_bitset_sparse.cpp
* @brief Unit tests for sparse classes
* @details created  ?, last_update 19/02/2025
* @author pss
* 
* TODO - complete BitSet class interface (25/02/2025)
* TODO - check disabled tests (i.e., clear_bit)
**/

#include "bitscan/bbscan_sparse.h"
#include "bitscan/bbset.h"
#include "gtest/gtest.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <type_traits>

using namespace std;
using namespace bitgraph;

class BitSetSpClassTest : public ::testing::Test {
protected:
	BitSetSpClassTest() :bbsp(301) {}
	virtual void SetUp() {
		for (int i = 0; i <= 300; i += 50) {
			bbsp.set_bit(i);
			pSet.push_back(i);
		}
	}

	//////////////////////
	//data members
	BitsetSp bbsp;	
	vector<int> pSet;
};

TEST(Sparse, construction_basic){

	BitsetSp bbsp(1000);
	
	bbsp.set_bit(500);
	bbsp.set_bit(700);
	bbsp.set_bit(900);

	EXPECT_EQ(3, bbsp.count());						//bbsp = {500, 700, 900}
	EXPECT_TRUE(bbsp.is_bit(500));
	EXPECT_TRUE(bbsp.is_bit(700));
	EXPECT_TRUE(bbsp.is_bit(900));

	bbsp.init(100000);
	bbsp.set_bit(10);
	EXPECT_TRUE(bbsp.is_bit(10));
}

TEST(Sparse, construction_from_blocks) {

	auto bbsp = BitsetSp::from_num_blocks(16);

	bbsp.set_bit(500);
	bbsp.set_bit(700);
	bbsp.set_bit(900);

	EXPECT_EQ(3, bbsp.count());						//bbsp = {500, 700, 900}
	EXPECT_TRUE(bbsp.is_bit(500));
	EXPECT_TRUE(bbsp.is_bit(700));
	EXPECT_TRUE(bbsp.is_bit(900));
}

TEST(Sparse, construction_from_vector) {

	vector<int> lv = { 100, 200, 300, 400, 500 };
	BitsetSp bbsp(1000, lv);

	EXPECT_TRUE(bbsp.is_bit(100));
	EXPECT_TRUE(bbsp.is_bit(200));
	EXPECT_TRUE(bbsp.is_bit(300));
	EXPECT_TRUE(bbsp.is_bit(400));
	EXPECT_TRUE(bbsp.is_bit(500));
	EXPECT_EQ(5, bbsp.count());
	EXPECT_EQ(5, bbsp.size());			//sparse number of blocks, one per element in this case 
}

TEST(Sparse, construction_initalizer_list) {
	BitsetSp bbsp{ 1000, {1, 7, 10} };

	EXPECT_TRUE(bbsp.is_bit(1));
	EXPECT_TRUE(bbsp.is_bit(7));
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_EQ(3, bbsp.count());
}

TEST_F(BitSetSpClassTest, copy_constructor) {

	BitsetSp bbspCOPY(bbsp);

	EXPECT_TRUE(bbspCOPY.is_bit(0));
	EXPECT_TRUE(bbspCOPY.is_bit(50));
	EXPECT_TRUE(bbspCOPY.is_bit(100));
	EXPECT_TRUE(bbspCOPY.is_bit(150));
	EXPECT_TRUE(bbspCOPY.is_bit(200));
	EXPECT_TRUE(bbspCOPY.is_bit(250));
	EXPECT_TRUE(bbspCOPY.is_bit(300));
	EXPECT_EQ(7, bbspCOPY.count());
	EXPECT_EQ(5, bbspCOPY.num_blocks());
	EXPECT_EQ(5, bbspCOPY.size());			//maximum density!
}

TEST(Sparse_intrinsic, assignment) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	//assignment
	BBScanSp bbspASSIGN(34);
	bbspASSIGN.set_bit(22);
	bbspASSIGN.set_bit(23);

	EXPECT_TRUE(bbspASSIGN.is_bit(22));
	EXPECT_TRUE(bbspASSIGN.is_bit(23));
	EXPECT_EQ(1, bbspASSIGN.size());

	/////////////////////
	bbsp = bbspASSIGN;
	/////////////////////

	EXPECT_EQ(bbsp.count(), bbspASSIGN.count());
	EXPECT_EQ(bbsp.num_blocks(), bbspASSIGN.num_blocks());
	EXPECT_EQ(bbsp.size(), bbspASSIGN.size());
}

TEST_F(BitSetSpClassTest, reset) {

	//size
	bbsp.reset(10);
	EXPECT_EQ(0, bbsp.count());							//number of 1-bis
	EXPECT_EQ(0, bbsp.size());				//number of non_zero bitblocks

	//size and bits
	bitpos_list lv = { 10, 20, 100 };
	bbsp.reset(200, lv);
	EXPECT_EQ(3, bbsp.count());							//number of 1-bis
	EXPECT_EQ(2, bbsp.size());				//number of non_zero bitblocks
	EXPECT_TRUE(bbsp.is_bit(10));						//number of 1-bis		
	EXPECT_TRUE(bbsp.is_bit(20));						//number of 1-bis		
	EXPECT_TRUE(bbsp.is_bit(100));						//number of 1-bis		
}

TEST(Sparse, find_block)
{
	BitsetSp bbsp(1000);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(998);
		
	BITBOARD bb = bbsp.find_block(WDIV(1000));		//bb = 998
	EXPECT_TRUE(bblock::is_bit(bb, 38));			//the bitset starts at 960, so 998 has offset 38.

	//variant returning also the position in the collection (not its real index)
	auto pos = BitsetSp::npos;
	auto it = bbsp.find_block(WDIV(1000), pos);		//bb = 998
	EXPECT_EQ(1, pos);								//second block in the sparse bitset

	//empty bitset
	it = bbsp.find_block(WDIV(650), pos);		
	EXPECT_EQ(BBObject::noBit, pos);
}

TEST(Sparse, find_block_2) {

	BitsetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_FALSE(bbsp.is_bit(63));
	EXPECT_EQ(3,bbsp.count());

	bbsp.erase_bit(10);
	EXPECT_FALSE(bbsp.is_bit(10));
	EXPECT_EQ(2,bbsp.count());

	//find functions for blocks (not that the actual bit in WDIV(nBit) is not important, it is the bitblock index that has to exist)
	EXPECT_EQ	(0, bbsp.find_block_pos(WDIV(10)).second);
	EXPECT_TRUE	(bbsp.find_block_pos(WDIV(10)).first);
	EXPECT_EQ	(1, bbsp.find_block_pos(WDIV(64)).second);
	EXPECT_TRUE	(bbsp.find_block_pos(WDIV(64)).second);
	
	//test for a block which does not exist
	EXPECT_FALSE(bbsp.find_block_pos(WDIV(129)).first);
	EXPECT_EQ(EMPTY_ELEM, bbsp.find_block_pos(WDIV(129)).second);
		
}

TEST(Sparse, member_masks) {

	BitsetSp lhs;
	lhs.init(200);
	lhs.set_bit(63);
	lhs.set_bit(126);
	lhs.set_bit(189);	
	
	BitsetSp rhs;
	rhs.init(200);
	rhs.set_bit(63);
	rhs.set_bit(127);
	
	lhs.AND_block(1,rhs);
	EXPECT_EQ(1, lhs.count());
	EXPECT_TRUE(lhs.is_bit(63));

	lhs.set_block(1, -1 , rhs);
	EXPECT_TRUE(lhs.is_bit(127));
	EXPECT_EQ(2, lhs.count());
} 



TEST(Sparse, population_count){

	BitsetSp bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_EQ(3, bb.count());
	EXPECT_EQ(2, bb.count(11, -1));
	EXPECT_EQ(1, bb.count(21, -1));
	EXPECT_EQ(0, bb.count(65, -1));
	EXPECT_EQ(1, bb.count(64, -1));
}

TEST(Sparse, set_bits) {
	
	BitsetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	EXPECT_TRUE(bbsp.is_bit(126));

	//range
	bbsp.set_bit(3, 27);
	EXPECT_TRUE(bbsp.is_bit(3));
	EXPECT_TRUE(bbsp.is_bit(27));
	EXPECT_EQ(32, bbsp.count());
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));
	

	bbsp.set_bit(1002,1002);
	EXPECT_TRUE(bbsp.is_bit(1002));
	EXPECT_EQ(33, bbsp.count());
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));


	//range
	bbsp.set_bit(29, 125);
	EXPECT_FALSE(bbsp.is_bit(28));
	EXPECT_TRUE(bbsp.is_bit(29));
	EXPECT_TRUE(bbsp.is_bit(125));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));

	bbsp.set_bit(1001,1100);
	EXPECT_FALSE(bbsp.is_bit(999));
	EXPECT_TRUE(bbsp.is_bit(1001));
	EXPECT_TRUE(bbsp.is_bit(1100));
	EXPECT_FALSE(bbsp.is_bit(1101));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));

}

TEST(Sparse, set_bits_from_bitset) {

	BitsetSp bbsp(10000);
	BitsetSp bbsp1(10000);			//same population	

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	bbsp1.set_bit(9150);
	bbsp1.set_bit(3);

	////////////////////////
	bbsp.set_bit(bbsp1);
	////////////////////////

	EXPECT_TRUE(bbsp.is_bit(9150));	
	EXPECT_TRUE(bbsp.is_bit(3));
	EXPECT_EQ(9, bbsp.count());
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));
}


TEST(Sparse, erase_block_range) {

	BitsetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	BitsetSp bbsp1(10000);
		
	bbsp1.set_bit(2);
	bbsp1.set_bit(126);
	bbsp1.set_bit(127);
	bbsp1.set_bit(1000);
		
	//range
	bbsp.erase_block(0, WDIV(7000), bbsp1);
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_TRUE(bbsp.is_bit(1));
	EXPECT_TRUE(bbsp.is_bit(9000));

}

TEST(Sparse, set_blocks) {
	
	BitsetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	BitsetSp bbsp1(10000);

	bbsp1.set_bit(5);
	bbsp1.set_bit(8);
	bbsp1.set_bit(129);
	bbsp1.set_bit(5000);

	//range
	bbsp.set_block(2,2, bbsp1);
	EXPECT_TRUE(bbsp.is_bit(129));
	EXPECT_FALSE(bbsp.is_bit(5000));


	//range
	bbsp.set_block(2,INDEX_1TO0(5000), bbsp1);
	EXPECT_TRUE(bbsp.is_bit(5000));

	bbsp.erase_block(0, -1,  bbsp1);
	EXPECT_EQ(7, bbsp.count());

	//range
	bbsp.set_block(2, WDIV(10000) - 1, bbsp1);
	EXPECT_TRUE(bbsp.is_bit(129));
	EXPECT_TRUE(bbsp.is_bit(5000));
}

TEST(Sparse, erase_bits) {
	
	BitsetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);


	bbsp.erase_bit(126);
	EXPECT_FALSE(bbsp.is_bit(126));

	//range
	bbsp.erase_bit(2, 1000);
	EXPECT_FALSE(bbsp.is_bit(2));
	EXPECT_FALSE(bbsp.is_bit(1000));
	EXPECT_EQ(3, bbsp.count());
	EXPECT_EQ(4, bbsp.size());		//all bitblocks are still there (with 0 value)
	
	//range
	bbsp.erase_bit(1,1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.count());
	
	//range trying to erase blocks in higher positions than any existing
	bbsp.reset(10000);
	bbsp.set_bit(55, 58);		
	bbsp.erase_bit(77, 100);		//no bits in index>=2 so nothing is done
	EXPECT_FALSE(bbsp.is_empty());
	
	//last bit in range does not correspond to a block but there are blocks with higher index
	bbsp.reset(10000);
	bbsp.set_bit(1, 5);	
	bbsp.set_bit(200,205);
	bbsp.erase_bit(67, 69);	
	EXPECT_EQ(11, bbsp.count());
	EXPECT_TRUE(bbsp.is_bit(200));

	//first bit in range does not correspond to a block
	bbsp.reset(10000);
	bbsp.set_bit(70, 75);	
	bbsp.erase_bit(63, 70);	
	EXPECT_EQ(5,bbsp.count());
	EXPECT_FALSE(bbsp.is_bit(70));
	EXPECT_TRUE(bbsp.is_bit(71));
}



TEST(Sparse, member_OR_operator) {

	BitsetSp bbsp(130);
	bbsp.set_bit(64);
	bbsp.set_bit(120);
	bbsp.shrink_to_fit();
	

	BitsetSp bbsp1(130);
	bbsp1.set_bit(5);
	bbsp1.set_bit(54);
	
	
	//OR
	//////////////////
	bbsp |= bbsp1; 	
	/////////////////
		
	EXPECT_EQ(4, bbsp.count());
	EXPECT_TRUE(bbsp.is_bit(5));
	EXPECT_TRUE(bbsp.is_bit(54));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(bbsp.is_bit(120));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));
	
}

TEST(Sparse, member_XOR_operator) {

	BitsetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(30);
	bbsp.set_bit(64);
	bbsp.set_bit(128);

	BitsetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);

	//XOR- set difference
	//////////////////
	bbsp ^= bbsp1;
	/////////////////

	EXPECT_EQ(3, bbsp.count());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(54));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));

}

TEST(Sparse, OR_block) {

	BitsetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	BitsetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);

	//OR
	//////////////////
	bbsp.set_block(1, -1, bbsp1);
	/////////////////
	
	EXPECT_EQ(4, bbsp.count());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(bbsp.is_bit(128));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));

}

TEST(Sparse, AND_block) {

	BitsetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(115);
	bbsp.set_bit(128);

	BitsetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(115);
		
	//////////////////
	bbsp.AND_block(1, bbsp1);		//AND masks non-zero bitblocks with index 2 and 3
	/////////////////

	EXPECT_EQ(3, bbsp.count());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(115));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));

}

TEST(Sparse, friend_AND_block_range) {

	BitsetSp bbsp(10000);
	BitsetSp bbspAND(10000);
	BitsetSp res(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	bbspAND.set_bit(0);
	bbspAND.set_bit(1);
	bbspAND.set_bit(2);
	bbspAND.set_bit(126);
	bbspAND.set_bit(127);
	bbspAND.set_bit(10001);
	bbspAND.set_bit(9000);

	/////////////////////////////////////////////////////
	AND_block(2, WDIV(10000) - 1, bbsp, bbspAND, res);
	/////////////////////////////////////////////////////

	EXPECT_EQ(1, res.count());
	EXPECT_TRUE(res.is_bit(9000));

}

TEST(Sparse, member_AND_operator) {

	BitsetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	bbsp.set_bit(129);

	BitsetSp bbsp1(130);
	bbsp1.set_bit(10);
	bbsp1.set_bit(64);
	bbsp1.set_bit(128);

	//AND
	//////////////////
	bbsp &= bbsp1;
	/////////////////

	EXPECT_EQ(2, bbsp.count());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitsetSp::pBlock_less()));

}

TEST(Sparse, operators){

	BitsetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	BitsetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);
	
	//OR
	BitsetSp resOR(130);
	OR(bbsp1, bbsp, resOR);
	EXPECT_EQ(6, resOR.count());

	//=
	bbsp=resOR;
	EXPECT_EQ(6, bbsp.count());
	EXPECT_TRUE(bbsp==resOR);

	//AND
	BitsetSp bbsp2(130);
	bbsp2.set_bit(54);
	BitsetSp resAND(130);
	AND(bbsp1,bbsp2,resAND);
	EXPECT_TRUE(resAND.is_bit(54));
	EXPECT_EQ(1, resAND.count());
}

TEST(Sparse, insertion) {

	BitsetSp bbsp(130);					
	bbsp.set_bit(64);
	bbsp.set_bit(65);
	bbsp.set_bit(12);					//obligatory

	EXPECT_TRUE(bbsp.is_bit(12));
}

TEST(Sparse, integration) {

	BitsetSp bbs(10000);
	bbs.set_bit(10);
	bbs.set_bit(1000);
	bbs.set_bit(70);
	bbs.set_bit(150);
	
	BitsetSp bbs1(10000);
	bbs1.set_bit(1000);

	BitsetSp bbs2(10000);
	AND(bbs1, bbs, bbs2);
	EXPECT_EQ(1, bbs2.count());
	EXPECT_TRUE(bbs2.is_bit(1000));
	EXPECT_TRUE(bbs2==bbs1);

	BitsetSp bbs3(10000);
	OR(bbs1,bbs, bbs3);
	EXPECT_EQ(4, bbs3.count());
	EXPECT_TRUE(bbs3.is_bit(1000));
	EXPECT_TRUE(bbs3.is_bit(10));
	EXPECT_TRUE(bbs3.is_bit(70));
	EXPECT_TRUE(bbs3.is_bit(150));

	bbs3.erase_bit(1000);
	EXPECT_EQ(3, bbs3.count());			//even though one bit block is empty
	EXPECT_EQ(4, bbs3.size());		
}

TEST(Sparse, copy_up_to_some_bit) {

	BBScanSp bbsp(10000);
	BBScanSp bbcopy(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	bbsp.set_bit(9999);
	
	bbcopy.reset_bit(0, 126, bbsp);
	EXPECT_EQ(4, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(126));

	bbcopy.reset_bit(0, 1000, bbsp);
	EXPECT_EQ(6, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.reset_bit(0, 9000, bbsp);
	EXPECT_EQ(7, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(9000));

	bbcopy.reset_bit(0, 9999, bbsp);
	EXPECT_TRUE(bbcopy==bbsp);
}

TEST(Sparse, copy_in_closed_range){

	BBScanSp bbsp(10000);
	BBScanSp bbcopy(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	bbsp.set_bit(9999);

	bbcopy.reset_bit(126, 1000, bbsp);
	bbcopy.print();
	EXPECT_EQ(3, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(126));
	EXPECT_TRUE(bbcopy.is_bit(127));
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.reset(10000);
	bbcopy.reset_bit(127, 129, bbsp);
	EXPECT_EQ(1, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(127));

	bbcopy.reset(10000);
	bbcopy.reset_bit(0, 10000, bbsp);
	EXPECT_TRUE( bbcopy == bbsp);

	bbcopy.reset(10000);
	bbcopy.reset_bit(9000, 9999, bbsp);
	EXPECT_EQ(2, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(9000));
	EXPECT_TRUE(bbcopy.is_bit(9999));

	bbcopy.reset(10000);
	bbcopy.reset_bit(0, 0, bbsp);
	EXPECT_EQ(1, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(0));

	bbcopy.reset(10000);
	bbcopy.reset_bit(9999, 9999, bbsp);
	EXPECT_EQ(1, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(9999));
}

TEST(Sparse, copy_in_closed_range_special_cases){

	BBScanSp bbsp(120);
	BBScanSp bbcopy(120);
	bbsp.set_bit(64);

	bbcopy.reset_bit(0, 51, bbsp);
	bbcopy.print();
	EXPECT_EQ(0, bbcopy.count());

	bbcopy.reset(120);
	bbcopy.reset_bit(64, 65, bbsp);
	EXPECT_EQ(1, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(64));

	bbcopy.reset(120);
	bbcopy.reset_bit(63, 64, bbsp);
	EXPECT_EQ(1, bbcopy.count());
	EXPECT_TRUE(bbcopy.is_bit(64));
}

TEST(Sparse, operator_and_eq) {

	BBScanSp bbsp(10000);
	BBScanSp bbkeep(10000);
	BBScanSp bbkeep2(10000);
	BBScanSp bbkeep3(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	
	bbkeep.set_bit(0);
	bbkeep.set_bit(1);
	bbkeep.set_bit(2);
	bbkeep.set_bit(126);
	bbkeep.set_bit(127);
	bbkeep.set_bit(1000);
	bbkeep.set_bit(9000);

	bbkeep2.set_bit(0);
	bbkeep2.set_bit(1);
	bbkeep2.set_bit(126);
	bbkeep2.set_bit(1000);

	bbkeep3.set_bit(0);

	/////////////////
	bbsp &= bbkeep;
	//////////////////
	EXPECT_TRUE(bbsp == bbkeep);

	bbsp.AND_block(1, bbkeep2);
	bbsp.print();
	EXPECT_TRUE(bbsp.is_bit(126));
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_FALSE(bbsp.is_bit(127));		//last bit of block 1 deleted
	EXPECT_EQ(5, bbsp.count());

	//////////////////
	bbsp &= bbkeep3;
	///////////////////
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_EQ(1, bbsp.count());

}

TEST(Sparse, DISABLED_clear_bits) {

	BitsetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	//clear bits removing bitblocks in range
	bbsp.clear_bit(2, 1000);
	EXPECT_FALSE(bbsp.is_bit(2));
	EXPECT_FALSE(bbsp.is_bit(1000));
	EXPECT_EQ(3, bbsp.count());
	EXPECT_EQ(3, bbsp.size());		//first and 9000 bitblocks are there + 1000 bitblock set to 0

	bbsp.clear_bit(2, 1064);
	EXPECT_EQ(2, bbsp.size());		//removes 1000 bitblock

	//range
	bbsp.clear_bit(1, 1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.count());
	EXPECT_EQ(2, bbsp.size());


	//end slices
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	bbsp.clear_bit(1001, EMPTY_ELEM);
	EXPECT_TRUE(bbsp.is_bit(1000));
	EXPECT_EQ(3, bbsp.size());

	bbsp.clear_bit(EMPTY_ELEM, 1);
	EXPECT_TRUE(bbsp.is_bit(2));
	EXPECT_EQ(3, bbsp.size());

	bbsp.clear_bit(EMPTY_ELEM, EMPTY_ELEM);
	EXPECT_TRUE(bbsp.is_empty());
}

////////////////////
//
// CHECK TESTS
//
////////////////////

//TEST(Sparse, next_bit_del_pos) {
//
//	BBScanSp bbsp(10000);
//
//	bbsp.set_bit(0);
//	bbsp.set_bit(1);
//	bbsp.set_bit(2);
//	bbsp.set_bit(126);
//	bbsp.set_bit(127);
//	bbsp.set_bit(1000);
//	bbsp.set_bit(9000);
//
//	//bitscannning loop
//	bbsp.init_scan(BBObject::DESTRUCTIVE);
//	int posBB=EMPTY_ELEM;
//	while(true){
//		int	nBit=bbsp.next_bit_del_pos(posBB);
//		if(nBit==EMPTY_ELEM) 
//				break;
//		cout<<"nBit: "<<nBit<<" pos: "<<posBB<<endl;
//		
//	}
//	
//}


//TEST(Sparse, erase_block_pos) {
//
//	BBScanSp bbsp(10000);
//	BBScanSp bberase(10000);
//	bbsp.set_bit(0);
//	bbsp.set_bit(1);
//	bbsp.set_bit(2);
//	bbsp.set_bit(126);
//	bbsp.set_bit(127);
//	bbsp.set_bit(1000);
//	bbsp.set_bit(9000);
//	
//	bberase.set_bit(0);
//	bberase.set_bit(1);
//	bberase.set_bit(2);
//	bberase.set_bit(126);
//	bberase.set_bit(127);
//	bberase.set_bit(1000);
//	bberase.set_bit(9000);
//
//	bbsp.erase_block_pos(1,bberase);
//	EXPECT_FALSE(bbsp.is_bit(126));
//
//	bbsp.erase_block_pos(2,bberase);
//	EXPECT_FALSE(bbsp.is_bit(1000));
//
//
//	bbsp.erase_block_pos(3,bberase);
//	EXPECT_FALSE(bbsp.is_bit(9000));
//}


//////////////////////////////////////////////
// Bitset-compatible API of BitsetSp (see docs/bitsetsp_api_parity.md)

TEST(BitsetSp_api, aliases) {
	BitsetSp bb(200);
	bb.set(10).set(70).set(130);
	EXPECT_TRUE(bb.test(10));
	EXPECT_TRUE(bb.test(70));
	EXPECT_FALSE(bb.test(11));
	EXPECT_EQ(3, bb.count());

	bb.clear(70);
	EXPECT_FALSE(bb.test(70));
	EXPECT_EQ(2, bb.count());

	EXPECT_EQ(200 / 64 + 1, static_cast<int>(bb.num_blocks()));
	EXPECT_EQ(bb.num_blocks() * 64, bb.bit_capacity());

	bb.clear_all();
	EXPECT_TRUE(bb.is_empty());
	EXPECT_EQ(0, bb.count());
	EXPECT_EQ(4u, bb.num_blocks());			//population size preserved

	bb.set(5);
	bb.erase_all_bits();
	EXPECT_TRUE(bb.is_empty());
}

TEST(BitsetSp_api, masking_aliases) {
	BitsetSp a(200, { 1, 70, 130, 199 });
	BitsetSp b(200, { 70, 130, 150 });

	BitsetSp x(a);
	x.and_eq(b);
	EXPECT_EQ((bitpos_list{ 70, 130 }), static_cast<bitpos_list>(x));

	BitsetSp y(a);
	y.or_eq(b);
	EXPECT_EQ((bitpos_list{ 1, 70, 130, 150, 199 }), static_cast<bitpos_list>(y));

	BitsetSp z(a);
	z.andnot_eq(b);
	EXPECT_EQ((bitpos_list{ 1, 199 }), static_cast<bitpos_list>(z));
}

TEST(BitsetSp_api, is_singleton) {
	BitsetSp bb(200);
	EXPECT_EQ(0, bb.is_singleton());			//empty

	bb.set(100);
	EXPECT_EQ(1, bb.is_singleton());

	bb.set(101);
	EXPECT_EQ(-1, bb.is_singleton());			//same block

	bb.erase_bit(101);
	bb.set(5);
	EXPECT_EQ(-1, bb.is_singleton());			//different blocks

	//zero blocks must be skipped
	bb.erase_bit(5);
	EXPECT_EQ(1, bb.is_singleton());
	bb.erase_bit(100);
	EXPECT_EQ(0, bb.is_singleton());
	EXPECT_FALSE(bb.bitset().empty());			//blocks remain, all zero
}

TEST(BitsetSp_api, find_first_common) {
	BitsetSp a(300, { 1, 70, 130, 250 });
	BitsetSp b(300, { 2, 130, 250 });
	BitsetSp c(300, { 3, 71 });

	EXPECT_EQ(130, a.find_first_common(b));
	EXPECT_EQ(130, b.find_first_common(a));
	EXPECT_EQ(BBObject::noBit, a.find_first_common(c));
	EXPECT_EQ(BBObject::noBit, a.find_first_common(BitsetSp(300)));

	BitsetSp d(300, { 70 });
	EXPECT_EQ(70, a.find_first_common(d));
	EXPECT_EQ(a.is_disjoint(c), a.find_first_common(c) == BBObject::noBit);
}

TEST(BitsetSp_api, assign_bit) {
	BitsetSp src(200, { 3, 70, 190 });
	BitsetSp dst(200, { 5, 6, 100 });

	dst.assign_bit(src);
	EXPECT_TRUE(dst == src);
	EXPECT_EQ(4u, dst.num_blocks());

	dst.assign_bit(dst);						//self-assignment
	EXPECT_TRUE(dst == src);

	BitsetSp small(100);						//2 blocks: bit 190 is out of range
	small.assign_bit(src);
	EXPECT_EQ((bitpos_list{ 3, 70 }), static_cast<bitpos_list>(small));
}

TEST(BitsetSp_api, set_bit_list_and_constructor_from_list) {
	const bitpos_list lv = { 190, 3, 70, 64 };		//unsorted on purpose

	BitsetSp bb(200);
	bb.set_bit(lv);
	EXPECT_EQ((bitpos_list{ 3, 64, 70, 190 }), static_cast<bitpos_list>(bb));

	BitsetSp bc(lv);							//population size from the maximum bit
	EXPECT_EQ(static_cast<bitpos_list>(bb), static_cast<bitpos_list>(bc));	//== also compares the population size
	EXPECT_EQ(190 / 64 + 1, static_cast<int>(bc.num_blocks()));

	BitsetSp be((bitpos_list()));				//empty list
	EXPECT_TRUE(be.is_empty());
	EXPECT_EQ(0u, be.num_blocks());

	BitsetSp bi;
	bi.init(200, lv);
	EXPECT_TRUE(bi == bb);
}

TEST(BitsetSp_api, make_bitset_sp) {
	auto a = make_bitset_sp(200);
	EXPECT_TRUE(a.is_empty());
	auto b = make_bitset_sp(200, { 1, 2, 100 });
	EXPECT_EQ(3, b.count());
	bitpos_list lv = { 7, 8 };
	auto c = make_bitset_sp(200, lv);
	EXPECT_EQ(2, c.count());
}

TEST(BitsetSp_api, extract_functions) {
	BitsetSp bb(300, { 0, 63, 64, 70, 200, 299 });
	//a zero block in the middle must not matter
	bb.set(130).erase_bit(130);

	bit_index_set ls;
	bb.extract_set(ls);
	EXPECT_EQ((bit_index_set{ 0, 63, 64, 70, 200, 299 }), ls);

	utils::FixedStack<int> st(10);
	bb.extract_stack(st);
	EXPECT_EQ(6, static_cast<int>(st.size()));

	int arr[10];
	std::size_t sz = 0;
	bb.extract_array(arr, sz);
	ASSERT_EQ(6u, sz);
	EXPECT_EQ((std::vector<int>{ 0, 63, 64, 70, 200, 299 }), std::vector<int>(arr, arr + sz));

	bb.extract_array(arr, sz, true);
	ASSERT_EQ(6u, sz);
	EXPECT_EQ((std::vector<int>{ 299, 200, 70, 64, 63, 0 }), std::vector<int>(arr, arr + sz));

	BitsetSp be(300);
	be.extract_array(arr, sz);
	EXPECT_EQ(0u, sz);
}


//////////////////////////////////////////////
// Step 7 (performance work): regression tests of the rewritten functions

TEST(BitsetSp_perf, next_prev_bit_with_gaps_and_zero_blocks) {
	BitsetSp bb(1000, { 5, 63, 64, 300, 301, 700, 999 });
	bb.set(450).erase_bit(450);					//zero block in the middle

	std::vector<int> fwd, bwd;
	for (int v = bb.next_bit(BBObject::noBit); v != BBObject::noBit; v = bb.next_bit(v)) fwd.push_back(v);
	for (int v = bb.prev_bit(BBObject::noBit); v != BBObject::noBit; v = bb.prev_bit(v)) bwd.push_back(v);

	EXPECT_EQ((std::vector<int>{ 5, 63, 64, 300, 301, 700, 999 }), fwd);
	EXPECT_EQ((std::vector<int>{ 999, 700, 301, 300, 64, 63, 5 }), bwd);

	//starting points which are not set bits, in missing blocks and in zero blocks
	EXPECT_EQ(300, bb.next_bit(200));			//block 3 absent -> next non-empty
	EXPECT_EQ(700, bb.next_bit(450));			//zero block
	EXPECT_EQ(BBObject::noBit, bb.next_bit(999));
	EXPECT_EQ(301, bb.prev_bit(450));
	EXPECT_EQ(BBObject::noBit, bb.prev_bit(5));
	EXPECT_EQ(5, bb.prev_bit(63));

	BitsetSp be(100);
	EXPECT_EQ(BBObject::noBit, be.next_bit(BBObject::noBit));
	EXPECT_EQ(BBObject::noBit, be.next_bit(10));
	EXPECT_EQ(BBObject::noBit, be.prev_bit(10));
}

TEST(BitsetSp_perf, or_eq_interleaved_and_edge_cases) {
	auto bits = [](const BitsetSp& b) { return static_cast<bitpos_list>(b); };

	BitsetSp a(500, { 1, 130, 300, 450 });
	BitsetSp b(500, { 2, 70, 130, 200, 499 });
	a |= b;
	EXPECT_EQ((bitpos_list{ 1, 2, 70, 130, 200, 300, 450, 499 }), bits(a));

	BitsetSp e(500);							//empty this
	e |= b;
	EXPECT_EQ(bits(b), bits(e));

	BitsetSp c(b);
	c |= BitsetSp(500);						//empty rhs
	EXPECT_EQ(bits(b), bits(c));

	c |= c;									//self
	EXPECT_EQ(bits(b), bits(c));

	BitsetSp lo(500, { 1, 2 }), hi(500, { 400, 401 });	//rhs entirely after / before
	BitsetSp x(lo); x |= hi;
	EXPECT_EQ((bitpos_list{ 1, 2, 400, 401 }), bits(x));
	BitsetSp y(hi); y |= lo;
	EXPECT_EQ((bitpos_list{ 1, 2, 400, 401 }), bits(y));
	for (std::size_t i = 1; i < y.bitset().size(); ++i) {
		EXPECT_LT(y.bitset()[i - 1].idx_, y.bitset()[i].idx_);		//still sorted
	}
}

TEST(BitsetSp_perf, set_bit_orders) {
	BitsetSp inc(500), dec(500), mix(500);
	for (int v = 0; v < 500; v += 7) inc.set_bit(v);
	for (int v = 497; v >= 0; v -= 7) dec.set_bit(v);
	EXPECT_EQ(static_cast<bitpos_list>(inc), static_cast<bitpos_list>(dec));
	for (int v : { 400, 3, 200, 3, 450, 64, 63 }) mix.set_bit(v);
	EXPECT_EQ((bitpos_list{ 3, 63, 64, 200, 400, 450 }), static_cast<bitpos_list>(mix));
}

TEST(BitsetSp_perf, xor_eq_and_extract) {
	auto bits = [](const BitsetSp& b) { return static_cast<bitpos_list>(b); };

	BitsetSp a(500, { 1, 130, 300, 450 });
	BitsetSp b(500, { 2, 70, 130, 200, 450, 499 });
	BitsetSp x(a);
	x ^= b;
	EXPECT_EQ((bitpos_list{ 1, 2, 70, 200, 300, 499 }), bits(x));

	BitsetSp e(500);
	e ^= b;
	EXPECT_EQ(bits(b), bits(e));

	BitsetSp c(b);
	c ^= BitsetSp(500);
	EXPECT_EQ(bits(b), bits(c));

	c ^= c;										//self: empty
	EXPECT_TRUE(c.is_empty());
	EXPECT_EQ(0, c.count());

	BitsetSp y(a);
	y ^= a;
	EXPECT_TRUE(y.is_empty());

	//sorted after a merge with new blocks
	BitsetSp lo(500, { 1 }), hi(500, { 400 });
	hi ^= lo;
	ASSERT_EQ(2u, hi.bitset().size());
	EXPECT_LT(hi.bitset()[0].idx_, hi.bitset()[1].idx_);

	//extract with zero blocks, and a reused (non-empty) output vector
	BitsetSp z(500, { 5, 100, 400 });
	z.erase_bit(100);
	bitpos_list out = { 9, 9, 9 };
	z.extract(out);
	EXPECT_EQ((bitpos_list{ 5, 400 }), out);
	BitsetSp ze(100);
	ze.extract(out);
	EXPECT_TRUE(out.empty());
}


//////////////////////////////////////////////
// Additional tests of the Bitset-compatible API

TEST(BitsetSp_api, assign_bit_edge_cases) {
	auto bits = [](const BitsetSp& b) { return static_cast<bitpos_list>(b); };

	//empty rhs empties THIS but keeps its population size
	BitsetSp dst(200, { 5, 100 });
	dst.assign_bit(BitsetSp(200));
	EXPECT_TRUE(dst.is_empty());
	EXPECT_EQ(4u, dst.num_blocks());

	//THIS larger than rhs: takes the blocks of rhs, keeps its own population size
	BitsetSp big(500, { 400 });
	BitsetSp small(100, { 3, 70 });
	big.assign_bit(small);
	EXPECT_EQ((bitpos_list{ 3, 70 }), bits(big));
	EXPECT_EQ(8u, big.num_blocks());

	//zero blocks in rhs are copied as zero blocks (the bitstring is the same)
	BitsetSp rhs(200, { 5, 100 });
	rhs.erase_bit(100);
	BitsetSp t(200, { 150 });
	t.assign_bit(rhs);
	EXPECT_EQ((bitpos_list{ 5 }), bits(t));
	EXPECT_EQ(1, t.count());

	//the blocks in THIS stay sorted after dropping blocks of a larger rhs
	BitsetSp r2(300, { 1, 70, 130, 250 });
	BitsetSp t2(140);
	t2.assign_bit(r2);
	EXPECT_EQ((bitpos_list{ 1, 70, 130 }), bits(t2));

	//rhs is not modified
	EXPECT_EQ((bitpos_list{ 1, 70, 130, 250 }), bits(r2));
}

TEST(BitsetSp_api, erase_bit_returns_reference) {
	BitsetSp bb(200, { 1, 2, 100 });
	BitsetSp& ref = bb.erase_bit();
	EXPECT_EQ(&bb, &ref);
	EXPECT_TRUE(bb.is_empty());
	EXPECT_EQ(4u, bb.num_blocks());

	bb.set(7).set(8).clear(7);					//chaining
	EXPECT_EQ((bitpos_list{ 8 }), static_cast<bitpos_list>(bb));
	EXPECT_EQ(&bb, &bb.clear_all());
	EXPECT_EQ(&bb, &bb.erase_all_bits());
}

TEST(BitsetSp_api, signature_properties) {
	//queries which cannot throw
	BitsetSp b(10);
	const BitsetSp cb(10);
	EXPECT_TRUE(noexcept(cb.is_bit(1)));
	EXPECT_TRUE(noexcept(cb.test(1)));
	EXPECT_TRUE(noexcept(cb.next_bit(1)));
	EXPECT_TRUE(noexcept(cb.prev_bit(1)));
	EXPECT_TRUE(noexcept(cb.is_empty()));
	EXPECT_TRUE(noexcept(cb.is_disjoint(cb)));
	EXPECT_TRUE(noexcept(cb.is_singleton()));
	EXPECT_TRUE(noexcept(cb.find_first_common(cb)));
	EXPECT_TRUE(noexcept(cb.count()));
	EXPECT_TRUE(noexcept(b.erase_bit()));

	//no implicit conversion to a vector of bits, an explicit one exists
	EXPECT_FALSE((std::is_convertible<BitsetSp, bitpos_list>::value));
	EXPECT_TRUE((std::is_constructible<bitpos_list, BitsetSp>::value));

	//same return types as Bitset for the shared queries
	EXPECT_TRUE((std::is_same<decltype(cb.is_singleton()), decltype(Bitset(10).is_singleton())>::value));
	EXPECT_TRUE((std::is_same<decltype(cb.find_first_common(cb)), decltype(Bitset(10).find_first_common(Bitset(10)))>::value));
	EXPECT_TRUE((std::is_same<decltype(cb.count()), decltype(Bitset(10).count())>::value));
	EXPECT_TRUE((std::is_same<decltype(b.set(1)), BitsetSp&>::value));
}

TEST(BitsetSp_api, zero_blocks_in_queries) {
	BitsetSp a(300, { 1, 100, 200 });
	BitsetSp b(300, { 100, 250 });
	a.set(150).erase_bit(150);					//zero blocks in both operands
	b.set(20).erase_bit(20);

	EXPECT_EQ(100, a.find_first_common(b));
	EXPECT_FALSE(a.is_disjoint(b));
	a.erase_bit(100);
	EXPECT_EQ(BBObject::noBit, a.find_first_common(b));
	EXPECT_TRUE(a.is_disjoint(b));

	BitsetSp z(300, { 7 });
	z.erase_bit(7);								//only a zero block
	EXPECT_TRUE(z.is_empty());
	EXPECT_EQ(0, z.is_singleton());
	EXPECT_EQ(BBObject::noBit, z.find_first_common(b));
	EXPECT_EQ(BBObject::noBit, z.next_bit(BBObject::noBit));
	EXPECT_EQ(BBObject::noBit, z.prev_bit(BBObject::noBit));
	EXPECT_EQ(0, z.count());
}

//////////////////////////////////////////////
// Differential test: BitsetSp must behave as Bitset for the shared API

namespace {

	struct Pair {
		Bitset d;
		BitsetSp s;
		explicit Pair(int nPop) : d(nPop), s(nPop) {}
	};

	void fill(Pair& p, std::mt19937& rng, int nPop, int nbits) {
		std::uniform_int_distribution<int> pos(0, nPop - 1);
		for (int i = 0; i < nbits; ++i) {
			const int v = pos(rng);
			p.d.set_bit(v);
			p.s.set_bit(v);
		}
	}

	bitpos_list bits_of(const Bitset& b) { return static_cast<bitpos_list>(b); }
	bitpos_list bits_of(const BitsetSp& b) { return static_cast<bitpos_list>(b); }

	void expect_same(const Pair& p) {
		EXPECT_EQ(bits_of(p.d), bits_of(p.s));
		EXPECT_EQ(p.d.count(), p.s.count());
		EXPECT_EQ(p.d.is_empty(), p.s.is_empty());
		EXPECT_EQ(p.d.is_singleton(), p.s.is_singleton());
		EXPECT_EQ(p.d.msb(), p.s.msb());
		EXPECT_EQ(p.d.lsb(), p.s.lsb());
	}
}

TEST(BitsetSp_api, differential_against_bitset) {
	std::mt19937 rng(2026);
	const int nPop = 1000;

	for (int round = 0; round < 60; ++round) {
		//densities from very sparse to dense, including empty and singleton
		const int nA = (round % 6 == 0) ? 0 : (round % 6 == 1) ? 1 : 1 + static_cast<int>(rng() % 400);
		const int nB = static_cast<int>(rng() % 400);
		Pair a(nPop), b(nPop);
		fill(a, rng, nPop, nA);
		fill(b, rng, nPop, nB);
		expect_same(a);

		//scanning in both directions, from every 37th position
		for (int v = 0; v < nPop; v += 37) {
			EXPECT_EQ(a.d.next_bit(v), a.s.next_bit(v)) << "next_bit " << v;
			EXPECT_EQ(a.d.prev_bit(v), a.s.prev_bit(v)) << "prev_bit " << v;
			EXPECT_EQ(a.d.is_bit(v), a.s.is_bit(v));
		}
		EXPECT_EQ(a.d.next_bit(BBObject::noBit), a.s.next_bit(BBObject::noBit));
		EXPECT_EQ(a.d.prev_bit(BBObject::noBit), a.s.prev_bit(BBObject::noBit));

		//binary queries
		EXPECT_EQ(a.d.is_disjoint(b.d), a.s.is_disjoint(b.s));
		EXPECT_EQ(a.d.find_first_common(b.d), a.s.find_first_common(b.s));

		//modifying operations
		Pair x = a; x.d &= b.d; x.s &= b.s; expect_same(x);
		Pair y = a; y.d |= b.d; y.s |= b.s; expect_same(y);
		Pair z = a; z.d ^= b.d; z.s ^= b.s; expect_same(z);
		Pair w = a; w.d.erase_bit(b.d); w.s.erase_bit(b.s); expect_same(w);
		Pair u = a; u.d.andnot_eq(b.d); u.s.andnot_eq(b.s); expect_same(u);
		Pair v1 = a; v1.d.and_eq(b.d); v1.s.and_eq(b.s); expect_same(v1);
		Pair v2 = a; v2.d.or_eq(b.d); v2.s.or_eq(b.s); expect_same(v2);
		Pair v3 = a; v3.d.assign_bit(b.d); v3.s.assign_bit(b.s); expect_same(v3);

		//erasing single bits (leaves zero blocks in the sparse bitset)
		Pair e = a;
		for (int v : bits_of(a.d)) {
			if (v % 3 == 0) { e.d.erase_bit(v); e.s.erase_bit(v); }
		}
		expect_same(e);
		EXPECT_EQ(e.d.is_disjoint(b.d), e.s.is_disjoint(b.s));
		EXPECT_EQ(e.d.find_first_common(b.d), e.s.find_first_common(b.s));

		//extraction functions
		bit_index_set ds, ss;
		a.d.extract_set(ds);
		a.s.extract_set(ss);
		EXPECT_EQ(ds, ss);

		std::vector<int> da(nPop), sa(nPop);
		std::size_t dn = 0, sn = 0;
		for (bool rev : { false, true }) {
			a.d.extract_array(da.data(), dn, rev);
			a.s.extract_array(sa.data(), sn, rev);
			ASSERT_EQ(dn, sn);
			EXPECT_TRUE(std::equal(da.begin(), da.begin() + dn, sa.begin()));
		}
	}
}
