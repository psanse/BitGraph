//testanalyser.h interface for TestAnalyser class dedicated to report random experiments for graph algorithms
///////////////////////////

#ifndef __TEST_ANALYSER_H__
#define __TEST_ANALYSER_H__

#include <iostream>
#include <vector>
#include "result.h"

using namespace std;

//#define DEFAULT_PRINT_MODE	 NAME|SOL|LOWER_BOUND|STEPS|NFAIL|TIME|NCONT
#define DEFAULT_PRINT_MODE		 NAME|SIZE|EDGES|SORT|TIMEOUT|SOL|LOWER_BOUND|STEPS/*|NFAIL*/|TIME/*|NCONT*/|TIMEPRE

//aliases
using  vres_t = vector<Result>;
using  itres_t = vector<Result>::iterator;

//////////////////////////
// 
// TestAnalyser class
// 
// Analyses a matrix of tests: columns are algorithms, rows are repetitions
//
//////////////////////////

class TestAnalyser{
	
	friend std::ostream& operator << (std::ostream&, const TestAnalyser& );
	enum { ERR = -1, OK = 0 };

public:

	enum print_mode_t {
		NAME = 0x01, STEPS = 0x02, SOL = 0x04, STDDEV_SOL = 0x08, MAX_SOL = 0x10,
		STDDEV_STEPS = 0x20, TIME = 0x40, NFAIL = 0x80, NCONT = 0x100, LOWER_BOUND = 0x200,
		SIZE = 0x400, EDGES = 0x800, SORT = 0x1000, TIMEOUT = 0x2000, ALG = 0x4000, TIMEPRE = 0x8000
	};

	struct info_t{
		info_t():	same_lb(false), same_sol(false), same_steps(false),
					steps_first_greater(false), steps_lhs(0), steps_rhs(0) 
		{}
		bool same_steps;
		bool same_sol;
		bool same_lb;
		bool steps_first_greater;
		int steps_lhs;					//first algortihm
		int steps_rhs;					//second algorithm
	};
	
		
////////////////////	
//construction / destruction
	TestAnalyser						();

/////////////////////
//setters/getters
	void clear							();														//does not change print_mode
	void set_print_mode					(int mode)	{m_print_mode=mode;}
	void add_print_mode					(int mode)	{m_print_mode|=mode;}
	void remove_print_mode				(int mode)	{m_print_mode&=~mode;}
	vector<vres_t>	get_tests			()			{return arrayOfTests;} 
	vector<double>	get_times			()			{return arrayOfAvTimes;}
	vector<double>	get_times_preproc	()			{return arrayOfAvPreProcTimes; }
	vector<double>	get_sol				()			{return arrayOfAvSol;}
	vector<double>	get_steps			()			{return arrayOfAvSteps;}
	vector<vector<double> >get_counters ()			{return arrayOfCounters;}
	Result			get_result			(int idAlg)	{return arrayOfTests[0][idAlg];}		//returns the result of the first instance of the idAlg run
	
/////////////
// Basic operations

	void add_test						(bool isNewRep, Result);
	bool is_consistent_sol				(int& num_error);	
	int analyser						(info_t* = NULL);									//main driver

//////////////	
// I/O
	void print_single					(ostream &, int idAlg=-1 /*all alg*/);					//prints individual results of alg
	void print_single_rep				(ostream &, int nRep=0 /* 1 based*/, int idAlg=-1);	
			
	static void print_current_time(ostream & = cout);
	static void print_line(const string line, ostream & = cout);

//////////////	
// Private interface
private:
	int update_sizes					();	

///////////////		
//data members

	vector<vres_t>					arrayOfTests;			//[rep][algorithm], main DB  (matrix of results - main data to be processed)

	vector<double>					arrayOfAvTimes;			//[alg]	
	vector<double>					arrayOfAvPreProcTimes;	//[alg]	
	vector<double>					arrayOfAvSol;
	vector<double>					arrayOfMaxSol;	
	vector<double>					arrayOfAvLB;	
	vector<int>						arrayOfFails;			//random
	vector<double>					arrayOfAvSteps;			//random
	vector<std::vector<double> >	arrayOfCounters;		//[alg][counter]

	//***std deviations analysis
	//vector<double>				arrayOfSdTime;			
	vector<double>					arrayOfSdSol;	
	
	int m_nRep;
	int m_nAlg;
	
	//E/S
	int m_print_mode;
};


#endif