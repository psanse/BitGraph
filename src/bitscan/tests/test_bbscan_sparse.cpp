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
#include <vector>

using namespace std;
using namespace bitgraph;

class BBScanSpClassTest : public ::testing::Test {
protected:
	BBScanSpClassTest() : bbsc(301) {}
	virtual void SetUp() {
		for (int i = 0; i <= 300; i += 50) {
			bbsc.set_bit(i);
			pSet.push_back(i);
		}
	}

	//////////////////////
	//data members
	BBScanSp bbsc;	
	std::vector<int> pSet;
};

TEST(BitScanSparse, exception_handlers) {

	BBScanSp bbsp(130);

	EXPECT_THROW(bbsp.init_scan(BBObject::NON_DESTRUCTIVE), BitScanError);

}

TEST_F(BBScanSpClassTest, check_example) {

	EXPECT_EQ(7, bbsc.count());
	EXPECT_TRUE(bbsc.is_bit(0));
	EXPECT_TRUE(bbsc.is_bit(50));
	EXPECT_TRUE(bbsc.is_bit(100));
	EXPECT_TRUE(bbsc.is_bit(150));
	EXPECT_TRUE(bbsc.is_bit(200));
	EXPECT_TRUE(bbsc.is_bit(250));
	EXPECT_TRUE(bbsc.is_bit(300));

	EXPECT_EQ(5, bbsc.num_blocks());
	EXPECT_EQ(5, bbsc.size());

	//I/O
	//bbsc.print();
}
//
TEST_F(BBScanSpClassTest, copy_constructor) {

	BBScanSp bbscCOPY(bbsc);

	EXPECT_EQ	(7, bbscCOPY.count());
	EXPECT_TRUE	(bbscCOPY.is_bit(0));
	EXPECT_TRUE	(bbscCOPY.is_bit(50));
	EXPECT_TRUE	(bbscCOPY.is_bit(100));
	EXPECT_TRUE	(bbscCOPY.is_bit(150));
	EXPECT_TRUE	(bbscCOPY.is_bit(200));
	EXPECT_TRUE	(bbscCOPY.is_bit(250));
	EXPECT_TRUE	(bbscCOPY.is_bit(300));

	EXPECT_EQ	(5, bbscCOPY.num_blocks());
	EXPECT_EQ	(5, bbscCOPY.size());
}

TEST(BBScanSpClass, population_count){

	BBScanSp bbs(130);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	EXPECT_EQ(3, bbs.count());
	EXPECT_EQ(2, bbs.count(11, -1));
	EXPECT_EQ(1, bbs.count(21, -1));
	EXPECT_EQ(0, bbs.count(65, -1));
	EXPECT_EQ(1, bbs.count(64, -1));
}

TEST(BBScanSpClass, set_bits) {
	
	BBScanSp sb(150);
	sb.reset_bit(30,40);
	EXPECT_TRUE(sb.is_bit(30));
	EXPECT_TRUE(sb.is_bit(40));
	EXPECT_EQ(11, sb.count());

	sb.reset_bit(55);
	EXPECT_TRUE(sb.is_bit(55));
	EXPECT_EQ(1, sb.count());
}

TEST(BBScanSpClass, boolean_properties){
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


TEST(BBScanSpClass, non_destructive_direct){

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
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks

}

TEST(BBScanSpClass, non_destructive_reverse) {

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
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks
		
}

TEST(BBScanSpClass, non_destructive_with_starting_point){

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
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks
}

TEST(BBScanSpClass, non_destructive_reverse_with_starting_point) {

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
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks
}

TEST(BBScanSpClass, destructive){

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
	EXPECT_EQ(0, bbsp.count());							//population count
	EXPECT_EQ(2, bbsp.size());				//2 blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//nBB_ = 3	
}

TEST(BBScanSpClass, destructive_reverse) {

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
	EXPECT_EQ(0, bbsp.count());							//population count
	EXPECT_EQ(2, bbsp.size());							//2 blocks 
	EXPECT_EQ(3, bbsp.num_blocks());					//nBB_ = 3	
}
