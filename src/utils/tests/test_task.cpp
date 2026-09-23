/**
* @file test_task.cpp
* @brief Unit tests for async tasks interface in task.h file
* @details created 25/01/2025, last_update 21/07/2025
* @dev pss
*
* TODO - Check ThreadExample tests  - possibly remove (19/07/2025)
**/

#include "gtest/gtest.h"
#include "utils/task_utils.h"
#include <future>
#include <thread>
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

