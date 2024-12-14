/*
 *info_base.h: interface for result logs of clique and clique-based algorithms
 *@date 12/12/2024
 *@last_update 13/23/2024
 *@dev pss
 */

 //TODO - UNDER DEVELOPMENT!!!!!!!!!!!! (12/12/2024)

#ifndef _INFO_BASE_GRAPH_H_
#define _INFO_BASE_GRAPH_H_

#include "../prec_timer.h"
#include <vector>
#include <iostream>
#include <limits>

using tpoint_t = PreciseTimer::timepoint_t;

/////////////////////////////////////////////////////////
constexpr std::string FILE_LOG = "info_clique.txt";
/////////////////////////////////////////////////////////

//////////////////////
//
//	infoBase
// 
//  @brief base class to report results of graph algorithms
//
///////////////////////

struct infoBase {
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
	int idHeur_;							//root heuristic policy (currently AMTS, no AMTS or combined with other heuristics)	
	int idSort_;							//sorting policy selected as input configuration parameter (might not be the final choice)
		
//////////////////////
// timers
//////////////////////
	
	tpoint_t startTimeParse_;
	double timeParse_;						//parsing time(in seconds)

	tpoint_t startTimePreproc_;
	double timePreproc_;					//preprocessing time(in seconds)

	tpoint_t startTimeSearch_;
	double timeSearch_;						//search time(in seconds)
		
	tpoint_t startTimeIncumbent_;
	double timeIncumbent_;					//time when last new incumbent was found (in seconds)

/////////////////////////////
//interface

	infoBase() :
		nameFileLog_(FILE_LOG), nameInstance_(""),
		N_(0), M_(0), K_(0),
		TIME_OUT_(std::numeric_limits<double>::max()), TIME_OUT_HEUR_(std::numeric_limits<double>::max()),
		idAlg_(-1), idHeur_(-1), idSort_(-1)
	{}
			
	//timers
	void startTimer(phase_t t);
	double readTimer(phase_t t);
	double elapsedTime(tpoint_t start_time);
		
	//context 
	void clearGeneralInfo();				//CHECK comment: "manually at the start of every run"	
	void clearTimers();						//clears all timers
	void clearTimer(phase_t t);
	
	void clear() {
		clearGeneralInfo();					//CHECK comment "manually at the start of every run"
		clearTimers();

		//other info in derived classes
		clearPreprocInfo();					//virtual call - does nothing at this level
		clearSearchInfo();					//virtual call - does nothing at this level
	}

	virtual void clearPreprocInfo() {}
	virtual void clearSearchInfo() {}

	/////////////
	//I/O
	friend std::ostream& operator<<(std::ostream&, const infoBase&);
	virtual std::ostream& printTable(std::ostream & = std::cout) const;								//results in table format for output file

	std::ostream& printParams(std::ostream& = std::cout) const;
	std::ostream& printTimers(std::ostream& = std::cout) const;

	virtual std::ostream& printResults(std::ostream& o = std::cout)  const { return o; }			//Does nothing at this level
};

//////////////////////
//
//	infoCLQ
// 
//  @brief info on results for MCP algorithms
//		   (some are CliSAT specific)
//
///////////////////////

struct infoCLQ : public infoBase {
	infoCLQ() :infoBase(), 
		incumbent_(0), optimum_(0), nSteps_(0), isTimeOutReached_(false),
		LB_0_no_AMTS(0), LB_0_AMTS(0),
		branchingRootSize_(0),
		idSortReal_(-1), 
		isDegOrd_(false),
		nLastIsetPreFilterCalls(0), nLastIsetCalls(0), nCurrIsetCalls(0), nVertexCalls(0), nUBpartCalls(0),
		nsLastIsetPreFilterCalls(0), nsLastIsetCalls(0), nsCurrIsetCalls(0), nsVertexCalls(0), nsUBpartCalls(0)
	{}

	//context
	void clearPreprocInfo() override;
	void clearSearchInfo() override;
	
	/*
	*@signature clear(bool lazy)
	*@brief clears context
	*@param lazy - if true, only search and preprocessing info is cleared
	*/ 
	void clear(bool lazy=true){	
		clearPreprocInfo()
		clearSearchInfo();													
		clearTimers();
		if (!lazy) {
			infoBase::clear();						//clears general info
		}		
	}

	//I/O
	std::ostream& printResults(std::ostream & = std::cout) const;							//results (summary) in nice format for console			
	std::ostream& printTable(std::ostream & = std::cout) const override;					//results in table format for output file

/////////////////////
//data members

	/////////////////////
	//search
	////////////////////
	uint32_t incumbent_;					//best solution value found during search (not in preproc)
	uint32_t optimum_;						//best solution value (omega) - will be the same as incumbent is time out limit is not reached
	uint64_t nSteps_;						//recursive calls to the algorithm		
	bool  isTimeOutReached_;				//flag time out	
	std::vector<int> sol_;					//best solution (vertices)

	
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
	// (specific of certain algorithms - CliSAT)
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






