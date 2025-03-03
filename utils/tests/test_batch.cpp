#include "gtest/gtest.h"
#include "utils/batch.h"
#include "utils/logger.h"

using namespace std;

//configuration data for the hierarchy of algorithms
struct param_t{
	int i_;
	int j_;
};

//base class representing an algorithm
class AlgBase{
public:
explicit AlgBase(param_t d): result_(d){}
	AlgBase(){}
	virtual param_t start() { return result_; }
	param_t data() const { return result_; }
protected:
	param_t result_;
};

//derived classes
class AlgDer1 : public AlgBase {
public:
	using AlgBase::AlgBase;
	param_t start() override{
		LOGG_DEBUG("params of AlgDer1: ", "(", result_.i_, ",", result_.j_, ")");		
		return result_;
	}
};

class AlgDer2 : public AlgDer1 {
public:
	using AlgDer1::AlgDer1;
	param_t start() override{
		LOGG_DEBUG("params of AlgDer2: " , "(" , result_.i_ , "," , result_.j_ , ")");		
		return result_;
	}
};


TEST(Batch, basic){

	//configuration data
	param_t d = { 30, 20 };
	
	//builds tests
	Batch<AlgBase, param_t> b;

	b.add_test<AlgBase>(d);					
	b += d;								//same as above
	b.add_test<AlgDer1>(d);
	b.add_test<AlgDer2>(d);
	b.add_test<AlgDer2>(d);				//second level hierarchy

	//check tests
	EXPECT_EQ(5, b.number_of_tests());

	//check test construction
	for (auto i = 0; i< 5; i++) {
		EXPECT_EQ(30, b.get_test(i)->data().i_);
		EXPECT_EQ(20, b.get_test(i)->data().j_);
	}

	//excute tests...
	b.run_all_tests();

	//check cleaning
	b.clear();
	EXPECT_EQ(0, b.number_of_tests());						

}

