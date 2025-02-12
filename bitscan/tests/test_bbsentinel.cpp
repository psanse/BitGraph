/**
* @file test_bbsentinel.cpp
* @brief Unit tests of the BBSentinel class
* @details Taken from the unit tests in test_bitstring.cpp (2014), filtering out other bitstring classes in the hierarchy
* @created 12/02/2025
* @author pss
* 
* TODO - refactor BBSentinel class and then refactor these tests
**/

#include "bitscan/bbsentinel.h"
#include "gtest/gtest.h"
#include <iostream>
#include <set>

using namespace std;

class BBSentinelClassTest : public ::testing::Test{
protected:
	BBSentinelClassTest():bbn(301), bbs(301){}
	virtual void SetUp(){
	  for(int i = 0; i <= 300; i += 50){
		  bbn.set_bit(i);
		  bbs.set_bit(i);
		  sol.insert(i); 
	  }
	}

//////////////////////
//data members
	BitSet bbn;
	BBSentinel bbs;
	set<int> sol;
};

TEST_F(BBSentinelClassTest, non_destructive) {
	std::set<int> res;

	int nBit = EMPTY_ELEM;
	while (true) {
		nBit = bbn.next_bit(nBit);
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

	res.clear();

	if (bbs.init_scan(BBObject::NON_DESTRUCTIVE) != EMPTY_ELEM) {	//it is necessary to check if the bitstring is empty
		while (true) {
			nBit = bbs.next_bit();
			if (nBit == EMPTY_ELEM) break;
			res.insert(nBit);
		}
		EXPECT_TRUE(res == sol);
	}
}

TEST_F(BBSentinelClassTest, DISABLED_non_destructive_with_starting_point) {
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

	//bbs.init_scan(50, BBObject::NON_DESTRUCTIVE);
	//	while(true){
	//		nBit=bbs.next_bit();
	//		if(nBit==EMPTY_ELEM) break;
	//		res.insert(nBit);
	//	}

	//EXPECT_EQ(5, res.size() );
	//EXPECT_EQ(1, res.count(100));
	//EXPECT_EQ(0, res.count(50));

}

TEST_F(BBSentinelClassTest, reverse_non_destructive) {
	std::set<int> res;

	int nBit = EMPTY_ELEM;
	while (true) {
		nBit = bbn.prev_bit(nBit);
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

	
	/////////////////////
	res.clear();
	bbs.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while (true) {
		nBit = bbs.prev_bit();
		if (nBit == EMPTY_ELEM) break;
		res.insert(nBit);
	}
	// CTG: eliminar salida a pantalla de los test
	//for(set<int>::iterator it=res.begin(); it!=res.end(); ++it){
	//	cout<<*it<<" ";
	//}
	//cout<<"----------------------------------------------"<<endl;

	EXPECT_TRUE(res == sol);
}

TEST_F(BBSentinelClassTest, DISABLED_reverse_non_destructive_with_starting_point) {
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

	/////////////////////
	res.clear();

	/*bbs.init_scan(50, BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbs.prev_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);
	}

	EXPECT_EQ(1, res.size() );
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));*/

}

TEST_F(BBSentinelClassTest, destructive) {
	std::set<int> res;
	int nBit = EMPTY_ELEM;


	//sentinels
	res.clear();
	BBSentinel bbs1(bbs);
	if (bbs1.init_scan(BBObject::DESTRUCTIVE) != EMPTY_ELEM) {
		while (true) {
			nBit = bbs1.next_bit_del();
			if (nBit == EMPTY_ELEM) break;
			res.insert(nBit);
		}
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbs1.size());
}

TEST_F(BBSentinelClassTest, reverse_destructive) {
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


	//sentinels
	res.clear();
	BBSentinel bbs1(bbs);
	if (bbs1.init_scan(BBObject::DESTRUCTIVE_REVERSE) != EMPTY_ELEM) {
		while (true) {
			nBit = bbs1.prev_bit_del();
			if (nBit == EMPTY_ELEM) break;
			res.insert(nBit);
		}
	}
	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbs1.size());
}

TEST(BBSentinelClass, to_vector){
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

}

TEST(BBSentinelClass, GenRandom){
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

TEST(BBSentinelClass, basic) {

	BBSentinel bbs(200);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(150);

	bbs.update_sentinels();
	EXPECT_EQ(0, bbs.get_sentinel_L());
	EXPECT_EQ(2, bbs.get_sentinel_H());
	EXPECT_EQ(3, bbs.popcn64());


	bbs.erase_bit(20);
	bbs.erase_bit(150);
	bbs.print();

	bbs.update_sentinels();
	EXPECT_EQ(0, bbs.get_sentinel_L());
	EXPECT_EQ(0, bbs.get_sentinel_H());

	//redefiniton of equality (same sentinels and bitblocks in sentinel range)
	BBSentinel bbs1(200);
	bbs1 = bbs;
	EXPECT_EQ(bbs.get_sentinel_L(), bbs1.get_sentinel_L());
	EXPECT_EQ(bbs.get_sentinel_H(), bbs1.get_sentinel_H());
	EXPECT_TRUE(bbs1.is_bit(10));

	//bit deletion
	bbs1.erase_bit();									//in sentinel range
	EXPECT_EQ(EMPTY_ELEM, bbs1.update_sentinels());
	bbs1.init_sentinels(true);
	EXPECT_EQ(EMPTY_ELEM, bbs1.get_sentinel_L());
	EXPECT_EQ(EMPTY_ELEM, bbs1.get_sentinel_H());

	//AND
	bbs1 = bbs;
	bbs1.set_bit(27);
	BBSentinel bbs2(200);
	AND(bbs1, bbs, bbs2);
	EXPECT_TRUE(bbs2.is_bit(10));
	bbs2.update_sentinels();
	EXPECT_EQ(1, bbs2.popcn64());

	//erase_bit and update
	bbs.init_sentinels();
	bbs.erase_bit();
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(150);
	bbs.update_sentinels();
	bbs.erase_bit_and_update(150);						//erases and updates sentinels		
	EXPECT_EQ(0, bbs.get_sentinel_L());
	EXPECT_EQ(0, bbs.get_sentinel_H());

}

	

class BBSentinelClassTest_1 : public ::testing::Test {
protected:
	virtual void SetUp() {
		int aux[] = {4,8,15,16,23,42};
		val.assign (aux,aux+6);
		bbn.init(45,val);	
		bbs.init(45,val);
	}

	vector<int> val;
	BitSet bbn;
	BBSentinel bbs;
};

TEST_F(BBSentinelClassTest_1, miscellanous){
	
	bbs.init_sentinels(true);
	EXPECT_EQ(bbn.size(),bbs.size());	
	EXPECT_EQ(to_vector(bbn), to_vector(bbs));
}









