/**
 * @file test_benchmark.cpp
 * @brief unit tests for the Benchmark class in benchmark.h
 * @date ?
 * @last_update 17/01/25
 * @author pss
 **/


#include "gtest/gtest.h"
#include "utils/benchmark.h"

#define PATH "c:\\path"

using namespace std;

////////////////////////
//
// class Bk to test the Benchmark class
// (since the Benchmark class has a private constructor)
//
//////////////////////
class Bk : public Benchmark{
public:
	Bk(string path) : Benchmark(path){}
};

TEST(Benchmark_test, basic){
	
	Bk b(PATH);
	b.add_test("file_1");
	b.add_test("file_2");
	b.add_test("file_3");
	b.add_test("file_4",4);
	b.add_test("file_5",5);

	EXPECT_EQ(5, b.number_of_instances());

	//set filename PATH + "\\file_4"	
	string file(PATH);

#ifdef _WIN32
	file += "\\file_4";
#elif __GNUC__
	file += "/file_4";
#endif

	//check the value of the filename in the Bk map	
	map<string, int> m = b.getMapOfFilenames();
	EXPECT_EQ(4, m[file]);
	EXPECT_TRUE( (bool)m.count(file) );
	
	//I/O
	//cout << b;

	int val = b.get_value(file);
	EXPECT_EQ(4, val);

	//set filename PATH + "\\file_3"	
	string file3(PATH);
	file3 += "\\file_3";

	//check that the filename has no value in the  Bk map	
	val = b.get_value(file3);
	EXPECT_EQ(-1, val);

}

