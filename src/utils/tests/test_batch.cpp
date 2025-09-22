#include "gtest/gtest.h"
#include "utils/batch.h"
#include "utils/logger.h"

using namespace std;
using namespace bitgraph;


//configuration data for the hierarchy of algorithms
struct param_t{
	int i_ = 0;
	int j_ = 3;
};

TEST(Batch, basic){

	//configuration data
	param_t d = { 30, 20 };
	
	//builds tests
	Batch<param_t> b;

	b.add_test(d);
	b += d;								//same as above
	b.add_test(d);
	b.add_test(d);
	b.add_test(d);						//second level hierarchy

	//check tests
	EXPECT_EQ(5, b.number_of_tests());

	//check test construction
	for (auto i = 0; i< 5; i++) {
		EXPECT_EQ(30, b.test(i).i_);
		EXPECT_EQ(20, b.test(i).j_);
	}

	//check cleaning
	b.clear();
	EXPECT_EQ(0, b.number_of_tests());						

}

