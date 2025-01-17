/**
* @file test_common.cpp 
* @brief Testing of common utilities
* @date ?
* @last_update 17/01/25
* @author pss
* 
* TODO - CHECK and refactor tests (17/01/25)
**/

#include "utils/common.h"
#include "gtest/gtest.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace com;

using vdob = std::vector<double>;

TEST(Common, mean_and_stdev) {

	//collection
	vdob col = { 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3 };
	
	//compute mean - implicit cast to double on return, MUST USE for_each and return double
	double mean = for_each(col.begin(), col.end(), com::mat::MeanValue());					

	/////////////////////////////
	EXPECT_DOUBLE_EQ(3, mean);
	/////////////////////////////

	//compute standard deviation - implicit cast to double on return, MUST USE for_each and return double
	double stdDev = for_each(col.begin(), col.end(), com::mat::StdDevValue(mean));			
	
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

TEST(Common, number_of_words){

	string str1("hello my 2 3 4");
	int nw = counting::number_of_words(str1);
	
	EXPECT_EQ(5,nw);

	string str2 ("e 1 2 25");
	nw = counting::number_of_words(str2);
	
	EXPECT_EQ(4,nw);

}


TEST(Common, stack_basic) {

	const int N = 10;
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

	cout << s;

	s.erase();
	EXPECT_TRUE(s.empty());

}

TEST(Common, stack_pop){
/////////////////
// Fixed popped so that i really is multi-type
// date@10/3/18


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

}

TEST(sort, insert_ordered){

	int data[4]; data[0]=0; data[1]=1; data[2]=2;				/* items sorted non-increasing score  */
	int score[4]; score[0]=30;  score[1]=20;  score[2]=5;		/* sorting criteria */
	
	const int N=4;												
	int pos=com::sort::INSERT_ORDERED_SORT_NON_INCR(data,score, N /* tamaño tras inserción N */, 3, 21);
	for(int i=0; i<N; i++){
		cout<<"["<<data[i]<<","<<score[data[i]]<<"]"<<endl;		
	}
	cout<<"inserted at: "<<pos<<endl;


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


}


//TEST(Common, my_silly_pointer_to_elem_basics) {
	//		
	//	struct s_triplet {
	//		int a = 10;
	//		int b = 20;
	//		int c = 30;
	//		ostream& print(ostream& o) {
	//			o << "[" << a << " " << b << " " << c << "]";
	//			return o;
	//		}
	//	};
	//
	//	//constructor
	//	pt_elem<int> pElem(20);
	//
	//	///////////////////////////////////////
	//	EXPECT_EQ(20, *pElem);							//operator * overload
	//	EXPECT_EQ(20, *pElem.get_elem());				//getter
	//	///////////////////////////////////////
	//
	//	//setter
	//	pElem.set_elem(10);
	//
	//	/////////////////////////////////////
	//	EXPECT_EQ(10, *pElem.get_elem() );
	//	/////////////////////////////////////
	//
	//	//pointer to struct
	//	s_triplet tr;
	//	pt_elem<s_triplet> pTriplet(tr);
	//	
	//	///////////////////////////////////////
	//	EXPECT_EQ(10, pTriplet.get_elem()->a);
	//	EXPECT_EQ(20, pTriplet.get_elem()->b);
	//	EXPECT_EQ(30, pTriplet.get_elem()->c);
	//	///////////////////////////////////////
	//
	//	//resets pointer
	//	s_triplet triplet;
	//	triplet.a = 100;
	//	pTriplet.set_elem(triplet);
	//
	//	///////////////////////////////////////
	//	EXPECT_EQ(100, pTriplet.get_elem()->a);
	//	///////////////////////////////////////
	//
	//}

//TEST(Common, my_silly_pointer_to_elem_swap) {
//
//	pt_elem<int> pInt1(10);
//	pt_elem<int> pInt2(20);
//	pInt1.swap(pInt2);
//
//	///////////////////////////////////////
//	EXPECT_EQ(20, *pInt1.get_elem());
//	EXPECT_EQ(10, *pInt2.get_elem());
//
//}


