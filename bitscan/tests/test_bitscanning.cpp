/**
* @file test_bitscanning.cpp
* @brief attempt to simplify syntax of bitscans with generic programming
* @details created 10/02/2025
**/

#include "bitscan/bbscanFeature.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace std;

class BitScanningTest : public ::testing::Test {
protected:
	BitScanningTest() :bb(65) {}
	virtual void SetUp() {
		bb.set_bit(0);
		bb.set_bit(1);
		bb.set_bit(64);
	}

	BitSet bb;
};
	
//tests the 4 types of scanning
TEST_F(BitScanningTest, basic) {
	
	int bit = bscan::noBit;
	std::vector<int> lbits;
	std::vector<int> lbits_exp;

	//direct scanning
	auto sc1 = bscan::make_scan(bb);
	bit = bscan::noBit;
	while ((bit = sc1.next_bit()) != bscan::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 0, 1, 64 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//direct reverse scanning
	auto sc2 = bscan::make_scan_rev(bb);
	bit = bscan::noBit;

	lbits.clear();
	while ((bit = sc2.next_bit()) != bscan::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 64, 1, 0 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////
	
	//destructive scanning
	auto sc3 = bscan::make_scan_dest(bb);
	bit = bscan::noBit;

	lbits.clear();
	while ((bit = sc3.next_bit()) != bscan::noBit) {
		lbits.emplace_back(bit);
	}

	///////////////////////////////
	lbits_exp = { 0, 1, 64};
	EXPECT_EQ(lbits_exp, lbits);
	EXPECT_TRUE(bb.is_empty());
	////////////////////////////////

	//restores original bitset
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);

	//destructive reverse scanning
	auto sc4 = bscan::make_scan_rev_dest(bb);
	bit = bscan::noBit;

	lbits.clear();
	while ((bit = sc4.next_bit()) != bscan::noBit) {
		lbits.emplace_back(bit);
	}

	///////////////////////////////
	lbits_exp = { 64, 1, 0 };
	EXPECT_EQ(lbits_exp, lbits);
	EXPECT_TRUE(bb.is_empty());
	////////////////////////////////
	
}

	