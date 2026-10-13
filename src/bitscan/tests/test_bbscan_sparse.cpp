/**
* @file test_bbscan_sparse.cpp
* @brief Unit tests for BBScan class which manages efficient bitscanning for sparse bitsets
* @details Manual bitscanning tests, not using nested classes for bitscanning
* @details created 25/02/2025
* @author pss
**/

#include "bitscan/bitscan.h"
#include "gtest/gtest.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <type_traits>

using namespace std;
using namespace bitgraph;

class BBScanSpClassTest : public ::testing::Test {
protected:
	BBScanSpClassTest() : bbsc(301) {}
	virtual void SetUp() {
		for (int i = 0; i <= 300; i += 50) {
			bbsc.set_bit(i);
			pSet.push_back(i);
		}
	}

	//////////////////////
	//data members
	BBScanSp bbsc;	
	std::vector<int> pSet;
};

TEST_F(BBScanSpClassTest, check_example) {

	EXPECT_EQ(7, bbsc.count());
	EXPECT_TRUE(bbsc.is_bit(0));
	EXPECT_TRUE(bbsc.is_bit(50));
	EXPECT_TRUE(bbsc.is_bit(100));
	EXPECT_TRUE(bbsc.is_bit(150));
	EXPECT_TRUE(bbsc.is_bit(200));
	EXPECT_TRUE(bbsc.is_bit(250));
	EXPECT_TRUE(bbsc.is_bit(300));

	EXPECT_EQ(5, bbsc.num_blocks());
	EXPECT_EQ(5, bbsc.size());

	//I/O
	//bbsc.print();
}
//
TEST_F(BBScanSpClassTest, copy_constructor) {

	BBScanSp bbscCOPY(bbsc);

	EXPECT_EQ	(7, bbscCOPY.count());
	EXPECT_TRUE	(bbscCOPY.is_bit(0));
	EXPECT_TRUE	(bbscCOPY.is_bit(50));
	EXPECT_TRUE	(bbscCOPY.is_bit(100));
	EXPECT_TRUE	(bbscCOPY.is_bit(150));
	EXPECT_TRUE	(bbscCOPY.is_bit(200));
	EXPECT_TRUE	(bbscCOPY.is_bit(250));
	EXPECT_TRUE	(bbscCOPY.is_bit(300));

	EXPECT_EQ	(5, bbscCOPY.num_blocks());
	EXPECT_EQ	(5, bbscCOPY.size());
}

TEST(BBScanSpClass, population_count){

	BBScanSp bbs(130);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	EXPECT_EQ(3, bbs.count());
	EXPECT_EQ(2, bbs.count(11, -1));
	EXPECT_EQ(1, bbs.count(21, -1));
	EXPECT_EQ(0, bbs.count(65, -1));
	EXPECT_EQ(1, bbs.count(64, -1));
}

TEST(BBScanSpClass, set_bits) {
	
	BBScanSp sb(150);
	sb.reset_bit(30,40);
	EXPECT_TRUE(sb.is_bit(30));
	EXPECT_TRUE(sb.is_bit(40));
	EXPECT_EQ(11, sb.count());

	sb.reset_bit(55);
	EXPECT_TRUE(sb.is_bit(55));
	EXPECT_EQ(1, sb.count());
}

TEST(BBScanSpClass, boolean_properties){
//Properties, inherited from BitSetSp

	BBScanSp bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BBScanSp bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_disjoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));

	//is disjoint with ranges
	EXPECT_TRUE	(bb.is_disjoint_block(0,0,bb1));
	EXPECT_FALSE(bb.is_disjoint_block(0,1,bb1));
	EXPECT_FALSE(bb.is_disjoint_block(1,2,bb1));

}


TEST(BBScanSpClass, non_destructive_direct){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//non destructive
	int nBit = BBObject::noBit;
	bbsp.init_scan(BBObject::NON_DESTRUCTIVE);
		
	//scanning
	while ( (nBit = bbsp.next_bit()) != BBObject::noBit ){
		v.push_back(nBit);
	}
			
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks

}

