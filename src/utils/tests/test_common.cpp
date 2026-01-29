/**
* @file test_common.cpp 
* @brief Testing of common utilities
* @details
*	- last_update 15/01/2026
* @date ?
* @author pss

* TODO - add tests, check disabled tests  (18/01/25)
**/

#include "utils/common.h"
#include "gtest/gtest.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>

using namespace std;
using namespace bitgraph;

//useful alias
namespace bitgraph {
	
	using vdob = std::vector<double>;
}

TEST(Common_stack, constructor) {

	bitgraph::com::FixedStack<int> s(3);
	s.push(10);
	s.push(20);
		
	bitgraph::com::FixedStack<int> s2 = std::move(s);

	EXPECT_EQ(2, s2.size());
	EXPECT_EQ(20, s2.top());
	EXPECT_EQ(10, s2.bottom());
}


TEST(Common_stack, forward_iteration) {

	bitgraph::com::FixedStack<int> s(3);
	s.push(10);
	s.push(20);
	s.push(30);

	std::vector<int> elems;

	//forward iteration
	for (int elem : s) {
		elems.push_back(elem);
	}

	EXPECT_EQ(3, elems.size());
	EXPECT_EQ(10, elems[0]);
	EXPECT_EQ(20, elems[1]);
	EXPECT_EQ(30, elems[2]);
}



TEST(Common_stack, backward_iteration) {

	bitgraph::com::FixedStack<int> s(3);
	s.push(10);
	s.push(20);
	s.push(30);

	std::vector<int> elems;

	//backward iteration
	for (auto it = s.rbegin(); it!=s.rend(); ++it) {
		elems.push_back(*it);
	}

	EXPECT_EQ(3, elems.size());
	EXPECT_EQ(30, elems[0]);
	EXPECT_EQ(20, elems[1]);
	EXPECT_EQ(10, elems[2]);

	//backward iteration safe with empty stack
	s.clear();
	elems.clear();

	for (auto it = s.rbegin(); it != s.rend(); ++it) {
		elems.push_back(*it);
	}

	EXPECT_EQ(0, elems.size());

}



TEST(Common_mat, mean_and_stdev) {

	//collection
	vdob col = { 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3 };
	
	//compute mean - implicit cast to double on return, MUST USE for_each and return double
	double mean = for_each(col.begin(), col.end(), _mat::MeanValue());					

	/////////////////////////////
	EXPECT_DOUBLE_EQ(3, mean);
	/////////////////////////////

	//compute standard deviation - implicit cast to double on return, MUST USE for_each and return double
	double stdDev = for_each(col.begin(), col.end(), _mat::StdDevValue(mean));			
	
	//compute the standard deviation in a more traditional way
	double stdDevExp = 0;
	for (auto elem : col) {
		stdDevExp += (mean - elem) * (mean - elem);
	}
	stdDevExp /= col.size();	
	stdDevExp = std::sqrt(stdDevExp);

	/////////////////////////////////////
	EXPECT_DOUBLE_EQ(stdDevExp, stdDev);
	/////////////////////////////////////
}

TEST(Common_counting, number_of_words){

	string str1("hello my 2 3 4");
	int nw = _count::number_of_words(str1);
	
	EXPECT_EQ(5,nw);

	string str2 ("e 1 2 25");
	nw = _count::number_of_words(str2);
	
	EXPECT_EQ(4,nw);

}

TEST(Common, fixed_stack_basic) {

	const int N = 10;
	bitgraph::com::FixedStack<int> s(N);

	EXPECT_EQ(0, s.size());

	s.push(10);
	s.push(20);
	s.push(30);

	EXPECT_EQ(3, s.size());

	EXPECT_EQ(30, s.top());
	s.pop();
	EXPECT_EQ(20, s.top());
	s.pop();
	EXPECT_EQ(10, s.top());
	s.pop();

	EXPECT_EQ(0, s.size());
	EXPECT_TRUE( s.empty());

	///////////////////

	s.push(10);
	s.push(20);
	s.push(30);

	//I/O
	//std::cout << s;
	
	s.clear();

	EXPECT_TRUE(s.empty());

}

TEST(Common, fixed_stack_pop){

	const int N=10;		
	bitgraph::com::FixedStack<int> s(N);
	s.push(10);
	s.push(30);

		
	EXPECT_EQ(30, s.top());
	
	s.pop();

	EXPECT_EQ(10, s.top());
	
	//note:popping another element should cause an assertion failure in DEBUG MODE

	s.clear();
	EXPECT_TRUE(s.empty());

}

TEST(Common_dir, path){

	string path_1("c:/kk/");		//POSIX
	_dir::append_slash(path_1);
	EXPECT_STREQ("c:/kk/", path_1.c_str());

#ifdef _MSC_VER
	string path_2("c:\\kk");		//WINDOWS	
	_dir::append_slash(path_2);
	EXPECT_STREQ("c:\\kk\\", path_2.c_str());

	string path_3(".\\kk");
	_dir::append_slash(path_3);
	EXPECT_STREQ(".\\kk\\", path_3.c_str());
#endif

}

TEST(Common_stl, all_equal){

	vector<int> v(10, 1);
	EXPECT_TRUE(_stl::all_equal(v));
		
	v.push_back(2);
	EXPECT_FALSE(_stl::all_equal(v));
}


TEST(Common_timer, elapsedTime) {


	auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(120));

	////////////////////////////////////////////////////
	double elapsed = com::_time::elapsedTime(start);				//seconds
	////////////////////////////////////////////////////

	//wide range to avoid false negatives due to scheduling
	EXPECT_GE(elapsed, 0.11);
	EXPECT_LE(elapsed, 0.30);
	
}


/////////////
//
// DISABLED TESTS
//
/////////////

TEST(Common_sort, DISABLED_insert_ordered) {

	const int N = 4;

	int data[N];
	data[0] = 0; 
	data[1] = 1; 
	data[2] = 2;	

	//sorting criteria
	int score[N];
	score[0] = 30;
	score[1] = 20;
	score[2] = 5;		
		
	int pos = _sort::INSERT_ORDERED_SORT_NON_INCR(data, score, N /* tamaño tras inserción N */, 3, 21);
	
	//TODO TEST data 


	//I/O
	/*for (int i = 0; i < N; i++) {
		std::cout << "[" << data[i] << "," << score[data[i]] << "]" << std::endl;
	}
	std::cout << "inserted at: " << pos << std::endl;*/

}






