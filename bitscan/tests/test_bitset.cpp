/**
* @file test_bitset.cpp
* @brief Unit tests of the BitSet class
* @details Taken from the original test_bitstring.cpp (2014), filtering out other bitstring classes in the hierarchy
* @created 31/01/2025, last_update 13/02/2025
* @authos pss
**/

#include "bitscan/bbset.h"	
#include "bitscan/bbalgorithm.h"
#include "gtest/gtest.h"
#include <iostream>
#include <set>		

using namespace std;

class BitSetClassTest: public ::testing::Test{
protected:
	BitSetClassTest():bbn(301){}
	virtual void SetUp(){
	  for(auto i = 0; i <= 300; i += 50){
		  bbn.set_bit(i);
		  sol.insert(i); 
	  }
	}

//////////////////////
//data members
	BitSet bbn;
	set<int> sol;
};

class BitSetClassTest_1 : public ::testing::Test {
protected:
	BitSetClassTest_1() : bb(130), bb1(130) {}
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
	BitSet bb;
	BitSet bb1;

};



TEST_F(BitSetClassTest, non_destructive) {
	std::set<int> res;

	int nBit = EMPTY_ELEM;
	while ((nBit = bbn.next_bit(nBit)) != EMPTY_ELEM) {
			res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
}

TEST_F(BitSetClassTest, non_destructive_with_starting_point) {
	std::set<int> res;

	int nBit = 50;
	while ((nBit = bbn.next_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
	}

	EXPECT_EQ(5, res.size());
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));
}

TEST_F(BitSetClassTest, reverse_non_destructive) {
	std::set<int> res;

	int nBit = EMPTY_ELEM;
	while ((nBit = bbn.prev_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);
}

TEST_F(BitSetClassTest, reverse_non_destructive_with_starting_point) {
	std::set<int> res;

	int nBit = 50;
	while ((nBit = bbn.prev_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
	}

	EXPECT_EQ(1, res.size());
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));
}


