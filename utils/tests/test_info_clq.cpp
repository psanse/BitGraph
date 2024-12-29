/*
* test_info_clq.cpp: tests the infoCLQ class
 *
 *@date 15/12/2024
 *@comment automatic tests by copilot - TODO better tests
 * 
 */

#include "gtest/gtest.h"
#include "../utils/info/info_clq.h"
#include <thread>

using namespace com;

class InfoCLQTest : public ::testing::Test {
protected:
    infoCLQ info;

    void SetUp() override {
        // Initialize any necessary data here
    }

    void TearDown() override {
        // Clean up any necessary data here
    }
};

TEST_F(InfoCLQTest, DefaultConstructor) {
    EXPECT_EQ(info.incumbent_, 0);
    EXPECT_EQ(info.optimum_, 0);
    EXPECT_EQ(info.nSteps_, 0);
    EXPECT_FALSE(info.isTimeOutReached_);
    EXPECT_EQ(info.LB_0_no_AMTS_, 0);
    EXPECT_EQ(info.LB_0_AMTS_, 0);
    EXPECT_EQ(info.branchingRootSize_, 0);
    EXPECT_EQ(info.idSortReal_, -1);
    EXPECT_FALSE(info.isDegOrd_);
    EXPECT_EQ(info.nLastIsetPreFilterCalls_, 0);
    EXPECT_EQ(info.nLastIsetCalls_, 0);
    EXPECT_EQ(info.nCurrIsetCalls_, 0);
    EXPECT_EQ(info.nVertexCalls_, 0);
    EXPECT_EQ(info.nUBpartCalls_, 0);
    EXPECT_EQ(info.nsLastIsetPreFilterCalls_, 0);
    EXPECT_EQ(info.nsLastIsetCalls_, 0);
    EXPECT_EQ(info.nsCurrIsetCalls_, 0);
    EXPECT_EQ(info.nsVertexCalls_, 0);
    EXPECT_EQ(info.nsUBpartCalls_, 0);
}

TEST_F(InfoCLQTest, ClearPreprocInfo) {
    info.LB_0_no_AMTS_ = 10.0;
    info.LB_0_AMTS_ = 20.0;
    info.branchingRootSize_ = 5;
    info.idSortReal_ = 1;
    info.isDegOrd_ = true;
    info.clearPreprocInfo();
    EXPECT_EQ(info.LB_0_no_AMTS_, 0);
    EXPECT_EQ(info.LB_0_AMTS_, 0);
    EXPECT_EQ(info.branchingRootSize_, 0);
    EXPECT_EQ(info.idSortReal_, -1);
    EXPECT_FALSE(info.isDegOrd_);
}

TEST_F(InfoCLQTest, ClearSearchInfo) {
    info.incumbent_ = 10;
    info.optimum_ = 20;
    info.nSteps_ = 100;
    info.isTimeOutReached_ = true;
    info.sol_ = { 1, 2, 3 };
    info.nLastIsetPreFilterCalls_ = 10;
    info.nLastIsetCalls_ = 20;
    info.nCurrIsetCalls_ = 30;
    info.nVertexCalls_ = 40;
    info.nUBpartCalls_ = 50;
    info.nsLastIsetPreFilterCalls_ = 60;
    info.nsLastIsetCalls_ = 70;
    info.nsCurrIsetCalls_ = 80;
    info.nsVertexCalls_ = 90;
    info.nsUBpartCalls_ = 100;
    info.clearSearchInfo();
    EXPECT_EQ(info.incumbent_, 0);
    EXPECT_EQ(info.optimum_, 0);
    EXPECT_EQ(info.nSteps_, 0);
    EXPECT_FALSE(info.isTimeOutReached_);
    EXPECT_TRUE(info.sol_.empty());
    EXPECT_EQ(info.nLastIsetPreFilterCalls_, 0);
    EXPECT_EQ(info.nLastIsetCalls_, 0);
    EXPECT_EQ(info.nCurrIsetCalls_, 0);
    EXPECT_EQ(info.nVertexCalls_, 0);
    EXPECT_EQ(info.nUBpartCalls_, 0);
    EXPECT_EQ(info.nsLastIsetPreFilterCalls_, 0);
    EXPECT_EQ(info.nsLastIsetCalls_, 0);
    EXPECT_EQ(info.nsCurrIsetCalls_, 0);
    EXPECT_EQ(info.nsVertexCalls_, 0);
    EXPECT_EQ(info.nsUBpartCalls_, 0);
}

TEST_F(InfoCLQTest, Clear) {
    info.incumbent_ = 10;
    info.optimum_ = 20;
    info.nSteps_ = 100;
    info.isTimeOutReached_ = true;
    info.sol_ = { 1, 2, 3 };
    info.LB_0_no_AMTS_ = 10.0;
    info.LB_0_AMTS_ = 20.0;
    info.branchingRootSize_ = 5;
    info.idSortReal_ = 1;
    info.isDegOrd_ = true;
    info.clear(false);
    EXPECT_EQ(info.incumbent_, 0);
    EXPECT_EQ(info.optimum_, 0);
    EXPECT_EQ(info.nSteps_, 0);
    EXPECT_FALSE(info.isTimeOutReached_);
    EXPECT_TRUE(info.sol_.empty());
    EXPECT_EQ(info.LB_0_no_AMTS_, 0);
    EXPECT_EQ(info.LB_0_AMTS_, 0);
    EXPECT_EQ(info.branchingRootSize_, 0);
    EXPECT_EQ(info.idSortReal_, -1);
    EXPECT_FALSE(info.isDegOrd_);
}

TEST_F(InfoCLQTest, PrintResults) {
    std::ostringstream oss;
    info.printResults(oss);
    EXPECT_FALSE(oss.str().empty());
}

TEST_F(InfoCLQTest, PrintTable) {
    std::ostringstream oss;
    info.printTable(oss);
    EXPECT_FALSE(oss.str().empty());
}
