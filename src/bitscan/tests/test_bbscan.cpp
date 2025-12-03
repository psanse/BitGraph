/**
* @file test_bbscan.cpp
* @brief Unit tests of the BBScan class - only related to non-nested bitscanning
* @details Taken from the unit tests in test_bitstring.cpp (2014), filtering out other bitstring classes in the hierarchy
* @created 12/02/2025, last_update 29/04/2025
* @author pss
**/

#include "bitscan/bbscan.h"
#include "bitscan/bbalgorithm.h"
#include "gtest/gtest.h"
#include <iostream>
#include <set>

using namespace std;
using namespace bitgraph;


class BBScanClassTest: public ::testing::Test{
protected:
	BBScanClassTest():bbn(301), bbsc(301){}
	virtual void SetUp(){
	  for(int i = 0; i <= 300; i += 50){
		  bbn.set_bit(i);
		  bbsc.set_bit(i);
		  sol.insert(i); 
	  }
	}

//////////////////////
//data members
	BitSet bbn;
	BBScan bbsc;
	set<int> sol;
};

TEST_F(BBScanClassTest, non_destructive) {
	std::set<int> res;

	int nBit = bbo::noBit;
	while ( (nBit = bbn.next_bit(nBit)) != bbo::noBit) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

	res.clear();
	bbsc.init_scan(bbo::NON_DESTRUCTIVE);
	while ( (nBit = bbsc.next_bit())!= bbo::noBit) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	
}

TEST_F(BBScanClassTest, non_destructive_with_starting_point) {
	std::set<int> res;

	int nBit = 50;
	while ( (nBit = bbn.next_bit(nBit))!= bbo::noBit ) {
		res.insert(nBit);
	}

	EXPECT_EQ(5, res.size());
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbsc.init_scan(50, BBObject::NON_DESTRUCTIVE);
	while ( (nBit = bbsc.next_bit()) != bbo::noBit ) {
		res.insert(nBit);
	}

	EXPECT_EQ(5, res.size());
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));
	/////////////////////
}

TEST_F(BBScanClassTest, reverse_non_destructive) {
	std::set<int> res;

	int nBit = bbo::noBit;
	while ( (nBit = bbn.prev_bit(nBit)) != bbo::noBit ) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

	res.clear();
	bbsc.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while ( (nBit = bbsc.prev_bit()) != bbo::noBit) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	/////////////////////
	
}

TEST_F(BBScanClassTest, reverse_non_destructive_with_starting_point) {

	std::set<int> res;

	int nBit = 50;
	while ( (nBit = bbn.prev_bit(nBit)) != bbo::noBit) {
		res.insert(nBit);
	}

	EXPECT_EQ(1, res.size());
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbsc.init_scan(50, BBObject::NON_DESTRUCTIVE_REVERSE);
	while ( (nBit = bbsc.prev_bit())!=bbo::noBit ) {
		res.insert(nBit);
	}

	EXPECT_EQ(1, res.size());
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));
	/////////////////////

}

TEST_F(BBScanClassTest, destructive) {
	std::set<int> res;
	int nBit = EMPTY_ELEM;

	//intrinsic
	res.clear();
	BBScan bbsc1(bbsc);
	bbsc1.init_scan(BBObject::DESTRUCTIVE);
	while ((nBit = bbsc1.next_bit_del()) != bbo::noBit) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbsc1.count());

}

TEST_F(BBScanClassTest, reverse_destructive) {

	std::set<int> res;

	BitSet bbn1(bbn);
	int nBit = bbo::noBit;
	while ( (nBit = bbn1.prev_bit(nBit)) != bbo::noBit) {
		res.insert(nBit);
		bbn1.erase_bit(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbn1.count());

	//intrinsic
	res.clear();
	BBScan bbsc1(bbsc);
	bbsc1.init_scan(BBObject::DESTRUCTIVE_REVERSE);
	while ( (nBit = bbsc1.prev_bit_del()) != bbo::noBit) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbsc1.count());
}

TEST(BBScanClass, setters_and_getters) {
	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_FALSE(bb.is_bit(63));

	//assignment
	BitSet bb1(34);
	bb1.set_bit(22);
	bb1.set_bit(23);
	bb=bb1;

	EXPECT_TRUE(bb.is_bit(22));
	EXPECT_TRUE(bb.is_bit(23));
	EXPECT_EQ(1,bb.num_blocks());

	//copy constructor
	BitSet bb2(bb);
	EXPECT_TRUE(bb2.is_bit(22));
	EXPECT_TRUE(bb2.is_bit(23));
	EXPECT_EQ(1,bb2.num_blocks());

}

