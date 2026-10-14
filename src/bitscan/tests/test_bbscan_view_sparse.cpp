/**
 * @file test_bbscan_view_sparse.cpp
 * @brief Unit tests for independent-cursor scan views over sparse bitsets.
 *
 * Sparse counterpart of test_bbscan_view.cpp: same fixture layout and same test
 * names for the shared behavior. Tests BBScanViewSpT with the public BBScanSp
 * type (through the convenience aliases in bitgraph::view) and with the basic
 * BitsetSp type (through the generic alias templates).
 *
 * The first groups repeat the tests of test_bbscan_view.cpp. The last group
 * covers sparse-specific cases (blocks absent from the collection, zero
 * blocks, empty bitsets) and compares the views with the basic next_bit loop.
 *
 * @date Created: 14/10/2026
 * @author P. San Segundo
 */

#include "bitscan/bitscan.h"
#include "gtest/gtest.h"
#include <random>
#include <set>
#include <type_traits>
#include <vector>

using namespace bitgraph;

class BBScanViewSpTest : public ::testing::Test {
protected:
	BBScanViewSpTest() : bitset(301) {}

	void SetUp() override {
		for (int bit = 0; bit <= 300; bit += 50) {
			bitset.set_bit(bit);
			expected.insert(bit);
		}
	}

	BBScanSp bitset;
	std::set<int> expected;
};

namespace {

	template<class View>
	std::vector<int> collect(View& v) {
		std::vector<int> res;
		for (int b = v.next_bit(); b != BBObject::noBit; b = v.next_bit()) res.push_back(b);
		return res;
	}

	const std::vector<int> kBits = { 3, 63, 64, 70, 200, 201, 450, 699 };
}

/**
 * @test Verifies that a non-destructive Forward view returns every set bit
 *       and leaves the source bitset unchanged.
 */
TEST_F(BBScanViewSpTest, template_non_destructive_forward_scan) {
	view::ForwardSp scan(bitset);
	std::set<int> result;

	for (int bit = scan.next_bit(); bit != BBObject::noBit; bit = scan.next_bit()) {
		result.insert(bit);
	}

	EXPECT_EQ(expected, result);
	EXPECT_EQ(expected.size(), bitset.count());
}

/**
 * @test Verifies that a DestructiveReverse view returns set bits in decreasing
 *       position order and clears them from the source bitset.
 */
TEST_F(BBScanViewSpTest, template_destructive_reverse_scan) {
	BBScanSp bitsetCopy(bitset);
	view::DestructiveReverseSp scan(bitsetCopy);

	const std::vector<int> expected_order{ 300, 250, 200, 150, 100, 50, 0 };

	EXPECT_EQ(expected_order, collect(scan));
	EXPECT_TRUE(bitsetCopy.is_empty());
}

/**
 * @test Verifies that a reverse scan initialized at a specified position
 *       returns only set bits strictly below that position.
 */
TEST_F(BBScanViewSpTest, template_scan_honors_start) {
	view::ReverseSp scan(bitset, 200);		// init scan at 200

	EXPECT_EQ((std::vector<int>{150, 100, 50, 0}), collect(scan));
	EXPECT_EQ(expected.size(), bitset.count());
}

/**
 * @test Verifies that scan initialization terminates the process when the
 *       initial position is negative and differs from BBObject::noBit, or
 *       when it lies outside the bitset.
 */
TEST_F(BBScanViewSpTest, template_scan_rejects_invalid_start) {
	BBScanSp bits(128, { 1, 63, 64, 90 });
	view::ForwardSp scan(bits);

	EXPECT_DEATH(scan.init_scan(-2), "");
	EXPECT_DEATH(scan.init_scan(static_cast<int>(bits.bit_capacity())), "");
}

/**
 * @test Verifies that two non-destructive views maintain independent cursors
 *       while scanning the same bitset.
 */
