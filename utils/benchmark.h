/**
 * @file benchmark.h
 * @brief interface for Benchmark class, a wrapper to manage graph datasets for testing
 * @date ?
 * @last_update 17/01/25
 * @author pss
 **/

#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>

//aliases
using vstr_t = std::vector<std::string>;
using mstri_t = std::map<std::string, int>;

///////////////////
//
// Benchmark class
//
//////////////////

class Benchmark{
	friend std::ostream& operator<<			(std::ostream& o, Benchmark& b)	 { return b.print(o); }

	//move semantics and copy semantics disallowed
	Benchmark				(const Benchmark& b) = delete;
	Benchmark& operator=	(const Benchmark& b) = delete;
	Benchmark				(Benchmark&& b)		 = delete;
	Benchmark& operator=	(Benchmark&& b)		 = delete;

protected:	
	//constructor (protected) - cannot be instantiated 
explicit Benchmark(const std::string& pathName);			
	
public:
	//destructor
	virtual ~Benchmark(){}

///////////
// setters and getters
		
	int number_of_instances					()									{return lf_.size();}

	void setArrayOfFilenames				(std::vector<std::string> list)		{ lf_ = list; }
	
	int get_value							(std::string filename);
	std::string get_path					()									{ return PATH_; }
	vstr_t& getArrayOfFilenames				()									{ return lf_; }
	mstri_t& getMapOfFilenames				()									{ return mf_; }

/////////////
// Context
	void clear_instances					()									{ lf_.clear(); mf_.clear(); }


//////////////
//Benchmark operations

	/**
	* @brief Adds a test with a value to the benchmark list
	* **/
	virtual void add_test					(const std::string, int val);

	/**
	* @brief Adds a test without value to the benchmark list
	* **/
	virtual void add_test					(const std::string);


/////////////
// I/O

	std::ostream& print						(std::ostream & = std::cout);
	
////////////
//Data members
protected:
	const std::string		PATH_;		//FIXED path for all filenames
	vstr_t					lf_;		//list of filemanes (PATH_ + name) 
	mstri_t					mf_;		//mapping filenames -> value (typically the optimal solution value or a bound)
};

#endif