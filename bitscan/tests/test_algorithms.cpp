/**
* @file test_algorithms.cpp
* @brief Unit tests for the bitstring algorithms and data strcutures in bbalgorithm.h
* @details created 28/3/17, last_update 14/02/2025
* @detials added while working on the MWCP algorithm
* @author pss
**/

#include "bitscan/bbalgorithm.h"				
#include "bitscan/bbscan.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

//aliases
using vint = vector<int>;

TEST(stack_type, basic){
	
	sbb_t<BBScan> s;
	s.init(65);
	s.push(10);
	s.push(64);
	s.push(65);

	EXPECT_EQ(3, s.get_size());
	EXPECT_EQ(true, s.is_synchro());
	//s.print();

	//does not increase size
	s.push(65);
	EXPECT_EQ(3, s.get_size());
	EXPECT_EQ(true, s.is_synchro());

	s.bb.erase_bit(65);
	EXPECT_EQ(false, s.is_synchro());
	
	s.update_stack();
	EXPECT_EQ(2, s.get_size());
	
}

TEST(bb_type, basic_operations){
	
	bb_t<BBScan> b;
	b.reset(65);
/*	b.push(10);
	b.push(64);
	b.push(65);
	b.print();
	EXPECT_EQ(3, b.get_size());
	EXPECT_EQ(65,b.pop());
	EXPECT_EQ(2, b.get_size());

	EXPECT_EQ(64,b.pop());
	EXPECT_EQ(1, b.get_size());

	EXPECT_EQ(10,b.pop());
	EXPECT_EQ(0, b.get_size());

	EXPECT_EQ(-1,b.pop());
	EXPECT_TRUE(b.is_empty()); */
	
	//CTG: sera mas bien asi?
	
	b.set_bit(10);
	b.set_bit(64);
	b.set_bit(65);
	
	EXPECT_EQ(3, b.size());
	EXPECT_EQ(65, b.pop_msb());
	EXPECT_EQ(2, b.size());

	EXPECT_EQ(64, b.pop_msb());
	EXPECT_EQ(1, b.size());

	EXPECT_EQ(10, b.pop_msb());
	EXPECT_EQ(0, b.size());

	EXPECT_EQ(-1, b.pop_msb());
	EXPECT_TRUE(b.is_empty());	
}

TEST(bba_t, basic){
//date: 9/8/17 for MWCP upper bound computation
	
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
