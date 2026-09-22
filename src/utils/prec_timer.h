/**
* @file: prec_timer.h
* @brief: header for class PreciseTimer that manages timestamps and time intervals
* @details: uses std::chrono 
* @detals: created 01/11/2024, last update 06/07/2025
**/

#ifndef BITSCAN_UTILS_PRECISION_TIMER_H
#define BITSCAN_UTILS_PRECISION_TIMER_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS		//for std::localtime Windows (VS)
#elif __GNUC__
#include <sys/time.h>				//TODO@ CHECK if it is necessary after the refactoring (26/01/2025)
#endif

#include "time_utils.h"

/******************
*
* Class PrecisionTimer
*
* computes timestamp and time intervals
*
********************/

namespace bitgraph {

	namespace utils {

		class PrecisionTimer
		{
		public:
			using clock_t = std::chrono::high_resolution_clock;					//chrono::steady_clock = chrono::high_resolution_clock in VS 2015
			using wall_clock_t = std::chrono::system_clock;						//MUST BE
			using timepoint_t = std::chrono::time_point<clock_t>;
			using wall_timepoint_t = std::chrono::time_point<wall_clock_t>;

		public:
			void wall_tic() { wall_time = get_wall_time(); }
			double wall_toc()  const noexcept{ return to_seconds(get_wall_time() - wall_time); };
			void cpu_tic() { cpu_time = get_cpu_time(); }
			double cpu_toc() const noexcept { return to_seconds(get_cpu_time() - cpu_time); };

			static std::string local_timestamp(bool date = true) {
				return to_local_timestamp(wall_clock_t::now(), date);				//MUST BE wall clock
			}
		private:
			timepoint_t get_cpu_time() const { return clock_t::now(); }
			wall_timepoint_t get_wall_time() const { return wall_clock_t::now(); }

		private:
			timepoint_t cpu_time;
			wall_timepoint_t wall_time;
		};

	}//end namespace _impl

	// Backward-compatible alias. New code should use bitgraph::utils::PrecisionTimer.
	using utils::PrecisionTimer;

}//end namespace bitgraph

#endif // BITSCAN_UTILS_PRECISION_TIMER_H


