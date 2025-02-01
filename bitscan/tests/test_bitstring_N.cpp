/**
* @file test_bitstring_N.cpp
* @brief Unit tests of the BitBoardN class
* @details Taken from the original test_bitstring.cpp (2014), filtering out other bitstring classes in the hierarchy
* @created 31/01/2025
* @authos pss
**/

#include "bitscan/bitboardn.h"	
#include "bitscan/bbalg.h"
#include "gtest/gtest.h"
#include <iostream>
#include <set>		

using namespace std;

class BBNScanningTest: public ::testing::Test{
protected:
	BBNScanningTest():bbn(301){}
	virtual void SetUp(){
	  for(auto i = 0; i <= 300; i += 50){
		  bbn.set_bit(i);
		  sol.insert(i); 
	  }
	}

//////////////////////
//data members
	BitBoardN bbn;
	set<int> sol;
};

TEST_F(BBNScanningTest, non_destructive) {
	std::set<int> res;

	int nBit = EMPTY_ELEM;
	while ((nBit = bbn.next_bit(nBit)) != EMPTY_ELEM) {
			res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

}

TEST_F(BBNScanningTest, non_destructive_with_starting_point) {
	std::set<int> res;

	int nBit = 50;
	while ((nBit = bbn.next_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
	}

	EXPECT_EQ(5, res.size());
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));


}

TEST_F(BBNScanningTest, reverse_non_destructive) {
	std::set<int> res;

	int nBit = EMPTY_ELEM;
	while ((nBit = bbn.previous_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
	}

	EXPECT_TRUE(res == sol);

}

TEST_F(BBNScanningTest, reverse_non_destructive_with_starting_point) {
	std::set<int> res;

	int nBit = 50;
	while ((nBit = bbn.previous_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
	}

	EXPECT_EQ(1, res.size());
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));


}

TEST_F(BBNScanningTest, destructive) {
	std::set<int> res;

	BitBoardN bbN1(bbn);
	int nBit = EMPTY_ELEM;
	while ((nBit = bbN1.next_bit_if_del(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
		bbN1.erase_bit(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbN1.popcn64());

}


TEST_F(BBNScanningTest, reverse_destructive) {
	std::set<int> res;

	BitBoardN bbN1(bbn);
	int nBit = EMPTY_ELEM;
	while ((nBit = bbN1.previous_bit(nBit)) != EMPTY_ELEM) {
		res.insert(nBit);
		bbN1.erase_bit(nBit);
	}

	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0, bbN1.popcn64());

}

TEST(BBNtest, setters_and_getters) {
	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_FALSE(bb.is_bit(63));

	//assignment
	BitBoardN bb1(34);
	bb1.set_bit(22);
	bb1.set_bit(23);
	bb = bb1;

	EXPECT_TRUE(bb.is_bit(22));
	EXPECT_TRUE(bb.is_bit(23));
	EXPECT_EQ(1,bb.number_of_bitblocks());

	//copy constructor
	BitBoardN bb2(bb);
	EXPECT_TRUE(bb2.is_bit(22));
	EXPECT_TRUE(bb2.is_bit(23));
	EXPECT_EQ(1,bb2.number_of_bitblocks());

}

TEST(BBNtest, boolean_disjoint){

	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BitBoardN bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_disjoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));

	BitBoardN bb2(130);
	bb2.set_bit(11);				//in common in bb1 and bb2 but not bb
	bb2.set_bit(22);
	bb2.set_bit(66);
	EXPECT_TRUE(bb.is_disjoint(bb1, bb2));

	bb.set_bit(11);
	EXPECT_FALSE(bb.is_disjoint(bb1, bb2));
}

TEST(BBNtest, boolean_joint){

	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BitBoardN bb1(130);
	bb1.set_bit(10);
	bb1.set_bit(20);
	bb1.set_bit(64);

	//single_joint
	int v;
	EXPECT_EQ(0, bb.single_joint(bb1,v));		//same set
	EXPECT_EQ(EMPTY_ELEM,v);
	
	bb.set_bit(65);
	bb.print();
	EXPECT_EQ(1, bb.single_joint(bb1,v));
	EXPECT_EQ(65,v);

	bb.set_bit(123);
	int w;
	EXPECT_EQ(2, bb.double_joint(bb1,v,w));
	EXPECT_EQ(65,v);
	EXPECT_EQ(123,w);

	//non-joint by more than 2 vertices
	bb.set_bit(125);
	EXPECT_EQ(EMPTY_ELEM, bb.double_joint(bb1,v,w));
	EXPECT_EQ(EMPTY_ELEM,v);
	EXPECT_EQ(EMPTY_ELEM,w);
}

