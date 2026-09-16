/**
 * @file test_bbscan_view.cpp
 * @brief Unit tests for independent-cursor bitset scan views.
 *
 * Tests BBScanViewT with both the public BBScan type and the basic Bitset
 * type. The test suite covers:
 *
 * - forward and reverse traversal;
 * - non-destructive and destructive scanning;
 * - initialization from a specified bit position;
 * - independent cursors over the same bitset;
 * - rejection of invalid initial positions.
 *
 * The convenience aliases in bitgraph::view are tested with BBScan, while
 * the generic alias templates are instantiated explicitly with Bitset.
 *
 * These tests were adapted from the original bitstring tests implemented
 * in test_bitstring.cpp (2014).
 *
 * @date Created: 12/02/2025
 * @date Last updated: 16/09/2026
 * @author P. San Segundo
 */

#include "bitscan/bbscan_view.h"
#include "gtest/gtest.h"
#include <set>
#include <vector>

using namespace bitgraph;

class BBScanViewTest : public ::testing::Test {
protected:
	BBScanViewTest() : bitset(301) {}

	void SetUp() override {
		for (int bit = 0; bit <= 300; bit += 50) {
			bitset.set_bit(bit);
			expected.insert(bit);
		}
	}

	
	BBScan bitset;
	std::set<int> expected;
};

/**
 * @test Verifies that a non-destructive Forward view returns every set bit
 *       and leaves the source bitset unchanged.
 */
TEST_F(BBScanViewTest, template_non_destructive_forward_scan) {
	view::Forward scan(bitset);
	std::set<int> result;

	scan.init_scan();
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
TEST_F(BBScanViewTest, template_destructive_reverse_scan) {
	view::DestructiveReverse scan(bitset);
	std::vector<int> result;

	scan.init_scan();
	for (int bit = scan.next_bit();
		bit != BBObject::noBit;
		bit = scan.next_bit()) {
		result.push_back(bit);
	}

	const std::vector<int> expected_order{
		300, 250, 200, 150, 100, 50, 0
	};

	EXPECT_EQ(expected_order, result);
	EXPECT_TRUE(bitset.is_empty());
}

/**
 * @test Verifies that a reverse scan initialized at a specified position
 *       returns only set bits strictly below that position.
 *
 * In this test, initialization at position 200 produces the sequence
 * 150, 100, 50, 0; the set bit at position 200 is not returned.
 */
TEST_F(BBScanViewTest, template_scan_honors_start) {
	view::Reverse scan(bitset);
	std::vector<int> result;

	scan.init_scan(200);
	for (int bit = scan.next_bit(); bit != BBObject::noBit; bit = scan.next_bit()) {
		result.push_back(bit);
	}

	EXPECT_EQ((std::vector<int>{150, 100, 50, 0}), result);
	EXPECT_EQ(expected.size(), bitset.count());
}

/**
 * @test Verifies that scan initialization terminates the process when the
 *       initial position is negative and differs from BBObject::noBit, or
 *       when it lies outside the bitset.
 */
TEST(BBScanViewTest, template_scan_rejects_invalid_start) {
	BBScan bits(128, { 1, 63, 64, 90 });
	view::DestructiveForward scan(bits);

	EXPECT_DEATH(scan.init_scan(-2), "");
	EXPECT_DEATH(
		scan.init_scan(static_cast<int>(bits.size())),
		""
	);
}

/**
 * @test Verifies that two non-destructive views maintain independent cursors
 *       while scanning the same bitset.
 */
TEST_F(BBScanViewTest, views_have_independent_cursors) {
	view::Forward forward(bitset);
	view::Reverse reverse(bitset);

	forward.init_scan();
	reverse.init_scan();

	// Advance both views alternately.
	EXPECT_EQ(0, forward.next_bit());
	EXPECT_EQ(300, reverse.next_bit());

	EXPECT_EQ(50, forward.next_bit());
	EXPECT_EQ(250, reverse.next_bit());

	EXPECT_EQ(100, forward.next_bit());
	EXPECT_EQ(200, reverse.next_bit());

	// Each view continues from its own cursor position.
	EXPECT_EQ(150, forward.next_bit());
	EXPECT_EQ(150, reverse.next_bit());

	EXPECT_EQ(200, forward.next_bit());
	EXPECT_EQ(100, reverse.next_bit());

	EXPECT_EQ(250, forward.next_bit());
	EXPECT_EQ(50, reverse.next_bit());

	EXPECT_EQ(300, forward.next_bit());
	EXPECT_EQ(0, reverse.next_bit());

	// Both scans are now exhausted.
	EXPECT_EQ(BBObject::noBit, forward.next_bit());
	EXPECT_EQ(BBObject::noBit, reverse.next_bit());

	// Non-destructive views must leave the bitset unchanged.
	EXPECT_EQ(expected.size(), bitset.count());
}

/**
 * @test Verifies that the generic ForwardView alias operates directly on a
 *       Bitset without requiring the persistent scanning state of BBScan.
 */
TEST(BBScanViewGenericTest, non_destructive_forward_scan_with_bitset) {
	Bitset bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::ForwardView<Bitset> scan(bits);
	std::vector<int> result;

	scan.init_scan();
	for (int bit = scan.next_bit();
		bit != BBObject::noBit;
		bit = scan.next_bit()) {
		result.push_back(bit);
	}

	const std::vector<int> expected{
		0, 50, 100, 150, 200, 250, 300
	};

	EXPECT_EQ(expected, result);
	EXPECT_EQ(7u, bits.count());
}


TEST(BBScanViewGenericTest, non_destructive_reverse_scan_with_bitset) {
	Bitset bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::ReverseView<Bitset> scan(bits);
	std::vector<int> result;

	scan.init_scan();
	for (int bit = scan.next_bit();
		bit != BBObject::noBit;
		bit = scan.next_bit()) {
		result.push_back(bit);
	}

	const std::vector<int> expected{
		300, 250, 200, 150, 100, 50, 0
	};

	EXPECT_EQ(expected, result);
	EXPECT_EQ(7u, bits.count());
}


TEST(BBScanViewGenericTest, destructive_forward_scan_with_bitset) {
	Bitset bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::DestructiveForwardView<Bitset> scan(bits);
	std::vector<int> result;

	scan.init_scan();
	for (int bit = scan.next_bit();
		bit != BBObject::noBit;
		bit = scan.next_bit()) {
		result.push_back(bit);
	}

	const std::vector<int> expected{
		0, 50, 100, 150, 200, 250, 300
	};

	EXPECT_EQ(expected, result);
	EXPECT_TRUE(bits.is_empty());
}


TEST(BBScanViewGenericTest, destructive_reverse_scan_with_bitset) {
	Bitset bits(301);
	for (int bit = 0; bit <= 300; bit += 50) {
		bits.set_bit(bit);
	}

	view::DestructiveReverseView<Bitset> scan(bits);
	std::vector<int> result;

	scan.init_scan();
	for (int bit = scan.next_bit();
		bit != BBObject::noBit;
		bit = scan.next_bit()) {
		result.push_back(bit);
	}

	const std::vector<int> expected{
		300, 250, 200, 150, 100, 50, 0
	};

	EXPECT_EQ(expected, result);
	EXPECT_TRUE(bits.is_empty());
}








