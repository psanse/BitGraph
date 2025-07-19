/**
* @file test_thread.cpp
* @brief Unit tests for thread interface in thread.h file
* @details created 25/01/2025, last_update 19/07/2025
* @dev pss
*
* TODO - 1) Add tests with params, 2) revise ThreadExample test (19/07/2025)
**/

#include "gtest/gtest.h"
#include "utils/thread.h"
#include <thread>

using namespace bitgraph;


TEST(run_thread, callable) {

	struct functor {
		void operator()() {
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
		}
	};

	//////////////////////////////////////
	functor f;
	run_thread(f);
	//////////////////////////////////////
}

TEST(run_thread, member_function) {

	class X {
	public:
		void run() {
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
		}
	};


	X x;
	//////////////////////////////////////
	run_thread(&X::run, x);
	//////////////////////////////////////
}

//////////////////////////
// CHECK THIS CODE (19/07/25)

//class ThreadExample final{
//	
//public:
//	ThreadExample(int d):res_(d){};
//		
//	/**
//	* @brief Mock task to be run by the thread
//	**/
//	int task_mul_by_2 (){
//			
//		//task
//		res_ *= 2;
//
//		//sleep for 0.2s
//		std::this_thread::sleep_for(std::chrono::milliseconds(200));
//
//		//LOG_DEBUG("Thread finished");
//
//		return res_;
//	};
//
//	
//	/**
//	* @brief driver to execute member function task_mul_by_2 in a new thread
//	*		 and wait for the thread to finish
//	* @returns result of task_mul_by_2
//	**/
//	int Start(double tout_ms){
//		using Tf = int(ThreadExample::*)();
//		Thread<ThreadExample, Tf>::Start(&ThreadExample::task_mul_by_2, this);
//		return res_;
//	}
//
//private:
////////////
//	int res_;
//};	

//TEST(Thread, basic){
//	
//	ThreadExample t(5);
//	int retVal= t.Start(1000);
//	EXPECT_EQ(10, retVal);			//finished correctly
//
//	//LOG_DEBUG("passed through here");
//
//	//retVal = t.Start(4000);
//	//EXPECT_EQ(0, retVal);			//finished correctly
//
//}