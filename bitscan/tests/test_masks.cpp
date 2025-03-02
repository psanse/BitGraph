/**
* @file test_masks.cpp
* @brief unit tests for (logic) masks for sparse and non-sparse graphs
* @created 17/12/2015, last_update 14/02/2025
* @authos pss
* 
* TODO- CHECK BEHAVIOUR OF AND_stateless tests,  change to closed range of bits (12/02/2025)
* TODO- CHECK DISABLED sparse tests - first refactor sparse bitset classes (13/02/2025)
**/

#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>

#include "bitscan/bitscan.h"					//bit string library - REMOVE DEPENDENCY (12/02/2025)
#include "gtest/gtest.h"

using namespace std;

class MasksTest : public ::testing::Test {
protected:
	MasksTest() : bb(130), bb1(130) {}
	virtual void SetUp() {
		bb.set_bit(10);
		bb.set_bit(20);
		bb.set_bit(63);

		bb1.set_bit(10);
		bb1.set_bit(64);
		bb1.set_bit(100);
	}

	//////////////////////
	//data members
	simple_bitarray bb;
	simple_bitarray bb1;
	
};

class MasksTest_1 : public ::testing::Test {
protected:
	MasksTest_1() : bb(130), bb1(130) {}
	virtual void SetUp() {
		bb.set_bit(10);
		bb.set_bit(20);
		bb.set_bit(64);

		bb1.set_bit(10);
		bb1.set_bit(64);
		bb1.set_bit(100);
	}

	//////////////////////
	//data members
	simple_bitarray bb;
	simple_bitarray bb1;

};

TEST_F(MasksTest, OR_range) {

	const int POPSIZE = 130;
	simple_bitarray res(POPSIZE);
		
	OR<true>(0, 65, bb, bb1, res);						// res = {10, 20, 63, 64}
	EXPECT_TRUE(res.is_bit(10));
	EXPECT_TRUE(res.is_bit(20));
	EXPECT_TRUE(res.is_bit(63));	
	EXPECT_TRUE(res.is_bit(64));
	EXPECT_EQ(4, res.size());
	
	//cleans res outside the range
	OR<true>(0, 100, bb, bb1, res);						// res = {10, 20, 63, 64, 100}		
	EXPECT_TRUE(res.is_bit(10));
	EXPECT_TRUE(res.is_bit(20));
	EXPECT_TRUE(res.is_bit(63));
	EXPECT_TRUE(res.is_bit(64));
	EXPECT_TRUE(res.is_bit(100));
	EXPECT_EQ(5, res.size());
	
	//cleans res outside the range
	OR<true>(12, POPSIZE-1, bb, bb1, res);				// res = { 20, 63, 64 100}
	EXPECT_FALSE(res.is_bit(10));
	EXPECT_TRUE(res.is_bit(20));
	EXPECT_TRUE(res.is_bit(63));
	EXPECT_TRUE(res.is_bit(64));
	EXPECT_TRUE(res.is_bit(100));
	EXPECT_EQ(4, res.size());

}


TEST_F(MasksTest, AND_OR) {

	simple_bitarray resAND(130);
	simple_bitarray resOR(130);
	
	
	///////////////////////
	AND(bb, bb1, resAND);
	//////////////////////

	EXPECT_TRUE	(resAND.is_bit(10));
	EXPECT_FALSE(resAND.is_bit(64));
	EXPECT_EQ	(1, resAND.size());


	///////////////////////
	OR(bb, bb1, resOR);
	//////////////////////

	EXPECT_EQ	(5, resOR.size());
	EXPECT_TRUE	(resOR.is_bit(10));
	EXPECT_TRUE	(resOR.is_bit(20));
	EXPECT_TRUE	(resOR.is_bit(63));
	EXPECT_TRUE	(resOR.is_bit(64));
	EXPECT_TRUE	(resOR.is_bit(100));

}

TEST_F(MasksTest_1, AND_stateless){

	bitarray resAND(130);
	
	AND<false>(0, 11, bb, bb1, resAND);				//resAND = {10, 64}
	EXPECT_TRUE	(resAND.is_bit(10));
	EXPECT_EQ	(1, resAND.size());

	//cleans resAND outside the range
	AND<true>(0, 10, bb, bb1, resAND);				//resAND = {10}
	EXPECT_TRUE	(resAND.is_bit(10));
	EXPECT_EQ	(1, resAND.size());

	//cleans resAND outside the range
	AND<true>(0, 63, bb, bb1, resAND);				//resAND = {10}
	EXPECT_FALSE(resAND.is_bit(64));
	EXPECT_EQ(1, resAND.size());

	//cleans resAND outside the range
	AND<true>(0, 64, bb, bb1, resAND);				//resAND = {10, 64}
	EXPECT_TRUE (resAND.is_bit(64));
	EXPECT_TRUE (resAND.is_bit(10));
	EXPECT_EQ	(2, resAND.size());
		
	//cleans resAND outside the range				
	AND<true>(0, 5, bb, bb1, resAND);				//resAND = {0}	
	EXPECT_TRUE	 (resAND.is_empty());
}

