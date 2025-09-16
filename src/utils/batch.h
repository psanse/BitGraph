/**
* @file batch.h
* @brief A templatized batch class to run tests with parameter configuration ParamT
*		 (Factory of tests)
* @details: the tested algorithms must have a constructor that takes ParamT as input
**/

#ifndef __BATCH_H__
#define __BATCH_H__

#include <vector>
#include <memory>

/// <summary>
/// @brief: lightweight Batch class for tests with configuration ParamT
/// @details: the tested algorithms must have a constructor that takes ParamT as input
/// </summary>
/// <typeparam name="ParamT"> configuration parameters </typeparam>

template <class ParamT>
class Batch{
public:

	//construction / destruction
	virtual ~Batch() = default;			//TODO- use predefined destructor

	//getters and setters
	int number_of_tests				()			{ return tests.size(); }
	ParamT& get_test				(int id)	{ return tests[id]; }

/////////////////////////
//basic operations

	/**
	* @brief adds tests of type AlgVar_t (derived from AlgT)  - deallocates memory
	**/
	void add_test			(const ParamT& p)	{ tests.emplace_back(p); }
		
	void operator +=		(const ParamT& p) { tests.emplace_back(p); }

	/**
	* @brief clears all tests - deallocates memory
	**/
	void clear(){ tests.clear(); }
	
	
/////////
//data members
protected:
	std::vector < ParamT > tests;
};

#endif





