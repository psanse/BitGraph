/*
 * @file  result_analyser.h
 * @brief InfoAnalyser class to manage benchmarking of graph algorithms.
 *		  Manages a matrix of test results:	columns are algorithms, rows are repetitions
 *		  Takes into account tests which timed-out in the reported results
 * @date  2013
 * @last_update 20/01/2025
 * @author pss  
 * 
 * TODO - generic for other types different from Result (20/01/25)
 * 
 */

#ifndef __INFO_ANALYSER_H__
#define __INFO_ANALYSER_H__

#include <iostream>
#include <vector>
#include "result.h"



//////////////////////////
// 
// InfoAnalyser class
// 
// Analyses the result of an algorithm (AlgInfo_t type)
//
//////////////////////////

template<class AlgInfo_t>								
class InfoAnalyser {

	//potentially useful types
	using vInfo_t = std::vector<AlgInfo_t>;
	using basic_type = AlgInfo_t;
	using type = InfoAnalyser<AlgInfo_t>;
	using W_t = typename AlgInfo_t::basic_type;
		
	/**
	* @brief Streams the report of the InfoAnalyser, based on the current @print_mode_ - main driver for the class
	* param o: input/output stream
	* param t: InfoAnalyser object
	**/
	template<class AlgInfo_t> 
	friend std::ostream& operator << (std::ostream& o, const InfoAnalyser<AlgInfo_t>& t);
	
public:

	//streaming options
	enum  : uint64_t  {
		NAME = 0x01, STEPS = 0x02, SOL = 0x04, STDDEV_SOL = 0x08, MAX_SOL = 0x10,
		STDDEV_STEPS = 0x20, TIME = 0x40, NFAIL = 0x80, NCONT = 0x100, LOWER_BOUND = 0x200,
		SIZE = 0x400, EDGES = 0x800, SORT = 0x1000, TIMEOUT = 0x2000, ALG = 0x4000, TIMEPRE = 0x8000,
		NREP = 0x10000 
	};

	//streaming configurations
	static const int DEFAULT_PRINT_MODE = NAME | SIZE | EDGES | SORT | TIMEOUT | SOL | LOWER_BOUND | STEPS/*|NFAIL*/ | TIME/*|NCONT*/ | TIMEPRE	;
	//static const int DEFAULT_PRINT_MODE = NAME | SOL | LOWER_BOUND | STEPS | NFAIL | TIME | NCONT;
	
	//nested struct for comparison purposes in tests with two algorithms
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
	InfoAnalyser						() : print_mode_(DEFAULT_PRINT_MODE), nAlg_(0), nRep_(0) {}
	
	//copy and move semantics disallowed
	InfoAnalyser			(const InfoAnalyser&)	= delete;
	InfoAnalyser& operator= (const InfoAnalyser&)	= delete;
	InfoAnalyser			(InfoAnalyser&&)		= delete;
	InfoAnalyser& operator= (InfoAnalyser&&)		= delete;
	
	~InfoAnalyser			()						= default;

/////////////////////
//setters/getters
	
	std::vector<vInfo_t>& get_tests				()			{ return arrayOfTests_; }
	std::vector<double> & get_times				()			{ return arrayOfAvTimes; }
	std::vector<double> & get_times_preproc		()			{ return arrayOfAvPreProcTimes; }
	std::vector<double> & get_sol				()			{ return arrayOfAvSol; }
	std::vector<double> & get_steps				()			{ return arrayOfAvSteps; }
	std::vector<vector<double>> &get_counters 	()			{ return arrayOfCounters; }
	AlgInfo_t&			get_result				(int idAlg) { return arrayOfTests_[0][idAlg]; }		//returns the result of the first instance of the idAlg run

	int number_of_repetitions				()			{ return nRep_; }
	int number_of_algorithms				()			{ return nAlg_; }		

	//I/O
	void set_print_mode					(int mode)	{print_mode_ = mode;}
	void add_print_mode					(int mode)	{print_mode_ |= mode;}
	void remove_print_mode				(int mode)	{print_mode_ &= ~mode;}
	
/////////////
// Basic operations
	/**
	 * @brief main driver - analyzes main DB and stores results in the appropiate part of the state 
	 *
	 *		I. A test is each repetition of an algorithm
	 *		II.All algorithms are assumed to have the same number of counters for each test
	 *		III.Averaged results only consider NOT FAILED tests (as long as there is one test, results will be shown)
	 *		IV. If all tests FAIL no results are shown
	 *		
	 *		TODO: Standard deviation analysis
	 * 
	 * @param info: output information that compares tests with only two algorithms
	 *				(if NULL, no output)
	 * 
	 * @returns ERR(-1) if the dataset is empty / inconsistent, OK(0) otherwise
	 * 
	 **/
	int analyser						(info_t* info = NULL);									

