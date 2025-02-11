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


//tests the 2 types of non-destructive scanning
//note: destructive scans cannot fix the initial bit, for efficiency reasons
TEST_F(BitScanningTest, firstBit) {

	int bit = bscan::noBit;
	std::vector<int> lbits;
	std::vector<int> lbits_exp;

	//direct scanning
	auto sc1 = bscan::make_scan(bb, 10);
	bit = bscan::noBit;
	while ((bit = sc1.next_bit()) != bscan::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = {64 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//direct reverse scanning
	auto sc2 = bscan::make_scan_rev(bb, 10);
	bit = bscan::noBit;

	lbits.clear();
	while ((bit = sc2.next_bit()) != bscan::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 1, 0 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

}

TEST_F(BitScanningTest, block_info) {

	int bit = bscan::noBit;
	std::vector<int> lblocks;
	std::vector<int> lblocks_exp;


	//direct scanning
	auto sc1 = bscan::make_scan(bb);
	bit = bscan::noBit;
	while ((bit = sc1.next_bit()) != bscan::noBit) {
		lblocks.push_back(sc1.get_block());
	}

	//////////////////////////////
	lblocks_exp = {0, 0, 1 };
	EXPECT_EQ(lblocks_exp, lblocks);
	//////////////////////////////

}

TEST_F(BitScanningTest, delete_additional_bitstring) {

	int bit = bscan::noBit;
	std::vector<int> lblocks;
	std::vector<int> lblocks_exp;

	BitSet bb_del(65);
	bb_del.set_bit(0);
	bb_del.set_bit(3);
	bb_del.set_bit(10);


	//direct scanning
	auto sc1 = bscan::make_scan(bb);
	bit = bscan::noBit;
	while ((bit = sc1.next_bit(bb_del)) != bscan::noBit) { ; }
	
	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	//////////////////////////////

	//reverse scanning
	auto sc2 = bscan::make_scan_rev(bb);
	bit = bscan::noBit;
	while ((bit = sc2.next_bit(bb_del)) != bscan::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	//////////////////////////////

	//destructive scanning
	auto sc3 = bscan::make_scan_dest(bb);
	bit = bscan::noBit;
	while ((bit = sc3.next_bit(bb_del)) != bscan::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	/////////////////////////////


	//destructive reverse scanning
	auto sc4 = bscan::make_scan_rev_dest(bb);
	bit = bscan::noBit;
	while ((bit = sc4.next_bit(bb_del)) != bscan::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	/////////////////////////////

}

TEST_F(BitScanningTest, to_C_array) {

	int lv[10];
	std::size_t nPop =0;

	int* pArray = bscan::to_C_array (bb, lv, nPop, false);

	/////////////////////
	EXPECT_EQ(3, nPop);
	EXPECT_EQ(pArray[0], 0);
	EXPECT_EQ(pArray[1], 1);
	EXPECT_EQ(pArray[2], 64);
	////////////////////

}

	