/**
* @file test_bitblock.cpp
* @brief Unit tests for the BitBoard class which manages operations with 64-bit numbers
* @details The tests are done with the Google Test framework
* 
* TODO - add more tests, Check performance disable test (30/01/25)
**/

#include "bitscan/bitboard.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

TEST(BitBoard, masks) {

	//sets the mask all 1s except[10...15]
	BITBOARD bb = BitBoard::MASK_0(10, 15);
	EXPECT_FALSE(bb & Tables::mask[10]);
	EXPECT_FALSE(bb & Tables::mask[15]);
	EXPECT_TRUE(bb & Tables::mask[9]);
	EXPECT_TRUE(bb & Tables::mask[16]);
	
	//sets the mask all 0s except 1-bits in [10...15]
	BITBOARD bb1 = BitBoard::MASK_1(10, 15);
	EXPECT_TRUE(bb1 & Tables::mask[10]);
	EXPECT_TRUE(bb1 & Tables::mask[15]);
	EXPECT_FALSE(bb1 & Tables::mask[9]);
	EXPECT_FALSE(bb1 & Tables::mask[16]);
}


TEST(BitBoard, trimming) {

	//sets the mask all 1s except[10...15]
	BITBOARD bb = BitBoard::MASK_0(10, 15);
	BITBOARD bb_trimmed = BitBoard::trim_right(bb, 4);

	EXPECT_FALSE(bb_trimmed & Tables::mask[3]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[2]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[1]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[0]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[5]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[9]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[10]);

	//sets the mask all 0s except 1-bits in [10...15]
	bb_trimmed = BitBoard::trim_left (bb, 4);
		
	EXPECT_FALSE(bb_trimmed & Tables::mask[5]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[6]);
	EXPECT_FALSE(bb_trimmed & Tables::mask[7]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[4]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[3]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[2]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[1]);
	EXPECT_TRUE(bb_trimmed & Tables::mask[0]);

	//trimming at endpoints
	BITBOARD bb_trim_extreme_left  = BitBoard::trim_left(bb, 63);		//no trimming, nothing to the left of 63 to trim
	BITBOARD bb_trim_extreme_right = BitBoard::trim_right(bb, 0);		//no trimming, nothing to the right of 0 to trim
	
	EXPECT_TRUE(bb_trim_extreme_left & Tables::mask[63]);
	EXPECT_TRUE(bb_trim_extreme_right & Tables::mask[0]);
}

TEST(BitBoard, basic_operations) {

	BITBOARD bb = 0xf0f0f0f0;
	EXPECT_EQ(16, BitBoard::popc64(bb));
	EXPECT_EQ(4,  BitBoard::lsb64_intrinsic(bb));
	EXPECT_EQ(31, BitBoard::msb64_intrinsic(bb));
}

TEST(BitBoard, print) {

	BITBOARD bb = 0xf0f0f0f0;

	stringstream sstr;
	BitBoard::print(bb, sstr);

	EXPECT_FALSE(sstr.str().empty());

	//I/O
	//cout << sstr.str() << endl;		

}

TEST(BITBOARD_Performance, DISABLED_standard) {
	BITBOARD bb1 = 0xf0f0f0f0;
	BITBOARD bb2 = 0x070f0f0f;
	// check time ?
	for (int i = 0; i < 100000; i++) {
		EXPECT_EQ(16, BitBoard::popc64(bb1));
		EXPECT_EQ(15, BitBoard::popc64(bb2));
	}

}

