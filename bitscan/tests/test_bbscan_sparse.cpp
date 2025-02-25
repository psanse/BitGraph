/**
* @file test_bbscan_sparse.cpp
* @brief Unit tests for BBScan class which manages efficient bitscanning for sparse bitsets
* @details Manual bitscanning tests, not using nested classes for bitscanning
* @details created 25/02/2025
* @author pss
**/

#include "bitscan/bbscan_sparse.h"
#include "gtest/gtest.h"
#include <iostream>
#include <algorithm>

using namespace std;

class BBScanClassTest : public ::testing::Test {
protected:
	BBScanClassTest() : bbsc(301) {}
	virtual void SetUp() {
		for (int i = 0; i <= 300; i += 50) {
			bbsc.set_bit(i);
			pSet.push_back(i);
		}
	}

	//////////////////////
	//data members
	BBScanSp bbsc;	
	vector<int> pSet;
};

TEST_F(BBScanClassTest, check_example) {

	EXPECT_EQ(7, bbsc.size());
	EXPECT_TRUE(bbsc.is_bit(0));
	EXPECT_TRUE(bbsc.is_bit(50));
	EXPECT_TRUE(bbsc.is_bit(100));
	EXPECT_TRUE(bbsc.is_bit(150));
	EXPECT_TRUE(bbsc.is_bit(200));
	EXPECT_TRUE(bbsc.is_bit(250));
	EXPECT_TRUE(bbsc.is_bit(300));

	EXPECT_EQ(5, bbsc.capacity());
	EXPECT_EQ(5, bbsc.number_of_blocks());

	//I/O
	//bbsc.print();
}

TEST_F(BBScanClassTest, copy_constructor) {

	BBScanSp bbscCOPY(bbsc);

	EXPECT_EQ	(7, bbscCOPY.size());
	EXPECT_TRUE	(bbscCOPY.is_bit(0));
	EXPECT_TRUE	(bbscCOPY.is_bit(50));
	EXPECT_TRUE	(bbscCOPY.is_bit(100));
	EXPECT_TRUE	(bbscCOPY.is_bit(150));
	EXPECT_TRUE	(bbscCOPY.is_bit(200));
	EXPECT_TRUE	(bbscCOPY.is_bit(250));
	EXPECT_TRUE	(bbscCOPY.is_bit(300));

	EXPECT_EQ	(5, bbscCOPY.capacity());
	EXPECT_EQ	(5, bbscCOPY.number_of_blocks());
}

TEST(BBScan, population_count){

	BBScanSp bbs(130);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	EXPECT_EQ(3, bbs.size());
	EXPECT_EQ(2, bbs.size(11, -1));
	EXPECT_EQ(1, bbs.size(21, -1));
	EXPECT_EQ(0, bbs.size(65, -1));
	EXPECT_EQ(1, bbs.size(64, -1));
}

TEST(BBScan, set_bits) {
	
	BBScanSp sb(150);
	sb.reset_bit(30,40);
	EXPECT_TRUE(sb.is_bit(30));
	EXPECT_TRUE(sb.is_bit(40));
	EXPECT_EQ(11, sb.size());

	sb.reset_bit(55);
	EXPECT_TRUE(sb.is_bit(55));
	EXPECT_EQ(1, sb.size());	
}

TEST(BBScan, boolean_properties){
//Properties, inherited from BitSetSp

	BBScanSp bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BBScanSp bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_disjoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));

	//is disjoint with ranges
	EXPECT_TRUE	(bb.is_disjoint_block(0,0,bb1));
	EXPECT_FALSE(bb.is_disjoint_block(0,1,bb1));
	EXPECT_FALSE(bb.is_disjoint_block(1,2,bb1));

}

TEST(Scanning, non_destructive_direct){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//non destructive
	int nBit = BBObject::noBit;
	int retVal = bbsp.init_scan(BBObject::NON_DESTRUCTIVE);

	//////////////////////////
	EXPECT_NE(-1, retVal);
	//////////////////////////

	//scanning
	while ( (nBit = bbsp.next_bit()) != BBObject::noBit ){
		v.push_back(nBit);
	}
			
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_EQ(2, bbsp.number_of_blocks());				//number of blocks 
	EXPECT_EQ(3, bbsp.capacity());						//number of blocks

}

TEST(Scanning, non_destructive_reverse) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	int retVal = bbsp.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);

	//////////////////////////
	EXPECT_NE(-1, retVal);
	//////////////////////////

	//scanning
	while ((nBit = bbsp.prev_bit()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
	EXPECT_EQ(2, bbsp.number_of_blocks());				//number of blocks 
	EXPECT_EQ(3, bbsp.capacity());						//number of blocks
		
}

TEST(Scanning, non_destructive_with_starting_point){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	int retVal = bbsp.init_scan(15, BBObject::NON_DESTRUCTIVE);

	//////////////////////////
	EXPECT_NE(-1, retVal);
	//////////////////////////

	//scanning
	while ((nBit = bbsp.next_bit()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(20, v[0]);
	EXPECT_EQ(64, v[1]);
	EXPECT_EQ(2, bbsp.number_of_blocks());				//number of blocks 
	EXPECT_EQ(3, bbsp.capacity());						//number of blocks
}

TEST(Scanning, non_destructive_reverse_with_starting_point) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	int retVal = bbsp.init_scan(15, BBObject::NON_DESTRUCTIVE_REVERSE);

	//////////////////////////
	EXPECT_NE(-1, retVal);
	//////////////////////////

	//scanning
	while ((nBit = bbsp.prev_bit()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(2, bbsp.number_of_blocks());				//number of blocks 
	EXPECT_EQ(3, bbsp.capacity());						//number of blocks
}

TEST(Scanning, destructive){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	int retVal = bbsp.init_scan(BBObject::DESTRUCTIVE);

	//////////////////////////
	EXPECT_NE(-1, retVal);
	//////////////////////////

	//scanning
	while ((nBit = bbsp.next_bit_del()) != BBObject::noBit) {
		v.push_back(nBit);
	}
		
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_TRUE(bbsp.is_empty());
	EXPECT_EQ(0, bbsp.size());							//population count
	EXPECT_EQ(2, bbsp.number_of_blocks());				//2 blocks 
	EXPECT_EQ(3, bbsp.capacity());						//nBB_ = 3	
}

TEST(Scanning, destructive_reverse) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	int retVal = bbsp.init_scan(BBObject::DESTRUCTIVE_REVERSE);

	//////////////////////////
	EXPECT_NE(-1, retVal);
	//////////////////////////

	//scanning
	while ((nBit = bbsp.prev_bit_del()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
	EXPECT_TRUE(bbsp.is_empty());
	EXPECT_EQ(0, bbsp.size());							//population count
	EXPECT_EQ(2, bbsp.number_of_blocks());				//2 blocks 
	EXPECT_EQ(3, bbsp.capacity());						//nBB_ = 3	
}
