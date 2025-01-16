/*
 *info_base.cpp: implementation of info_base class for result logs of clique and clique-based algorithms
 *@date 12/12/2012
 *@last_update 13/23/2024
 *@dev pss
 */

#include "info_base.h"
#include "utils/logger.h"

#include <iostream>

using namespace std;

namespace com {

	ostream& operator<<(ostream& o, const infoBase& info) {
		return info.printTable(o);
	}

	std::ostream& infoBase::printParams(std::ostream& o) const
	{	
		o << endl;
		o << "*****************************";
		o << "DATA:" << nameInstance_.c_str() << "\t N:" << N_ << "\t M:" << M_ << "\t D:" << 2 * M_ / (float)((N_ - 1) * N_) << endl;
		o << "TIME_LIMIT:" << TIME_OUT_ << endl;
		o << "TIME_LIMIT_HEUR:" << TIME_OUT_HEUR_ << endl;
		o << "ALG:" << idAlg_ << endl;
		o << "SORTING:" << idSort_ << endl;
		o << "HEUR:" << idHeur_ << endl;
		o << "*****************************" << endl;
		
		if (this->K_ != 0) {
			o << "MAX UB:" << K_ << endl;
		}
			
		return o;
	}
	std::ostream& infoBase::printTimers(std::ostream& o) const
	{
		o << endl;
		o << "*****************************";
		//LOG_INFO("start_time_parse:" << startTimeParse_ / (float)CLOCKS_PER_SEC);
		//LOG_INFO("start_time_preproc:" << startTimePreproc_ / (float)CLOCKS_PER_SEC);
		//LOG_INFO("start_time_incumbent:" << startTimeIncumbent_ / (float)CLOCKS_PER_SEC);
		//LOG_INFO("start_time_search:" << startTimeSearch_ / (float)CLOCKS_PER_SEC);
		o << "time_parse:" << timeParse_ << endl;
		o << "time_preproc:" << timePreproc_ << endl;
		o << "time_incumbent:" << timeIncumbent_ << endl;
		o << "time_search:" << timeSearch_ << endl;
		o << "TIME_LIMIT:" << TIME_OUT_ << endl;
		o << "TIME_LIMIT_HEUR:" << TIME_OUT_HEUR_ << endl;
		o << "*****************************" << endl;
			
		return o;
	}


	std::ostream& infoBase::printTable(std::ostream& o, bool is_endl) const
	{
		o << nameInstance_.c_str() << "\t" << N_ << "\t" << M_ << "\t" << TIME_OUT_ << "\t" << TIME_OUT_HEUR_ << "\t"
			<< idAlg_ << "\t"
			<< idSort_ << "\t"
			<< idHeur_ << "\t"
			<< timeParse_ << "\t" << timePreproc_ << "\t" << timeIncumbent_ << "\t" << timeSearch_ << "\t";

			if (is_endl) {
				o << std::endl;
			}
		

		return o;
	}

	void infoBase::startTimer(phase_t t)
	{
		//TODO -here: reset data for search and preprocessing selectively in this function (i.e. reset_preproc_info(); reset_search_info(); reset_bound_info())....

		switch (t) {
		case phase_t::SEARCH:
			startTimeSearch_ = PrecisionTimer::clock_t::now();
			//resetSearchInfo();					//CHECK()?
			break;
		case phase_t::PREPROC:
			startTimePreproc_ = PrecisionTimer::clock_t::now();
			//resetPreprocInfo();					//CHECK()?
			break;
		case phase_t::PARSE:
			startTimeParse_ = PrecisionTimer::clock_t::now();
			break;
		case phase_t::LAST_INCUMBENT:
			startTimeIncumbent_ = PrecisionTimer::clock_t::now();
			break;

		default:
			LOG_ERROR("bizarre timer type, exiting... - com::infoCLQ::start_timer");
			LOG_ERROR("timer type: ", (int)t, " - com::infoBase::start_timer");
			std::exit(-1);
		}
	}


	void infoBase::clearTimer(phase_t t) {
		switch (t) {
		case phase_t::SEARCH:
			//startTimeSearch_ = startTimeSearch_();
			timeSearch_ = 0.0;
			break;
		case phase_t::PARSE:
			//startTimeParse_ = startTimeParse_();
			timeParse_ = 0.0;
			break;
		case phase_t::PREPROC:
			//startTimePreproc_ = startTimePreproc_();
			timePreproc_ = 0.0;
			break;
		case phase_t::LAST_INCUMBENT:
			//startTimeIncumbent_ = startTimeIncumbent_();
			timeIncumbent_ = 0.0;
			break;
		default:
			LOG_ERROR("bizarre timer type, exiting... - com::infoBase::clearTimer");
			LOG_ERROR("timer type: ", (int)t, " - com::infoBase::clearTimer");
			std::exit(-1);
		}
	}

	void infoBase::clearTimers() {
		clearTimer(phase_t::SEARCH);
		clearTimer(phase_t::PARSE);
		clearTimer(phase_t::PREPROC);
		clearTimer(phase_t::LAST_INCUMBENT);
	}

	double infoBase::readTimer(phase_t t)
	{
		double elapsedTime;
		tpoint_t endTime = PrecisionTimer::clock_t::now();

		switch (t) {
		case phase_t::SEARCH:
			timeSearch_ = com::time::toDouble(endTime - startTimeSearch_);
			elapsedTime = timeSearch_;
			break;
		case phase_t::PREPROC:
			timePreproc_ = com::time::toDouble(endTime - startTimeSearch_);
			elapsedTime = timePreproc_;
			break;
		case phase_t::PARSE:
			timeParse_ = com::time::toDouble(endTime - startTimeSearch_);
			elapsedTime = timeParse_;
			break;
		case phase_t::LAST_INCUMBENT:
			timeIncumbent_ = com::time::toDouble(endTime - startTimeSearch_);
			elapsedTime = timeIncumbent_;
			break;

		default:
			LOG_ERROR("bizarre timer type, exiting...-infoCLQ::read_time");
			cout << "timer type: " << (int)t << endl;
			std::exit(-1);
		}

		return elapsedTime;
	}

	double infoBase::elapsedTime(tpoint_t startTime)
	{
		tpoint_t endTime = PrecisionTimer::clock_t::now();
		return com::time::toDouble(endTime - startTime);

	}

	void infoBase::clearGeneralInfo() {
		nameFileLog_.clear();
		nameInstance_.clear();
		N_ = 0;
		M_ = 0;
		K_ = 0;
		TIME_OUT_ = DBL_MAX;
		TIME_OUT_HEUR_ = DBL_MAX;
		idAlg_ = -1;
		idHeur_ = -1;
		idSort_ = -1;
	}

	void infoBase::clear(bool lazy) {
		clearTimers();
		if (!lazy) {
			clearGeneralInfo();					//CHECK comment "manually at the start of every run"
		}
	}
}
















