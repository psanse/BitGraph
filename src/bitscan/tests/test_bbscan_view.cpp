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

TEST_F(BBScanViewTest, template_destructive_reverse_scan) {
	view::DestructiveReverse scan(bitset);
	std::set<int> result;

	scan.init_scan();
	for (int bit = scan.prev_bit(); bit != BBObject::noBit; bit = scan.prev_bit()) {
		result.insert(bit);
	}

	EXPECT_EQ(expected, result);
	EXPECT_TRUE(bitset.is_empty());
}

TEST_F(BBScanViewTest, template_scan_honors_start) {
	view::Reverse scan(bitset);
	std::vector<int> result;

	scan.init_scan(200);
	for (int bit = scan.prev_bit(); bit != BBObject::noBit; bit = scan.prev_bit()) {
		result.push_back(bit);
	}

	EXPECT_EQ((std::vector<int>{150, 100, 50, 0}), result);
	EXPECT_EQ(expected.size(), bitset.count());
}

TEST(BBScanViewTemplateTest, template_scan_rejects_invalid_start) {
	Bitset bits(128, {1, 63, 64, 90});
	view::Destructive scan(bits);

	EXPECT_DEATH(scan.init_scan(-2), "");
	EXPECT_DEATH(scan.init_scan(static_cast<int>(bits.size())), "");

}





