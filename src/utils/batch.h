/**
* @file batch.h
* @brief A templatized batch class to run tests of type AlgT (or derived) with Params ParamT
*		 (Factory of tests)
* 
*		I. AlgT must have a constructor for ParamT
* 		II. AlgVar_t should derive from AlgT (base of the hierarchy of algorithms)	
**/

#ifndef __BATCH_H__
#define __BATCH_H__

#include <vector>
#include <memory>

template <class AlgT, class ParamT>
class Batch{
public:

	//construction / destruction
	virtual ~Batch() = default;			//TODO- use predefined destructor

	//getters and setters
	int number_of_tests				()			{ return tests.size(); }
std::unique_ptr<AlgT>& get_test	(int id)	{ return tests[id]; }

/////////////////////////
//basic operations

	/**
	* @brief adds tests of type AlgVar_t (derived from AlgT)  - deallocates memory
	**/
	template<class AlgVar_t =  AlgT>
	void add_test			(ParamT p)	{ tests.emplace_back( std::make_unique<AlgVar_t>(p) ); }

	template<class AlgVar_t = AlgT>
	void operator +=		(ParamT p) { tests.emplace_back( std::make_unique<AlgVar_t>(p)); }

	/**
	* @brief clears all tests - deallocates memory
	**/
	void clear(){ tests.clear(); }
	
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
	std::vector < std::unique_ptr<AlgT> > tests;
};

#endif





