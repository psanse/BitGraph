/**
* @file test_bbscan.cpp
* @brief Unit tests of the BBScan class
* @details Taken from the unit tests in test_bitstring.cpp (2014), filtering out other bitstring classes in the hierarchy
* @created 12/02/2025
* @author pss
**/

#include "bitscan/bbscan.h"
#include "bitscan/bbalg.h"
#include "gtest/gtest.h"
#include <iostream>
#include <set>

using namespace std;


class BBScanClassTest: public ::testing::Test{
protected:
	BBScanClassTest():bbn(301), bbi(301){}
	virtual void SetUp(){
	  for(int i = 0; i <= 300; i += 50){
		  bbn.set_bit(i);
		  bbi.set_bit(i);
		  sol.insert(i); 
	  }
	}

//////////////////////
//data members
	BitSet bbn;
	BBScan bbi;
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
	bbi.init_scan(BBObject::NON_DESTRUCTIVE);
	while (true) {
		nBit = bbi.next_bit();
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
	bbi.init_scan(50, BBObject::NON_DESTRUCTIVE);
	while (true) {
		nBit = bbi.next_bit();
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
	bbi.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while (true) {
		nBit = bbi.prev_bit();
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
	bbi.init_scan(50, BBObject::NON_DESTRUCTIVE_REVERSE);
	while (true) {
		nBit = bbi.prev_bit();
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
	BBScan bbi1(bbi);
	bbi1.init_scan(BBObject::DESTRUCTIVE);
	while (true) {
		nBit = bbi1.next_bit_del();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbi1.size());

	
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
	BBScan bbi1(bbi);
	bbi1.init_scan(BBObject::DESTRUCTIVE_REVERSE);
	while (true) {
		nBit = bbi1.prev_bit_del();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbi1.size());
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


TEST(BBScanClass, algorithms) {
//simple test for algorithms in bbalg.h

	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	////get_first_k_bits
	//BITBOARD b8=bb.get_bitstring()[0];
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 1)), 1);
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 2)), 2);
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 3)), 0);
}

TEST(BBScanClass, population_count){
	
	BBScan bbi(130);
	bbi.set_bit(10);
	bbi.set_bit(20);
	bbi.set_bit(64);

	//no range
	EXPECT_EQ(3, bbi.size());
	
	//[firstBit, endOfBitset)
	EXPECT_EQ(2, bbi.size(11));
	EXPECT_EQ(1, bbi.size(21));
	EXPECT_EQ(0, bbi.size(65));
	EXPECT_EQ(1, bbi.size(64));

	//[firstBit, lastBit]
	EXPECT_EQ(1, bbi.size(0, 10));
	EXPECT_EQ(1, bbi.size(20, 63));
	EXPECT_EQ(2, bbi.size(20, 64));
}

TEST(BBScanClass, to_vector){
	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	vector<int> sol;
	sol.push_back(10);  sol.push_back(20); sol.push_back(64);

	vector<int> vint;
	bb.to_vector(vint);
	EXPECT_EQ(sol, vint);

	
///////////////
// old vector interface (4/7/16)
	int v[130]; std::size_t size=0;
	for(int i=0; i<130; i++){
		v[i]=EMPTY_ELEM;
	}
		
	bb.to_C_array(v, size);
	EXPECT_EQ(3, size);
	copy(v, v+size, vint.begin());
	EXPECT_EQ(sol, vint);

	//BBScan redefinition
	BBScan bbi(130);
	bbi.set_bit(10);
	bbi.set_bit(20);
	bbi.set_bit(64);
	bbi.to_C_array(v,size, false);
	EXPECT_EQ(3, size);
	copy(v, v+size, vint.begin());
	EXPECT_EQ(3, vint.size());
	EXPECT_EQ(sol, vint);
		
}


TEST(BBScanClass, GenRandom){
///////////
// deterministic test, unless the seed is changed 
// date: 2/6/14

	//srand(time(NULL));
	vector<double> vd;
	const int NUM_TRIES=100;
	const double DENSITY=.7;
	const double TOLERANCE=.05;

	for(int i=0; i<NUM_TRIES; i++){
		BITBOARD bb=gen_random_bitboard(DENSITY);  //random bits
		vd.push_back(bblock::popc64(bb)/(double)WORD_SIZE);
	}

	//average value
	double sum_of_elems=0;
	for(vector<double>::iterator j=vd.begin();j!=vd.end();j++){
		sum_of_elems += *j;
	}
	double av_of_elems=sum_of_elems/NUM_TRIES;
	EXPECT_TRUE(abs(av_of_elems-DENSITY)<TOLERANCE);
}


