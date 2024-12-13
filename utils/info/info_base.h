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

/////////////////////////////////////////////////////////
constexpr std::string FILE_LOG = "info_clique.txt";
/////////////////////////////////////////////////////////

//////////////////////
//
//	info_base
// 
//  @brief base class to report results of graph algorithms
//
///////////////////////
template <typename res_t = int>
struct info_base {
	enum class phase_t { SEARCH = 0, PREPROC, LAST_INCUMBENT, PARSE };

/////////////////////
//general configuration
////////////////////
	std::string nameFileLog_;				//log file name
	std::string nameInstance_;				//instance name
	uint32_t N_;							//number of vertices
	uint64_t M_;							//number of edges		
	uint32_t K_;							//for k-clique search- EXPERIMENTAL, TODO@add to output
	double TIME_OUT_;						//in seconds
	double TIME_OUT_HEUR_;					//in seconds

	/////////////////////
	//algorithms
	////////////////////
	int idAlg_;								//algorithm identifier
	int idAMTS_;							//clique heuristic strategy (AMTS, no AMTS or combined with other heuristics)	
	int idSort_;							//sorting policy selected as input configuration parameter (might not be the final choice)

/////////////////////
//search
////////////////////
	res_t incumbent_;						//best solution value found during search (not in preproc)
	res_t optimum_;							//best solution value (omega) - will be the same as incumbent is time out limit is not reached
	uint64_t nSteps_;						//recursive calls to the algorithm		
	bool  isTimeOutReached_;					//flag time out	
	std::vector<int> sol_;					//best solution (vertices)
	
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

/////////////////////////////
//interface

	info_base() :
		nameFileLog_(FILE_LOG), nameInstance_(""),
		N_(0), M_(0), K_(0),
		TIME_OUT_(DBL_MAX), TIME_OUT_HEUR_(DBL_MAX),
		incumbent_(0), optimum_(0), nSteps_(0), isTimeOutReached_(false),
		idAlg_(-1), idAMTS_(-1), idSort_(-1)
	{}
			
	//timers
	void startTimer(phase_t t);
	double readTimer(phase_t t);
	double elapsedTime(clock_t start_time);
		
	
	//context 
	void resetGeneralInfo();				//CHECK comment: "manually at the start of every run"	
	void resetTimers();						//reset all timers
	void resetTimer(phase_t t);
	
	void reset() {
		resetGeneralInfo();					//CHECK comment "manually at the start of every run"	
		resetSearchInfo();					
		resetTimers();
	}

	virtual void resetSearchInfo();


	/////////////
	//I/O
	friend std::ostream& operator<<(std::ostream&, const info_base&);
	virtual std::ostream& printTable(std::ostream & = std::cout);

	std::ostream& printParams(std::ostream& = std::cout);
	std::ostream& printResults(std::ostream& = std::cout);
	std::ostream& printTimers(std::ostream& = std::cout);

};

//////////////////////
//
//	infoCLQ
// 
//  @brief info on results of the clique algorithms
//		   (some are CliSAT specific)
//
///////////////////////

struct infoCLQ : public info_base {
	infoCLQ() :info_base(), 
		LB_0_no_AMTS(0), LB_0_AMTS(0),
		branching_root_size(0),
		id_AMTS(-1), id_sorting_alg_real(-1), id_sorting_alg_called(-1),	
		is_degree_ordering(false),
		nLastIsetPreFilterCalls(0), nLastIsetCalls(0), nCurrIsetCalls(0), nVertexCalls(0), nUBpartCalls(0),
		nsLastIsetPreFilterCalls(0), nsLastIsetCalls(0), nsCurrIsetCalls(0), nsVertexCalls(0), nsUBpartCalls(0)
	{}

	void resetPreprocInfo();
	void resetSearchInfo() override;
	
	void reset(bool lazy=true){		
		resetPreprocInfo()
		resetSearchInfo();					//CHECK comment "MUST BE MANUAL!"									
		resetTimers();
		if (!lazy) {
			info_base::reset();
		}		
	}

	std::ostream& print_table(std::ostream & = std::cout) override;

/////////////////////
//data members
	
	//////////////////////
	//  Preproc info  
	//////////////////////
	
	//LB bounds - root
	double LB_0_no_AMTS_;					//LB_0 without AMTS
	double LB_0_AMTS_;						//LB_0 with AMTS
	
	//UB bounds root
	int branchingRootSize_;					//size of Branching set at ROOT	
	
	//sorting
	int idSortReal_;						//sorting policy really used by the algorithm
	bool isDegOrd_;							//FALSE - sorting partition based (COLOR_SORT), TRUE- sorting degree based (DEGREE_SORT)
		
	//////////////////////
	//	Additional search info 
	// (specific of certain algorithms)
	//////////////////////
	uint64_t nLastIsetPreFilterCalls_;		//calls to the bound
	uint64_t nLastIsetCalls_;				//calls to the bound
	uint64_t nCurrIsetCalls_;
	uint64_t nVertexCalls_;
	uint64_t nUBpartCalls_;

	uint64_t nsLastIsetPreFilterCalls_;
	uint64_t nsLastIsetCalls_;				//succesful calls to the bound
	uint64_t nsCurrIsetCalls_;
	uint64_t nsVertexCalls_;
	uint64_t nsUBpartCalls_;
						
};

//extern infoCLQ all_info;



#endif






