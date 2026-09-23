/**
 * @file precision_timer.h
 * @brief Defines the PrecisionTimer elapsed-time utility.
 *
 * PrecisionTimer measures elapsed time using std::chrono::steady_clock and
 * generates local calendar timestamps using std::chrono::system_clock.
 *
 * The historical wall_tic(), wall_toc(), cpu_tic(), and cpu_toc() functions
 * are retained for backward compatibility. Despite their names, both timer
 * pairs measure elapsed wall time rather than process CPU time.
 *
 * @author Pablo San Segundo
 * @date Created: 01/11/2024
 * @date Last updated: 23/09/2026
 */

#ifndef BITSCAN_UTILS_PRECISION_TIMER_H
#define BITSCAN_UTILS_PRECISION_TIMER_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS		//for std::localtime Windows (VS)
#elif __GNUC__
#include <sys/time.h>				//TODO@ CHECK if it is necessary after the refactoring (26/01/2025)
#endif

#include <chrono>
#include <string>

#include "time_utils.h"

namespace bitgraph {

	namespace utils {

		/**
		 * @brief Measures elapsed time and produces local timestamps.
		 *
		 * Elapsed intervals are measured with std::chrono::steady_clock so that
		 * system-clock adjustments cannot affect the result. Calendar timestamps use
		 * std::chrono::system_clock.
		 *
		 * @note The cpu_tic() and cpu_toc() compatibility functions do not measure
		 *       process CPU consumption; they measure elapsed monotonic time.
		 */

		class PrecisionTimer
		{
		public:
			using clock_t = std::chrono::steady_clock;					
			using wall_clock_t = std::chrono::system_clock;						
			using timepoint_t = clock_t::time_point;
			using wall_timepoint_t = wall_clock_t::time_point;

		public:

			/** @brief Starts the historical wall-time timer. */
			void wall_tic() noexcept { 
				wall_time = get_wall_time(); 
			}

			/**
			 * @brief Returns the elapsed time since the last wall_tic() call.
			 * @return Elapsed time in seconds.
			 */
			double wall_toc()  const noexcept{ 
				return to_seconds(get_wall_time() - wall_time); 
			};

			/** @brief Starts the historical CPU timer. */
			void cpu_tic() {
				cpu_time = get_cpu_time(); 
			}

			/**
			 * @brief Returns the elapsed time since the last cpu_tic() call.
			 *
			 * @return Elapsed monotonic time in seconds.
			 * @note This function does not measure process CPU consumption.
			 */
			double cpu_toc() const noexcept {
				return to_seconds(get_cpu_time() - cpu_time); 
			};

			/**
			* @brief Returns the current local timestamp.
			*
			* @param include_date Whether to include the calendar date.
			* @return Formatted local timestamp.
			*/
			static std::string local_timestamp(bool include_date = true) {
				return to_local_timestamp(
					wall_clock_t::now(), 
					include_date);				//MUST BE wall clock
			}
		private:
			timepoint_t get_cpu_time() const { 
				return clock_t::now(); 
			}
			wall_timepoint_t get_wall_time() const {
				return wall_clock_t::now(); 
			}

		private:
			timepoint_t cpu_time;
			wall_timepoint_t wall_time;
		};

	}//end namespace utils

	// Backward-compatible alias. New code should use
	// bitgraph::utils::PrecisionTimer.
	using utils::PrecisionTimer;

}//end namespace bitgraph

#endif // BITSCAN_UTILS_PRECISION_TIMER_H


