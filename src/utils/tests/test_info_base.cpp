/**
* test_info_base.cpp: tests the info_base class
 *
 *@date: 15/12/2024, last_update 25/08/2025
 * 
 **/

#include "gtest/gtest.h"
#include "utils/info/info_base.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <sstream>

using namespace bitgraph;

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


TEST_F(InfoBaseTest, DefaultConstructor) {
	
	EXPECT_EQ(info.name_, "");
	EXPECT_EQ(info.N_, 0);
	EXPECT_EQ(info.M_, 0);
	EXPECT_EQ(info.TIME_OUT_	 , std::numeric_limits<double>::max());
	EXPECT_EQ(info.TIME_OUT_HEUR_, std::numeric_limits<double>::max());
	/*EXPECT_EQ(info.algSearch_, -1);
	EXPECT_EQ(info.algHeur_, -1);
	EXPECT_EQ(info.algSort_, -1);*/
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

// Asegúrate de que los enums de phase_t estén definidos correctamente en infoBase

TEST_F(InfoBaseTest, StartAndReadTimerParse) {
	info.startTimer(infoBase::phase_t::PARSE);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	double elapsed = info.readTimer(infoBase::phase_t::PARSE);
	EXPECT_GE(elapsed, 0.09);
	EXPECT_LE(elapsed, 0.2);
}

TEST_F(InfoBaseTest, StartAndReadTimerPreproc) {
	info.startTimer(infoBase::phase_t::PREPROC);
	std::this_thread::sleep_for(std::chrono::milliseconds(80));
	double elapsed = info.readTimer(infoBase::phase_t::PREPROC);
	EXPECT_GE(elapsed, 0.07);
	EXPECT_LE(elapsed, 0.2);
}

TEST_F(InfoBaseTest, StartAndReadTimerIncumbent) {
	info.startTimer(infoBase::phase_t::LAST_INCUMBENT);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	double elapsed = info.readTimer(infoBase::phase_t::LAST_INCUMBENT);
	EXPECT_GE(elapsed, 0.04);
	EXPECT_LE(elapsed, 0.15);
}

TEST_F(InfoBaseTest, ClearTimerResetsIndividualTimer) {
	info.startTimer(infoBase::phase_t::SEARCH);
	std::this_thread::sleep_for(std::chrono::milliseconds(60));
	info.readTimer(infoBase::phase_t::SEARCH);
	info.clearTimer(infoBase::phase_t::SEARCH);
	EXPECT_DOUBLE_EQ(info.timeSearch_, 0.0);
}

TEST_F(InfoBaseTest, ClearTimersResetsAllTimers) {
	info.startTimer(infoBase::phase_t::PARSE);
	info.startTimer(infoBase::phase_t::PREPROC);
	info.startTimer(infoBase::phase_t::SEARCH);
	info.startTimer(infoBase::phase_t::LAST_INCUMBENT);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	info.readTimer(infoBase::phase_t::PARSE);
	info.readTimer(infoBase::phase_t::PREPROC);
	info.readTimer(infoBase::phase_t::SEARCH);
	info.readTimer(infoBase::phase_t::LAST_INCUMBENT);
	info.clearAllTimers();
	EXPECT_DOUBLE_EQ(info.timeParse_, 0.0);
	EXPECT_DOUBLE_EQ(info.timePreproc_, 0.0);
	EXPECT_DOUBLE_EQ(info.timeSearch_, 0.0);
	EXPECT_DOUBLE_EQ(info.timeIncumbent_, 0.0);
}

TEST_F(InfoBaseTest, ElapsedTimeStatic) {
	auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	double elapsed = infoBase::elapsedTime(start);
	EXPECT_GE(elapsed, 0.02);
	EXPECT_LE(elapsed, 0.1);
}





