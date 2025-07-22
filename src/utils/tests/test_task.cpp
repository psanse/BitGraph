/**
* @file test_task.cpp
* @brief Unit tests for async tasks interface in task.h file
* @details created 25/01/2025, last_update 21/07/2025
* @dev pss
*
* TODO - Check ThreadExample test  - possibly remove (19/07/2025)
**/

#include "gtest/gtest.h"
#include "utils/task.h"
#include <future>
#include <chrono>

using namespace bitgraph;

TEST(run_task, callable) {

	struct functor {
		int operator()(int a) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
			return a;
		}
	};

	//////////////////////////////////////
	//functor f;
	auto res = run_task_async(functor(), 5);
	//////////////////////////////////////

	EXPECT_EQ(res, 5);
}

TEST(run_task, member_function) {

	class X {
	public:
		int run() {
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
			return 0;
		}
	};


	X x;
	//////////////////////////////////////
	auto res = run_task_async(&X::run, x);
	//////////////////////////////////////

	EXPECT_EQ(res, 0);
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