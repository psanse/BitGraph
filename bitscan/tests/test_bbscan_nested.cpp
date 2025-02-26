/**
* @file test_bbscan_nested.cpp
* @brief unit tests for efficient bitscanning implemented with nested scanning classes as wrappers
* @details created 10/02/2025, last_update 12/02/2025
**/

#include "bitscan/bbset.h"
#include "bitscan/bbscan.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace std;

class BitScanNestedTest : public ::testing::Test {
protected:
	BitScanNestedTest() :bb(65) {}
	virtual void SetUp() {
		bb.set_bit(0);
		bb.set_bit(1);
		bb.set_bit(64);
	}

	BBScan bb;
};
	
//tests the 4 types of scanning
TEST_F(BitScanNestedTest, basic) {
		
	int bit = BBObject::noBit;
	std::vector<int> lbits;
	std::vector<int> lbits_exp;

	//direct scanning
	BBScan::scan sc1(bb);
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit()) != BBObject::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 0, 1, 64 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//direct reverse scanning
	BBScan::scanR sc2(bb);
	bit = BBObject::noBit;

	lbits.clear();
	while ((bit = sc2.next_bit()) != BBObject::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 64, 1, 0 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////
	
	//destructive scanning
	BBScan::scanD sc3(bb);
	bit = BBObject::noBit;

	lbits.clear();
	while ((bit = sc3.next_bit()) != BBObject::noBit) {
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
	BBScan::scanDR sc4(bb);
	bit = BBObject::noBit;

	lbits.clear();
	while ((bit = sc4.next_bit()) != BBObject::noBit) {
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
TEST_F(BitScanNestedTest, firstBit) {

	int bit = BBObject::noBit;
	std::vector<int> lbits;
	std::vector<int> lbits_exp;

	//direct scanning
	BBScan::scan sc1(bb, 10);
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit()) != BBObject::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = {64 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//direct reverse scanning
	BBScan::scanR sc2(bb, 10);
	bit = BBObject::noBit;

	lbits.clear();
	while ((bit = sc2.next_bit()) != BBObject::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 1, 0 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

}


TEST_F(BitScanNestedTest, block_info) {

	int bit = BBObject::noBit;
	std::vector<int> lblocks;
	std::vector<int> lblocks_exp;


	//direct scanning
	BBScan::scan sc1(bb);	
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit()) != BBObject::noBit) {
		lblocks.push_back(sc1.get_block());
	}

	//////////////////////////////
	lblocks_exp = {0, 0, 1 };
	EXPECT_EQ(lblocks_exp, lblocks);
	//////////////////////////////

}

TEST_F(BitScanNestedTest, delete_additional_bitstring) {

	int bit = BBObject::noBit;
	std::vector<int> lblocks;
	std::vector<int> lblocks_exp;

	BBScan bb_del(65);
	bb_del.set_bit(0);
	bb_del.set_bit(3);
	bb_del.set_bit(10);


	//direct scanning
	BBScan::scan sc1(bb);
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit(bb_del)) != BBObject::noBit) { ; }
	
	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	//////////////////////////////

	//reverse scanning
	BBScan::scanR sc2(bb);
	bit = BBObject::noBit;
	while ((bit = sc2.next_bit(bb_del)) != BBObject::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	//////////////////////////////

	//destructive scanning
	BBScan::scanD sc3(bb);
	bit = BBObject::noBit;
	while ((bit = sc3.next_bit(bb_del)) != BBObject::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	/////////////////////////////


	//destructive reverse scanning
	BBScan::scanDR sc4(bb);
	bit = BBObject::noBit;
	while ((bit = sc4.next_bit(bb_del)) != BBObject::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	/////////////////////////////

}



	