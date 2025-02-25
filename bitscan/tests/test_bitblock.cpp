/**
* @file test_bitblock.cpp
* @brief Unit tests for the bblock class which manages operations with 64-bit numbers
* @details The tests are done with the Google Test framework
* 
* TODO - add more tests, Check performance disable test (30/01/25)
**/

#include "bitscan/bitblock.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

TEST(bblockTest, masks_in_range) {

	//sets the mask all 1s except[10...15]
	BITBOARD bb = bblock::MASK_0(10, 15);
	EXPECT_FALSE(bb & Tables::mask[10]);
	EXPECT_FALSE(bb & Tables::mask[15]);
	EXPECT_TRUE(bb & Tables::mask[9]);
	EXPECT_TRUE(bb & Tables::mask[16]);
	
	//sets the mask all 0s except 1-bits in [10...15]
	BITBOARD bb1 = bblock::MASK_1(10, 15);
	EXPECT_TRUE(bb1 & Tables::mask[10]);
	EXPECT_TRUE(bb1 & Tables::mask[15]);
	EXPECT_FALSE(bb1 & Tables::mask[9]);
	EXPECT_FALSE(bb1 & Tables::mask[16]);
}


TEST(bblockTest, masks_single_argument) {

	//sets all 1s from 10 [10...63]
	BITBOARD bb = bblock::MASK_1_HIGH(10);
	EXPECT_TRUE(bb & Tables::mask[10]);
	EXPECT_TRUE(bb & Tables::mask[11]);
	EXPECT_TRUE(bb & Tables::mask[12]);
	EXPECT_TRUE(bb & Tables::mask[63]);

	//sets all 1s from 10 [0...10]
	bb = bblock::MASK_1_LOW(10);
	EXPECT_TRUE(bb & Tables::mask[10]);
	EXPECT_TRUE(bb & Tables::mask[9]);
	EXPECT_TRUE(bb & Tables::mask[8]);
	EXPECT_TRUE(bb & Tables::mask[7]);
	EXPECT_FALSE(bb & Tables::mask[11]);
	EXPECT_FALSE(bb & Tables::mask[63]);

	//sets all 0s from 10 [10...63]
	bb = bblock::MASK_0_HIGH(10);
	EXPECT_FALSE(bb & Tables::mask[10]);
	EXPECT_FALSE(bb & Tables::mask[11]);
	EXPECT_FALSE(bb & Tables::mask[12]);
	EXPECT_FALSE(bb & Tables::mask[63]);

	//sets all 0s from 10 [0...10]
	bb = bblock::MASK_0_LOW(10);
	EXPECT_FALSE(bb & Tables::mask[10]);
	EXPECT_FALSE(bb & Tables::mask[9]);
	EXPECT_FALSE(bb & Tables::mask[8]);
	EXPECT_FALSE(bb & Tables::mask[7]);
	EXPECT_FALSE(bb & Tables::mask[0]);
	EXPECT_TRUE(bb & Tables::mask[11]);
	EXPECT_TRUE(bb & Tables::mask[63]);
	
}



TEST(bblockTest, trimming) {

	//sets the mask all 1s except[10...15]
	BITBOARD bb = bblock::MASK_0(10, 15);
	BITBOARD bb_trimmed = bblock::trim_low(bb, 4);

	EXPECT_FALSE(bb_trimmed & Tables::mask[3]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[2]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[1]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[0]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[5]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[9]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[10]);

	//sets the mask all 0s except 1-bits in [10...15]
	bb_trimmed = bblock::trim_high (bb, 4);
		
	EXPECT_FALSE(bb_trimmed & Tables::mask[5]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[6]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[7]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[4]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[3]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[2]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[1]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[0]);

	//trimming at endpoints
	BITBOARD bb_trim_extreme_left  = bblock::trim_high(bb, 63);		//no trimming, nothing to the left of 63 to trim
	BITBOARD bb_trim_extreme_right = bblock::trim_low(bb, 0);		//no trimming, nothing to the right of 0 to trim
	
	EXPECT_TRUE(bb_trim_extreme_left & Tables::mask[63]);
	EXPECT_TRUE(bb_trim_extreme_right & Tables::mask[0]);
}

