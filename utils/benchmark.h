//benchmark.h interface for Benchmark class, a wrapper to manage graph instances
//author:pablo san segundo

#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Benchmark{
	friend ostream& operator <<(ostream&, Benchmark& b);

protected:	//cannot be instantiated directly
	Benchmark(const string& pathName);		
	virtual ~Benchmark(){}

public:
///////////
// setters and getters
	string get_path_name				()						{return m_PATH;}
	void setArrayOfFilenames			(vector<string> list)	{m_lf=list;}
	void clear_list_of_names			()						{m_lf.clear(); m_mf.clear();}
	vector<string> getArrayOfFilenames	()						{return m_lf;}
	map<string, int> getMapOfFilenames	()						{return m_mf;}
	int number_of_instances				()						{return m_lf.size();}
	
	int get_Value						(string);					
/////////////
// E/S
	void print(ostream& = cout) ;

//////////////
//Benchmarks (graph)
	virtual void add_test(const string, int val);
	virtual void add_test(const string);
	
////////////
//Data members
protected:
	const string		m_PATH;		//fixed path for all files
	vector<string>		m_lf;		//path + filename 
	map<string, int>	m_mf;		//(path + filename) --> value
};

#endif