class BBScanClassTest_1 : public ::testing::Test {
protected:
	virtual void SetUp() {
		int aux[] = {4,8,15,16,23,42};
		val.assign (aux,aux+6);
		bbn.init(45,val);
		bbi.init(45,val);
	}

	vector<int> val;
	BitSet bbn;
	BBScan bbi;	
};

TEST_F(BBScanClassTest_1, miscellanous){
	EXPECT_EQ(bbn.size(),bbi.size());
	EXPECT_EQ(to_vector(bbn), to_vector(bbi));
}


TEST(BBScanClass,is_singleton){
///////////////////////
// Determines if there is 0 or 1 bit in a range,
// both included
 
	BitSet bb(130);
	bb.set_bit(62); bb.set_bit(63); bb.set_bit(64);
	EXPECT_EQ(0,bb.is_singleton(10, 20));
	EXPECT_EQ(1,bb.is_singleton(60, 62));
	EXPECT_EQ(-1,bb.is_singleton(62, 64));
	EXPECT_NE(0,bb.is_singleton(64, 65));
	EXPECT_EQ(-1, bb.is_singleton(63, 64));
	EXPECT_EQ(1, bb.is_singleton(64, 65));

	bb.erase_bit(64);
	EXPECT_EQ(0, bb.is_singleton(64, 65));

}

TEST(BBScanClass, init_scan_specific){
//added specifically because some doubts were creeping during CSP implementation
//

	BBScan bbi(100);
	bbi.set_bit(10);
	bbi.set_bit(50);
	bbi.set_bit(64);
	bbi.init_scan(10, bbo::NON_DESTRUCTIVE);

	BBScan bbres(100);
	while(true){
		int v=bbi.next_bit();
		if(v==EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_FALSE(bbres.is_bit(10));
	EXPECT_TRUE(bbres.is_bit(50));
	EXPECT_TRUE(bbres.is_bit(64));
	EXPECT_EQ(2,bbres.size());

	//scan from the beginning
	bbres.erase_bit();
	bbi.set_bit(0);
	bbi.init_scan(EMPTY_ELEM, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan(0, ..) is not the same */
	while(true){
		int v=bbi.next_bit();
		if(v==EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_EQ(4,bbres.size());
	EXPECT_TRUE(bbres.is_bit(0));
	EXPECT_TRUE(bbres.is_bit(10));

	//incorrect scan from the beginning
	bbres.erase_bit();
	bbi.init_scan(0, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan(0, ..) is not the same */
	while(true){
		int v=bbi.next_bit();
		if(v==EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_FALSE(bbres.is_bit(0));
}


TEST_F(BBScanClassTest, nested_implementation) {

	BitSet bb(65);
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);
	

	int bit = BBObject::noBit;
	std::vector<int> lbits;
	std::vector<int> lbits_exp;

	//direct scanning

	BitSet::scan sc1(bb);
	bit = BitSet::noBit;

	while ((bit = sc1.next_bit()) != BitSet::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 0, 1, 64 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//direct reverse scanning
	BitSet::scanR sc2(bb);
	bit = BitSet::noBit;

	lbits.clear();
	while ((bit = sc2.next_bit()) != BitSet::noBit) {
		lbits.emplace_back(bit);
	}

	//////////////////////////////
	lbits_exp = { 64, 1, 0 };
	EXPECT_EQ(lbits_exp, lbits);
	//////////////////////////////

	//destructive scanning
	BitSet::scanD sc3(bb);
	bit = BitSet::noBit;

	lbits.clear();
	while ((bit = sc3.next_bit()) != BitSet::noBit) {
		lbits.emplace_back(bit);
	}

	///////////////////////////////
	lbits_exp = { 0, 1, 64 };
	EXPECT_EQ(lbits_exp, lbits);
	EXPECT_TRUE(bb.is_empty());
	////////////////////////////////

	//restores original bitset
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);

	//destructive reverse scanning
	BitSet::scanDR sc4(bb);
	bit = BitSet::noBit;

	lbits.clear();
	while ((bit = sc4.next_bit()) != BitSet::noBit) {
		lbits.emplace_back(bit);
	}

	///////////////////////////////
	lbits_exp = { 64, 1, 0 };
	EXPECT_EQ(lbits_exp, lbits);
	EXPECT_TRUE(bb.is_empty());
	////////////////////////////////

}



