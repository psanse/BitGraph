//logger.h: a light logger class (no timestamps, no file configuration, no multiple IDs)
//last update: 24/02/15

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

enum {LOGGER_ERROR=0,LOGGER_WARNING,LOGGER_INFO,LOGGER_PRINT, LOGGER_DEBUG};

//complete log level
#define LOG_ERROR(msg)		Logger(LOGGER_ERROR).Log()<<msg
#define LOG_WARNING(msg)	Logger(LOGGER_WARNING).Log()<<msg
#define LOG_INFO(msg)		Logger(LOGGER_INFO).Log()<<msg
#define LOG_PRINT(msg)		Logger(LOGGER_PRINT).Log()<<msg
#define LOG_DEBUG(msg)		Logger(LOGGER_DEBUG).Log()<<msg

////////////////////
#define LOG_PAK()			Logger(LOGGER_ERROR).Log()<<"press any key to continue"
#define LOG_LINE()			Logger(LOGGER_ERROR).Log()<<"-------------------------"

class Logger {
public:
	Logger(int type); 
	virtual ~Logger();

	ostringstream& Log(){return os;}

	//redirects / appends std::cout to ofstream. 
	static void SetFileStream(std::string filename="");
	static void CleanFileStream();

	//duplicates output to ostringstream
	static void SetStringStream(bool set=true){use_string_stream=set;}
	static void SetInformationLevel(int type=LOGGER_PRINT)	{predefined_type=type;}

	static std::string GetString(){
		string str=string_stream.str();
		string_stream.str("");
		return str;
	}
private:
	int set_type;								//type of the current log
		
protected:
	
	ostringstream os;
	static string filename;						/* added in 5/1/17 */
	static ofstream file;
	static ostringstream string_stream;
	static bool use_string_stream;
	static int predefined_type;					//configured type for the Logger pattern

};


#endif 

