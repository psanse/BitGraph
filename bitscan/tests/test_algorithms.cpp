/**
* @file test_algorithms.cpp
* @brief Unit tests for the bitstring algorithms and data strcutures in bbalgorithm.h
* @details created 28/3/17, last_update 14/02/2025
* @detials added while working on the MWCP algorithm
* @author pss
**/

#include "bitscan/bbalgorithm.h"				
#include "bitscan/bbscan.h"
#include "bitscan/bbscan_sparse.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

//aliases
using vint = vector<int>;

class bbSizeClassTest : public ::testing::Test {
protected:
	bbSizeClassTest() : bb(NV), bbs (NV){}
	void SetUp() override {
		bb.set_bit(10);
		bb.set_bit(64);
		bb.set_bit(65);

		bbs.set_bit(10);
		bbs.set_bit(64);
		bbs.set_bit(65);
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 65;
	bbSize_t<BBScan> bb;												//undirected graph with integer weights
	bbSize_t<BBScanSp> bbs;
};

class StackClassTest : public ::testing::Test {
protected:
	StackClassTest() : bb(NV), bbs(NV) {}
	void SetUp() override {
		bb.push(10);
		bb.push(64);
		bb.push(65);

		bbs.push(10);
		bbs.push(64);
		bbs.push(65);
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 65;
	sbb_t<BBScan> bb;												//undirected graph with integer weights
	sbb_t<BBScanSp> bbs;
};

TEST_F(bbSizeClassTest, pop_msb) {
	
	//pop_msb non-sparse
	EXPECT_EQ(3, bb.size());
	EXPECT_EQ(65,bb.pop_msb());
	EXPECT_EQ(2, bb.size());

	EXPECT_EQ(64,bb.pop_msb());
	EXPECT_EQ(1, bb.size());

	EXPECT_EQ(10, bb.pop_msb());
	EXPECT_EQ(0, bb.size());

	EXPECT_EQ(-1, bb.pop_msb());
	EXPECT_TRUE(bb.is_empty());

	//pop_msb sparse
	EXPECT_EQ(3, bbs.size());
	EXPECT_EQ(65, bbs.pop_msb());
	EXPECT_EQ(2, bbs.size());

	EXPECT_EQ(64, bbs.pop_msb());
	EXPECT_EQ(1, bbs.size());

	EXPECT_EQ(10, bbs.pop_msb());
	EXPECT_EQ(0, bbs.size());

	EXPECT_EQ(-1, bbs.pop_msb());
	EXPECT_TRUE(bbs.is_empty());
}

TEST_F(bbSizeClassTest, pop_lsb) {

	//pop_msb
	EXPECT_EQ(3, bb.size());
	EXPECT_EQ(10, bb.pop_lsb());
	EXPECT_EQ(2, bb.size());

	EXPECT_EQ(64, bb.pop_lsb());
	EXPECT_EQ(1, bb.size());

	EXPECT_EQ(65, bb.pop_lsb());
	EXPECT_EQ(0, bb.size());

	EXPECT_EQ(-1, bb.pop_lsb());
	EXPECT_TRUE(bb.is_empty());

	//pop_msb -sparse
	EXPECT_EQ(3, bbs.size());
	EXPECT_EQ(10, bbs.pop_lsb());
	EXPECT_EQ(2, bbs.size());

	EXPECT_EQ(64, bbs.pop_lsb());
	EXPECT_EQ(1, bbs.size());

	EXPECT_EQ(65, bbs.pop_lsb());
	EXPECT_EQ(0, bbs.size());

	EXPECT_EQ(-1, bbs.pop_lsb());
	EXPECT_TRUE(bbs.is_empty());
}

TEST_F(bbSizeClassTest, reset) {

	//non-sparse
	bb.reset(100);
	EXPECT_EQ(0, bb.size());
	EXPECT_EQ(0, bb.pc_);
	EXPECT_EQ(2, bb.bb_.capacity());

	//sparse
	bbs.reset(100);
	EXPECT_EQ(0, bbs.size());
	EXPECT_EQ(0, bbs.pc_);
	EXPECT_EQ(2, bbs.bb_.capacity());

}

TEST_F(bbSizeClassTest, bit_operations) {

	//non-sparse
	//bb:{10, 64, 65}, size=3
	EXPECT_EQ(3, bb.size());
	EXPECT_EQ(65, bb.msb());
	EXPECT_EQ(10, bb.lsb());
	EXPECT_EQ(2, bb.erase_bit(65));
	EXPECT_EQ(2, bb.size());
	EXPECT_EQ(1, bb.erase_bit(64));
	EXPECT_EQ(1, bb.size());
	EXPECT_EQ(0, bb.erase_bit(10));
	EXPECT_EQ(0, bb.size());
	EXPECT_TRUE(bb.is_empty());	

	//sparse
	EXPECT_EQ(3, bbs.size());
	EXPECT_EQ(65, bbs.msb());
	EXPECT_EQ(10, bbs.lsb());
	EXPECT_EQ(2, bbs.erase_bit(65));
	EXPECT_EQ(2, bbs.size());
	EXPECT_EQ(1, bbs.erase_bit(64));
	EXPECT_EQ(1, bbs.size());
	EXPECT_EQ(0, bbs.erase_bit(10));
	EXPECT_EQ(0, bbs.size());
	EXPECT_TRUE( bbs.is_empty());
}

TEST_F(bbSizeClassTest, lazy_bit_erase) {

	////////////
	//non-sparse
	 
	//cleans the bitset without modifying the bitset
	bb.erase_bit(true);										//bb:{10, 64, 65}, size=0 (lazy)
	
	//empty bitset to all effects
	EXPECT_EQ(BBObject::noBit, bb.msb());
	EXPECT_EQ(BBObject::noBit, bb.lsb());
	EXPECT_EQ(BBObject::noBit, bb.pop_msb());
	EXPECT_EQ(BBObject::noBit, bb.pop_lsb());
	EXPECT_TRUE(bb.is_empty());
	EXPECT_EQ(0, bb.size());

	//not sync
	EXPECT_FALSE(bb.is_sync_pc());

	//sync cached popcount - restores bb
	bb.sync_pc();						//restores bb:{10, 64, 65} with size=3	
	EXPECT_TRUE(bb.is_sync_pc());
	EXPECT_EQ(3, bb.size());
	EXPECT_TRUE(bb.bb_.is_bit(10));
	EXPECT_TRUE(bb.bb_.is_bit(64));
	EXPECT_TRUE(bb.bb_.is_bit(65));

	////////////
	//sparse

	//cleans the bitset without modifying the bitset
	bbs.erase_bit(true);										//bb:{10, 64, 65}, size=0 (lazy)

	//empty bitset to all effects
	EXPECT_EQ(BBObject::noBit, bbs.msb());
	EXPECT_EQ(BBObject::noBit, bbs.lsb());
	EXPECT_EQ(BBObject::noBit, bbs.pop_msb());
	EXPECT_EQ(BBObject::noBit, bbs.pop_lsb());
	EXPECT_TRUE(bbs.is_empty());
	EXPECT_EQ(0, bbs.size());

	//not sync
	EXPECT_FALSE(bbs.is_sync_pc());

	//sync cached popcount - restores bbs
	bbs.sync_pc();							//bbs:{10, 64, 65} with size=3	
	EXPECT_TRUE(bbs.is_sync_pc());
	EXPECT_EQ(3, bbs.size());
	EXPECT_TRUE(bbs.bb_.is_bit(10));
	EXPECT_TRUE(bbs.bb_.is_bit(64));
	EXPECT_TRUE(bbs.bb_.is_bit(65));

}

TEST_F(StackClassTest, DISABLED_copy_construction) {
	
	//cannot copy

	//not valid move semantics	

}

TEST_F(StackClassTest, basic_operations){
	
	//non-sparse
	EXPECT_EQ(3, bb.size());
	EXPECT_EQ(true, bb.is_sync());
	
	//does not increase size
	bb.push(65);
	EXPECT_EQ(3, bb.size());
	EXPECT_EQ(true, bb.is_sync());

	bb.bb_.erase_bit(65);
	EXPECT_EQ(false, bb.is_sync());
	
	bb.sync_stack();
	EXPECT_EQ(2, bb.size());

	//sparse
	EXPECT_EQ(3, bbs.size());
	EXPECT_EQ(true, bbs.is_sync());

	//does not increase size
	bbs.push(65);
	EXPECT_EQ(3, bbs.size());
	EXPECT_EQ(true, bbs.is_sync());

	bbs.bb_.erase_bit(65);
	EXPECT_EQ(false, bbs.is_sync());

	bbs.sync_stack();
	EXPECT_EQ(2, bbs.size());
	
}

TEST(bba_t, basic){
	
	bba_t<BBScan> b;		
	b.init(10, 65);
	b.set_bit(0,10);
	b.set_bit(0,64);
	b.set_bit(0,65);
	b.set_bit(9,10);
	b.set_bit(9,20);
	b.set_bit(9,30);
	
	EXPECT_TRUE(b.is_bit(0,10));
	EXPECT_TRUE(b.is_bit(0,64));
	EXPECT_TRUE(b.is_bit(9,10));
	EXPECT_TRUE(b.is_bit(9,30));
	EXPECT_FALSE(b.is_bit(9,31));

	EXPECT_EQ(3,b.popcn(0));
	EXPECT_EQ(3,b.popcn(9));
	EXPECT_NE(3,b.popcn(8));

	//reuse
	b.init(1, 65);
	b.set_bit(0,10);
	
	EXPECT_TRUE(b.is_bit(0,10));
	EXPECT_EQ(1, b.capacity);
	
}

TEST(k_bits, basic){
//date: 18/8/17 during MWCP upper bound computation
	
	BBScan bb(100);	
	bb.set_bit(10);
	bb.set_bit(64);
	bb.set_bit(65);
	
	//solution in vector
	vint bits (3,-1);
	first_k_bits(3,bb,bits);			

	vint sol;
	sol.push_back(10); sol.push_back(64); sol.push_back(65);
	EXPECT_EQ(sol, bits);

	//solution in a classical C-array
	int cbits[3]; vint vbits;
	first_k_bits(3,bb, cbits);	
	copy(cbits, cbits+3,back_inserter(vbits));
	EXPECT_EQ(sol, vbits);
	
}
