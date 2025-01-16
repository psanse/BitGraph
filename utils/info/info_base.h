/*
 *info_base.h: interface for result logs of clique and clique-based algorithms
 *@date 12/12/2024
 *@last_update 13/23/2024
 *@dev pss
 */

#ifndef _INFO_BASE_GRAPH_H_
#define _INFO_BASE_GRAPH_H_

#include "../prec_timer.h"
#include <iostream>
#include <limits>
#include <vector>

namespace com {

	using tpoint_t = PrecisionTimer::timepoint_t;

	/////////////////////////////////////////////////////////
	constexpr char* FILE_LOG = "info_clique.txt";
	/////////////////////////////////////////////////////////

	//////////////////////
	//
	//	infoBase
	// 
	//  @brief base class to report results of graph algorithms.
	// 
	//  Supports basic configuration parameters and timers.
	// 
	///////////////////////

	struct infoBase {
		enum class phase_t { SEARCH = 0, PREPROC, LAST_INCUMBENT, PARSE };

		/////////////////////
		//general info
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
		int idHeur_;							//root heuristic policy (e.g. AMTS, no AMTS or combined with other heuristics)	
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
			timeParse_(0.0), timePreproc_(0.0), timeSearch_(0.0), timeIncumbent_(0.0),
			idAlg_(-1), idHeur_(-1), idSort_(-1)
		{}

		//timers
		void startTimer	(phase_t t);
		double readTimer(phase_t t);

		/*
		* @brief elapsed time from start_time to now
		*/
		static double elapsedTime(tpoint_t start_time);		

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

		//to override - Template Method Pattern
		virtual void clearPreprocInfo() {}
		virtual void clearSearchInfo() {}

		/////////////
		//I/O
		friend std::ostream& operator<<	(std::ostream&, const infoBase&);
		virtual std::ostream& printTable(std::ostream & = std::cout) const;								//results in table format for output file

		std::ostream& printParams		(std::ostream & = std::cout) const;
		std::ostream& printTimers		(std::ostream & = std::cout) const;

		virtual std::ostream& printResults(std::ostream& o = std::cout)  const { return o; }			//Does nothing at this level
	};
}




#endif