TEST_F(BitSetClassTest, reverse_destructive) {
	std::set<int> res;

	BitSet bbN1(bbn);
	int nBit = EMPTY_ELEM;
	while ((nBit = bbN1.prev_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
		bbN1.erase_bit(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbN1.size());
}

TEST(BitSetClass, set_bit_basic) {
	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_FALSE(bb.is_bit(63));

	//erases all and sets bit 64
	bb.erase_bit();
	bb.set_bit(64);			
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_FALSE(bb.is_bit(10));
	EXPECT_FALSE(bb.is_bit(20));
	EXPECT_FALSE(bb.is_bit(63));

	//assignment
	BitSet bb1(34);
	bb1.set_bit(22);
	bb1.set_bit(23);
	bb = bb1;

	EXPECT_TRUE(bb.is_bit(22));
	EXPECT_TRUE(bb.is_bit(23));
	EXPECT_EQ(1, bb.number_of_blocks());

	//copy constructor
	BitSet bb2(bb);
	EXPECT_TRUE(bb2.is_bit(22));
	EXPECT_TRUE(bb2.is_bit(23));
	EXPECT_EQ(1, bb2.number_of_blocks());
}

TEST(BitSetClass, set_bit_range) {

	BitSet bb(130);

	bb.set_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(0));
	EXPECT_TRUE(bb.is_bit(64));

	//erases all and sets bit 64
	bb.erase_bit();
	bb.set_bit(63, 65);
	EXPECT_TRUE(bb.is_bit(63));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_TRUE(bb.is_bit(65));
	EXPECT_FALSE(bb.is_bit(0));
	EXPECT_FALSE(bb.is_bit(62));


	BitSet bb1(130);
	bb1.set_bit(0, 0);
	EXPECT_TRUE(bb1.is_bit(0));

	bb1.set_bit(64, 64);
	EXPECT_TRUE(bb1.is_bit(64));
	EXPECT_TRUE(bb1.is_bit(0));

	bb1.set_bit(55, 56);					//bb1={0...55 56...64}
	EXPECT_TRUE(bb1.size(4, 130));	
	EXPECT_TRUE(bb1.is_bit(0));
	EXPECT_TRUE(bb1.is_bit(55));
	EXPECT_TRUE(bb1.is_bit(56));
	EXPECT_TRUE(bb1.is_bit(64));

	//same range
	BitSet bb2(130);
	bb2.set_bit(5, 5);
	EXPECT_EQ(1, bb2.size());
	EXPECT_TRUE(bb2.is_bit(5));
}


TEST(BitSetClass, set_bit_from_another_bitstring) {

	BitSet bb(130);
	BitSet bb3(130);
	bb3.set_bit(50);
	bb3.set_bit(80);
	
	//copies [0, 79] bits from bb3 to bb	
	bb.set_bit(79, bb3);						//bb={50}

	EXPECT_TRUE(bb.is_bit(50));
	EXPECT_FALSE(bb.is_bit(80));
	EXPECT_EQ(1, bb.size());
}

TEST(BitSetClass, boolean_disjoint){

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

TEST(BitSetClass, find_set_difference){

	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BitSet bb1(130);
	bb1.set_bit(10);
	bb1.set_bit(20);
	bb1.set_bit(64);

	int bit1 = EMPTY_ELEM;
	int bit2 = EMPTY_ELEM;	

	//empty set difference 
	//bb1 is contained in bb (since bb1 = bb)
	EXPECT_EQ(0, bb.find_diff_singleton(bb1, bit1));		
	EXPECT_EQ(EMPTY_ELEM, bit1);
		
	//set difference is the singleton {65}
	bb.set_bit(65);
	EXPECT_EQ(1, bb.find_diff_singleton(bb1, bit1));
	EXPECT_EQ(65, bit1);

	//set difference is the pair {65, 123}
	bb.set_bit(123);
	EXPECT_EQ(2, bb.find_diff_pair(bb1, bit1, bit2));
	EXPECT_EQ(65, bit1);
	EXPECT_EQ(123, bit2);

	//set difference is the triplet {65, 123, 125}
	//returns -1, bit1 and bit2 = EMPTY_ELEM
	bb.set_bit(125);
	EXPECT_EQ(EMPTY_ELEM, bb.find_diff_pair(bb1, bit1, bit2));
	EXPECT_EQ(EMPTY_ELEM, bit1);
	EXPECT_EQ(EMPTY_ELEM, bit2);
}

TEST(BitSetClass, erase_bit_range){
	BitSet bb(130);
	bb.set_bit(0, 129);				//all bits to 1

	bb.erase_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(65));
	EXPECT_FALSE(bb.is_bit(64));
	EXPECT_FALSE(bb.is_bit(0));

	bb.erase_bit(115, 116);
	EXPECT_TRUE(bb.is_bit(114));
	EXPECT_FALSE(bb.is_bit(115));

	//range till the end
	bb.erase_bit(126, -1);
	EXPECT_FALSE(bb.is_bit(129));
	EXPECT_FALSE(bb.is_bit(128));
	EXPECT_FALSE(bb.is_bit(127));
	EXPECT_FALSE(bb.is_bit(126));
	EXPECT_TRUE	(bb.is_bit(125));


	//same range
	BitSet bb1(130);
	bb.set_bit(0, 129);

	/////////////////////
	bb.erase_bit(5, 5);
	/////////////////////

	EXPECT_FALSE(bb.is_bit(5));
	EXPECT_EQ(129, bb.size());
}

TEST(BitSetClass, erase_bit_union){
/////////////
// erases the union of two sets from bitset caller

	BitSet bb(130);
	bb.set_bit(0, 129);

	BitSet bb1(130);
	bb1.set_bit(0, 63);

	BitSet bb2(130);
	bb2.set_bit(65, 129);
	
	//////////////////////////
	bb.erase_bit(bb1, bb2);				//bb={64}
	//////////////////////////

	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_EQ(1,bb.size());
}

TEST(BitSetClass, population_count){

	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_EQ(3, bb.size());
	EXPECT_EQ(2, bb.size(11, 130));
	EXPECT_EQ(1, bb.size(21, 130));
	EXPECT_EQ(0, bb.size(65, 130));
	EXPECT_EQ(1, bb.size(64, 130));

	//same bit index
	EXPECT_EQ(1, bb.size(10, 10));
	EXPECT_EQ(1, bb.size(20, 20));

}



TEST(BitSetClass, to_vector) {

	const int POPULATION_SIZE = 130;

	BitSet bb(POPULATION_SIZE);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	vector<int> lv_exp;
	lv_exp.emplace_back(10);
	lv_exp.emplace_back(20);
	lv_exp.emplace_back(64);

	//conversion to vector
	vector<int> lv;
	bb.to_vector(lv);

	////////////////////////////
	EXPECT_EQ(lv_exp, lv);
	///////////////////////////

///////////////
// C-array interface (4/7/16)

	int c_array[POPULATION_SIZE];
	for (auto i = 0; i < POPULATION_SIZE; ++i) {
		c_array[i] = EMPTY_ELEM;
	}

	//conversion to C-array
	std::size_t size = 0;
	bb.to_C_array(c_array, size);
	
	////////////////////////
	EXPECT_EQ(3, size);
	EXPECT_TRUE(std::find(std::begin(c_array), std::end(c_array), 10) != std::end(c_array)) ;
	EXPECT_TRUE(std::find(std::begin(c_array), std::end(c_array), 20) != std::end(c_array));
	EXPECT_TRUE(std::find(std::begin(c_array), std::end(c_array), 64) != std::end(c_array));
	/////////////////////////
}

TEST(BitSetClass, cast_to_vector) {

	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	//cast to vector
	vint lv = bb;	

	//expected lv
	vint lv_exp = { 10, 20, 64 };

	/////////////////////////
	EXPECT_EQ(lv_exp, lv);
	////////////////////////
}

TEST(BitSetClass, vector_operations) {

	const int POPULATION_SIZE = 50;

	BitSet bbN1(POPULATION_SIZE);
	vector<int> lv = {10, 20, 45, 62};

	bbN1.set_bit(lv[0]);
	bbN1.set_bit(lv[1]);
	bbN1.set_bit(lv[2]);
	bbN1.set_bit(lv[3]);

	////////////////////////
	//construction from vector (bitset with lv.size)
	BitSet bbN2(lv);

	vector<int> lvN1 = bbalg::to_vector(bbN1);
	vector<int> lvN2 = bbalg::to_vector(bbN2);

	EXPECT_EQ(lv, lvN1);
	EXPECT_EQ(lv, lvN2);
	
	///////////////
	//construction from vector and popsize (12/11/16)
	lv = {10, 20};
	BitSet bbN3(POPULATION_SIZE, lv);
	EXPECT_EQ(2,bbN3.size());
	EXPECT_TRUE(bbN3.is_bit(10));
	EXPECT_TRUE(bbN3.is_bit(20));

	//bitstring with population size 20 
	BitSet bbN4(21, lv);						//vector element 20 will not make part of the bitstring
	EXPECT_EQ(2, bbN4.size());
	EXPECT_TRUE(bbN4.is_bit(10));
	EXPECT_TRUE(bbN4.is_bit(20));

	////////////////////////
	//setting bits from vector
	lv = { 10, 19, 45, 62, 250 };
	
	//bbN5 = {10, 19, 45, 62}
	BitSet bbN5(100);						//max population size 100						
	bbN5.set_bit(lv);

	EXPECT_EQ(4, bbN5.size());
	EXPECT_TRUE(bbN5.is_bit(10));
	EXPECT_TRUE(bbN5.is_bit(19));
	EXPECT_TRUE(bbN5.is_bit(45));
	EXPECT_TRUE(bbN5.is_bit(62));
	EXPECT_FALSE(bbN5.is_bit(63));
		
	//I/O
	//bbN5.print();
}

TEST(BitSetClass, conversion_to_vector) {
	BitSet bb1(25);
	bb1.set_bit(0,24);

	//////////////////////////////
	EXPECT_EQ(25, bb1.size());
	//////////////////////////////

	vector<int> lv_exp(25,0);	
	for(auto i = 0; i < 25; ++i){
		lv_exp[i]=i;
	}

	////////////////////////////////////
	EXPECT_EQ(lv_exp, bbalg::to_vector(bb1));
	///////////////////////////////////

}

TEST(BitSetClass, GenRandom){
///////////
// deterministic test, unless the seed is changed 
// date: 2/6/14

	//srand(time(NULL));
	vector<double> vd;
	const int NUM_TRIES=100;
	const double DENSITY=.7;
	const double TOLERANCE=.05;

	for(auto i = 0; i < NUM_TRIES; ++i){
		BITBOARD bb = bbalg::gen_random_block(DENSITY);  //random bits
		vd.emplace_back(bblock::popc64(bb)/(double)WORD_SIZE);
	}

	//average value
	double sum_of_elems=0;
	for(vector<double>::iterator j=vd.begin();j!=vd.end();j++){
		sum_of_elems += *j;
	}
	double av_of_elems=sum_of_elems/NUM_TRIES;
	EXPECT_TRUE(abs(av_of_elems-DENSITY)<TOLERANCE);
}

TEST(BitSetClass, set_block){

	BitSet bb(130);
	bb.set_bit(0,54);
	BitSet bb1(130);
	bb1.set_bit(50,100);

	bb.erase_bit();
	bb.set_block(1, -1, bb1);			//second block
	EXPECT_FALSE(bb.is_bit(33));
	EXPECT_FALSE(bb.is_bit(50));
	EXPECT_TRUE(bb.is_bit(64));

	bb.erase_bit();
	bb.set_block(0, 0, bb1);	
	EXPECT_TRUE(bb.is_bit(50));
	EXPECT_TRUE(bb.is_bit(63));
	EXPECT_FALSE(bb.is_bit(64));
}

TEST(BitSetClass, erase_block) {
	
	BitSet bb(130);
	bb.set_bit(49, 54);
	BitSet bb1(130);
	bb1.set_bit(50, 100);
		
	bb.erase_block(0, -1, bb1);			//bb={49}
	EXPECT_FALSE(bb.is_bit(50));
	EXPECT_TRUE(bb.is_bit(49));
	EXPECT_EQ(1, bb.size());

}

TEST_F(BitSetClassTest_1, erase_block_2) {

	BitSet bbres(130);

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

TEST(BitSetClass, find_single_bit_intersection){

	BitSet bb(130);
	BitSet bb1(130);
	bb.set_bit(1,10);
	bb1.set_bit(10,20);
	int v = EMPTY_ELEM;

	////////////////////////////////////////////
	int res = bb.find_common_singleton(bb1, v);
	////////////////////////////////////////////

	EXPECT_EQ(10, v);
	EXPECT_EQ(1, res);
}


TEST(BitSetClass, is_singleton){

	BitSet bb(130);
	bb.set_bit(62);
	bb.set_bit(63);
	bb.set_bit(64);

	EXPECT_EQ(0,bb.is_singleton(10, 20));			//no 1-bit in the range
	EXPECT_EQ(1, bb.is_singleton(60, 62));
	EXPECT_EQ(-1, bb.is_singleton(62, 64));			//more than one 1-bit in the range
	EXPECT_NE(0, bb.is_singleton(64, 65));
	EXPECT_EQ(-1, bb.is_singleton(63, 64));			//more than one 1-bit in the range
	EXPECT_EQ(1, bb.is_singleton(64, 65));

	bb.erase_bit(64);
	EXPECT_EQ(0, bb.is_singleton(64, 65));			//no 1-bit in the range

}


TEST(BitSetClass, find_first_common_bit){
//////////////////////
// testing first element in common between sets 

	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	//assignment
	BitSet bb1(130);
	bb1.set_bit(22);
	bb1.set_bit(23);
	bb1.set_bit(64);
	bb1.set_bit(72);
	

	int ff=bb.find_first_common(bb1);
	EXPECT_EQ(64, ff);

	bb.erase_bit(64);
	ff=bb.find_first_common(bb1);
	EXPECT_EQ(EMPTY_ELEM, ff);

	bb.set_bit(72);
	ff=bb.find_first_common(bb1);
	EXPECT_EQ(72, ff);
}

TEST(BitSetClass, find_singleton) {
	
	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	int singleton_bit = EMPTY_ELEM;
	int retVal = -1;

	//tests {0, 15] - singleton 10
	retVal = bb.find_singleton(0, 15, singleton_bit);
	EXPECT_EQ(1, retVal);
	EXPECT_EQ(10, singleton_bit);

	//tests {21, 63] - empty
	retVal = bb.find_singleton(21, 63, singleton_bit);
	EXPECT_EQ(0, retVal);
	EXPECT_EQ(-1, singleton_bit);

	//tests {63, 64] - singleton 64
	retVal = bb.find_singleton(63, 64, singleton_bit);
	EXPECT_EQ(1, retVal);
	EXPECT_EQ(64, singleton_bit);
}

TEST(BitSetClass, IO) {

	BitSet bb(32);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(63);

	string s = bb.to_string();
	EXPECT_FALSE(s.empty());

	//I/O
	//bb.print(std::cout, true, true);
	//cout << s.c_str() << endl;
}

TEST(BitSetClass, AND) {

	BitSet bb(130);
	BitSet bb1(130);
	BitSet bbresAND(130);

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

	//AND bit-range [10, 63], bits to 0 outside the range
	AND<true>(10, 63, bb, bb1, bbresAND);				//bbresAND={10}
	EXPECT_TRUE(bbresAND.is_bit(10));
	EXPECT_EQ(1, bbresAND.size());
	
	//AND bit-range [10, 63], preserving bits outside the range
	bbresAND.set_bit(100);								//sets a bit outside the range [10, 63]
	AND<false> (10, 63, bb, bb1, bbresAND);				//bbresAND={10, 100}
	EXPECT_TRUE(bbresAND.is_bit(10));
	EXPECT_TRUE(bbresAND.is_bit(100));
	EXPECT_EQ(2, bbresAND.size());

}

TEST(BitSetClass, AND_by_blocks) {

	BitSet bb(130);
	BitSet bb1(130);

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);
	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//AND of third block - rest of bits are always removed
	BitSet bbresAND = AND_block(2, 2, bb, bb1);

	////////////////////////////////
	EXPECT_EQ(0, bbresAND.size());
	////////////////////////////////

	//bbresAND = {3}
	bbresAND.erase_bit();
	bbresAND.set_bit(3);

	//AND of third block - rest of bits NOT removed
	AND_block<false>(2, 2, bb, bb1, bbresAND);					//resAND = {3}, since the first two blocks remain the same
	EXPECT_EQ(1, bbresAND.size());
	EXPECT_TRUE(bbresAND.is_bit(3));

	//AND of third block - rest of bits are removed
	AND_block<true>(1, 2, bb, bb1, bbresAND);					//bbresAND={64}, 64 is the only bit in the range of blocks
	EXPECT_EQ(1, bbresAND.size());
	EXPECT_TRUE(bbresAND.is_bit(64));
}


TEST(BitSetClass, AND_with_allocation) {

	BitSet bb(130);
	BitSet bb1(130);	

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);
	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//AND
	BitSet bbresAND  =  AND(bb, bb1);
	EXPECT_TRUE(bbresAND.is_bit(10));
	EXPECT_TRUE(bbresAND.is_bit(64));
	EXPECT_EQ(2, bbresAND.size());
	EXPECT_EQ(3, bbresAND.capacity());									//capacity = number of bitblocks	
	EXPECT_EQ(3, bbresAND.number_of_blocks());

}