TEST(bblockTest, bitscan) {

	BITBOARD bb = 0xf0f0f0f0;
	
	//least significant bit variants
	EXPECT_EQ(4, bblock::lsb(bb));
	EXPECT_EQ(4, bblock::lsb64_intrinsic(bb));
	EXPECT_EQ(4, bblock::lsb64_lup(bb));
	EXPECT_EQ(4, bblock::lsb64_lup_eff(bb));
	EXPECT_EQ(4, bblock::lsb64_de_Bruijn(bb));
		
	//most significant bit variants
	EXPECT_EQ(31, bblock::msb(bb));
	EXPECT_EQ(31, bblock::msb64_intrinsic(bb));
	EXPECT_EQ(31, bblock::msb64_de_Bruijn(bb));
}

TEST(bblockTest, popcount) {

	BITBOARD bb = 0xf0f0f0f0;

	//population count variants
	EXPECT_EQ(16, bblock::popc64(bb));
	EXPECT_EQ(16, bblock::popc64_lup(bb));
	EXPECT_EQ(16, bblock::popc64_lup_1(bb));

}

TEST(bblockTest, copy_bits) {

	//sets the mask all 1s except[10...15]
	BITBOARD bbsrc =  bblock::MASK_1(10, 15);
	BITBOARD bbdest = bblock::MASK_0(5, 20);
	
	////////////////////////////////
	bblock::copy(10, 15, bbsrc, bbdest);
	////////////////////////////////

	//check bits copied
	EXPECT_TRUE(bbdest & Tables::mask[10]);
	EXPECT_TRUE(bbdest & Tables::mask[11]);
	EXPECT_TRUE(bbdest & Tables::mask[12]);
	EXPECT_TRUE(bbdest & Tables::mask[13]);
	EXPECT_TRUE(bbdest & Tables::mask[14]);
	EXPECT_TRUE(bbdest & Tables::mask[15]);

	//check bits not copied
	EXPECT_FALSE(bbdest & Tables::mask[9]);
	EXPECT_FALSE(bbdest & Tables::mask[16]);

}

TEST(bblockTest, copy_low_bits) {

	//sets the mask all 1s except[10...15]
	BITBOARD bbsrc = bblock::MASK_1(10, 15);
	BITBOARD bbdest = bblock::MASK_1(20, 25);

	////////////////////////////////
	bblock::copy_low(11, bbsrc, bbdest);
	////////////////////////////////

	//check bits copied
	EXPECT_TRUE(bbdest & Tables::mask[10]);
	EXPECT_TRUE(bbdest & Tables::mask[11]);
	EXPECT_TRUE(bbdest & Tables::mask[20]);
	EXPECT_TRUE(bbdest & Tables::mask[25]);

	//check bits not copied
	EXPECT_FALSE(bbdest & Tables::mask[9]);
	EXPECT_FALSE(bbdest & Tables::mask[12]);
}

TEST(bblockTest, copy_high_bits) {

	//sets the mask all 1s except[10...15]
	BITBOARD bbsrc = bblock::MASK_1(10, 15);
	BITBOARD bbdest = bblock::MASK_1(8, 10);

	////////////////////////////////
	bblock::copy_high(14, bbsrc, bbdest);
	////////////////////////////////
	
	//check bits copied
	EXPECT_TRUE(bbdest & Tables::mask[14]);
	EXPECT_TRUE(bbdest & Tables::mask[15]);
	EXPECT_TRUE(bbdest & Tables::mask[8]);
	EXPECT_TRUE(bbdest & Tables::mask[10]);

	//check bits not copied
	EXPECT_FALSE(bbdest & Tables::mask[13]);
	EXPECT_FALSE(bbdest & Tables::mask[16]);


}


TEST(bblockTest, print) {

	BITBOARD bb = 0xf0f0f0f0;

	stringstream sstr;
	bblock::print(bb, sstr);

	EXPECT_FALSE(sstr.str().empty());

	//I/O
	//cout << sstr.str() << endl;		

}

TEST(bblockTest, DISABLED_performance) {
	BITBOARD bb1 = 0xf0f0f0f0;
	BITBOARD bb2 = 0x070f0f0f;
	// check time ?
	for (int i = 0; i < 100000; i++) {
		EXPECT_EQ(16, bblock::popc64(bb1));
		EXPECT_EQ(15, bblock::popc64(bb2));
	}

}

