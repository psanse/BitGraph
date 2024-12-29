//test_precise_timer.cpp: unit tests for precision timer 
//
//
//@date: 06/11/2024
//@comments: done by copilot


#include "gtest/gtest.h"
#include "../prec_timer.h"
#include <math.h>
#include <thread> // for std::this_thread::sleep_for


using namespace std;

class PrecisionTimerTest : public ::testing::Test {
protected:
	PrecisionTimer timer;
};

// Test wall_tic and wall_toc
TEST_F(PrecisionTimerTest, WallTimeMeasurement) {
	timer.wall_tic();                             // Start wall clock
	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for 100 ms
	double elapsed = timer.wall_toc();            // Measure elapsed time
	EXPECT_GE(elapsed, 0.1);                      // Expect at least 0.1 seconds
}

// Test cpu_tic and cpu_toc
TEST_F(PrecisionTimerTest, CpuTimeMeasurement) {
	timer.cpu_tic();                              // Start CPU timer
	std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate some delay
	double elapsed = timer.cpu_toc();             // Measure elapsed time
	EXPECT_GE(elapsed, 0);                        // Expect a non-negative elapsed time
}

// Test local_timestamp with date and without date
TEST_F(PrecisionTimerTest, LocalTimestampWithDate) {
	std::string timestamp = PrecisionTimer::local_timestamp(true); // Test with date
	EXPECT_FALSE(timestamp.empty());              // Ensure it's not empty
}

TEST_F(PrecisionTimerTest, LocalTimestampWithoutDate) {
	std::string timestamp = PrecisionTimer::local_timestamp(false); // Test without date
	EXPECT_FALSE(timestamp.empty());              // Ensure it's not empty
}


