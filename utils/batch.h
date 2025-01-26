/**
* @file batch.h
* @brief A templatized batch class to run tests of type Alg_t (or derived) with Params Param_t
*		 (Factory of tests)
* 
*		I. Alg_t must have a constructor for Param_t
* 		II. AlgVar_t should derive from Alg_t (base of the hierarchy of algorithms)	
**/

#ifndef __BATCH_H__
#define __BATCH_H__

#include <vector>

template <class Alg_t, class Param_t>
class Batch{
public:

	//construction / destruction
	~Batch() { clear(); }

	//getters and setters
	int number_of_tests		()			{ return tests.size(); }
	Alg_t* get_test			(int id)	{ return tests[id]; }

/////////////////////////
//basic operations

	/**
	* @brief adds tests of type AlgVar_t (derived from Alg_t)  - deallocates memory
	**/
	template<class AlgVar_t =  Alg_t>
	void add_test			(Param_t p)	{ tests.emplace_back( new AlgVar_t(p) ); }

	template<class AlgVar_t = Alg_t>
	void operator +=		(Param_t p) { tests.emplace_back( new AlgVar_t(p) ); }

	/**
	* @brief clears all tests - deallocates memory
	**/
	void clear(){

		for (auto& pTest : tests) {
			delete pTest;
		}
		
		tests.clear();
	}
	
	/**
	* @brief runs all tests
	**/
	virtual void run_all_tests(){
		for (auto& pTest : tests) {
			//pTest->setup();
			pTest->start();
			//pTest->tear_down();
		}
	}
	

/////////
//data members
protected:
	std::vector<Alg_t*> tests; 
};

#endif





