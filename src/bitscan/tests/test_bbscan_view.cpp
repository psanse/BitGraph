/**
* @file test_bbscan_view.cpp
* @brief Unit tests of the BBScan class  - only related to non-nested bitscanning and BBScanView classes
* @details Taken from the unit tests in test_bitstring.cpp (2014), filtering out other bitstring classes in the hierarchy
* @created 12/02/2025, last_update 29/04/2025
* @author pss
**/

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

	Bitset bitset;
	std::set<int> expected;
};

TEST_F(BBScanViewTest, non_destructive_scans_forward_and_preserves_source) {
	BBScanView scan(bitset);
	std::set<int> result;


	ASSERT_EQ(0, scan.init_scan(BBObject::NON_DESTRUCTIVE));
	for (int bit = scan.next_bit(); bit != BBObject::noBit; bit = scan.next_bit()) {
		result.insert(bit);
	}

	EXPECT_EQ(expected, result);
	EXPECT_EQ(expected.size(), bitset.count());
}

TEST_F(BBScanViewTest, non_destructive_scan_honors_start_and_does_not_mutate_source) {
	BBScanView scan(bitset);
	std::set<int> result;

	ASSERT_EQ(0, scan.init_scan(50, BBObject::NON_DESTRUCTIVE));
	for (int bit = scan.next_bit(); bit != BBObject::noBit; bit = scan.next_bit()) {
		result.insert(bit);
	}

	EXPECT_EQ((std::set<int>{100, 150, 200, 250, 300}), result);
	EXPECT_EQ(expected.size(), bitset.count());
	EXPECT_TRUE(bitset.is_bit(0));
	EXPECT_TRUE(bitset.is_bit(50));
}

TEST_F(BBScanViewTest, destructive_mode_with_next_prev_mutates_source) {
	BBScanView forward(bitset);
	std::set<int> forwardResult;

	ASSERT_EQ(0, forward.init_scan(BBObject::DESTRUCTIVE));
	for (int bit = forward.next_bit(); bit != BBObject::noBit; bit = forward.next_bit()) {
		forwardResult.insert(bit);
	}

	EXPECT_EQ(expected, forwardResult);
	EXPECT_TRUE(bitset.is_empty());

	for (int bit : expected) {
		bitset.set_bit(bit);
	}

	BBScanView reverse(bitset);
	std::set<int> reverseResult;
	ASSERT_EQ(0, reverse.init_scan(BBObject::DESTRUCTIVE_REVERSE));
	for (int bit = reverse.prev_bit(); bit != BBObject::noBit; bit = reverse.prev_bit()) {
		reverseResult.insert(bit);
	}

	EXPECT_EQ(expected, reverseResult);
	EXPECT_TRUE(bitset.is_empty());
}

TEST_F(BBScanViewTest, reverse_non_destructive_scan_honors_start) {
	BBScanView scan(bitset);
	std::vector<int> result;

	ASSERT_EQ(0, scan.init_scan(200, BBObject::NON_DESTRUCTIVE_REVERSE));
	for (int bit = scan.prev_bit(); bit != BBObject::noBit; bit = scan.prev_bit()) {
		result.push_back(bit);
	}

	EXPECT_EQ((std::vector<int>{150, 100, 50, 0}), result);
	EXPECT_EQ(expected.size(), bitset.count());
}

TEST_F(BBScanViewTest, destructive_scans_honor_start_within_a_block) {
	Bitset forwardBits(128, {1, 10, 20, 63, 64, 90});
	BBScanView forward(forwardBits);
	std::vector<int> forwardResult;

	ASSERT_EQ(0, forward.init_scan(10, BBObject::DESTRUCTIVE));
	for (int bit = forward.next_bit(); bit != BBObject::noBit; bit = forward.next_bit()) {
		forwardResult.push_back(bit);
	}
	EXPECT_EQ((std::vector<int>{20, 63, 64, 90}), forwardResult);
	EXPECT_TRUE(forwardBits.is_bit(1));
	EXPECT_TRUE(forwardBits.is_bit(10));

	Bitset reverseBits(128, {1, 10, 20, 63, 64, 90});
	BBScanView reverse(reverseBits);
	std::vector<int> reverseResult;

	ASSERT_EQ(0, reverse.init_scan(63, BBObject::DESTRUCTIVE_REVERSE));
	for (int bit = reverse.prev_bit(); bit != BBObject::noBit; bit = reverse.prev_bit()) {
		reverseResult.push_back(bit);
	}
	EXPECT_EQ((std::vector<int>{20, 10, 1}), reverseResult);
	EXPECT_TRUE(reverseBits.is_bit(63));
	EXPECT_TRUE(reverseBits.is_bit(64));
	EXPECT_TRUE(reverseBits.is_bit(90));
}

TEST_F(BBScanViewTest, rejects_invalid_start_and_uninitialized_scan_is_safe) {
	BBScanView scan(bitset);

	EXPECT_EQ(BBObject::noBit, scan.next_bit());
	EXPECT_EQ(BBObject::noBit, scan.prev_bit());
	EXPECT_EQ(-1, scan.init_scan(-2, BBObject::NON_DESTRUCTIVE));
	EXPECT_EQ(BBObject::noBit, scan.next_bit());
	EXPECT_EQ(-1, scan.init_scan(bitset.num_blocks() * WORD_SIZE,
		BBObject::NON_DESTRUCTIVE_REVERSE));
	EXPECT_EQ(BBObject::noBit, scan.prev_bit());
}

TEST_F(BBScanViewTest, template_non_destructive_forward_scan) {
	BBScanViewT<BBObject::NON_DESTRUCTIVE> scan(bitset);
	std::set<int> result;

	ASSERT_EQ(0, scan.init_scan());
	for (int bit = scan.next_bit(); bit != BBObject::noBit; bit = scan.next_bit()) {
		result.insert(bit);
	}

	EXPECT_EQ(expected, result);
	EXPECT_EQ(expected.size(), bitset.count());
}

TEST_F(BBScanViewTest, template_destructive_reverse_scan) {
	BBScanViewT<BBObject::DESTRUCTIVE_REVERSE> scan(bitset);
	std::set<int> result;

	ASSERT_EQ(0, scan.init_scan());
	for (int bit = scan.prev_bit(); bit != BBObject::noBit; bit = scan.prev_bit()) {
		result.insert(bit);
	}

	EXPECT_EQ(expected, result);
	EXPECT_TRUE(bitset.is_empty());
}

TEST_F(BBScanViewTest, template_scan_honors_start) {
	BBScanViewT<BBObject::NON_DESTRUCTIVE_REVERSE> scan(bitset);
	std::vector<int> result;

	ASSERT_EQ(0, scan.init_scan(200));
	for (int bit = scan.prev_bit(); bit != BBObject::noBit; bit = scan.prev_bit()) {
		result.push_back(bit);
	}

	EXPECT_EQ((std::vector<int>{150, 100, 50, 0}), result);
	EXPECT_EQ(expected.size(), bitset.count());
}

TEST(BBScanViewTemplateTest, template_scan_rejects_invalid_start) {
	Bitset bits(128, {1, 63, 64, 90});
	BBScanViewT<BBObject::DESTRUCTIVE> scan(bits);

	EXPECT_EQ(-1, scan.init_scan(-2));
	EXPECT_EQ(BBObject::noBit, scan.next_bit());
	EXPECT_EQ(-1, scan.init_scan(bits.num_blocks() * WORD_SIZE));
	EXPECT_EQ(BBObject::noBit, scan.next_bit());
}





