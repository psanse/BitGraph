#include "gtest/gtest.h"
#include "../test_analyser.h"
#include "../file.h"

#define NUM_REP	5
#define NUM_ALG	2
#define INSTANCE_NAME	"graph"

TEST(TestAnalyser, basic){
	cout<<"TestAnalyser::basic------------------"<<endl;
	
	TestAnalyser ta;
	for(int r=0; r<NUM_REP; r++){
		bool new_rep=true;
		for(int a=0; a<NUM_ALG; a++){
			Result res;
			res.set_number_of_steps(10);
			res.set_UB(50);
			res.set_LB(10);
			res.tic();
#ifdef _MSC_VER 
			Sleep(200);
#elif __linux__
			sleep(0.2);
#endif
			res.toc();
			//res.set_is_tout(true);
			res.set_name(INSTANCE_NAME);				//same instance
			res.inc_counter(0);
			res.inc_counter(1,7);
			ta.add_test(new_rep, res);
			new_rep=false;
		}
	}
	
	ta.analyser();
	int error;
	EXPECT_TRUE(ta.is_consistent_sol(error));
	EXPECT_EQ(-1, error);				//no valid error index
	cout<<ta<<endl;
	
	cout<<"-------------------------------"<<endl;
}

TEST(TestAnalyser, info){
	cout<<"TestAnalyser::info------------------"<<endl;
	
	TestAnalyser ta;
	for(int r=0; r<NUM_REP; r++){
		bool new_rep=true;
		for(int a=0; a<NUM_ALG-1; a++){
			Result res;
			res.set_number_of_steps(10);
			res.set_UB(50);
			res.set_LB(10);

			res.set_name(INSTANCE_NAME);				//same instance
			ta.add_test(new_rep, res);
			new_rep=false;
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

	cout<<"-------------------------------"<<endl;
}

TEST(TestAnalyser, all_fail){
	cout<<"TestAnalyser::all_fail------------------"<<endl;
	
	TestAnalyser ta;
	for(int r=0; r<NUM_REP; r++){
		bool new_rep=true;
		for(int a=0; a<NUM_ALG; a++){
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
	int error;
	EXPECT_TRUE(ta.is_consistent_sol(error));			//2 algorithms all fail
	cout<<ta<<endl;										//nothing is shown becuase there are more than
	
	cout<<"-------------------------------"<<endl;
}

TEST(TestAnalyser, only_one_test_and_fails){
	cout<<"TestAnalyser::only_one_and_fails------------------"<<endl;
	
	TestAnalyser ta;
	for(int r=0; r<1; r++){			//one test only
		bool new_rep=true;
		for(int a=0; a<NUM_ALG; a++){
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
	int error;
	EXPECT_TRUE(ta.is_consistent_sol(error));			//1 algorithm only
	cout<<ta<<endl;										//shows results 
	
	cout<<"-------------------------------"<<endl;
}