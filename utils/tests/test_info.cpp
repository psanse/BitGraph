/*
* test_info.cpp: tests the info_base class
 *
 *@date: 15/12/2024
 * 
 */

#include "gtest/gtest.h"
#include "utils/info/info_base.h"
#include "utils/info/info_clq.h"
#include <thread>
#include <iostream>
#include <sstream>

using namespace com;

class InfoBaseTest : public ::testing::Test {
protected:
	infoBase info;

	void SetUp() override {
		// Initialize any necessary data here
	}

	void TearDown() override {
		// Clean up any necessary data here
	}
};

class InfoCLQTest : public ::testing::Test {
protected:
	infoCLQ<int> infoCLQ;

	void SetUp() override {
		// Initialize any necessary data here
	}

	void TearDown() override {
		// Clean up any necessary data here
	}
};

TEST_F(InfoBaseTest, DefaultConstructor) {
	
	EXPECT_EQ(info.nameInstance_, "");
	EXPECT_EQ(info.N_, 0);
	EXPECT_EQ(info.M_, 0);
	EXPECT_EQ(info.K_, 0);
	EXPECT_EQ(info.TIME_OUT_	 , std::numeric_limits<double>::max());
	EXPECT_EQ(info.TIME_OUT_HEUR_, std::numeric_limits<double>::max());
	EXPECT_EQ(info.idAlg_, -1);
	EXPECT_EQ(info.idHeur_, -1);
	EXPECT_EQ(info.idSort_, -1);
}

TEST_F(InfoBaseTest, StartTimer) {

	info.startTimer(infoBase::phase_t::SEARCH);
	EXPECT_NO_THROW(info.startTimer(infoBase::phase_t::SEARCH));
}

TEST_F(InfoBaseTest, ReadTimer) {

	info.startTimer(infoBase::phase_t::SEARCH);
	std::this_thread::sleep_for (std::chrono::duration<double>(0.3));
	double elapsed = info.readTimer(infoBase::phase_t::SEARCH);

	EXPECT_GE(elapsed, 0.25);
	EXPECT_LE(elapsed, 0.35);

	//test other timers...
}

TEST_F(InfoBaseTest, ElapsedTime) {

	auto start_time = std::chrono::high_resolution_clock::now();
	double elapsed = info.elapsedTime(start_time);

	EXPECT_GE(elapsed, 0.0);
}

TEST_F(InfoBaseTest, ClearGeneralInfo) {

	info.N_ = 10;
	info.M_ = 20;
	info.clear();

	EXPECT_EQ(info.N_, 0);
	EXPECT_EQ(info.M_, 0);
}

TEST_F(InfoBaseTest, ClearTimers) {

	info.clear(false);

	EXPECT_DOUBLE_EQ(info.timeParse_, 0.0);
	EXPECT_DOUBLE_EQ(info.timePreproc_, 0.0);
	EXPECT_DOUBLE_EQ(info.timeSearch_, 0.0);
	EXPECT_DOUBLE_EQ(info.timeIncumbent_, 0.0);
}

TEST_F(InfoBaseTest, Clear) {

	info.N_ = 10;
	info.M_ = 20;

	//clears all - lazy = false
	info.clear();	

	EXPECT_EQ(info.N_, 0);
	EXPECT_EQ(info.M_, 0);	

	info.N_ = 10;
	info.M_ = 20;

	//preserves general info - lazy = true
	info.clear(true);

	EXPECT_EQ(info.N_, 10);
	EXPECT_EQ(info.M_, 20);


	//test other members...
}

TEST_F(InfoBaseTest, PrintParams) {

	std::ostringstream oss;
	info.printParams(oss);

	EXPECT_FALSE(oss.str().empty());
}

TEST_F(InfoBaseTest, PrintTimers) {

	std::ostringstream oss;
	info.printTimers(oss);

	EXPECT_FALSE(oss.str().empty());
}

TEST_F(InfoBaseTest, printReport) {

	std::ostringstream oss;
	info.printReport(oss);

	EXPECT_FALSE(oss.str().empty());
}

///////////////////////
//
// infoCLQ TESTS
//
//////////////////////


TEST_F(InfoCLQTest, DefaultConstructor) {

	//infoBase part
	EXPECT_EQ	(infoCLQ.nameInstance_, "");
	EXPECT_EQ	(infoCLQ.N_, 0);
	EXPECT_EQ	(infoCLQ.M_, 0);
	EXPECT_EQ	(infoCLQ.K_, 0);
	EXPECT_EQ	(infoCLQ.TIME_OUT_, std::numeric_limits<double>::max());
	EXPECT_EQ	(infoCLQ.TIME_OUT_HEUR_, std::numeric_limits<double>::max());
	EXPECT_EQ	(infoCLQ.idAlg_, -1);
	EXPECT_EQ	(infoCLQ.idHeur_, -1);
	EXPECT_EQ	(infoCLQ.idSort_, -1);

	//infoCLQ part
	EXPECT_EQ	(infoCLQ.lbRootNoHeur, 0);
	EXPECT_EQ	(infoCLQ.lbRootHeur, 0);
	EXPECT_EQ	(infoCLQ.branchingRootSize_, 0);
	EXPECT_EQ	(infoCLQ.idSortReal_, -1);
	EXPECT_EQ	(infoCLQ.isDegOrd_, -1);
	EXPECT_EQ	(infoCLQ.incumbent_, 0);
	EXPECT_EQ	(infoCLQ.optimum_, 0);
	EXPECT_EQ	(infoCLQ.nSteps_, 0);
	EXPECT_FALSE(infoCLQ.isTimeOutReached_);
	
}

TEST_F(InfoCLQTest, printSummary) {

	std::ostringstream ostr;
	infoCLQ.printSummary(ostr);

	EXPECT_FALSE(ostr.str().empty());
}

TEST_F(InfoCLQTest, DISABLED_IO) {

	//I/O - DISABLE, just shows the output
	infoCLQ.printParams();
	infoCLQ.printSummary();
	infoCLQ.printTimers();
	infoCLQ.printReport(std::cout, true);

}





