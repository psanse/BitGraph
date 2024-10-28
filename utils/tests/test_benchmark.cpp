#include "gtest/gtest.h"
#include "../benchmark.h"

#define PATH "c:\\path"

class Bk:public Benchmark{
public:
	Bk(string path):Benchmark(path){}
};

TEST(Benchmark, basic){
	cout<<"Benchmark::basic------------------"<<endl;
	
	Bk b(PATH);
	b.add_test("file_1");
	b.add_test("file_2");
	b.add_test("file_3");
	b.add_test("file_4",4);
	b.add_test("file_5",5);

	EXPECT_EQ(5, b.number_of_instances());
	map<string, int> m=b.getMapOfFilenames();
	string file(PATH);
#ifdef _WIN32
	file+="\\file_4";
#elif __GNUC__
	file+="/file_4";
#endif
	EXPECT_EQ(4,m[file]);
	EXPECT_TRUE( (bool)m.count(file));
	cout<<b;

	int val=b.get_Value(file);
	EXPECT_EQ(4, val);

	string file3(PATH);
	file3+="\\file_3";
	val=b.get_Value(file3);
	EXPECT_EQ(-1, val);

	cout<<"----------------------------------"<<endl;
}

