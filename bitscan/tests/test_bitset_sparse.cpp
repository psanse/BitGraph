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
#include "gtest/gtest.h"
#include <iostream>
#include <algorithm>

using namespace std;

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
	BitSetSp bbsp;	
	vector<int> pSet;
};

TEST(Sparse, construction_basic){

	BitSetSp bbsp(1000);
	
	bbsp.set_bit(500);
	bbsp.set_bit(700);
	bbsp.set_bit(900);

	EXPECT_EQ(3, bbsp.size());						//bbsp = {500, 700, 900}
	EXPECT_TRUE(bbsp.is_bit(500));
	EXPECT_TRUE(bbsp.is_bit(700));
	EXPECT_TRUE(bbsp.is_bit(900));

	bbsp.init(100000);
	bbsp.set_bit(10);
	EXPECT_TRUE(bbsp.is_bit(10));
}

TEST(Sparse, construction_from_vector) {

	vector<int> lv = { 100, 200, 300, 400, 500 };
	BitSetSp bbsp(1000, lv);

	EXPECT_TRUE(bbsp.is_bit(100));
	EXPECT_TRUE(bbsp.is_bit(200));
	EXPECT_TRUE(bbsp.is_bit(300));
	EXPECT_TRUE(bbsp.is_bit(400));
	EXPECT_TRUE(bbsp.is_bit(500));
	EXPECT_EQ(5, bbsp.size());
	EXPECT_EQ(5, bbsp.number_of_blocks());			//sparse number of blocks, one per element in this case 
}

TEST_F(BitSetSpClassTest, copy_constructor) {

	BitSetSp bbspCOPY(bbsp);

	EXPECT_TRUE(bbspCOPY.is_bit(0));
	EXPECT_TRUE(bbspCOPY.is_bit(50));
	EXPECT_TRUE(bbspCOPY.is_bit(100));
	EXPECT_TRUE(bbspCOPY.is_bit(150));
	EXPECT_TRUE(bbspCOPY.is_bit(200));
	EXPECT_TRUE(bbspCOPY.is_bit(250));
	EXPECT_TRUE(bbspCOPY.is_bit(300));
	EXPECT_EQ(7, bbspCOPY.size());
	EXPECT_EQ(5, bbspCOPY.capacity());
	EXPECT_EQ(5, bbspCOPY.number_of_blocks());			//maximum density!
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
	EXPECT_EQ(1, bbspASSIGN.number_of_blocks());

	/////////////////////
	bbsp = bbspASSIGN;
	/////////////////////

	EXPECT_EQ(bbsp.size(), bbspASSIGN.size());
	EXPECT_EQ(bbsp.capacity(), bbspASSIGN.capacity());
	EXPECT_EQ(bbsp.number_of_blocks(), bbspASSIGN.number_of_blocks());
}

TEST_F(BitSetSpClassTest, reset) {

	//size
	bbsp.reset(10);
	EXPECT_EQ(0, bbsp.size());							//number of 1-bis
	EXPECT_EQ(0, bbsp.number_of_blocks());				//number of non_zero bitblocks

	//size and bits
	vint lv = { 10, 20, 100 };
	bbsp.reset(200, lv);
	EXPECT_EQ(3, bbsp.size());							//number of 1-bis
	EXPECT_EQ(2, bbsp.number_of_blocks());				//number of non_zero bitblocks
	EXPECT_TRUE(bbsp.is_bit(10));						//number of 1-bis		
	EXPECT_TRUE(bbsp.is_bit(20));						//number of 1-bis		
	EXPECT_TRUE(bbsp.is_bit(100));						//number of 1-bis		
}

TEST(Sparse, find_block)
{
	BitSetSp bbsp(1000);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(998);
		
	BITBOARD bb = bbsp.find_block(WDIV(1000));		//bb = 998
	EXPECT_TRUE(bblock::is_bit(bb, 38));			//the bitset starts at 960, so 998 has offset 38.

	//variant returning also the position in the collection (not its real index)
	auto pos = BBObject::noBit;	
	auto it = bbsp.find_block(WDIV(1000), pos);		//bb = 998
	EXPECT_EQ(1, pos);								//second block in the sparse bitset

	//empty bitset
	it = bbsp.find_block(WDIV(650), pos);		
	EXPECT_EQ(BBObject::noBit, pos);
}

TEST(Sparse, find_block_2) {

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_FALSE(bbsp.is_bit(63));
	EXPECT_EQ(3,bbsp.size());

	bbsp.erase_bit(10);
	EXPECT_FALSE(bbsp.is_bit(10));
	EXPECT_EQ(2,bbsp.size());

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

	BitSetSp lhs;
	lhs.init(200);
	lhs.set_bit(63);
	lhs.set_bit(126);
	lhs.set_bit(189);	
	
	BitSetSp rhs;
	rhs.init(200);
	rhs.set_bit(63);
	rhs.set_bit(127);
	
	lhs.AND_block(1,rhs);
	EXPECT_EQ(1, lhs.size());
	EXPECT_TRUE(lhs.is_bit(63));

	lhs.set_block(1, -1 , rhs);
	EXPECT_TRUE(lhs.is_bit(127));
	EXPECT_EQ(2, lhs.size());
} 



