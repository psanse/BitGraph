/**
* @file test_bbscan.cpp
* @brief Unit tests of the BBScan class - only related to non-nested bitscanning
* @details Taken from the unit tests in test_bitstring.cpp (2014), filtering out other bitstring classes in the hierarchy
* @created 12/02/2025
* @author pss
**/

#include "bitscan/bbscan.h"
#include "bitscan/bbalgorithm.h"
#include "gtest/gtest.h"
#include <iostream>
#include <set>

using namespace std;


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

	int nBit = EMPTY_ELEM;
	while (true) {
		nBit = bbn.next_bit(nBit);
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

	res.clear();
	bbsc.init_scan(BBObject::NON_DESTRUCTIVE);
	while (true) {
		nBit = bbsc.next_bit();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	
}

TEST_F(BBScanClassTest, non_destructive_with_starting_point) {
	std::set<int> res;

	int nBit = 50;
	while (true) {
		nBit = bbn.next_bit(nBit);
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_EQ(5, res.size());
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbsc.init_scan(50, BBObject::NON_DESTRUCTIVE);
	while (true) {
		nBit = bbsc.next_bit();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_EQ(5, res.size());
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));
	/////////////////////
	res.clear();

}

TEST_F(BBScanClassTest, reverse_non_destructive) {
	std::set<int> res;

	int nBit = EMPTY_ELEM;
	while (true) {
		nBit = bbn.prev_bit(nBit);
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

	res.clear();
	bbsc.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while (true) {
		nBit = bbsc.prev_bit();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	/////////////////////
	
}

TEST_F(BBScanClassTest, reverse_non_destructive_with_starting_point) {

	std::set<int> res;

	int nBit = 50;
	while (true) {
		nBit = bbn.prev_bit(nBit);
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_EQ(1, res.size());
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbsc.init_scan(50, BBObject::NON_DESTRUCTIVE_REVERSE);
	while (true) {
		nBit = bbsc.prev_bit();
		if (nBit == EMPTY_ELEM) break;
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
	while (true) {
		nBit = bbsc1.next_bit_del();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbsc1.size());

}

TEST_F(BBScanClassTest, reverse_destructive) {

	std::set<int> res;

	BitSet bbn1(bbn);
	int nBit = EMPTY_ELEM;
	while (true) {
		nBit = bbn1.prev_bit(nBit);
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
		bbn1.erase_bit(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbn1.size());

	//intrinsic
	res.clear();
	BBScan bbsc1(bbsc);
	bbsc1.init_scan(BBObject::DESTRUCTIVE_REVERSE);
	while (true) {
		nBit = bbsc1.prev_bit_del();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbsc1.size());
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
	EXPECT_EQ(1,bb.number_of_bitblocks());

	//copy constructor
	BitSet bb2(bb);
	EXPECT_TRUE(bb2.is_bit(22));
	EXPECT_TRUE(bb2.is_bit(23));
	EXPECT_EQ(1,bb2.number_of_bitblocks());

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
	EXPECT_TRUE(bb1.size(4, 129));
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
	while (true) {
		int v = bbsc.next_bit();
		if (v == EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_FALSE(bbres.is_bit(10));
	EXPECT_TRUE(bbres.is_bit(50));
	EXPECT_TRUE(bbres.is_bit(64));
	EXPECT_EQ(2, bbres.size());

	//scan from the beginning
	bbres.erase_bit();
	bbsc.set_bit(0);
	bbsc.init_scan(EMPTY_ELEM, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan(0, ..) is not the same */
	while (true) {
		int v = bbsc.next_bit();
		if (v == EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_EQ(4, bbres.size());
	EXPECT_TRUE(bbres.is_bit(0));
	EXPECT_TRUE(bbres.is_bit(10));

	//incorrect scan from the beginning
	bbres.erase_bit();
	bbsc.init_scan(0, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan(0, ..) is not the same */
	while (true) {
		int v = bbsc.next_bit();
		if (v == EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_FALSE(bbres.is_bit(0));
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
	EXPECT_EQ(bbn.size(),bbsc.size());
	EXPECT_EQ(to_vector(bbn), to_vector(bbsc));
}


