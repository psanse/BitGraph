
#include <iostream>
#include <sstream>
#include "../logger.h"
#include "gtest/gtest.h"

using namespace std;

#define LOGGER_FILE_A "log_a.txt"
#define LOGGER_FILE_B "log_b.txt"

TEST(Logger, loglevels){
	cout<<"Logger:------------------------"<<endl;

	Logger::SetInformationLevel(LOGGER_DEBUG);		//base level of logs
	LOG_DEBUG("LOG DEBUG ****");
	LOG_PRINT("LOG PRINT ****");
	LOG_INFO("LOG INFO ****");
	LOG_WARNING("LOG WARNING ****");
	LOG_ERROR("LOG ERROR ****");

	Logger::SetInformationLevel(LOGGER_ERROR);		//base level of logs
	LOG_DEBUG("LOG DEBUG ****");
	LOG_PRINT("LOG PRINT ****");
	LOG_INFO("LOG INFO ****");
	LOG_WARNING("LOG WARNING ****");
	LOG_ERROR("LOG ERROR ****");

	//duplicates output to stringstream in memory
	Logger::SetStringStream(true);
	
	//redirects output to file
	Logger::SetFileStream(LOGGER_FILE_A);
	LOG_ERROR("LOG ERROR TO FILE ****");

	//redirects output to another file
	Logger::SetFileStream(LOGGER_FILE_B);
	LOG_ERROR("LOG ERROR TO FILE ****");

	//prints duplicated output
	cout<<"******* LOG OUTPUT IN MEMORY ******* \n"<<Logger::GetString();
	Logger::SetStringStream("");

	cout<<"------------------------------"<<endl;
}