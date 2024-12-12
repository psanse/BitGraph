/*
 *info.h: interface for result logs of clique and clique-based algorithms
 *@date 12/12/2012
 *@dev pss
 */

 //TODO - UNDER DEVELOPMENT!!!!!!!!!!!! (12/12/2024)


#ifndef __INFO_BASE_CLQ_INFO_H__
#define __INFO_BASE_CLQ_INFO_H__

#include <vector>
#include <iostream>
#include <cfloat>
#include <time.h>

///////////////////////////////////////////////////////////
constexpr std::string FILE_LOG = "info_clique.txt";
/////////////////////////////////////////////////////////

 /**
  * @brief info on results for clique and clique-based algorithms (base class)
  */
struct infoCLQ_Base {
	enum class phase_t { SEARCH = 0, PREPROC, LAST_INCUMBENT, PARSE };
		
	infoCLQ_Base() :nameInstance_(""), nameFileLog_(FILE_LOG), N_(0), M_(0), K_(0), 
					TIME_OUT_(DBL_MAX), TIME_OUT_HEUR_(DBL_MAX), timeOutReached_(false) {}

/////////////////////
//general configuration
////////////////////
	std::string nameFileLog_;				//log file name
			
/////////////////////
//main info
////////////////////
	std::string nameInstance_;				//instance name
	uint32_t N_;							//number of vertices
	uint64_t M_;							//number of edges		
	uint32_t K_;							//for k-clique search- EXPERIMENTAL, TODO@add to output
	double TIME_OUT_;						//in seconds
	double TIME_OUT_HEUR_;					//in seconds
	bool  timeOutReached_;					//flag time out	


	//////////////////////
// timers
//////////////////////
	
	clock_t startTimeParse_;
	double timeParse_;						//parsing time(in seconds)

	clock_t startTimePreproc_;
	double timePreproc_;					//preprocessing time(in seconds)

	clock_t startTimeSearch_;
	double timeSearch_;						//search time(in seconds)
		
	clock_t startTimeIncumbent_;
	double timeIncumbent_;					//time when last new incumbent was found (in seconds)
	
	void reset() {
		resetGeneralInfo();					//CHECK comment "MUST BE MANUAL!"												
		resetTimers();
	}

	//////////////////////////
	//new interface

	void startTimer(phase_t t);
	double readTimer(phase_t t);
	double elapsedTime(clock_t start_time);

	
/////////////
//context 
/////////////
	void resetTimer(phase_t t);
	void resetTimers();
	void resetGeneralInfo();					//CHECK comment: "manually at the start of every run"	

/////////////
//I/O
/////////////
	/*friend std::ostream& operator<<(std::ostream&, const infoCLQ_Base&);
	std::ostream& print_params(std::ostream& = std::cout);
	std::ostream& print_results(std::ostream& = std::cout);
	std::ostream& print_timers(std::ostream& = std::cout);*/
};

/**
 * @brief info on results for my MCP algorithms
 */
struct infoCLQ : public infoCLQ_Base {
	infoCLQ() :infoCLQ_Base(), incumbent_(0), optimum_(0), nSteps_(0){}

	void resetPreprocInfo();
	void resetSearchInfo();
	
	void reset(bool lazy=true){		
		resetPreprocInfo()
		resetSearchInfo();					//CHECK comment "MUST BE MANUAL!"									
		resetTimers();
		if (!lazy) {
			infoCLQ_Base::reset();
		}
	}

/////////////////////
//data members

	int incumbent_;							//best solution value found during search (not in preproc)
	int optimum_;							//best solution value (omega) - will be the same as incumbent is time out limit is not reached
	uint64_t nSteps_;						//recursive calls to the algorithm		
	std::vector<uint32_t> sol_;				//best solution

	//////////////////////
	//  Preproc info  TODO	- REFACTOR NAMES!
	//////////////////////

	//LB bounds - root
	double LB_0_no_AMTS;
	double LB_0_AMTS;
	
	//UB bounds root
	int branching_root_size;

	//algortihm identifiers
	int id_AMTS;
	int id_alg;
	int id_sorting_alg_real;				//sorting policy used by the algorithm
	int id_sorting_alg_called;				//sorting policy selected as input configuration parameter (might not be the final choice)

	//sorting
	bool is_degree_ordering;				//FALSE- sorting partition based (COLOR_SORT), TRUE- sorting degree based (DEGREE_SORT)
		
	//////////////////////
	//  search info 
	//////////////////////
	uint64_t nLastIsetPreFilterCalls;		//calls to the bound
	uint64_t nLastIsetCalls;				//calls to the bound
	uint64_t nCurrIsetCalls;
	uint64_t nVertexCalls;
	uint64_t nUBpartCalls;

	uint64_t nsLastIsetPreFilterCalls;
	uint64_t nsLastIsetCalls;				//succesful calls to the bound
	uint64_t nsCurrIsetCalls;
	uint64_t nsVertexCalls;
	uint64_t nsUBpartCalls;
						
};

//extern infoCLQ all_info;



#endif