TEST(BBNtest, set_bit_range){

	BitBoardN bb(130);
	bb.set_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(0));
	EXPECT_TRUE(bb.is_bit(64));
	
	BitBoardN bb1(130);
	bb1.set_bit(0, 0);
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(64, 64);
	EXPECT_TRUE(bb1.is_bit(64));
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(55, 56);
	EXPECT_TRUE(bb1.popcn64(4));
}

TEST(BBNtest, erase_bit_range){
	BitBoardN bb(130);
	bb.set_bit(0, 129);

	bb.erase_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(65));
	EXPECT_FALSE(bb.is_bit(64));
	

	bb.erase_bit(115, 116);
	EXPECT_TRUE(bb.is_bit(114));
	EXPECT_FALSE(bb.is_bit(115));

}

TEST(BBNtest, erase_bit_joint){
/////////////
// erases the union of two sets from bitset caller

	BitBoardN bb(130);
	bb.set_bit(0, 129);

	BitBoardN bb1(130);
	bb1.set_bit(0, 63);

	BitBoardN bb2(130);
	bb2.set_bit(65, 129);
	
	bb.erase_bit_joint(bb1, bb2);
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_EQ(1,bb.popcn64());
}


TEST(BBNtest, population_count){

	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_EQ(3, bb.popcn64());
	EXPECT_EQ(2, bb.popcn64(11));
	EXPECT_EQ(1, bb.popcn64(21));
	EXPECT_EQ(0, bb.popcn64(65));
	EXPECT_EQ(1, bb.popcn64(64));
}

