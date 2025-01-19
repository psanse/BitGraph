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

using namespace com;


class InfoCLQTest : public ::testing::Test {
protected:   
    void SetUp() override {
        // Initialize any necessary data here
        info.nameInstance_ = "test";
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
    EXPECT_EQ(info.lbRootNoHeur_, 0);
    EXPECT_EQ(info.lbRootHeur_, 0);
    EXPECT_EQ(info.branchingRootSize_, 0);
    EXPECT_EQ(info.idSortReal_, -1);
    EXPECT_FALSE(info.isDegOrd_);

    //search info
    EXPECT_EQ(info.incumbent_, 0);
    EXPECT_EQ(info.optimum_, 0);
    EXPECT_EQ(info.nSteps_, 0);
    EXPECT_FALSE(info.isTimeOutReached_);
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

    info.lbRootNoHeur_ = 10.0;
    info.lbRootHeur_ = 20.0;
    info.branchingRootSize_ = 5;
    info.idSortReal_ = 1;
    info.isDegOrd_ = true;
    
    info.incumbent_ = 10;
    info.optimum_ = 20;
    info.nSteps_ = 100;
    info.isTimeOutReached_ = true;
    info.sol_ = { 1, 2, 3 };

	//lazy cleaning - does not clean general info
    info.clear(true);

    EXPECT_EQ(info.lbRootNoHeur_, 0);
    EXPECT_EQ(info.lbRootHeur_, 0);
    EXPECT_EQ(info.branchingRootSize_, 0);
    EXPECT_EQ(info.idSortReal_, -1);
    EXPECT_FALSE(info.isDegOrd_);

    EXPECT_EQ(info.incumbent_, 0);
    EXPECT_EQ(info.optimum_, 0);
    EXPECT_EQ(info.nSteps_, 0);
    EXPECT_FALSE(info.isTimeOutReached_);
    EXPECT_TRUE(info.sol_.empty());

	//general info available
    EXPECT_EQ(10, info.N_);
	EXPECT_EQ(20, info.M_);
	EXPECT_STREQ("test", info.nameInstance_.c_str());
  
    //cleans general info
    info.clear(false);

    //general info unavailable
    EXPECT_EQ(0, info.N_);
    EXPECT_EQ(0, info.M_);
    EXPECT_TRUE(info.nameInstance_.empty());
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

