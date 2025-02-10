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

	auto sc1 = make_scan_rev(bb);
	int bit = BBObject::NOBIT;
	while ((bit = sc1.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	auto sc2 = make_scan(bb);
	bit = BBObject::NOBIT;
	while ((bit = sc2.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}


	auto sc3 = make_scan_dest(bb);
	bit = BBObject::NOBIT;
	while ((bit = sc3.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	EXPECT_TRUE(bb.is_empty());

	//restores original bitset
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);

	auto sc4 = make_scan_rev_dest(bb);
	bit = BBObject::NOBIT;
	while ((bit = sc4.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	EXPECT_TRUE(bb.is_empty());

	
}

	