TEST(BitSetClass, OR) {

	BitSet bb(130);
	BitSet bb1(130);
	BitSet bbresOR(130);

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);
	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//OR
	OR(bb, bb1, bbresOR);
	EXPECT_TRUE(bbresOR.is_bit(10));
	EXPECT_TRUE(bbresOR.is_bit(20));
	EXPECT_TRUE(bbresOR.is_bit(64));
	EXPECT_TRUE(bbresOR.is_bit(100));
	EXPECT_EQ(4, bbresOR.size());

	//OR bit-range [10, 20], preserving bits outside the range							
	OR<false>(10, 20, bb, bb1, bbresOR);			//bbresOR={10, 20, 64, 100}		
	EXPECT_TRUE(bbresOR.is_bit(10));
	EXPECT_TRUE(bbresOR.is_bit(20));
	EXPECT_TRUE(bbresOR.is_bit(64));
	EXPECT_TRUE(bbresOR.is_bit(100));
	EXPECT_EQ(4, bbresOR.size());

	//OR bit-range [10, 20], setting to 0 bits outside the range
	OR<true>(10, 20, bb, bb1, bbresOR);				//bbresOR={10, 20}
	EXPECT_TRUE(bbresOR.is_bit(10));
	EXPECT_TRUE(bbresOR.is_bit(20));
	EXPECT_EQ(2, bbresOR.size());
}

