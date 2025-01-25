/**
* @file test_logger.cpp
* @brief Unit tests for the basic logy v1.2 logger in logger.h file
*		 Giovanni Squillero <giovanni.squillero@polito.it> (Summer 2018)
* @date 06/11/2024
* @last_update 25/01/25
* @author copilot
*
* TODO - no real tests, just visual inspection
**/

#include "utils/logger.h"
#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;


 //Test _Debug log function
TEST(LoggerTest, DebugLog) {
	_Debug(" %s: debug message", "Test Debug Log");
	LOG_DEBUG("Test Debug Log: debug message");													//not possible to use format string with this and the rest of macros
	
	//EXPECT_TRUE(OutputContains("DEBUG:", "Test Debug Log: debug message"));
}

// Test _Info log function
TEST(LoggerTest, InfoLog) {
	_Info("Test info Log: info message");
	LOG_INFO ("Test info Log: info message");
	
	//EXPECT_TRUE(OutputContains("INFO:", "Test info Log: info message"));
}

// Test _Warning log function
TEST(LoggerTest, WarningLog) {
	_Warning("Test Warning Log: warning message");
	LOG_WARNING("Test Warning Log: warning message");
	
	//EXPECT_TRUE(OutputContains("WARNING:", "Test Warning Log: warning message"));
}

// Test _Error log function
TEST(LoggerTest, ErrorLog) {
	_Error("Test Error Log: error occurred");
	LOG_ERROR("Test Error Log: error occurred");
	
	//EXPECT_TRUE(OutputContains("ERROR:", "Test Error Log: error occurred"));
}

// Test direct print log function _Debug2
TEST(LoggerTest, DebugLogDirectPrint) {
	_Debug2("Direct print Debug log", 123);
	LOGG_DEBUG("Direct print Debug log", 123);
	
	//EXPECT_TRUE(OutputContains("DEBUG:", "Direct print Debug log 123"));
}

// Test direct print log function _Info2
TEST(LoggerTest, InfoLogDirectPrint) {
	_Info2("Direct print Info log", 456);
	LOGG_INFO("Direct print Info log", 456);
	
	//EXPECT_TRUE(OutputContains("INFO:", "Direct print Info log 456"));
}

// Test direct print log function _Warning2
TEST(LoggerTest, WarningLogDirectPrint) {
	_Warning2("Direct print Warning log", 789);
	LOGG_INFO("Direct print Warning log", 789);
	
	//EXPECT_TRUE(OutputContains("WARNING:", "Direct print Warning log 789"));
}

// Test direct print log function _Error2
TEST(LoggerTest, ErrorLogDirectPrint) {
	_Error2("Direct print Error log", 199);
	LOGG_INFO("Direct print Error log", 199);
	
	//EXPECT_TRUE(OutputContains("ERROR:", "Direct print Error log critical issue"));
}


