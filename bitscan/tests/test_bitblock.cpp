/**
* @file test_bitblock.cpp
* @brief Unit tests for the BitBoard class which manages operations with 64-bit numbers
* @details The tests are done with the Google Test framework
**/

#include "bitscan/bitboard.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

TEST(Masks, single_block) {

	BITBOARD bb = BitBoard::MASK_0(10, 15);
	EXPECT_FALSE(bb & Tables::mask[10]);
	EXPECT_FALSE(bb & Tables::mask[15]);


	BITBOARD bb1 = BitBoard::MASK_1(10, 15);
	EXPECT_TRUE(bb1 & Tables::mask[10]);
	EXPECT_TRUE(bb1 & Tables::mask[15]);
}

TEST(Scan, single_block) {

	BITBOARD bb = 0xf0f0f0f0;
	EXPECT_EQ(16, BitBoard::popc64(bb));
	EXPECT_EQ(4,  BitBoard::lsb64_intrinsic(bb));
	EXPECT_EQ(31, BitBoard::msb64_intrinsic(bb));
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

