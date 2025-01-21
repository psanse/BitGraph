/**
 * @file test_infoAnalyzer.cpp
 * @brief Unit tests for the InfoAnalyser class that manages benchmarking graph algorithms. *		  
 * @date  2013
 * @last_update 20/01/2025
 * @author pss
  **/

#include "gtest/gtest.h"
#include "utils/info_analyser.h"
#include "utils/info/info_clq.h"
#include "utils/file.h"
#include <thread>

#define NUM_REP	5
#define NUM_ALG	2
#define INSTANCE_NAME	"graph"


TEST(InfoAnalyser, basic){
		
	InfoAnalyser<com::infoCLQ<int>> ta;

	for(auto r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(auto a = 0;  a < NUM_ALG; ++a){
			com::infoCLQ<int> info;
			info.nSteps_=10;
			info.ub_=50;
			info.lb_=10;

			info.startTimer(com::infoBase::SEARCH);
			
#ifdef _MSC_VER 
			std::this_thread::sleep_for(std::chrono::milliseconds(100));  
#elif __linux__
			sleep(0.2);
#endif
			info.readTimer(com::infoBase::SEARCH);
					
			info.name_= INSTANCE_NAME;					//same instance
			
			/////////////////////////////
			ta.add_test(new_rep, info);					//first time in the loop new repetition
			//////////////////////////////

			new_rep = false;							//rest of times in the loop same repetition, different algorithm
		}
	}
	
	//////////////////
	ta.analyser();
	/////////////////

	int errorIdx = -1;

	//////////////////////////////////////////////////
	EXPECT_TRUE(ta.consistent_sol_val(errorIdx) );
	EXPECT_EQ(-1, errorIdx);							//no error, all solutions are the same, with default values
	EXPECT_DOUBLE_EQ(10, ta.get_steps()[0]);
	EXPECT_DOUBLE_EQ(10, ta.get_steps()[1]);
	///////////////////////////////////////////////////

	//I/O
	/////////////////////
	cout << ta << endl;									//reports results 
	////////////////////
}


TEST(InfoAnalyser, info){
		
	InfoAnalyser<com::infoCLQ<int>> ta;

	for(int r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(int a = 0; a < NUM_ALG - 1; ++a){
			com::infoCLQ<int> info;		
			info.nSteps_ = 10;
			info.ub_ = 50;
			info.lb_ = 10;
		
			info.name_ = INSTANCE_NAME;					//same instance
			ta.add_test(new_rep, info);
			new_rep = false;
		}

		//last algorithm (change the number of steps)
		com::infoCLQ<int> info;
		info.nSteps_ = 11;
		info.ub_ = 50;
		info.lb_ = 10;

		info.name_ = INSTANCE_NAME;					//same instance		
		ta.add_test(new_rep, info);
	}
				
	decltype(ta)::info_t info;

	///////////////////
	ta.analyser(&info);
	///////////////////

	EXPECT_TRUE(info.same_sol);
	EXPECT_FALSE(info.same_steps);
	EXPECT_TRUE(info.same_lb);
	
}


TEST(TestAnalyser, all_fail){
		
	InfoAnalyser<com::infoCLQ<int>> ta;

	for(auto r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(auto a = 0; a < NUM_ALG; ++a){
			com::infoCLQ<int> info;
			info.nSteps_ = 10;
			info.ub_ = 50;
			info.lb_ = 10;
			info.isTimeOut_ = true;						//all fail
			info.name_=INSTANCE_NAME;
			
			ta.add_test(new_rep, info);
			new_rep=false;
		}
	}
	
	//////////////////
	ta.analyser();
	/////////////////

	int errorIdx = -1;
	EXPECT_TRUE(ta.consistent_sol_val( errorIdx) );			//2 algorithms all fail

	//I/O
	/////////////////////
	cout << ta << endl;									//reports results - no report expected here
	/////////////////////
	
}

TEST(TestAnalyser, only_one_test_and_fails){
		
	InfoAnalyser<com::infoCLQ<int>> ta;
	
	for(auto r = 0; r < 1; ++r){			//one test only
		bool new_rep = true;
		for(auto a = 0; a < NUM_ALG; ++a){
			com::infoCLQ<int> info;
			info.nSteps_ = 10;
			info.ub_ = 50;
			info.lb_ = 10;
			info.isTimeOut_ = true;						//all fail
			info.name_= INSTANCE_NAME;
			

			ta.add_test(new_rep, info);
			new_rep = false;
		}
	}
	
	//////////////////
	ta.analyser();
	/////////////////

	int errorIdx = -1;
	EXPECT_TRUE(ta.consistent_sol_val( errorIdx) );		//1 algorithm only
	
	//I/O
	/////////////////////
	cout << ta << endl;									//reports results 
	/////////////////////
}