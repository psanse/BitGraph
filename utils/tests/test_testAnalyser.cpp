/*
 * @file test_testAnalyzer.cpp
 * @brief Unit tests for the TestAnalyser class that manages benchmarking graph algorithms. *		  
 * @date  2013
 * @last_update 20/01/2025
 * @author pss
 */

#include "gtest/gtest.h"
#include "utils/test_analyser.h"
#include "utils/file.h"
#include <thread>

using namespace std;

#define NUM_REP	5
#define NUM_ALG	2
#define INSTANCE_NAME	"graph"

TEST(TestAnalyser, basic){
		
	TestAnalyser ta;
	for(auto r = 0; r < NUM_REP; ++r){
		bool new_rep=true;
		for(auto a = 0;  a < NUM_ALG; ++a){
			Result res;
			res.set_number_of_steps(10);
			res.set_UB(50);
			res.set_LB(10);
			res.tic();
#ifdef _MSC_VER 
			std::this_thread::sleep_for(std::chrono::milliseconds(100));  
#elif __linux__
			sleep(0.2);
#endif
			res.toc();
			//res.set_is_tout(true);
			res.set_name(INSTANCE_NAME);				//same instance
			res.inc_counter(0);
			res.inc_counter(1,7);
			ta.add_test(new_rep, res);
			new_rep = false;
		}
	}
	
	ta.analyser();
	int errorIdx;

	EXPECT_TRUE(ta.consistent_sol_val(errorIdx) );
	EXPECT_EQ(-1, errorIdx);							//no valid error index

	//I/O
	/////////////////////
	cout << ta << endl;									//reports results 
	////////////////////
}

TEST(TestAnalyser, info){
	
	
	TestAnalyser ta;
	for(int r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(int a = 0; a < NUM_ALG - 1; ++a){
			Result res;
			res.set_number_of_steps(10);
			res.set_UB(50);
			res.set_LB(10);

			res.set_name(INSTANCE_NAME);				//same instance
			ta.add_test(new_rep, res);
			new_rep = false;
		}

		//last algorithm (change the number of steps)
		Result res;
		res.set_number_of_steps(11);
		res.set_UB(50);
		res.set_LB(10);

		res.set_name(INSTANCE_NAME);				
		ta.add_test(false, res);
	}
				
	TestAnalyser::info_t info;
	ta.analyser(&info);
	EXPECT_TRUE(info.same_sol);
	EXPECT_FALSE(info.same_steps);
	EXPECT_TRUE(info.same_lb);
	
}

TEST(TestAnalyser, all_fail){
		
	TestAnalyser ta;
	for(auto r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(auto a = 0; a < NUM_ALG; ++a){
			Result res;
			res.set_number_of_steps(10);
			res.set_UB(50);
			res.set_LB(10);
			res.set_is_tout(true);						//all fail
			res.set_name(INSTANCE_NAME);				
			res.inc_counter(0);
			res.inc_counter(1,7);
			ta.add_test(new_rep, res);
			new_rep=false;
		}
	}
	
	ta.analyser();
	int errorIdx;
	EXPECT_TRUE(ta.consistent_sol_val(errorIdx));			//2 algorithms all fail

	//I/O
	/////////////////////
	cout << ta << endl;									//reports results - no report expected here
	/////////////////////
	
}

TEST(TestAnalyser, only_one_test_and_fails){
		
	TestAnalyser ta;
	for(auto r = 0; r < 1; ++r){			//one test only
		bool new_rep = true;
		for(auto a = 0; a < NUM_ALG; ++a){
			Result res;
			res.set_number_of_steps(10);
			res.set_UB(50);
			res.set_LB(10);
			res.set_is_tout(true);						//all fail
			res.set_name(INSTANCE_NAME);				
			res.inc_counter(0);
			res.inc_counter(1,7);
			ta.add_test(new_rep, res);
			new_rep = false;
		}
	}
	
	ta.analyser();
	int errorIdx;
	EXPECT_TRUE(ta.consistent_sol_val(errorIdx));		//1 algorithm only
	
	//I/O
	/////////////////////
	cout << ta << endl;									//reports results 
	/////////////////////
}