#include "gtest/gtest.h"
#include "utils/thread.h"
#include <thread>

class ThreadExample{
	
public:
	ThreadExample(int d):res_(d){};
		
	/**
	* @brief Mock task to be run by the thread
	**/
	int task_mul_by_2 (){

		LOG_DEBUG("Thread running");
		
		//task
		res_ *= 2;

		//sleep for 0.2s
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		LOG_DEBUG("Thread finished");

	return 0;
	};

	
	/**
	* @brief driver to execute member function  task_mul_by_2 in a new thread
	*		 and wait for the thread to finish
	* @returns 0 if successful, -1 otherwise
	**/
	int Start(double tout_ms){
		using Tf = int(ThreadExample::*)();
		int retVal = com::Thread<ThreadExample, Tf>::Start(&ThreadExample::task_mul_by_2, this);
		return retVal;
	}

//////////
	int res_;
};	

TEST(Thread, basic){
	
	ThreadExample t(5);
	int retVal= t.Start(1000);
	EXPECT_EQ(0, retVal);			//finished correctly

	LOG_DEBUG("passed through here");

	//retVal = t.Start(4000);
	//EXPECT_EQ(0, retVal);			//finished correctly


}