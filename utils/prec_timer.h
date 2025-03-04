//prec_timer.h:		class PreciseTimer for timestamps and time intervals
//					uisng std::chrono from the SL
//
//@dev: pss
//@date: 01/11/2024
/////////////////////////////////////////

#ifndef __PRECISION_TIMER__
#define __PRECISION_TIMER__

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS		//for std::localtime Windows (VS)
#elif __GNUC__
#include <sys/time.h>				//TODO@ CHECK if it is necessary after the refactoring (26/01/2025)
#endif

#include "utils/common.h"

/******************
*
* Class PrecisionTimer
*
* computes timestamp and time intervals
*
********************/

class PrecisionTimer
{
public:
	using clock_t = std::chrono::high_resolution_clock;					//chrono::steady_clock = chrono::high_resolution_clock in VS 2015
	using wall_clock_t = std::chrono::system_clock;						//MUST BE
	using timepoint_t = std::chrono::time_point<clock_t>;
	using wall_timepoint_t = std::chrono::time_point<wall_clock_t>;

public:
	void wall_tic() { wall_time = get_wall_time(); }
	double wall_toc()  const { return com::time::toDouble(get_wall_time() - wall_time); };
	void cpu_tic() { cpu_time = get_cpu_time(); }
	double cpu_toc() const { return com::time::toDouble(get_cpu_time() - cpu_time); };

	static std::string local_timestamp(bool date = true) {
		return com::time::tp2string(wall_clock_t::now(), date);				//MUST BE wall clock
	}
private:
	timepoint_t get_cpu_time() const { return clock_t::now(); }
	wall_timepoint_t get_wall_time() const { return wall_clock_t::now(); }

private:
	timepoint_t cpu_time;
	wall_timepoint_t wall_time;
};

#endif


