#include "gtest/gtest.h"
#include "../batch.h"

//the actual configuration data
struct data_t{
	data_t(int j_out=0, int k_out=12):j(j_out), k(k_out){}
	int j;
	int k;
};

//wrapper for the data and interface for all algorithms
class AlgParam{
public:
	AlgParam(data_t d):data(d){}
	AlgParam(){}
	virtual void setup(){}
	virtual void run(){}
	virtual void tear_down(){}
protected:
	data_t data;
};

//algorithms
class Alg:public AlgParam{
public:
	Alg(AlgParam d):AlgParam(d){}
	void run(){ cout<<"params "<<"("<<data.j<<","<<data.k<<")"<<endl;}
};

class AlgDer:public Alg{
public:
	AlgDer(AlgParam d):Alg(d){}
	void run(){ cout<<"params "<<"("<<data.j<<","<<data.k<<")"<<endl;}
};

TEST(Batch, basic){
	cout<<"Batch::basic------------------"<<endl;

	data_t d(30, 20);
	AlgParam p(d);
	Batch<Alg, AlgParam> b;
	b.add_test<Alg>(p);					//factory of tests with parameters
	b.add_test<Alg>(p);
	b.add_test<AlgDer>(p);
	b.add_test<AlgDer>(p);
	b.add_test<AlgDer>(p);				//second level hierarchy

	EXPECT_EQ(5, b.number_of_tests());
	b.start();						
		
	cout<<"-------------------------------"<<endl;
}