TEST_F(BBScanViewSpTest, views_have_independent_cursors) {
	view::ForwardSp forward(bitset);
	view::ReverseSp reverse(bitset);

	EXPECT_EQ(0, forward.next_bit());
	EXPECT_EQ(300, reverse.next_bit());

	EXPECT_EQ(50, forward.next_bit());
	EXPECT_EQ(250, reverse.next_bit());

	EXPECT_EQ(100, forward.next_bit());
	EXPECT_EQ(200, reverse.next_bit());

	EXPECT_EQ(150, forward.next_bit());
	EXPECT_EQ(150, reverse.next_bit());

	EXPECT_EQ(200, forward.next_bit());
	EXPECT_EQ(100, reverse.next_bit());

	EXPECT_EQ(250, forward.next_bit());
	EXPECT_EQ(50, reverse.next_bit());

	EXPECT_EQ(300, forward.next_bit());
	EXPECT_EQ(0, reverse.next_bit());

	EXPECT_EQ(BBObject::noBit, forward.next_bit());
	EXPECT_EQ(BBObject::noBit, reverse.next_bit());

	EXPECT_EQ(expected.size(), bitset.count());
}

/**
 * @test Verifies that the facade aliases in bitgraph::view::sparse are the same
 *       types as the convenience aliases.
 */
TEST_F(BBScanViewSpTest, facade_aliases) {
	EXPECT_TRUE((std::is_same<view::sparse::scan, view::ForwardSp>::value));
	EXPECT_TRUE((std::is_same<view::sparse::scanR, view::ReverseSp>::value));
	EXPECT_TRUE((std::is_same<view::sparse::scanD, view::DestructiveForwardSp>::value));
	EXPECT_TRUE((std::is_same<view::sparse::scanDR, view::DestructiveReverseSp>::value));

	view::sparse::scan scan(bitset);
	EXPECT_EQ(0, scan.next_bit());
}

/**
 * @test Verifies that a view and the persistent scanner of the bitset do not
 *       interfere with each other.
 */
TEST_F(BBScanViewSpTest, view_and_persistent_scanner_are_independent) {
	view::ForwardSp v(bitset);
	BBScanSp::scan ps(bitset);

	EXPECT_EQ(0, v.next_bit());
	EXPECT_EQ(0, ps.next_bit());
	EXPECT_EQ(50, v.next_bit());
	EXPECT_EQ(50, ps.next_bit());
	EXPECT_EQ(100, ps.next_bit());
	EXPECT_EQ(100, v.next_bit());
}

//////////////////////////////////////////////
// Generic alias templates with the basic BitsetSp class

TEST(BBScanViewSpGenericTest, non_destructive_forward_scan_with_bitset) {
	BitsetSp bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::ForwardViewSp<BitsetSp> scan(bits);

	EXPECT_EQ((std::vector<int>{ 0, 50, 100, 150, 200, 250, 300 }), collect(scan));
	EXPECT_EQ(7, bits.count());
}

TEST(BBScanViewSpGenericTest, non_destructive_reverse_scan_with_bitset) {
	BitsetSp bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::ReverseViewSp<BitsetSp> scan(bits);

	EXPECT_EQ((std::vector<int>{ 300, 250, 200, 150, 100, 50, 0 }), collect(scan));
	EXPECT_EQ(7, bits.count());
}

TEST(BBScanViewSpGenericTest, destructive_forward_scan_with_bitset) {
	BitsetSp bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::DestructiveForwardViewSp<BitsetSp> scan(bits);

	EXPECT_EQ((std::vector<int>{ 0, 50, 100, 150, 200, 250, 300 }), collect(scan));
	EXPECT_TRUE(bits.is_empty());
}

TEST(BBScanViewSpGenericTest, destructive_reverse_scan_with_bitset) {
	BitsetSp bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::DestructiveReverseViewSp<BitsetSp> scan(bits);

	EXPECT_EQ((std::vector<int>{ 300, 250, 200, 150, 100, 50, 0 }), collect(scan));
	EXPECT_TRUE(bits.is_empty());
}

//////////////////////////////////////////////
// Sparse-specific cases

