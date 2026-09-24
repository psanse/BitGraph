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

namespace {
	template<class ParamT>
	class TestBatch : public Batch<ParamT>{
	public:
		using Batch::add_test;
		using Batch::operator+=;

	}; // end class TestBatch
}

TEST(Batch, basic){

	//configuration data
	param_t d = { 30, 20 };
	
	//builds tests
	TestBatch<param_t> b;

	b.add_test(d);
	b += d;								
	b.add_test(d);
	b.add_test(d);
	b.add_test(d);						

	
	EXPECT_EQ(5, b.number_of_tests());

	for (std::size_t i = 0; i< b.number_of_tests(); i++) {
		EXPECT_EQ(30, b.test(i).i_);
		EXPECT_EQ(20, b.test(i).j_);
	}

	//check cleaning
	b.clear();
	EXPECT_EQ(0, b.number_of_tests());						

}

