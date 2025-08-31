/*
 *info_base.cpp: implementation of info_base class for result logs of clique and clique-based algorithms
 *@date 12/12/2012
 *@last_update 13/23/2024
 *@author pss
 */

#include "info_base.h"
#include "utils/logger.h"

#include <iostream>

using namespace std;

using namespace bitgraph;

////////////////////////
// friend functions of infoBase
namespace bitgraph {

	namespace com {
		ostream& operator<< (ostream& o, const infoBase& info) {
			return info.printReport(o);
		}

	}
}


////////////////////////
// infoBase class implementation

std::ostream& infoBase::printParams(std::ostream& o) const
{
	o << "*****************************\n";
	o << "NAME:" << data_.name << "\t N:" << data_.N << "\t M:" << data_.M << "\t D:" << 2 * data_.M / (float)((data_.N - 1) * data_.N) << endl;
	o << "TIME_LIMIT:" << data_.TIME_OUT << endl;
	o << "TIME_LIMIT_HEUR:" << data_.TIME_OUT_HEUR << endl;
	/*o << "ALG:" << algSearch_ << endl;
	o << "SORTING:" << algSort_ << endl;
	o << "HEUR:" << algHeur_ << endl;*/
	o << "*****************************" << endl;

	return o;
}
std::ostream& infoBase::printTimers(std::ostream& o) const
{
	o << "*****************************\n";
	o << "time_parse:" << timeParse_ << endl;
	o << "time_preproc:" << timePreproc_ << endl;
	o << "time_incumbent:" << timeIncumbent_ << endl;
	o << "time_search:" << timeSearch_ << endl;
	o << "TIME_OUT(s):" << data_.TIME_OUT << endl;
	o << "TIME_OUT_HEUR(s):" << data_.TIME_OUT_HEUR << endl;
	o << "*****************************" << endl;

	return o;
}

std::ostream& infoBase::printReport(std::ostream& o, bool is_endl) const
{
	o << data_.name << "\t" << data_.N << "\t" << data_.M << "\t" << data_.TIME_OUT << "\t" << data_.TIME_OUT_HEUR << "\t"
		/*<< algSearch_ << "\t"
		<< algSort_ << "\t"
		<< algHeur_ << "\t"*/
		<< timeParse_ << "\t" << timePreproc_ << "\t" << timeIncumbent_ << "\t" << timeSearch_ << "\t";

	if (is_endl) {	o << endl;	}

	return o;
}

void infoBase::startTimer(phase_t t)
{
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
		LOG_ERROR("bizarre timer type, exiting... - com::infoBase::start_timer");
		LOGG_ERROR("timer type: ", (int)t, " - com::infoBase::start_timer");
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
		LOGG_ERROR("timer type: ", (int)t, " - com::infoBase::clearTimer");
		std::exit(-1);
	}
}

void infoBase::clearAllTimers() {
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
		timeSearch_ = com::_time::toDouble(endTime - startTimeSearch_);
		elapsedTime = timeSearch_;
		break;
	case phase_t::PREPROC:
		timePreproc_ = com::_time::toDouble(endTime - startTimePreproc_);
		elapsedTime = timePreproc_;
		break;
	case phase_t::PARSE:
		timeParse_ = com::_time::toDouble(endTime - startTimeParse_);
		elapsedTime = timeParse_;
		break;
	case phase_t::LAST_INCUMBENT:
		timeIncumbent_ = com::_time::toDouble(endTime - startTimeIncumbent_);
		elapsedTime = timeIncumbent_;
		break;

	default:
		LOG_ERROR("bizarre timer type, exiting... - infoBase::read_time");
		LOGG_ERROR("timer type : ", (int)t);
		std::exit(-1);
	}

	return elapsedTime;
}

//double infoBase::elapsedTime(tpoint_t startTime)
//{
//	tpoint_t endTime = PrecisionTimer::clock_t::now();
//	return com::_time::toDouble(endTime - startTime);
//
//}


void infoBase::clear(bool lazy) {
	clearAllTimers();
	if (!lazy) {
		clearGeneralInfo();					//CHECK comment: "manually at the start of every run"
	}
}

















