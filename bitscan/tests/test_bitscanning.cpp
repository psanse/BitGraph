/**
* @file test_bitscanning.cpp
* @brief attempt to simplify syntax of bitscans with generic programming
* @details created 10/02/2025
**/

#include "bitscan/bbscanning.h"
#include "bitscan/bbscanningFeature.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

TEST(BitScanningTest, basic) {

	BBIntrin bb(65);
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);
	
	SCAN<> sc1(bb);

	int bit = BBObject::NOBIT;
	while ( (bit= sc1.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;	
	}

	SCAN_REV<> sc2(bb);

	bit = BBObject::NOBIT;
	while ((bit = sc2.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	SCAN_DEST<> sc3(bb);

	bit = BBObject::NOBIT;
	while ((bit = sc3.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	EXPECT_TRUE(bb.is_empty());
}

TEST(BitScanningFeatureTest, basic) {

	BitSet bb(65);
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);

	scanRev<BitSet> scr(bb);
	int bit = BBObject::NOBIT;
	while ((bit = scr.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	scan<BitSet> sc(bb);
	bit = BBObject::NOBIT;
	while ((bit = sc.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}


	scanDest<BitSet> scd(bb);
	bit = BBObject::NOBIT;
	while ((bit = scd.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	EXPECT_TRUE(bb.is_empty());

	//restores original bitset
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);

	scanRevDest<BitSet> scrd(bb);
	bit = BBObject::NOBIT;
	while ((bit = scrd.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	EXPECT_TRUE(bb.is_empty());

	
}

	