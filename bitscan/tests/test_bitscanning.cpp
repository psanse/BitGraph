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
	
	SCAN<> scan(bb);

	int bit = -1;
	while ( (bit= scan.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;	
	}

	SCAN_REV<> scanRev(bb);

	bit = -1;
	while ((bit = scanRev.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	SCAN_DEST<> scanDest(bb);

	bit = -1;
	while ((bit = scanDest.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	EXPECT_TRUE(bb.is_empty());
}

TEST(BitScanningFeatureTest, basic) {

	BBIntrin bb(65);
	bb.set_bit(0);
	bb.set_bit(1);
	bb.set_bit(64);

	DirectRevScan<> scanRev(bb);

	int bit = -1;
	while ((bit = scanRev.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	DirectScan<> scan(bb);

	bit = -1;
	while ((bit = scan.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}


	DestructiveScan<> scanDest(bb);

	bit = -1;
	while ((bit = scanDest.next_bit()) != EMPTY_ELEM) {
		cout << "bit: " << bit << endl;
	}

	EXPECT_TRUE(bb.is_empty());



	
}

	