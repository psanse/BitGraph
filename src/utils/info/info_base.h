/**
 *@file info_base.h
* @brief: interface for result logs of clique and clique-based algorithms
 *@details: created 12/12/2024, last_update 13/23/2024
 *@dev pss
 **/

#ifndef _INFO_BASE_H_
#define _INFO_BASE_H_

#include "utils/prec_timer.h"
#include <iostream>
#include <string>
#include <limits>
#include <vector>

namespace bitgraph {

	namespace com {
		
		//////////////////////
		//
		//	infoBase
		// 
		//  @brief base struct to report results of graph algorithms.
		// 
		//  Supports basic configuration parameters and timers.
		// 
		///////////////////////

		struct infoBase {

			using tpoint_t = PrecisionTimer::timepoint_t;

			enum phase_t { SEARCH = 0, PREPROC, LAST_INCUMBENT, PARSE };

			/////////////////////
			// getters

			std::string name() const { return name_; }
			uint32_t number_of_vertices() const { return N_; }
			uint64_t number_of_edges() const { return M_; };
			double time_out() const { return TIME_OUT_; };
			double time_out_heur() const { return TIME_OUT_HEUR_; }
			/*int search_algorithm() const { return algSearch_; }
			int heuristic_algorithm() const { return algHeur_; }
			int sorting_algorithm() const { return algSort_; }*/
			double parsing_time() const { return timeParse_; }
			double preprocessing_time() const { return timePreproc_; }
			double search_time() const { return timeSearch_; }
			double incumbent_time() const { return timeIncumbent_; }

			//////////////////////

			/*
			* @brief elapsed time from start_time to now
			*/
			static double elapsedTime(tpoint_t start_time);

			//timers
			void startTimer(phase_t t);

			/*
			* @brief reads time in timer t (previously set with startTimer(...))
			*/
			double readTimer(phase_t t);

			/*
			* @brief clears appropiate time duration (concerning phase_t @t) 
			*/
			void clearTimer(phase_t t);

			/**
			* @brief clears all timers
			**/
			void clearAllTimers();


			//clear context 
			/*
			* @brief clears context
			* @param lazy - if true general info is NOT cleared, only timers
			*/
			virtual void clear(bool lazy = false);

		protected:
			/**
			* @brief clears general info - virtual since derived classes might have more general info to clear
			**/
			virtual void clearGeneralInfo();					//CHECK comment: "manually at the start of every run"	
			
			

			//I/O
		public:
			friend std::ostream& operator<<	(std::ostream&, const infoBase&);

			/*
			* @brief streams all info
			* @param o: output stream
			* @param is_endl: if true adds endl at the end
			* @returns output stream
			*
			* TODO Add @K_ to ouput conditionally
			*/
			virtual std::ostream& printReport(std::ostream& o = std::cout, bool is_endl = true) const;

			/*
			* @brief streams gereral info
			* @param o: output stream
			* @returns output stream
			*/
			virtual	std::ostream& printParams(std::ostream& o = std::cout) const;

			/*
			* @brief streams timer info
			* @param o output stream
			* @returns output stream
			*/
			std::ostream& printTimers(std::ostream& o = std::cout) const;

			//////////////////////
			//data members
			//////////////////////
						
			//general info
			std::string name_ = "";											//instance name
			uint32_t N_ = 0;												//number of vertices
			uint64_t M_ = 0;												//number of edges			
			double TIME_OUT_ = std::numeric_limits<double>::max();			//in seconds
			double TIME_OUT_HEUR_ = std::numeric_limits<double>::max();		//in seconds

			//algorithms

			//int algSearch_ = -1;						//algorithm identifier
			//int algHeur_ = -1;							//root heuristic policy (e.g. AMTS, no AMTS or combined with other heuristics)	
			//int algSort_ = -1;							//sorting policy selected as input configuration parameter (might not be the final choice)


			// timers		

			tpoint_t startTimeParse_;
			double timeParse_ = 0;						//parsing time(in seconds)

			tpoint_t startTimePreproc_;
			double timePreproc_ = 0;					//preprocessing time(in seconds)

			tpoint_t startTimeSearch_;
			double timeSearch_ = 0;						//search time (in seconds)

			tpoint_t startTimeIncumbent_;
			double timeIncumbent_ = 0;					//time when last new incumbent was found (in seconds)

		};

	}//namespace com

	using com::infoBase;
	using com::operator<<;

}//end namespace bitgraph



#endif






