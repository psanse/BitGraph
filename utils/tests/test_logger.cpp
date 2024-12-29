//test_logger.cpp: unit tests for LOGY logger in logger.h file
//
//@date: 06/11/2024
//@comments: done by copilot

#include "../logger.h"
#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

class LoggerTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Redirect stderr to a temporary file for capturing output
		tmp_filename = "temp_log_output.txt";
		freopen(tmp_filename.c_str(), "w", stderr);
	}

	void TearDown() override {
		// Close and delete the temporary file
		fclose(stderr);
		remove(tmp_filename.c_str());
	}

	std::string GetCapturedOutput() {
		// Flush stderr to ensure all output is written to the file
		fflush(stderr);

		// Read the content of the temporary file
		std::ifstream file(tmp_filename);
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		// Reopen stderr to the console to avoid affecting other tests
		freopen("/dev/tty", "a", stderr);

		return content;
	}

	bool OutputContains(const std::string& expected_prefix, const std::string& expected_content) {
		std::string output = GetCapturedOutput();
		return (output.find(expected_prefix) != std::string::npos) && (output.find(expected_content) != std::string::npos);
	}

	std::string tmp_filename;
};

// Test _Debug log function
TEST_F(LoggerTest, DebugLog) {
	_Debug("Test Debug Log: %s", "debug message");
	EXPECT_TRUE(OutputContains("DEBUG:", "Test Debug Log: debug message"));
}

// Test _Info log function
TEST_F(LoggerTest, InfoLog) {
	_Info("Test Info Log: %d", 42);
	EXPECT_TRUE(OutputContains("INFO:", "Test Info Log: 42"));
}

// Test _Warning log function
TEST_F(LoggerTest, WarningLog) {
	_Warning("Test Warning Log: %f", 3.14);
	EXPECT_TRUE(OutputContains("WARNING:", "Test Warning Log: 3.14"));
}

// Test _Error log function
TEST_F(LoggerTest, ErrorLog) {
	_Error("Test Error Log: %s", "error occurred");
	EXPECT_TRUE(OutputContains("ERROR:", "Test Error Log: error occurred"));
}

// Test direct print log function _Debug2
TEST_F(LoggerTest, DebugLogDirectPrint) {
	_Debug2("Direct print Debug log", 123);
	EXPECT_TRUE(OutputContains("DEBUG:", "Direct print Debug log 123"));
}

// Test direct print log function _Info2
TEST_F(LoggerTest, InfoLogDirectPrint) {
	_Info2("Direct print Info log", 456);
	EXPECT_TRUE(OutputContains("INFO:", "Direct print Info log 456"));
}

// Test direct print log function _Warning2
TEST_F(LoggerTest, WarningLogDirectPrint) {
	_Warning2("Direct print Warning log", 789);
	EXPECT_TRUE(OutputContains("WARNING:", "Direct print Warning log 789"));
}

// Test direct print log function _Error2
TEST_F(LoggerTest, ErrorLogDirectPrint) {
	_Error2("Direct print Error log", "critical issue");
	EXPECT_TRUE(OutputContains("ERROR:", "Direct print Error log critical issue"));
}


////////////////
//Unit tests for deprecated logger

//#define LOGGER_FILE_A "log_a.txt"
//#define LOGGER_FILE_B "log_b.txt"
//
//TEST(Logger, loglevels){
//	cout<<"Logger:------------------------"<<endl;
//
//	Logger::SetInformationLevel(LOGGER_DEBUG);		//base level of logs
//	LOG_DEBUG("LOG DEBUG ****");
//	LOG_PRINT("LOG PRINT ****");
//	LOG_INFO("LOG INFO ****");
//	LOG_WARNING("LOG WARNING ****");
//	LOG_ERROR("LOG ERROR ****");
//
//	Logger::SetInformationLevel(LOGGER_ERROR);		//base level of logs
//	LOG_DEBUG("LOG DEBUG ****");
//	LOG_PRINT("LOG PRINT ****");
//	LOG_INFO("LOG INFO ****");
//	LOG_WARNING("LOG WARNING ****");
//	LOG_ERROR("LOG ERROR ****");
//
//	//duplicates output to stringstream in memory
//	Logger::SetStringStream(true);
//	
//	//redirects output to file
//	Logger::SetFileStream(LOGGER_FILE_A);
//	LOG_ERROR("LOG ERROR TO FILE ****");
//
//	//redirects output to another file
//	Logger::SetFileStream(LOGGER_FILE_B);
//	LOG_ERROR("LOG ERROR TO FILE ****");
//
//	//prints duplicated output
//	cout<<"******* LOG OUTPUT IN MEMORY ******* \n"<<Logger::GetString();
//	Logger::SetStringStream("");
//
//	cout<<"------------------------------"<<endl;
//}