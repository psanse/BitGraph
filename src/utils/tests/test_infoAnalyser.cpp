/**
 * @file test_infoAnalyzer.cpp
 * @brief Unit tests for the InfoAnalyser class that manages benchmarking graph algorithms. *		  
 * @date  2013
 * @last_update 20/01/2025
 * @author pss
 * 
 * TODO - add more tests... (23/01/25)
 * 
 **/

#include "gtest/gtest.h"
#include "utils/info_analyser.h"
#include "utils/info/info_base.h"

#include "utils/file.h"
#include <thread>

#define NUM_REP	5
#define NUM_ALG	2
#define INSTANCE_NAME	"graph"

using namespace std;
using namespace bitgraph;

class InfoAnalyserTest : public ::testing::Test {
protected:
	void SetUp() override {
		for (int r = 0; r < NUM_REP; ++r) {
			bool new_rep = true;

			for (int a = 0; a < NUM_ALG; ++a) {

				com::infoBase info;
				info.N_ = 5;
				info.M_ = 7;
				info.TIME_OUT_ = 1000;
				info.TIME_OUT_HEUR_ = 200;
				//info.nSteps_ = 10 + a;						//diff number of steps for each algorithm in all rep.
				//info.ub_ = 50;
				//info.lb_ = 10;

				info.name_ = INSTANCE_NAME;					//same instance
				ta.add_test(new_rep, info);
				new_rep = false;
			}			
		}
	}
	void TearDown() override {}
		
	InfoAnalyser<com::infoBase> ta;
};

TEST_F(InfoAnalyserTest, toFile) {

	string strReport("infoAnalyser.txt");

	EXPECT_EQ(NUM_REP, ta.number_of_repetitions());
	EXPECT_EQ(NUM_ALG, ta.number_of_algorithms());

	//streaming of the tests results 

	//I/O
	//ta.print_alg(FILE_LOG(strReport.c_str(), WRITE), -1);
	//ta.print_alg(FILE_LOG(strReport.c_str(), WRITE), 1);
	//ta.print_rep(FILE_LOG(strReport.c_str(), WRITE), 7 );

	//streaming of the analyser results (summary of averages for all the algorithms)
	ta.analyser();

	///////////////////////////////////////
	int algDiffId = -1;
	EXPECT_TRUE(ta.check_test_consistency());
	EXPECT_TRUE(ta.check_solution_values(algDiffId));
	//////////////////////////////////////

	///////////////////////////////////////
	EXPECT_EQ(10, ta.get_steps().at(0));
	EXPECT_EQ(11, ta.get_steps().at(1));
	///////////////////////////////////////

	//I/O
	//ta.print_analyser_summary(FILE_LOG(strReport.c_str(), WRITE));

	//streams to console the print_analyser_summary
	//cout << ta << endl;											
}

TEST(InfoAnalyser, basic){
		
	auto duration = std::chrono::milliseconds(50);
	InfoAnalyser<com::infoBase> ta;

	for(auto r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(auto a = 0;  a < NUM_ALG; ++a){
			com::infoBase info;
			/*info.nSteps_=10;
			info.ub_=50;
			info.lb_=10;*/

			info.startTimer(com::infoBase::SEARCH);
			
#ifdef _MSC_VER 
			std::this_thread::sleep_for(duration);
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
	int retVal = ta.analyser();
	EXPECT_EQ(0, retVal);
	EXPECT_TRUE(ta.check_test_consistency());			//similar to the previous test
	/////////////////

	int errorIdx = -1;

	//////////////////////////////////////////////////
	EXPECT_TRUE(ta.check_solution_values(errorIdx) );
	EXPECT_EQ(-1, errorIdx);							//no error, all solutions are the same, with default values
	EXPECT_DOUBLE_EQ(10, ta.get_steps()[0]);
	EXPECT_DOUBLE_EQ(10, ta.get_steps()[1]);
	///////////////////////////////////////////////////

	//I/O
	/////////////////////
	//cout << ta << endl;									//reports results 
	////////////////////
}

TEST(InfoAnalyser, info){
		
	InfoAnalyser<com::infoBase> ta;

	for(int r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(int a = 0; a < NUM_ALG - 1; ++a){
			com::infoBase info;		
			/*info.nSteps_ = 10;
			info.ub_ = 50;
			info.lb_ = 10;*/
		
			info.name_ = INSTANCE_NAME;					//same instance
			ta.add_test(new_rep, info);
			new_rep = false;
		}
				

		//last algorithm (change the number of steps)
		//com::infoBase info;
		//info.nSteps_ = 11;
		//info.ub_ = 50;
		//info.lb_ = 10;

		//info.name_ = INSTANCE_NAME;					//same instance		
		//ta.add_test(new_rep, info);
	}
				
	decltype(ta)::comp_t info;

	////////////////
	int retVal = ta.analyser(&info);
	EXPECT_EQ(0, retVal);
	EXPECT_TRUE(ta.check_test_consistency());			//similar to the previous test
	///////////////////

	EXPECT_TRUE(info.same_sol);
	EXPECT_FALSE(info.same_steps);
	EXPECT_TRUE(info.same_lb);
	
}


TEST(InfoAnalyser, all_timeout){
		
	InfoAnalyser<com::infoBase> ta;

	for(auto r = 0; r < NUM_REP; ++r){
		bool new_rep = true;
		for(auto a = 0; a < NUM_ALG; ++a){
			com::infoBase info;
			//info.nSteps_ = 10;
			//info.ub_ = 50;
			//info.lb_ = 10;
			//info.isTimeOut_ = true;						//all fail
			info.name_=INSTANCE_NAME;
			
			ta.add_test(new_rep, info);
			new_rep=false;
		}
	}
		
	
	//////////////////
	int retVal = ta.analyser();
	EXPECT_EQ(0, retVal);
	EXPECT_TRUE(ta.check_test_consistency());				//similar to the previous test
	/////////////////


	int algDiffId = -1;
	EXPECT_TRUE(ta.check_solution_values(algDiffId) );			//2 algorithms all fail

	//I/O
	/////////////////////
	//cout << ta << endl;									//reports results - no report expected here
	/////////////////////
	
}

TEST(InfoAnalyser, only_one_test_and_fails){
		
	InfoAnalyser<com::infoBase> ta;
	
	for(auto r = 0; r < 1; ++r){			//one test only
		bool new_rep = true;
		for(auto a = 0; a < NUM_ALG; ++a){
			com::infoBase info;
			//info.nSteps_ = 10;
			//info.ub_ = 50;
			//info.lb_ = 10;
			//info.isTimeOut_ = true;						//all fail
			info.name_= INSTANCE_NAME;
			

			ta.add_test(new_rep, info);
			new_rep = false;
		}
	}
	
	//////////////////
	int retVal = ta.analyser();
	EXPECT_EQ(0, retVal);
	////////////////

	int algDiffId = -1;
	EXPECT_TRUE(ta.check_solution_values(algDiffId) );		//1 algorithm only
	
	//I/O
	/////////////////////
	//cout << ta << endl;									//reports results 
	/////////////////////
}