TEST(BitSetClass, OR_by_blocks) {

	BitSet bb(130);
	BitSet bb1(130);

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);
	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//OR creating a new bitset - rest of bits are always removed
	BitSet bbresOR = OR_block(1, 1, bb, bb1);

	////////////////////////////////
	EXPECT_TRUE(bbresOR.is_bit(64));
	EXPECT_TRUE(bbresOR.is_bit(100));
	EXPECT_EQ(2, bbresOR.size());
	////////////////////////////////
		
	//adds bit 129
	bb.set_bit(129);

	//OR setting a bitset - rest of bits NOT removed
	OR_block<false>(2, 2, bb, bb1, bbresOR);					//bbresOR = {64, 100, 129}, since the first two blocks remain the same
	EXPECT_TRUE(bbresOR.is_bit(64));
	EXPECT_TRUE(bbresOR.is_bit(100));
	EXPECT_TRUE(bbresOR.is_bit(129));
	EXPECT_EQ(3, bbresOR.size());
	
	//OR setting a bitset - rest of bits ARE removed
	OR_block<true>(2, 2, bb, bb1, bbresOR);					//bbresAND={129}, {64, 100} are removed, since they are outside the block range
	EXPECT_TRUE(bbresOR.is_bit(129));
	EXPECT_EQ(1, bbresOR.size());
}

