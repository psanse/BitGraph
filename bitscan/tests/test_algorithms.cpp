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

///////////////////////////
//predeclaration for g++
#ifdef __GNUC__
	const int BBObject::noBit;
#endif

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
	bbStack_t<BBScan> bb;												//undirected graph with integer weights
	bbStack_t<BBScanSp> bbs;
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

TEST(bbCol_tClass, basic){
	
	bbCol_t<BBScan, 20> bbc(130);		
	bbc.set_bit(0,10);
	bbc.set_bit(0,64);
	bbc.set_bit(0,65);
	bbc.set_bit(9,10);
	bbc.set_bit(9,20);
	bbc.set_bit(9,30);
	
	EXPECT_TRUE(bbc.is_bit(0,10));
	EXPECT_TRUE(bbc.is_bit(0,64));
	EXPECT_TRUE(bbc.is_bit(9,10));
	EXPECT_TRUE(bbc.is_bit(9,30));
	EXPECT_FALSE(bbc.is_bit(9,31));

	EXPECT_EQ(3, bbc.size(0));
	EXPECT_EQ(3, bbc.size(9));
	EXPECT_NE(3, bbc.size(8));

	//reuse
	bbc.reset(25);
	bbc.set_bit(0,10);
	
	EXPECT_TRUE(bbc.is_bit(0,10));
	EXPECT_EQ(20, bbc.capacity());

	//set bit with first bit info
	bool isFirstBit = BBObject::noBit;
	bbc.set_bit(0, 1, isFirstBit);

	EXPECT_TRUE(bbc.is_bit(0, 1));
	EXPECT_TRUE(isFirstBit);
	EXPECT_FALSE(bbc.is_bit(0, 0));		//not set, since 1 is the first bit
	
}

TEST(algorithms, first_k_bits){

	
	BBScan bb(100);	
	bb.set_bit(10);
	bb.set_bit(64);
	bb.set_bit(65);
	
	//solution in vector
	vint lbits (3,-1);
	bbalg::first_k_bits(3,bb, lbits);

	vint lbits_exp;
	lbits_exp.push_back(10);
	lbits_exp.push_back(64);
	lbits_exp.push_back(65);
	EXPECT_EQ(lbits_exp, lbits);
	
}

TEST(algorithms, random_bitblock) {
	
	//10% of density	
	BITBOARD bb = bbalg::gen_random_block(0.5);

	//around 30 bits
	EXPECT_GE(bblock::size(bb), 20);

}

TEST(algorithms, to_vector) {

	BBScan bb(100);
	bb.set_bit(10);
	bb.set_bit(64);
	bb.set_bit(65);

	//stateless conversion to vector
	auto lv = bbalg::to_vector(bb);
	EXPECT_EQ(10, lv[0]);
	EXPECT_EQ(64, lv[1]);
	EXPECT_EQ(65, lv[2]);

	//sparse bitset
	BBScanSp bbs(100);
	bbs.set_bit(10);
	bbs.set_bit(64);
	bbs.set_bit(65);

	//stateless conversion to vector
	auto lvs = bbalg::to_vector(bbs);
	EXPECT_EQ(10, lvs[0]);
	EXPECT_EQ(64, lvs[1]);
	EXPECT_EQ(65, lvs[2]);


}

