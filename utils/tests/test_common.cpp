#include <iostream>
#include <sstream>
#include "gtest/gtest.h"
#include "../common.h"
#include <string>
#include <vector>
#include "logger.h"

using namespace std;
using namespace com;

#define PATH "C:/Users/i7/Desktop/"

TEST(Common, counting_words){
/////////////////
// Testing my simple pointer to elem
// date@30/09/18
	LOG_INFO("Common:counting_words-----------------------");


	string str1("hello my 2 3 4");
	int nw=counting::count_words(str1);
	EXPECT_EQ(5,nw);

	string str2("e 1 2 25");
	nw=counting::count_words(str2);
	EXPECT_EQ(4,nw);

	LOG_INFO("--------------------------------------------");
	/*LOG_INFO("PRESS ANY KEY");
	cin.get();*/
}

TEST(Common, pt_elem_basic){
/////////////////
// Testing my simple pointer to elem
// date@17/09/18

	LOG_INFO("Common:my pt_elem_basic-----------------------");

	//TEST_A
	pt_elem<int> myp;
	myp.set_elem(10);	
	EXPECT_EQ(10, *(myp.get_elem()));

	//TEST_B
	struct s_triplet{
		s_triplet():a(10), b(20),c(30){}
		int a;
		int b;
		int c;
		ostream& print(ostream& o){o<<"["<<a<<" "<<b<<" "<<c<<"]"; return o;}
	};

	pt_elem<s_triplet> myp1;
	myp1.print_elem(); cout<<endl;		//takes default values

	EXPECT_EQ(10, myp1.get_elem()->a);
	EXPECT_EQ(20, myp1.get_elem()->b);
	EXPECT_EQ(30, myp1.get_elem()->c);

	//sets values 
	s_triplet data; data.a=100;
	myp1.set_elem(data);	
	EXPECT_EQ(100, myp1.get_elem()->a);
	

	LOG_INFO("----------------------------------------------");
	LOG_INFO("PRESS ANY KEY");
	cin.get();
}

TEST(Common, my_stack_pop){
/////////////////
// Fixed popped so that i really is multi-type
// date@10/3/18

	LOG_INFO("Common:my stack-----------------------");
	const int N=10;
	struct dato_t{
		dato_t():a(-1),b(-1){}
		dato_t(int a, int b):a(a),b(b){}
		int a;
		int b;
		ostream& print(ostream& o){o<<"("<<a<<","<<b<<")"; return o;}
	};

	com::stack_t<dato_t> s(N);
	
	EXPECT_EQ(0, s.size());

	dato_t mi_dato (8, 10);
	dato_t mi_dato_1 (10, 30);

	s.push(mi_dato);
	s.push(mi_dato_1);
	
	dato_t mi_dato_popped=s.pop();
	mi_dato_popped.print(cout);
	EXPECT_EQ(10, mi_dato_popped.a);
	EXPECT_EQ(30, mi_dato_popped.b);

	mi_dato_popped=s.pop();
	EXPECT_EQ(8, mi_dato_popped.a);
	EXPECT_EQ(10, mi_dato_popped.b);

	mi_dato_popped=s.pop();
	EXPECT_EQ(-1, mi_dato_popped.a);
	EXPECT_EQ(-1, mi_dato_popped.b);

	
////////////////
// INT
	com::stack_t<int> s1(N);
	s1.push(10);
	s1.push(30);

	int popped=s1.pop();
	EXPECT_EQ(30, popped);
	popped=s1.pop();
	EXPECT_EQ(10, popped);
	popped=s1.pop();
	EXPECT_EQ(-1, popped);

	
	s.erase();
	s1.erase();
	EXPECT_TRUE(s.empty());
	EXPECT_TRUE(s1.empty());

	LOG_INFO("Common:my stack-END-----------------------");
	LOG_INFO("Pres any key");
	cin.get();
}

TEST(sort, insert_ordered){
	LOG_INFO("sort:insert_ordered-----------------------");
	int data[4]; data[0]=0; data[1]=1; data[2]=2;				/* items sorted non-increasing score  */
	int score[4]; score[0]=30;  score[1]=20;  score[2]=5;		/* sorting criteria */
	
	const int N=4;												
	int pos=com::sort::INSERT_ORDERED_SORT_NON_INCR(data,score, N /* tamaño tras inserción N */, 3, 21);
	for(int i=0; i<N; i++){
		cout<<"["<<data[i]<<","<<score[data[i]]<<"]"<<endl;		
	}
	cout<<"inserted at: "<<pos<<endl;

	LOG_INFO("------------------------------------------");
	cin.get();
}

TEST(Common, read_FF_file_interdicted_nodes){
	LOG_INFO("Common:read_FF_file_interdicted_nodes-----------------------");

	string filename=PATH;
	filename+="interdictionBUG.txt";

	vector<int> vread;
	com::fileproc::READ_SET_OF_INTERDICTED_NODES(filename.c_str(), vread);

	com::stl::print_collection(vread);

	cin.get();
	LOG_INFO("-------------------------------------------------------------");
}

TEST(Common, path){
	string path_1("c:\\kk");
	string path_2("c:\\kk/");  //a posix slash at the end

	com::dir::append_slash(path_1);
	com::dir::append_slash(path_2);

	string sol1("c:\\kk\\");
	string sol2("c:\\kk/");
#ifdef _MSC_VER
	EXPECT_EQ(sol1, path_1);
	EXPECT_EQ(sol2, path_2);
#elif __GNUC__
	//***

#endif

	cout<<path_1<<endl;
	cout<<path_2<<endl;
}

TEST(Common, collection_equality){
//***FIXME THIS TEST BREAKS
	cout<<"Common:collection_equality-----------------------"<<endl;
	vector<int> v(10, 1);
	EXPECT_TRUE(com::stl::all_equal(v));
	com::stl::print_collection(v);
	
	v.push_back(2);
	EXPECT_FALSE(com::stl::all_equal(v));

	//***FIXME THIS TEST BREAKS
	//empty vector is equal as well 
	//v.swap(vector<int>());
	//EXPECT_TRUE(com::stl::all_equal(v));

	//single element is also equal
	/*v.push_back(1);
	EXPECT_TRUE(com::stl::all_equal(v));*/

	cout<<"-------------------------------------------------"<<endl;
}

TEST(Common, my_stack){
cout<<"Common:my stack-----------------------"<<endl;
	const int N=10;
	com::stack_t<int> s(N);
	
	EXPECT_EQ(0, s.size());

	s.push(10);
	s.push(20);
	s.push(30);

	EXPECT_EQ(3, s.size());
	EXPECT_EQ(30, s.pop());
	EXPECT_EQ(20, s.pop());
	EXPECT_EQ(10, s.pop());
	EXPECT_EQ(0, s.size());
	EXPECT_TRUE(s.empty());

	s.push(10);
	s.push(20);
	s.push(30);

	cout<<s;

	s.erase();
	EXPECT_TRUE(s.empty());

cout<<"Common:my stack-----------------------"<<endl;

}
