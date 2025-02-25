
#include "gtest/gtest.h"
#include "../result.h"
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;
using ull = unsigned long long;

//TODO - CHECK DISABLED TEST - time (16/02/2025)

TEST(Result, solutions){
	
	Result r;
	vector<usint> sol1(10,1);
	vector<usint> sol2(10,2);
	vector<usint> sol3(10,3);
	r.add_solution(sol1);
	r.add_solution(sol2);
	r.add_solution(sol3);

	EXPECT_EQ(3,r.get_number_of_solutions());
	EXPECT_EQ(sol1,r.get_first_solution());
	
	vector<vector<usint> > vsol=r.get_all_solutions();
	EXPECT_EQ(vsol[2],sol3);
	
}

TEST(Result, counters){
	
	Result r;
	usint index=r.inc_counter();
	EXPECT_EQ(index, 0);

	usint index_1=r.inc_counter(1, 3);
	EXPECT_EQ(1,index_1);
	EXPECT_EQ(3, r.get_counter_value(index_1));
	
	usint index_2 =r.inc_counter(5, 5);
	EXPECT_EQ(2,index_2);
	EXPECT_EQ(5, r.get_counter_value(index_2));

	r.inc_counter(index_2, 3);
	EXPECT_EQ(8, r.get_counter_value(index_2));
	
}

TEST(Result, DISABLED_time) {

	Result r;
	r.tic();
	for (ull i = 0; i < 5e8; i++) {}
	double elapsed_time = r.toc();
	cout << "elapsed time: " << elapsed_time << endl;
	EXPECT_TRUE(r.get_user_time() == elapsed_time);
	EXPECT_TRUE(elapsed_time > 0);

}
