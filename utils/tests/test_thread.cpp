#include "gtest/gtest.h"
#include "../thread.h"

class ThreadExample{
public:
	ThreadExample(int d):data(d){};

	//function member to be run as thread (prototype is fixed)
	int task_mul_by_2 (){
		LOG_INFO("Thread running");
#ifdef _MSC_VER
		Sleep(3000);
#elif __GNUC__
		sleep(3);
#endif
		data*=2;
	return 0;
	};

	//function wrapper to run thread
	int Start(DWORD tout_ms){
		int res = Thread<ThreadExample, int>::Start(&ThreadExample::task_mul_by_2,this,tout_ms);
		LOG_INFO("result:"<<data);	
		return res;
	}

//////////
	int data;
};

TEST(Thread, basic){
	cout<<"Thread::basic------------------"<<endl;
	ThreadExample t(5);
	int res= t.Start(1000);
	EXPECT_EQ(-1, res);			//time out

	int res1= t.Start(4000);
	EXPECT_EQ(0, res1);			//finished correctly

	cout<<"T------------------------------"<<endl;
}