TEST(Sparse, population_count){

	BitSetSp bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_EQ(3, bb.size());
	EXPECT_EQ(2, bb.size(11, -1));
	EXPECT_EQ(1, bb.size(21, -1));
	EXPECT_EQ(0, bb.size(65, -1));
	EXPECT_EQ(1, bb.size(64, -1));
}

TEST(Sparse, set_bits) {
	
	BitSetSp bbsp(10000);

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
	EXPECT_EQ(32, bbsp.size());
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));
	

	bbsp.set_bit(1002,1002);
	EXPECT_TRUE(bbsp.is_bit(1002));
	EXPECT_EQ(33, bbsp.size());
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));


	//range
	bbsp.set_bit(29, 125);
	EXPECT_FALSE(bbsp.is_bit(28));
	EXPECT_TRUE(bbsp.is_bit(29));
	EXPECT_TRUE(bbsp.is_bit(125));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));

	bbsp.set_bit(1001,1100);
	EXPECT_FALSE(bbsp.is_bit(999));
	EXPECT_TRUE(bbsp.is_bit(1001));
	EXPECT_TRUE(bbsp.is_bit(1100));
	EXPECT_FALSE(bbsp.is_bit(1101));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));

}

TEST(Sparse, set_bits_from_bitset) {

	BitSetSp bbsp(10000);
	BitSetSp bbsp1(10000);			//same population	

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
	EXPECT_EQ(9, bbsp.size());
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));
}


TEST(Sparse, erase_block_range) {

	BitSetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	BitSetSp bbsp1(10000);
		
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
	
	BitSetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	BitSetSp bbsp1(10000);

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
	EXPECT_EQ(7, bbsp.size());

	//range
	bbsp.set_block(2, WDIV(10000) - 1, bbsp1);
	EXPECT_TRUE(bbsp.is_bit(129));
	EXPECT_TRUE(bbsp.is_bit(5000));
}

TEST(Sparse, erase_bits) {
	
	BitSetSp bbsp(10000);

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
	EXPECT_EQ(3, bbsp.size());
	EXPECT_EQ(4, bbsp.number_of_blocks());		//all bitblocks are still there (with 0 value)
	
	//range
	bbsp.erase_bit(1,1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.size());
	
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
	EXPECT_EQ(11, bbsp.size());
	EXPECT_TRUE(bbsp.is_bit(200));

	//first bit in range does not correspond to a block
	bbsp.reset(10000);
	bbsp.set_bit(70, 75);	
	bbsp.erase_bit(63, 70);	
	EXPECT_EQ(5,bbsp.size());
	EXPECT_FALSE(bbsp.is_bit(70));
	EXPECT_TRUE(bbsp.is_bit(71));
}



TEST(Sparse, member_OR_operator) {

	BitSetSp bbsp(130);
	bbsp.set_bit(64);
	bbsp.set_bit(120);
	bbsp.shrink_to_fit();
	

	BitSetSp bbsp1(130);
	bbsp1.set_bit(5);
	bbsp1.set_bit(54);
	
	
	//OR
	//////////////////
	bbsp |= bbsp1; 	
	/////////////////
		
	EXPECT_EQ(4, bbsp.size());
	EXPECT_TRUE(bbsp.is_bit(5));
	EXPECT_TRUE(bbsp.is_bit(54));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(bbsp.is_bit(120));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));
	
}

TEST(Sparse, member_XOR_operator) {

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(30);
	bbsp.set_bit(64);
	bbsp.set_bit(128);

	BitSetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);

	//XOR- set difference
	//////////////////
	bbsp ^= bbsp1;
	/////////////////

	EXPECT_EQ(3, bbsp.size());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(54));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));

}

TEST(Sparse, OR_block) {

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	BitSetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);

	//OR
	//////////////////
	bbsp.set_block(1, -1, bbsp1);
	/////////////////
	
	EXPECT_EQ(4, bbsp.size());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(bbsp.is_bit(128));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));

}

TEST(Sparse, AND_block) {

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(115);
	bbsp.set_bit(128);

	BitSetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(115);
		
	//////////////////
	bbsp.AND_block(1, bbsp1);		//AND masks non-zero bitblocks with index 2 and 3
	/////////////////

	EXPECT_EQ(3, bbsp.size());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(115));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));

}

TEST(Sparse, friend_AND_block_range) {

	BitSetSp bbsp(10000);
	BitSetSp bbspAND(10000);
	BitSetSp res(10000);

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

	EXPECT_EQ(1, res.size());
	EXPECT_TRUE(res.is_bit(9000));

}

TEST(Sparse, member_AND_operator) {

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	bbsp.set_bit(129);

	BitSetSp bbsp1(130);
	bbsp1.set_bit(10);
	bbsp1.set_bit(64);
	bbsp1.set_bit(128);

	//AND
	//////////////////
	bbsp &= bbsp1;
	/////////////////

	EXPECT_EQ(2, bbsp.size());
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_TRUE(std::is_sorted(bbsp.bitset().begin(), bbsp.bitset().end(), BitSetSp::pBlock_less()));

}

