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
		
		//////////////////////////
		//
		// struct paramBase
		// (General parameter info for all combinatorial algorithms)
		//
		//////////////////////////
		struct paramBase {

			std::string name = "";											//instance name
			uint32_t N = 0;													//number of vertices
			uint64_t M = 0;													//number of edges			
			double TIME_OUT = std::numeric_limits<double>::max();			//in seconds
			double TIME_OUT_HEUR = std::numeric_limits<double>::max();		//in seconds

			int nThreads = 1;												// number of threads
			bool unrolled = false;											// loop unrolling - possibly deprecated

			virtual void reset() {
				name = "";
				N = 0;
				M = 0;
				TIME_OUT = std::numeric_limits<double>::max();
				TIME_OUT_HEUR = std::numeric_limits<double>::max();
				nThreads = 1;
				unrolled = false;
			}

			//I/O		
			virtual std::ostream& print(std::ostream& o = std::cout, bool endl = true) const {
				o << "NAME: " << name
					<< "\nN: " << N
					<< "\nM: " << M
					<< "\nTOUT(s): " << TIME_OUT
					<< "\nTOUT_HEUR(s): " << TIME_OUT_HEUR
					<< "\nnTHREADS: " << nThreads;
				//  << "\nunrolled: " << std::boolalpha << unrolled;
				if (endl) o << std::endl;
				return o;
			}
		};

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

			/*
			* @brief determines elapsed time from @start_time to now in seconds
			* @param start_time - starting time point to determine duration
			* @returns elapsed time in seconds
			* @details: utility for time measurement outside the class
			* @details - moved to utilities utils::com::_time namespace (common.h) 31/08/2025
			*/
			//static double elapsedTime(tpoint_t start_time);

			//////////////////////
			//data members
			
			paramBase data_;							//general info

			// timers
			tpoint_t startTimeParse_;
			double timeParse_ = 0;					//parsing time(in seconds)
			tpoint_t startTimePreproc_;
			double timePreproc_ = 0;				//preprocessing time(in seconds)
			tpoint_t startTimeSearch_;
			double timeSearch_ = 0;					//search time (in seconds)
			tpoint_t startTimeIncumbent_;
			double timeIncumbent_ = 0;				//time when last new incumbent was found (in seconds)


			///////////////////////
			//constructors / destructor

			infoBase() = default;
			explicit infoBase(const paramBase& p) : data_(p) {}

			/////////////////////
			// getters

			std::string name() const noexcept { return data_.name; }
			uint32_t number_of_vertices() const noexcept { return data_.N; }
			uint64_t number_of_edges() const noexcept { return data_.M; };
			double time_out() const noexcept { return data_.TIME_OUT; };
			double time_out_heur() const noexcept { return data_.TIME_OUT_HEUR; }
			int number_of_threads() const  noexcept { return data_.nThreads; }

			double parsing_time() const { return timeParse_; }
			double preprocessing_time() const { return timePreproc_; }
			double search_time() const { return timeSearch_; }
			double incumbent_time() const { return timeIncumbent_; }

			//////////////////////
			//setters - only for general info, timers should not be set manually
						
			void name(std::string name) { data_.name = std::move(name); }
			void number_of_vertices(uint32_t n) { data_.N = n; }
			void number_of_edges(uint64_t m) { data_.M = m; }
			void time_out(double t) { data_.TIME_OUT = t; }
			void time_out_heur(double t) { data_.TIME_OUT_HEUR = t; }
			void number_of_threads(int n) { data_.nThreads = n; }


			//timers
			/*
			* @brief sets initial time in timer @t (previously set with startTimer(...))
			* @param t - phase_t enum
			*/
			void startTimer(phase_t t);

			/*
			* @brief reads time in timer @t (previously set with startTimer(...))
			* @param t - phase_t enum
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
									
			/*
			* @brief resets to default values
			* @param lazy - if true general info is NOT cleared, only timers
			*/
			virtual void clear(bool lazy = false);

		protected:
			/**
			* @brief clears general info - virtual since derived classes might have more general info to clear
			**/
			virtual void clearGeneralInfo() { data_.reset(); }					
						

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

		};

	}//namespace com

	using com::paramBase;
	using com::infoBase;
	using com::operator<<;

	
}//end namespace bitgraph



#endif






