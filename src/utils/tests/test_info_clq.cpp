/*
* test_info_clq.cpp: tests the infoCLQ class
 *
 *@date 15/12/2024
 *@comment automatic tests by copilot - TODO better tests
 * 
 */

#include "gtest/gtest.h"
#include "utils/info/info_clq.h"
#include <thread>

using namespace bitgraph;


class InfoCLQTest : public ::testing::Test {
protected:   
    void SetUp() override {
        // Initialize any necessary data here
        info.name_ = "test";
		info.N_ = 10;
		info.M_ = 20;
    }

    void TearDown() override {
        // Clean up any necessary data here
    }

    infoCLQ<double> info;
};

TEST_F(InfoCLQTest, DefaultConstructor) {
	
    //preproc info
    EXPECT_EQ(info.lbRootBasicHeur_, 0);
    EXPECT_EQ(info.lbRootStrongHeur_, 0);
    EXPECT_EQ(info.branchingFactorRoot_, 0);
    EXPECT_EQ(info.algSortReal_, -1);
    EXPECT_FALSE(info.isAlgSortRealDeg_);

    //search info
    EXPECT_EQ(info.lb_, 0);
    EXPECT_EQ(info.ub_, 0);
    EXPECT_EQ(info.nSteps_, 0);
    EXPECT_FALSE(info.isTimeOut_);
	EXPECT_TRUE(info.sol_.empty());   

   /* EXPECT_EQ(info.nLastIsetPreFilterCalls_, 0);
    EXPECT_EQ(info.nLastIsetCalls_, 0);
    EXPECT_EQ(info.nCurrIsetCalls_, 0);
    EXPECT_EQ(info.nVertexCalls_, 0);
    EXPECT_EQ(info.nUBpartCalls_, 0);
    EXPECT_EQ(info.nsLastIsetPreFilterCalls_, 0);
    EXPECT_EQ(info.nsLastIsetCalls_, 0);
    EXPECT_EQ(info.nsCurrIsetCalls_, 0);
    EXPECT_EQ(info.nsVertexCalls_, 0);
    EXPECT_EQ(info.nsUBpartCalls_, 0);*/
}

TEST_F(InfoCLQTest, Clear) {

    info.lbRootBasicHeur_ = 10.0;
    info.lbRootStrongHeur_ = 20.0;
    info.branchingFactorRoot_ = 5;
    info.algSortReal_ = 1;
    info.isAlgSortRealDeg_= true;
    
    info.lb_ = 10;
    info.ub_ = 20;
    info.nSteps_ = 100;
    info.isTimeOut_ = true;
    info.sol_ = { 1, 2, 3 };

	//lazy cleaning - does not clean general info
    info.clear(true);

    EXPECT_EQ(info.lbRootBasicHeur_, 0);
    EXPECT_EQ(info.lbRootStrongHeur_, 0);
    EXPECT_EQ(info.branchingFactorRoot_, 0);
    EXPECT_EQ(info.algSortReal_, -1);
    EXPECT_FALSE(info.isAlgSortRealDeg_);

    EXPECT_EQ(info.lb_, 0);
    EXPECT_EQ(info.ub_, 0);
    EXPECT_EQ(info.nSteps_, 0);
    EXPECT_FALSE(info.isTimeOut_);
    EXPECT_TRUE(info.sol_.empty());

	//general info available
    EXPECT_EQ(10, info.N_);
	EXPECT_EQ(20, info.M_);
	EXPECT_STREQ("test", info.name_.c_str());
  
    //cleans general info
    info.clear(false);

    //general info unavailable
    EXPECT_EQ(0, info.N_);
    EXPECT_EQ(0, info.M_);
    EXPECT_TRUE(info.name_.empty());
}



TEST_F(InfoCLQTest, printReport) {
    std::ostringstream oss;
    info.printReport(oss);
    EXPECT_FALSE(oss.str().empty());
}

TEST_F(InfoCLQTest, printSummary) {
    std::ostringstream oss;
    info.printSummary(oss);
    EXPECT_FALSE(oss.str().empty());
}

//not a real test, just shows the output
TEST_F(InfoCLQTest, DISABLED_IO) {
    	
    info.printParams();
    info.printSummary();
    info.printTimers();
    info.printReport(std::cout, true);

}