TEST(BBScanViewSpSparseTest, empty_and_exhausted) {
	BBScanSp e(700);
	view::ForwardSp f(e);
	EXPECT_EQ(BBObject::noBit, f.next_bit());
	view::ReverseSp r(e);
	EXPECT_EQ(BBObject::noBit, r.next_bit());
	view::DestructiveForwardSp d(e);
	EXPECT_EQ(BBObject::noBit, d.next_bit());

	//exhausted views keep returning noBit
	BBScanSp b(700, std::initializer_list<int>{ 5 });
	view::ForwardSp v(b);
	EXPECT_EQ(5, v.next_bit());
	EXPECT_EQ(BBObject::noBit, v.next_bit());
	EXPECT_EQ(BBObject::noBit, v.next_bit());

	//only zero blocks
	BBScanSp z(700, std::initializer_list<int>{ 5, 300 });
	z.erase_bit(5).erase_bit(300);
	view::ForwardSp zf(z);
	EXPECT_EQ(BBObject::noBit, zf.next_bit());
	view::ReverseSp zr(z);
	EXPECT_EQ(BBObject::noBit, zr.next_bit());
}

TEST(BBScanViewSpSparseTest, starting_point_matches_reference) {
	BBScanSp b(700, std::initializer_list<int>{ 3, 63, 64, 70, 200, 201, 450, 699 });

	//starting points: set bits, bits in present blocks, bits in blocks absent from the collection, both ends
	for (int start : { 0, 3, 5, 63, 64, 65, 100, 128, 199, 200, 300, 449, 450, 600, 698, 699 }) {
		std::vector<int> fwd_exp, bwd_exp;
		for (int v : kBits) {
			if (v > start) fwd_exp.push_back(v);
			if (v < start) bwd_exp.insert(bwd_exp.begin(), v);
		}
		view::ForwardSp f(b, start);
		EXPECT_EQ(fwd_exp, collect(f)) << "forward from " << start;
		view::ReverseSp r(b, start);
		EXPECT_EQ(bwd_exp, collect(r)) << "reverse from " << start;

		//destructive scans erase only the bits after (before) start
		BBScanSp cd(b);
		view::DestructiveForwardSp d(cd, start);
		EXPECT_EQ(fwd_exp, collect(d)) << "destructive forward from " << start;
		BBScanSp cr(b);
		view::DestructiveReverseSp dr(cr, start);
		EXPECT_EQ(bwd_exp, collect(dr)) << "destructive reverse from " << start;
	}
}

TEST(BBScanViewSpSparseTest, works_with_basic_bitset_and_zero_blocks) {
	BitsetSp b(700, { 3, 63, 64, 70, 200, 201, 450, 699 });
	b.set(130).erase_bit(130);								//a zero block in the middle

	BBScanViewSpT<BitsetSp, BBObject::NON_DESTRUCTIVE> f(b);
	EXPECT_EQ(kBits, collect(f));
	BBScanViewSpT<BitsetSp, BBObject::NON_DESTRUCTIVE_REVERSE> r(b, 450);
	EXPECT_EQ((std::vector<int>{ 201, 200, 70, 64, 63, 3 }), collect(r));
}

TEST(BBScanViewSpSparseTest, differential_against_next_bit_loop) {
	std::mt19937 rng(7);
	for (int round = 0; round < 40; ++round) {
		BBScanSp b(1000);
		const int n = static_cast<int>(rng() % 300);
		for (int i = 0; i < n; ++i) b.set_bit(static_cast<int>(rng() % 1000));
		if (round % 4 == 0) { for (int v = 0; v < 1000; v += 3) b.erase_bit(v); }	//zero blocks

		std::vector<int> fwd, bwd;
		for (int v = b.BitsetSp::next_bit(BBObject::noBit); v != BBObject::noBit; v = b.BitsetSp::next_bit(v)) fwd.push_back(v);
		for (int v = b.BitsetSp::prev_bit(BBObject::noBit); v != BBObject::noBit; v = b.BitsetSp::prev_bit(v)) bwd.push_back(v);

		view::ForwardSp f(b);
		EXPECT_EQ(fwd, collect(f));
		view::ReverseSp r(b);
		EXPECT_EQ(bwd, collect(r));
	}
}
