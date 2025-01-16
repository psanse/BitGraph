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
#include <string>
#include <limits>
#include <vector>

namespace com {

	using tpoint_t = PrecisionTimer::timepoint_t;

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

		static const std::string FILE_DEFAULT_LOG;

		enum class phase_t { SEARCH = 0, PREPROC, LAST_INCUMBENT, PARSE };

		/*
		* @brief elapsed time from start_time to now
		*/
		static double elapsedTime(tpoint_t start_time);
			
		//timers
		void startTimer		(phase_t t);
		double readTimer	(phase_t t);
	
		//clear context 
		virtual void clear (bool lazy = false);

	protected:	
		void clearGeneralInfo();					//CHECK comment: "manually at the start of every run"	
		void clearTimers	 ();					
		void clearTimer		 (phase_t t);
		
		//I/O
	public:
		friend std::ostream& operator<<	(std::ostream&, const infoBase&);

		//results in table format for output file
		//TODO Add @K_ to ouput conditionally
virtual std::ostream& printReport		(std::ostream & = std::cout, bool is_endl = true) const;

		std::ostream& printParams		(std::ostream & = std::cout) const;
		std::ostream& printTimers		(std::ostream & = std::cout) const;

//////////////////////
//data members
 
		/////////////////////
		//general info
				
		std::string nameInstance_	= "";								//instance name
		uint32_t N_ = 0;												//number of vertices
		uint64_t M_ = 0;												//number of edges		
		uint32_t K_ = 0;												//for k-clique search
		double TIME_OUT_		= std::numeric_limits<double>::max();	//seconds
		double TIME_OUT_HEUR_	= std::numeric_limits<double>::max();	//seconds

		//algorithms
		 		
		int idAlg_  = -1;							//algorithm identifier
		int idHeur_ = -1;							//root heuristic policy (e.g. AMTS, no AMTS or combined with other heuristics)	
		int idSort_ = -1;							//sorting policy selected as input configuration parameter (might not be the final choice)

		
		// timers		

		tpoint_t startTimeParse_;
		double timeParse_ = 0 ;						//parsing time(in seconds)

		tpoint_t startTimePreproc_;
		double timePreproc_ = 0;					//preprocessing time(in seconds)

		tpoint_t startTimeSearch_;
		double timeSearch_ = 0;						//search time(in seconds)

		tpoint_t startTimeIncumbent_;
		double timeIncumbent_ = 0 ;					//time when last new incumbent was found (in seconds)

	};
	
}



#endif






