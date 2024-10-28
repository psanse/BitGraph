
#include "gtest/gtest.h"
#include "../result.h"
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;
typedef unsigned long long ull;

TEST(Result, time){
	cout<<"Result::time------------------"<<endl;
	Result r;
	r.tic();
	for( ull i=0; i<5e8; i++){}
	double elapsed_time=r.toc();
	cout<<"elapsed time: "<<elapsed_time<<endl;
	EXPECT_TRUE(r.get_user_time()==elapsed_time);
	EXPECT_TRUE(elapsed_time>0);
	cout<<"--------------------------------------"<<endl;
}

TEST(Result, solutions){
	cout<<"Result::solutions------------------"<<endl;
	Result r;
	vector<uint> sol1(10,1);
	vector<uint> sol2(10,2);
	vector<uint> sol3(10,3);
	r.add_solution(sol1);
	r.add_solution(sol2);
	r.add_solution(sol3);

	EXPECT_EQ(3,r.get_number_of_solutions());
	EXPECT_EQ(sol1,r.get_first_solution());
	
	vector<vector<uint> > vsol=r.get_all_solutions();
	EXPECT_EQ(vsol[2],sol3);
	
	cout<<"--------------------------------------"<<endl;
}

TEST(Result, counters){
	cout<<"Result::counters------------------"<<endl;
	Result r;
	uint index=r.inc_counter();
	EXPECT_EQ(index, 0);

	uint index_1=r.inc_counter(1, 3);
	EXPECT_EQ(1,index_1);
	EXPECT_EQ(3, r.get_counter_value(index_1));
	
	uint index_2 =r.inc_counter(5, 5);
	EXPECT_EQ(2,index_2);
	EXPECT_EQ(5, r.get_counter_value(index_2));

	r.inc_counter(index_2, 3);
	EXPECT_EQ(8, r.get_counter_value(index_2));
	
	cout<<"--------------------------------------"<<endl;
}