TEST(BBScanClass, boolean_disjoint){

	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BitSet bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_disjoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));

	BitSet bb2(130);
	bb2.set_bit(11);				//in common in bb1 and bb2 but not bb
	bb2.set_bit(22);
	bb2.set_bit(66);
	EXPECT_TRUE(bb.is_disjoint(bb1, bb2));

	bb.set_bit(11);
	EXPECT_FALSE(bb.is_disjoint(bb1, bb2));
}

TEST(BBScanClass, set_bit_range){
	BitSet bb(130);
	bb.set_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(0));
	EXPECT_TRUE(bb.is_bit(64));
	
	BitSet bb1(130);
	bb1.set_bit(0, 0);
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(64, 64);
	EXPECT_TRUE(bb1.is_bit(64));
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(55, 56);
	EXPECT_TRUE(bb1.count(4, 129));
}

TEST(BBScanClass, erase_bit_range){

	BitSet bb(130);
	bb.set_bit(0, 129);

	bb.erase_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(65));
	EXPECT_FALSE(bb.is_bit(64));
	

	bb.erase_bit(115, 116);
	EXPECT_TRUE(bb.is_bit(114));
	EXPECT_FALSE(bb.is_bit(115));

}

TEST(BBScanClass, init_scan_specific) {

	BBScan bbsc(100);
	bbsc.set_bit(10);
	bbsc.set_bit(50);
	bbsc.set_bit(64);

	bbsc.init_scan(10, bbo::NON_DESTRUCTIVE);

	BBScan bbres(100);
	int nBit = bbo::noBit;
	while ( (nBit = bbsc.next_bit()) != bbo::noBit) {
		bbres.set_bit(nBit);
	}

	EXPECT_FALSE(bbres.is_bit(10));
	EXPECT_TRUE(bbres.is_bit(50));
	EXPECT_TRUE(bbres.is_bit(64));
	EXPECT_EQ(2, bbres.count());

	//scan from the beginning
	bbres.erase_bit();
	bbsc.set_bit(0);
	bbsc.init_scan(EMPTY_ELEM, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan(0, ..) is not the same */
	while ((nBit = bbsc.next_bit())!= bbo::noBit) {
		bbres.set_bit(nBit);
	}

	EXPECT_EQ(4, bbres.count());
	EXPECT_TRUE(bbres.is_bit(0));
	EXPECT_TRUE(bbres.is_bit(10));

	//incorrect scan from the beginning
	bbres.erase_bit();
	bbsc.init_scan(0, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan(0, ..) is not the same */
	while ((nBit = bbsc.next_bit()) != bbo::noBit) {
		bbres.set_bit(nBit);
	}

	EXPECT_FALSE(bbres.is_bit(0));
}

TEST(BBScanClass, make_BBScan) {
	
	BBScan bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	//helper
	BBScan bb_helper = bitgraph::make_BBScan(130, { 10,20,64 });
	EXPECT_EQ(bb, bb_helper);
}

TEST(BBScanClass, make_BBScan_Empty) {

	//helper
	BBScan bb_helper = bitgraph::make_BBScan(130);
	EXPECT_TRUE(bb_helper.is_empty());
}


TEST(BBScanClass, make_BBScan_Full) {

	//helper
	int nPop = 130;
	BBScan bb_helper = bitgraph::make_BBScan_full(nPop);

	EXPECT_EQ(bb_helper.count(), nPop);
	EXPECT_EQ(bb_helper.lsb(), 0);
	EXPECT_EQ(bb_helper.msb(), nPop - 1);

}


class BBScanClassTest_1 : public ::testing::Test {
protected:
	virtual void SetUp() {
		int aux[] = {4,8,15,16,23,42};
		val.assign (aux,aux+6);
		bbn.init(45,val);
		bbsc.init(45,val);
	}

	vector<int> val;
	BitSet bbn;
	BBScan bbsc;	
};

TEST_F(BBScanClassTest_1, miscellanous){
	EXPECT_EQ(bbn.count(),bbsc.count());
	EXPECT_EQ(bbalg::to_vector(bbn), bbalg::to_vector(bbsc));
}


