/**
* @file test_bbscan_sparse_nested.cpp
* @brief unit tests for efficient sparse bitscanning implemented with nested scanning classes as wrappers
* @details created 25/02/2025
**/

#include "bitscan/bbscan_sparse.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace bitgraph;

class BitScanNestedSparseTest : public ::testing::Test {
protected:
	BitScanNestedSparseTest() :bb(65) {}
	virtual void SetUp() {
		bb.set_bit(0);
		bb.set_bit(1);
		bb.set_bit(64);
	}

	BBScanSp bb;
};
	
//tests the 4 types of scanning
TEST_F(BitScanNestedSparseTest, basic) {
		
	int bit = BBObject::noBit;
	std::vector<int> lbits;
	std::vector<int> lbits_exp;

	//direct scanning
	BBScanSp::scan sc1(bb);
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit()) != BBObject::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 0, 1, 64 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//direct reverse scanning
	BBScanSp::scanR sc2(bb);
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
	BBScanSp::scanD sc3(bb);
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
	BBScanSp::scanDR sc4(bb);
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
TEST_F(BitScanNestedSparseTest, firstBit) {

	int bit = BBObject::noBit;
	std::vector<int> lbits;
	std::vector<int> lbits_exp;

	//direct scanning
	BBScanSp::scan sc1(bb, 10);
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit()) != BBObject::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = {64 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//direct reverse scanning
	BBScanSp::scanR sc2(bb, 10);
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

TEST(BitScanNestedSparse, scanning_empty_bitsets) {
//sparse bitsets can be empty  and the scanning classes must handle this case

	std::vector<int> lb;
	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	//no empty check - necessary if there is a doubt it can be empty 
	BBScanSp::scan sc1(bbsp);
	int bit = BBObject::noBit;	
	while( (bit = sc1.next_bit()) != BBObject::noBit){
		lb.push_back(bit);
	}

	///////////////////////
	EXPECT_EQ(10, lb[0]);
	EXPECT_EQ(20, lb[1]);
	EXPECT_EQ(64, lb[2]);
	///////////////////////

	bbsp.erase_bit();
	lb.clear();

	EXPECT_THROW(sc1.init_scan(), BitScanError);		//empty bitset cannot be scanned

	////starts a new scan  - MUST capture the return val of init_scan
	////if there is a risk the bitset to be scanned is empty
	//if ( sc1.init_scan() == 0 ) {
	//	while ((bit = sc1.next_bit()) != BBObject::noBit) {
	//		lb.push_back(bit);
	//	}

	//	///////////////////////
	//	EXPECT_EQ(10, lb[0]);
	//	EXPECT_EQ(20, lb[1]);
	//	EXPECT_EQ(64, lb[2]);
	//	///////////////////////
	//}

}

TEST_F(BitScanNestedSparseTest, block_info) {

	int bit = BBObject::noBit;
	std::vector<int> lblocks;
	std::vector<int> lblocks_exp;


	//direct scanning
	BBScanSp::scan sc1(bb);	
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit()) != BBObject::noBit) {
		lblocks.push_back(sc1.get_block());
	}

	//////////////////////////////
	lblocks_exp = {0, 0, 1 };
	EXPECT_EQ(lblocks_exp, lblocks);
	//////////////////////////////

}

TEST_F(BitScanNestedSparseTest, delete_additional_bitstring) {

	int bit = BBObject::noBit;
	std::vector<int> lblocks;
	std::vector<int> lblocks_exp;

	BBScanSp bb_del(65);
	bb_del.set_bit(0);
	bb_del.set_bit(3);
	bb_del.set_bit(10);


	//direct scanning
	BBScanSp::scan sc1(bb);
	bit = BBObject::noBit;
	while ((bit = sc1.next_bit(bb_del)) != BBObject::noBit) { ; }
	
	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	//////////////////////////////

	//reverse scanning
	BBScanSp::scanR sc2(bb);
	bit = BBObject::noBit;
	while ((bit = sc2.next_bit(bb_del)) != BBObject::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	//////////////////////////////

	//destructive scanning
	BBScanSp::scanD sc3(bb);
	bit = BBObject::noBit;
	while ((bit = sc3.next_bit(bb_del)) != BBObject::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	/////////////////////////////


	//destructive reverse scanning
	BBScanSp::scanDR sc4(bb);
	bit = BBObject::noBit;
	while ((bit = sc4.next_bit(bb_del)) != BBObject::noBit) { ; }

	//////////////////////////////
	EXPECT_EQ(2, bb_del.size());
	EXPECT_TRUE(bb_del.is_bit(3));
	EXPECT_TRUE(bb_del.is_bit(10));
	/////////////////////////////

}



	