//TEST_F(MasksTest_1, AND_stateless_2_vertex_set){
//
//// Note: AND works in a half open range (excludes limiting bit)
//// CHANGE!
//	
//	int v[130];
//	int size = 0;
//
//	//AND
//	AND(11, bb, bb1,  v, size);			//v[0]=10;
//	vector<int> vset(v, v+size);
//	EXPECT_TRUE(find(vset.begin(), vset.end(), 10)!=vset.end());
//	EXPECT_FALSE(find(vset.begin(), vset.end(), 64)!=vset.end());
//	EXPECT_EQ(1, size);
//	
//	AND(10, bb, bb1,  v, size);			//v=[];
//	EXPECT_EQ(0, size);
//
//	AND(65, bb, bb1, v,size);			//v={10,64}
//	vector<int> vset2(v, v+size);
//	EXPECT_TRUE(find(vset2.begin(), vset2.end(), 64)!=vset2.end());
//
//}

TEST(Masks, DISABLED_AND_OR_sparse) {

	//non sparse
	sparse_bitarray bb(130);
	sparse_bitarray bb1(130);
	sparse_bitarray bbresAND(130);
	sparse_bitarray bbresOR(130);

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//AND
	AND(bb, bb1, bbresAND);
	EXPECT_TRUE(bbresAND.is_bit(10));
	EXPECT_TRUE(bbresAND.is_bit(64));
	EXPECT_EQ(2, bbresAND.size());

	//OR
	OR(bb, bb1, bbresOR);
	EXPECT_EQ(4, bbresOR.size());

	//&=
	sparse_bitarray bb2(130);
	sparse_bitarray bb3(130);

	bb2.set_bit(10);
	bb2.set_bit(20);
	bb2.set_bit(64);

	bb3.set_bit(10);
	bb3.set_bit(64);
	bb3.set_bit(100);

	bb2 &= bb3;
	EXPECT_TRUE(bb2.is_bit(10));
	EXPECT_TRUE(bb2.is_bit(64));
	EXPECT_EQ(2, bb2.size());

}

TEST(Masks, DISABLED_set_bits_sparse) {

	sparse_bitarray bb(130);
	sparse_bitarray bb1(130);
	sparse_bitarray bbres(130);

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//set range
	bb.set_block(0, WDIV(130) -1, bb1);
	EXPECT_TRUE(bb.is_bit(100));

	bb1.set_bit(129);
	bb.set_block(0, 1, bb1);
	EXPECT_FALSE(bb.is_bit(129));

	bb.set_block(0, 2, bb1);
	EXPECT_TRUE(bb.is_bit(129));

	//erase range
	bb.erase_block(2, WDIV(bb1.number_of_blocks()) -1,  bb1);
	EXPECT_FALSE(bb.is_bit(129));

	bb.erase_block(1, WDIV(bb1.number_of_blocks()) - 1, bb1);
	EXPECT_FALSE(bb.is_bit(100));
	EXPECT_FALSE(bb.is_bit(64));
		
	bb.erase_block(0, WDIV(bb1.number_of_blocks()) - 1, bb1);
	EXPECT_TRUE(bb.is_bit(20));
}


TEST(Masks, DISABLED_erase_block_sparse) {

	sparse_bitarray bb(130);
	sparse_bitarray bb1(130);
	sparse_bitarray bbres(130);

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	bb.erase_block(2, 2, bb1);		//nothing deleted
	EXPECT_EQ(3, bb.size());

	bb.erase_block(1, 1, bb1);		//nothing deleted
	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_FALSE(bb.is_bit(64));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_FALSE(bb.is_bit(100));

	bb.erase_block(0, 2, bb1);		//nothing deleted
	EXPECT_EQ(1, bb.size());
}

TEST(Masks, DISABLED_erase_bit_stateless) {

	//sparse
	sparse_bitarray bbs(130);
	sparse_bitarray bbs1(130);
	sparse_bitarray bbsERASE(130);


	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	bbs1.set_bit(10);
	bbs1.set_bit(64);
	bbs1.set_bit(100);

	//////////////////////////////	
	erase_bit(bbs, bbs1, bbsERASE);
	//////////////////////////////

	EXPECT_TRUE(bbsERASE.is_bit(20));
	EXPECT_FALSE(bbsERASE.is_bit(10));
	EXPECT_FALSE(bbsERASE.is_bit(64));
	EXPECT_EQ(1, bbsERASE.size());
}

TEST(Masks, DISABLED_ERASE_extreme_cases) {

	//sparse
	sparse_bitarray bbs(130);
	sparse_bitarray bbs1(130);			//empty
	sparse_bitarray bbsERASE(130);


	bbs.set_bit(10);
	bbs.set_bit(64);
	bbs.set_bit(100);


	//ERASE
	erase_bit(bbs, bbs1, bbsERASE);
	EXPECT_TRUE(bbs == bbsERASE);

	//erase when no blocks in same index: simple copy

	bbs1.erase_bit();
	bbs1.set_bit(100);

	bbs.print();
	bbs1.print();

	//ERASE
	erase_bit(bbs, bbs1, bbsERASE);
	EXPECT_TRUE(bbsERASE.is_bit(10));
	EXPECT_TRUE(bbsERASE.is_bit(64));
	EXPECT_EQ(2, bbsERASE.size());
}