/**
 * @file time_utils.h
 * @brief Time measurement, conversion, and formatting utilities.
 *
 * Provides functions for converting durations to seconds, measuring elapsed
 * monotonic time, formatting system-clock time points, and constructing
 * system-clock time points from calendar components.
 *
 * @author Pablo San Segundo
 * @date Last updated: 22/09/2026
 */

#ifndef BITSCAN_UTILS_TIME_UTILS_H
#define BITSCAN_UTILS_TIME_UTILS_H	

#include <chrono>
#include <iostream>
#include <string>

namespace bitgraph {

	namespace utils {
				
		/**
		 * @brief Writes a duration's tick count and period to a stream.
		 *
		 * @tparam Rep Arithmetic representation used by the duration.
		 * @tparam Period Duration tick period.
		 * @param out Output stream.
		 * @param duration Duration to write.
		 * @return Reference to @p out.
		 */
		template<typename Rep, typename Period>
		inline std::ostream& print_duration(
			std::ostream& out,
			const std::chrono::duration<Rep, Period>& duration);
		
		/**
		 * @brief Converts a duration to seconds.
		 *
		 * @tparam Rep Arithmetic representation used by the duration.
		 * @tparam Period Duration tick period.
		 * @param duration Duration to convert.
		 * @return Duration expressed in seconds.
		 */
		template<typename Rep, typename Period>
		inline double to_seconds(
			const std::chrono::duration<Rep, Period>& duration) noexcept;
		
		/**
		 * @brief Returns the elapsed monotonic time since @p start_time.
		 *
		 * @param start_time Starting point of the measured interval.
		 * @return Elapsed time in seconds.
		 */
		double elapsed_time(
			std::chrono::steady_clock::time_point start_time) noexcept;
			
		/**
		 * @brief Converts a system-clock time point to a local timestamp.
		 *
		 * @param time_point Time point to format.
		 * @param include_date Whether to include the calendar date.
		 * @return Formatted local timestamp.
		 *
		 * @note This implementation calls std::localtime(), which may not be
		 *       thread-safe.
		 */
		std::string to_local_timestamp(
			const std::chrono::system_clock::time_point& time_point,
			bool include_date = true);
		

		/**
		 * @brief Creates a system-clock time point from local calendar components.
		 *
		 * @param year Full calendar year.
		 * @param month Month in the range [1, 12].
		 * @param day Day of the month.
		 * @param hour Hour in the range [0, 23].
		 * @param minute Minute in the range [0, 59].
		 * @param second Second in the range [0, 60].
		 * @return Corresponding system-clock time point.
		 */
		std::chrono::system_clock::time_point
			makeTimePoint(
				int year, 
				int mon, 
				int day,
				int hour,
				int min,
				int sec = 0);
		
	}//end namespace utils

} // namespace bitgraph

/////////////////////////////////
// Necessary implementations in headers


namespace bitgraph {
	namespace utils {
			
		template<typename Rep, typename Period>
		inline std::ostream& print_duration(
			std::ostream& out,
			const std::chrono::duration<Rep, Period>& duration)
		{
			return out << '['
				<< duration.count()
				<< " ticks of "
				<< Period::num
				<< '/'
				<< Period::den
				<< " seconds]";
		}

		template<typename Rep, typename Period>
		inline double to_seconds(
			const std::chrono::duration<Rep, Period>& duration) noexcept
		{
			return std::chrono::duration_cast<
				std::chrono::duration<double>>(duration).count();
		}
			
		

	}	// namespace utils

}// namespace bitgraph


#endif // BITSCAN_UTILS_TIME_UTILS_H