TEST(Sparse, operators){

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	BitSetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);
	
	//OR
	BitSetSp resOR(130);
	OR(bbsp1, bbsp, resOR);
	EXPECT_EQ(6, resOR.size());

	//=
	bbsp=resOR;
	EXPECT_EQ(6, bbsp.size());
	EXPECT_TRUE(bbsp==resOR);

	//AND
	BitSetSp bbsp2(130,true);
	bbsp2.set_bit(54);
	BitSetSp resAND(130);
	AND(bbsp1,bbsp2,resAND);
	EXPECT_TRUE(resAND.is_bit(54));
	EXPECT_EQ(1, resAND.size());
}

TEST(Sparse, insertion) {

	BitSetSp bbsp(130);					
	bbsp.set_bit(64);
	bbsp.set_bit(65);
	bbsp.set_bit(12);					//obligatory

	EXPECT_TRUE(bbsp.is_bit(12));
}

TEST(Sparse, integration) {

	BitSetSp bbs(10000);
	bbs.set_bit(10);
	bbs.set_bit(1000);
	bbs.set_bit(70);
	bbs.set_bit(150);
	
	BitSetSp bbs1(10000);
	bbs1.set_bit(1000);

	BitSetSp bbs2(10000);
	AND(bbs1, bbs, bbs2);
	EXPECT_EQ(1, bbs2.size());
	EXPECT_TRUE(bbs2.is_bit(1000));
	EXPECT_TRUE(bbs2==bbs1);

	BitSetSp bbs3(10000);
	OR(bbs1,bbs, bbs3);
	EXPECT_EQ(4, bbs3.size());
	EXPECT_TRUE(bbs3.is_bit(1000));
	EXPECT_TRUE(bbs3.is_bit(10));
	EXPECT_TRUE(bbs3.is_bit(70));
	EXPECT_TRUE(bbs3.is_bit(150));

	bbs3.erase_bit(1000);
	EXPECT_EQ(3, bbs3.size());			//even though one bit block is empty
	EXPECT_EQ(4, bbs3.number_of_blocks());		
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
	EXPECT_EQ(4, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(126));

	bbcopy.reset_bit(0, 1000, bbsp);
	EXPECT_EQ(6, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.reset_bit(0, 9000, bbsp);
	EXPECT_EQ(7, bbcopy.size());
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
	EXPECT_EQ(3, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(126));
	EXPECT_TRUE(bbcopy.is_bit(127));
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.reset(10000);
	bbcopy.reset_bit(127, 129, bbsp);
	EXPECT_EQ(1, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(127));

	bbcopy.reset(10000);
	bbcopy.reset_bit(0, 10000, bbsp);
	EXPECT_TRUE( bbcopy == bbsp);

	bbcopy.reset(10000);
	bbcopy.reset_bit(9000, 9999, bbsp);
	EXPECT_EQ(2, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(9000));
	EXPECT_TRUE(bbcopy.is_bit(9999));

	bbcopy.reset(10000);
	bbcopy.reset_bit(0, 0, bbsp);
	EXPECT_EQ(1, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(0));

	bbcopy.reset(10000);
	bbcopy.reset_bit(9999, 9999, bbsp);
	EXPECT_EQ(1, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(9999));
}

TEST(Sparse, copy_in_closed_range_special_cases){

	BBScanSp bbsp(120);
	BBScanSp bbcopy(120);
	bbsp.set_bit(64);

	bbcopy.reset_bit(0, 51, bbsp);
	bbcopy.print();
	EXPECT_EQ(0, bbcopy.size());

	bbcopy.reset(120);
	bbcopy.reset_bit(64, 65, bbsp);
	EXPECT_EQ(1, bbcopy.size());
	EXPECT_TRUE(bbcopy.is_bit(64));

	bbcopy.reset(120);
	bbcopy.reset_bit(63, 64, bbsp);
	EXPECT_EQ(1, bbcopy.size());
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
	EXPECT_EQ(5, bbsp.size());

	//////////////////
	bbsp &= bbkeep3;
	///////////////////
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_EQ(1, bbsp.size());

}

TEST(Sparse, DISABLED_clear_bits) {

	BitSetSp bbsp(10000);

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
	EXPECT_EQ(3, bbsp.size());
	EXPECT_EQ(3, bbsp.number_of_blocks());		//first and 9000 bitblocks are there + 1000 bitblock set to 0

	bbsp.clear_bit(2, 1064);
	EXPECT_EQ(2, bbsp.number_of_blocks());		//removes 1000 bitblock

	//range
	bbsp.clear_bit(1, 1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.size());
	EXPECT_EQ(2, bbsp.number_of_blocks());


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
	EXPECT_EQ(3, bbsp.number_of_blocks());

	bbsp.clear_bit(EMPTY_ELEM, 1);
	EXPECT_TRUE(bbsp.is_bit(2));
	EXPECT_EQ(3, bbsp.number_of_blocks());

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