TEST(BBScanSpClass, non_destructive_reverse) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	bbsp.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);

	//scanning
	while ((nBit = bbsp.prev_bit()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks
		
}

TEST(BBScanSpClass, non_destructive_with_starting_point){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	bbsp.init_scan(15, BBObject::NON_DESTRUCTIVE);
	
	//scanning
	while ((nBit = bbsp.next_bit()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(20, v[0]);
	EXPECT_EQ(64, v[1]);
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks
}

TEST(BBScanSpClass, non_destructive_reverse_with_starting_point) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	bbsp.init_scan(15, BBObject::NON_DESTRUCTIVE_REVERSE);
		
	//scanning
	while ((nBit = bbsp.prev_bit()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(2, bbsp.size());				//number of blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//number of blocks
}

TEST(BBScanSpClass, destructive){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	bbsp.init_scan(BBObject::DESTRUCTIVE);

	//scanning
	while ((nBit = bbsp.next_bit_del()) != BBObject::noBit) {
		v.push_back(nBit);
	}
		
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_TRUE(bbsp.is_empty());
	EXPECT_EQ(0, bbsp.count());							//population count
	EXPECT_EQ(2, bbsp.size());				//2 blocks 
	EXPECT_EQ(3, bbsp.num_blocks());						//nBB_ = 3	
}

TEST(BBScanSpClass, destructive_reverse) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	vector<int> v;

	//non destructive reverse
	int nBit = BBObject::noBit;
	bbsp.init_scan(BBObject::DESTRUCTIVE_REVERSE);

	//scanning
	while ((nBit = bbsp.prev_bit_del()) != BBObject::noBit) {
		v.push_back(nBit);
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
	EXPECT_TRUE(bbsp.is_empty());
	EXPECT_EQ(0, bbsp.count());							//population count
	EXPECT_EQ(2, bbsp.size());							//2 blocks 
	EXPECT_EQ(3, bbsp.num_blocks());					//nBB_ = 3	
}


//////////////////////////////////////////////
// copy / move: a new BBScanSp never inherits an active scan (same rule as BBScan)

TEST_F(BBScanSpClassTest, copy_constructor_does_not_inherit_scan) {
	BBScanSp src(301, { 0, 50, 100, 150 });
	src.init_scan(BBObject::NON_DESTRUCTIVE);
	ASSERT_EQ(0, src.next_bit());

	BBScanSp cpy(src);
	EXPECT_TRUE(cpy == src);
	EXPECT_EQ(BBObject::noBit, cpy.scan_block());		//no scan
	EXPECT_EQ(MASK_LIM, cpy.scan_bit());
	EXPECT_EQ(BBObject::noBit, cpy.next_bit());			//scanning without init_scan is a no-op

	EXPECT_EQ(50, src.next_bit());						//the source keeps scanning

	std::vector<int> res;
	cpy.init_scan(BBObject::NON_DESTRUCTIVE);
	for (int v = cpy.next_bit(); v != BBObject::noBit; v = cpy.next_bit()) res.push_back(v);
	EXPECT_EQ((std::vector<int>{ 0, 50, 100, 150 }), res);
}

TEST_F(BBScanSpClassTest, move_constructor_does_not_inherit_scan) {
	BBScanSp src(301, { 0, 50, 100, 150 });
	src.init_scan(BBObject::NON_DESTRUCTIVE);
	ASSERT_EQ(0, src.next_bit());

	BBScanSp mv(std::move(src));
	EXPECT_EQ(BBObject::noBit, mv.scan_block());
	EXPECT_EQ(BBObject::noBit, mv.next_bit());
	EXPECT_EQ(4, mv.count());

	mv.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	EXPECT_EQ(150, mv.prev_bit());
}

TEST_F(BBScanSpClassTest, assignment_keeps_target_cursor) {
	BBScanSp src(301, { 0, 50, 100, 150 });
	src.init_scan(BBObject::NON_DESTRUCTIVE);
	ASSERT_EQ(0, src.next_bit());
	ASSERT_NE(BBObject::noBit, src.scan_block());

	BBScanSp dst(301);
	dst = src;
	EXPECT_TRUE(dst == src);
	EXPECT_EQ(BBObject::noBit, dst.scan_block());		//target had no scan, it has none now

	BBScanSp dst2(301);
	dst2 = std::move(src);
	EXPECT_EQ(BBObject::noBit, dst2.scan_block());

	//a scan in progress in the target keeps its own cursor
	BBScanSp t(301, { 10, 20, 30 });
	t.init_scan(BBObject::NON_DESTRUCTIVE);
	const int blk = t.scan_block(), bit = t.scan_bit();
	t = BBScanSp(301, { 10, 20, 30 });
	EXPECT_EQ(blk, t.scan_block());
	EXPECT_EQ(bit, t.scan_bit());

	t = t;												//self-assignment
	EXPECT_EQ(3, t.count());
}

TEST_F(BBScanSpClassTest, self_assignment) {
	BBScanSp& ref = bbsc;
	bbsc = ref;

	std::vector<int> res;
	bbsc.init_scan(BBObject::NON_DESTRUCTIVE);
	for (int v = bbsc.next_bit(); v != BBObject::noBit; v = bbsc.next_bit()) res.push_back(v);
	EXPECT_EQ(pSet, res);
}

TEST(BBScanSpClass, vector_of_scanners_reallocation) {
	//moves during reallocation must not carry cursors (and must be cheap: noexcept)
	EXPECT_TRUE((std::is_nothrow_move_constructible<BBScanSp>::value));
	EXPECT_TRUE((std::is_nothrow_move_assignable<BBScanSp>::value));

	std::vector<BBScanSp> v;
	for (int i = 0; i < 20; ++i) {
		v.emplace_back(200, std::initializer_list<int>{ i, i + 100 });
		v.back().init_scan(BBObject::NON_DESTRUCTIVE);
	}
	for (auto& b : v) {
		b.init_scan(BBObject::NON_DESTRUCTIVE);
		EXPECT_EQ(2, b.count());
		EXPECT_NE(BBObject::noBit, b.next_bit());
	}
}

//////////////////////////////////////////////
// cursor safety and constness

TEST(BBScanSpClass, non_destructive_scans_are_const) {
	BBScanSp b(301, { 5, 70, 200 });
	const BBScanSp& cb = b;

	b.init_scan(BBObject::NON_DESTRUCTIVE);
	EXPECT_EQ(5, cb.next_bit());
	EXPECT_EQ(70, cb.next_bit());

	b.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	EXPECT_EQ(200, cb.prev_bit());
	EXPECT_EQ(70, cb.prev_bit());
}

TEST(BBScanSpClass, stale_cursor_after_blocks_removed) {
	BBScanSp b(301, { 5, 70, 200 });
	b.init_scan(BBObject::NON_DESTRUCTIVE);
	ASSERT_EQ(5, b.next_bit());
	ASSERT_EQ(70, b.next_bit());
	ASSERT_EQ(200, b.next_bit());						//cursor on the third block

	b.clear_all();										//no blocks: the cursor is out of range
	EXPECT_EQ(BBObject::noBit, b.next_bit());
	EXPECT_EQ(BBObject::noBit, b.prev_bit());
	EXPECT_EQ(BBObject::noBit, b.next_bit_del());
	EXPECT_EQ(BBObject::noBit, b.prev_bit_del());

	//empty bitset: init_scan invalidates the scan
	b.init_scan(BBObject::NON_DESTRUCTIVE);
	EXPECT_EQ(BBObject::noBit, b.next_bit());
	b.init_scan(BBObject::DESTRUCTIVE);
	EXPECT_EQ(BBObject::noBit, b.next_bit_del());
}

TEST(BBScanSpClass, init_scan_from_a_bit_matches_reference) {
	const std::vector<int> bits = { 3, 63, 64, 70, 200, 201, 450, 699 };
	BBScanSp b(700, std::initializer_list<int>{ 3, 63, 64, 70, 200, 201, 450, 699 });

	//starting points: set bits, bits in present blocks, bits in absent blocks, ends
	for (int start : { 0, 3, 5, 63, 64, 65, 100, 128, 199, 200, 300, 449, 450, 600, 698, 699 }) {

		std::vector<int> fwd, bwd, fwd_exp, bwd_exp;
		for (int v : bits) {
			if (v > start) fwd_exp.push_back(v);
			if (v < start) bwd_exp.insert(bwd_exp.begin(), v);
		}

		b.init_scan(start, BBObject::NON_DESTRUCTIVE);
		for (int v = b.next_bit(); v != BBObject::noBit; v = b.next_bit()) fwd.push_back(v);
		EXPECT_EQ(fwd_exp, fwd) << "forward from " << start;

		b.init_scan(start, BBObject::NON_DESTRUCTIVE_REVERSE);
		for (int v = b.prev_bit(); v != BBObject::noBit; v = b.prev_bit()) bwd.push_back(v);
		EXPECT_EQ(bwd_exp, bwd) << "reverse from " << start;
	}
}

