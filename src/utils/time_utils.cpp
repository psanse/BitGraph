/**
 * @file time_utils.cpp
 * @brief Implements the non-template time utilities of the UTILS library.
 *
 * This translation unit implements monotonic elapsed-time measurement,
 * local timestamp formatting, and conversion of local calendar components
 * to std::chrono::system_clock time points.
 *
 * Template utilities are implemented in time_utils.h because their
 * definitions must remain visible at their points of instantiation.
 *
 * @note Calendar conversions use the process's current local time zone.
 * @note The current timestamp implementation uses std::localtime(), which may
 *       not be thread-safe on every supported platform.
 *
 * @author Pablo San Segundo
 * @date Last updated: 22/09/2026
 */

#include "time_utils.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>			// for std::put_time

namespace bitgraph {
	namespace utils {
		
		double elapsed_time(
			std::chrono::steady_clock::time_point start_time) noexcept
		{
			return to_seconds(std::chrono::steady_clock::now() - start_time);
		}

		std::string to_local_timestamp(
			const std::chrono::system_clock::time_point& time_point,
			bool include_date)
		{
			const std::time_t calendar_time =
				std::chrono::system_clock::to_time_t(time_point);

			const std::tm* local_time = std::localtime(&calendar_time);
			if (local_time == nullptr) {
				return {};
			}

			std::ostringstream stream;
			stream << std::put_time(
				local_time,
				include_date ? "%H:%M:%S --- %d/%b/%Y" : "%H:%M:%S");

			return stream.str();
		}
					
		std::chrono::system_clock::time_point
			makeTimePoint(
				int year, 
				int mon, 
				int day,
				int hour,
				int min,
				int sec) 
		{
			struct std::tm time_info;

			time_info.tm_sec = sec;						// second of minute (0 .. 59 and 60 for leap seconds)
			time_info.tm_min = min;						// minute of hour (0 .. 59)
			time_info.tm_hour = hour;					// hour of day (0 .. 23)
			time_info.tm_mday = day;					// day of month (0 .. 31)
			time_info.tm_mon = mon - 1;					// month of year (0 .. 11)
			time_info.tm_year = year - 1900;			// year since 1900
			time_info.tm_isdst = -1;					// determine whether daylight saving time

			const std::time_t calendar_time = std::mktime(&time_info);

			if (calendar_time == static_cast<std::time_t>(-1)) {
				throw std::runtime_error(
					"make_time_point: invalid or unrepresentable local time");
			}

			return std::chrono::system_clock::from_time_t(calendar_time);
		}
	

	}//end namespace utils

} // namespace bitgraph