	/**
	 * @brief  adds a new result to the matrix of results
	 * 
	 * @param isNewRep: if true, a new repetition/test is added (opens a new row in main DB),
	 *					if false, the result is added to the current repetition/test (same row in main DB)
	 * 
	 **/ 
	void add_test						(bool isNewRep, AlgInfo_t);
	
	/**
	* @brief cleans all data members except the print mode
	**/
	void clear							();													

//////////////
// Boolean operations

	/**
	* @brief Determines if the solutions of the NON-FAILED algorithms are consistent
	*				
	*		I.analyser() has to be executed first		
	* 
	* @param num_error: -1 ok, 0 - no solution available for any algorithm (returns FALSE),
						[1...nAlg_] the first algorithm with a different solution from the one first reported
	* @results TRUE if all the solution values are the same for all algorithms, FALSE otherwise
	**/
	bool check_solution_values(int& num_error);

	/**
	* @brief TRUE if the number of repetitions and algorithms is consistent with the main DB,	 
	*		 FALSE otherwise (for empty main DB, TRUE if nAlg_ = 0, and nRep_ = 0)
	* 
	* 		I. If FALSE, run make_consistent() to update the number of repetitions and algorithms
	*		II.  analyser() has to be executed first
	**/
	bool check_array_of_tests();

//////////////	
// I/O

	/**
	* @brief Streams all results available for the algorithm algID 
	* @param algID: algorithm id; if -1 all algorithms are reported (default value)
	* @param o: input / output stream
	* @returns the output stream
	**/
std::ostream& print_alg				(std::ostream& o, int algID = -1 /*all alg*/) const;					//prints individual results of alg
	
	/**
	* @brief Reports a specific repetition nRep of the algorith algID 
	* 
	* @param o: input / output stream* 
	* @param nRep: repetition number (1 based), if 0 all repetitions are reported (default value)
	* @param algID: algorithm id, if -1 all algorithms are reported (default value) 
	* @returns the output stream
	**/
std::ostream& print_rep				(std::ostream & o, int nRep = 0 /* 1 based*/, int algID = -1) const;

	/**
	* @brief Streams the summary of the results as produced by analyser()
	*		
	*		 I. It is more oriented for console / file quick reports, not for Excel sheet analysis
	*		 II. The summary includes the average values of the results of the algs which DID NOT time_out
	*		III. It prints the general information of the tests (name, size, edges, timeout, algorithm,...)
	*			 according to the configuration predefined in@print_mode_rt
	*
	* @param o: input / output stream
	* @param mode: print mode (default value = print_mode_)
	* @returns the output stream
	**/	
std::ostream& print_summary			(std::ostream& o) const;


//////////////	
// Private interface
public:
		
	/**
	* @brief updates number of repetitions @nRep_ and number of algorithms @nAlg_ based on the dataset @arrayOfTests_
	*		  
	* @returns ERR if the dataset is empty, OK otherwise
	**/
	int make_consistent					();

///////////////		
//data members
private:
	////////////////////////////////////////////////////
	vector<vInfo_t>						arrayOfTests_;			//[rep][algorithm] - main DB  (matrix of results - main data to be processed)
	int									nRep_;					//number of repetitions (convenient, redundant with main DB info)									
	int									nAlg_;					//number of algorithms	(convenient, redundant with main DB info)			
	////////////////////////////////////////////////////

	std::vector<double>					arrayOfAvTimes;			//[alg]	
	std::vector<double>					arrayOfAvPreProcTimes;	//[alg]	
	std::vector<double>					arrayOfAvSol;
	std::vector<double>					arrayOfAvLB;
	std::vector<double>					arrayOfAvSteps;			//random
	std::vector<std::vector<double>>	arrayOfCounters;		//[alg][counter] - currently not used
	std::vector<double>					arrayOfMaxSol;	
	std::vector<int>					arrayOfFails;			
	
	//E/S
	int print_mode_;

	//TODO
	
	//std::vector<double>				arrayOfSdTime;			//for std deviations analysis
	//std::vector<double>				arrayOfSdSol;			//for std deviations analysis	

};






#endif