TEST(BBNtest, to_vector) {

	const int POPULATION_SIZE = 130;

	BitBoardN bb(POPULATION_SIZE);
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

TEST(BBNtest, cast_to_vector) {

	BitBoardN bb(130);
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

TEST(BBNtest, vector_operations) {

	const int POPULATION_SIZE = 50;

	BitBoardN bbN1(POPULATION_SIZE);
	vector<int> lv = {10, 20, 45, 62};

	bbN1.set_bit(lv[0]);
	bbN1.set_bit(lv[1]);
	bbN1.set_bit(lv[2]);
	bbN1.set_bit(lv[3]);

	////////////////////////
	//construction from vector (bitset with lv.size)
	BitBoardN bbN2(lv);

	vector<int> lvN1 = to_vector(bbN1);
	vector<int> lvN2 = to_vector(bbN2);

	EXPECT_EQ(lv, lvN1);
	EXPECT_EQ(lv, lvN2);
	
	///////////////
	//construction from vector and popsize (12/11/16)
	lv = {10, 20};
	BitBoardN bbN3(lv, POPULATION_SIZE);
	EXPECT_EQ(2,bbN3.popcn64());
	EXPECT_TRUE(bbN3.is_bit(10));
	EXPECT_TRUE(bbN3.is_bit(20));

	//bitstring with population size 20 
	BitBoardN bbN4(lv, 20);						//vector element 20 will not make part of the bitstring
	EXPECT_EQ(1, bbN4.popcn64());
	EXPECT_TRUE(bbN4.is_bit(10));
	EXPECT_FALSE(bbN4.is_bit(20));

	////////////////////////
	//setting bits from vector
	lv = { 10, 20, 45, 62, 250 };
	
	//bbN5 = {10, 20, 45, 62}
	BitBoardN bbN5(100);						//max population size 100						
	bbN5.set_bit(lv);

	EXPECT_EQ(4, bbN5.popcn64());
	EXPECT_TRUE(bbN5.is_bit(10));
	EXPECT_TRUE(bbN5.is_bit(20));
	EXPECT_TRUE(bbN5.is_bit(45));
	EXPECT_TRUE(bbN5.is_bit(62));
	EXPECT_FALSE(bbN5.is_bit(63));

	//I/O
	//bbN5.print();
}

TEST(BBNtest, BitBoardNTo0) {
	BitBoardN bb1(25);
	BitBoardN bb2(bb1);

	bb1.set_bit();			//sets to ONE the whole bitblock
	bb2.set_bit(0,24);

	vector<int> v1(25,0);
	
	for(auto i = 0; i < 25; ++i){
		v1[i]=i;
	}

	EXPECT_LT(v1, to_vector(bb1));
	EXPECT_LT(25, bb1.popcn64());
	EXPECT_EQ(to_vector(bb2), v1);
	EXPECT_EQ(25, bb2.popcn64());

}

TEST(BBNtest, GenRandom){
///////////
// deterministic test, unless the seed is changed 
// date: 2/6/14

	//srand(time(NULL));
	vector<double> vd;
	const int NUM_TRIES=100;
	const double DENSITY=.7;
	const double TOLERANCE=.05;

	for(auto i = 0; i < NUM_TRIES; ++i){
		BITBOARD bb = gen_random_bitboard(DENSITY);  //random bits
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


TEST(BBNtest, init_bit_range){
//date: 21/12/14
	BitBoardN bb(130);
	bb.init_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(0));
	EXPECT_TRUE(bb.is_bit(64));
	

	BitBoardN bb1(130);
	bb1.init_bit(0, 0);
	EXPECT_TRUE(bb1.is_bit(0));
	EXPECT_EQ(1,bb1.popcn64());
	

	bb1.init_bit(64, 64);
	EXPECT_TRUE(bb1.is_bit(64));
	EXPECT_EQ(1,bb1.popcn64());
	

	bb1.init_bit(55, 56);
	EXPECT_TRUE(bb1.is_bit(55));
	EXPECT_TRUE(bb1.is_bit(56));
	EXPECT_EQ(2, bb1.popcn64());


	BitBoardN bb2(130);
	bb2.init_bit(43);
	bb2.init_bit(44);
	bb2.init_bit(129);					//this is the only one that counts
	EXPECT_TRUE(bb2.is_bit(129));
	EXPECT_EQ(1, bb2.popcn64());

//copy a bitstring in range (change names)
	BitBoardN bb3(130);
	bb3.set_bit(50);
	bb3.set_bit(80);

	bb.init_bit(79,bb3);
	EXPECT_TRUE(bb.is_bit(50));
	EXPECT_FALSE(bb.is_bit(80));
	EXPECT_EQ(1, bb.popcn64());
	
	bb.init_bit(80,bb3);
	EXPECT_EQ(2, bb.popcn64());

	bb.init_bit(80,bb3);
	EXPECT_EQ(2, bb.popcn64());

	bb.erase_bit();
	bb.init_bit(49,bb3);
	EXPECT_TRUE(bb.is_empty());

}

TEST(BBNtest, block_copying){
//date: 21/12/14
	BitBoardN bb(130);
	bb.init_bit(0,54);
	BitBoardN bb1(130);
	bb1.init_bit(50,100);

	bb.erase_bit();
	bb.copy_from_block(1,bb1);			//second block
	EXPECT_FALSE(bb.is_bit(33));
	EXPECT_FALSE(bb.is_bit(50));
	EXPECT_TRUE(bb.is_bit(64));

	bb.erase_bit();
	bb.copy_up_to_block(0,bb1);	
	EXPECT_TRUE(bb.is_bit(50));
	EXPECT_TRUE(bb.is_bit(63));
	EXPECT_FALSE(bb.is_bit(64));
}

TEST(BBNtest, single_disjoint){
	BitBoardN bb(130);
	BitBoardN bb1(130);
	bb.set_bit(1,10);
	bb1.set_bit(10,20);
	int v = EMPTY_ELEM;
	int res = bb.single_disjoint(bb1, v);

	EXPECT_EQ(10, v);
	EXPECT_EQ(1, res);
}


TEST(BBNtest,is_singleton){
///////////////////////
// Determines if there is 0 or 1 bit in a range,
// both included
 
	BitBoardN bb(130);
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

TEST(BBNtest, first_found){
//////////////////////
// testing first element in common between sets 

	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	//assignment
	BitBoardN bb1(34);
	bb1.set_bit(22);
	bb1.set_bit(23);
	bb1.set_bit(64);
	bb1.set_bit(72);

	int ff=bb.first_found(bb1);
	EXPECT_EQ(64, ff);

	bb.erase_bit(64);
	ff=bb.first_found(bb1);
	EXPECT_EQ(EMPTY_ELEM, ff);

	bb.set_bit(72);
	ff=bb.first_found(bb1);
	EXPECT_EQ(72, ff);
}


TEST(BBNtest, IO) {

	BitBoardN bb(32);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(63);

	string s = bb.to_string();
	EXPECT_FALSE(s.empty());

	//I/O
	//bb.print(std::cout, true, true);
	//cout << s.c_str() << endl;
}



////////////////////////
//
// DISABLED TESTS
//
///////////////////////

TEST(BBNtest, DISABLED_algorithms) {
	//simple test for algorithms in bbalg.h

	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	////get_first_k_bits
	//BITBOARD b8=bb.get_bitstring()[0];
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 1)), 1);
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 2)), 2);
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 3)), 0);
}