TEST(BitSetClass, set_bits) {

	BitSet bb(130);
	BitSet bb1(130);
	BitSet bbres(130);

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//set range
	bb.set_block(0, -1, bb1);
	EXPECT_TRUE(bb.is_bit(100));
	EXPECT_TRUE(bb.is_bit(20));

	bb1.set_bit(129);
	bb.set_block(0, 1, bb1);
	EXPECT_FALSE(bb.is_bit(129));

	bb.set_block(0, 2, bb1);
	EXPECT_TRUE(bb.is_bit(129));

	//erase range
	bb.erase_block(2, -1, bb1);
	EXPECT_FALSE(bb.is_bit(129));

	bb.erase_block(1, -1, bb1);
	EXPECT_FALSE(bb.is_bit(100));
	EXPECT_FALSE(bb.is_bit(64));

	bb.erase_block(0, -1, bb1);
	EXPECT_TRUE(bb.is_bit(20));
}


TEST_F(BitSetClassTest_1, erase_bit_stateless) {

	BitSet bbERASE(130);

	//////////////////////////////	
	erase_bit(bb, bb1, bbERASE);			//bb = {20}
	///////////////////////////////

	EXPECT_TRUE	(bbERASE.is_bit(20));
	EXPECT_EQ	(1, bbERASE.size());

}

TEST(BitSetClass, erase_bit_stateless) {
	
	BitSet bb(130);
	BitSet bb1(130);				//empty

	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BitSet bbERASE(130);
	bbERASE.set_bit(129);

	
	///////////////////////////////
	erase_bit(bb, bb1, bbERASE);		//erases empty bitset, result is bb
	///////////////////////////////

	EXPECT_TRUE(bb == bbERASE);
}

TEST_F(BitSetClassTest_1, operators) {

	/////////////
	bb &= bb1;			//bb={10, 64}
	/////////////

	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_EQ(2, bb.size());

	////////////////////////////////
	bb.AND_EQUAL_block(1, 1, bb1);		//bb={10, 64}
	////////////////////////////////

	EXPECT_TRUE	(bb.is_bit(10));
	EXPECT_TRUE	(bb.is_bit(64));
	EXPECT_EQ	(2, bb.size());

	/////////////
	bb |= bb1;			//bb={10, 64, 100}
	/////////////

	EXPECT_EQ(3, bb.size());
	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_TRUE(bb.is_bit(100));
	
}





////////////////////////
//
// DISABLED TESTS
//
///////////////////////

TEST(BitSetClass, DISABLED_algorithms) {
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


