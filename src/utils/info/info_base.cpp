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
			return info.printReport(o, infoBase::report_t::TABLE, true);
		}
	}
}


////////////////////////
// infoBase class implementation

std::ostream& infoBase::printParams(std::ostream& o) const
{
	o.setf(std::ios::fixed); // fixed notation for floating point values	
	o.precision(4);				// 4 decimal places

	o << "*****************************\n";
	o << "NAME:" << data_.name << "\t N:" << data_.N << "\t M:" << data_.M << "\t D:" << 2 * data_.M / (float)((data_.N - 1) * data_.N) << endl;
	o << "TIME_LIMIT:" << data_.TIME_OUT << endl;
	o << "TIME_LIMIT_HEUR:" << data_.TIME_OUT_HEUR << endl;
	o << "*****************************" << endl;

	return o;
}
std::ostream& infoBase::printTimers(std::ostream& o) const
{
	o.setf(std::ios::fixed);	// fixed notation for floating point values	
	o.precision(4);				// 4 decimal places

	o << "*****************************";
	o << "\ntime_parse:" << data_.timeElapsed
		<< "\ntime_preproc:" << timePreproc_
		<< "\ntime_incumbent:" << timeIncumbent_
		<< "\ntime_search:" << timeSearch_
		<< "\nTIME_OUT(s):" << data_.TIME_OUT
		<< "\nTIME_OUT_HEUR(s):" << data_.TIME_OUT_HEUR;
	
	return o;
}

std::ostream& infoBase::printReport( std::ostream& o, report_t r, bool is_endl) const
{
	o.setf(std::ios::fixed); // fixed notation for floating point values	
	o.precision(4);				// 4 decimal places


	if (r == report_t::TABLE) {
		o << data_.name << "\t" << data_.N << "\t" << data_.M << "\t" 
			<< data_.TIME_OUT << "\t" << data_.TIME_OUT_HEUR << "\t"
			<< data_.timeElapsed << "\t" << timePreproc_ << "\t" << timeIncumbent_ << "\t" << timeSearch_ << "\t";
	}
	if (r == report_t::VERBOSE) {
		o << "\n*****************************";
		o <<"\nNAME:"<< data_.name
		  << "\nN:" << data_.N
		  << "\nM:" << data_.M
		  << "\nTIME_OUT(s):" << data_.TIME_OUT
		  << "\nTIME_OUT_HEUR(s):" << data_.TIME_OUT_HEUR
		  << "\nTIME_PARSE(s):"	<< data_.timeElapsed
		  << "\nTIME_PREPROC(s):" << timePreproc_
		  << "\nTIME_INCUMBENT(s):" << timeIncumbent_ 
		  << "\nTIME_SEARCH(s)" << timeSearch_;
	}

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
	case phase_t::LAST_INCUMBENT:
		startTimeIncumbent_ = PrecisionTimer::clock_t::now();
		break;
	default:
		LOG_ERROR("bizarre timer type, exiting... - com::infoBase::start_timer");
		LOGG_ERROR("timer type: ", (int)t, " - com::infoBase::start_timer");
		std::abort();
	}
}


void infoBase::clearTimer(phase_t t) {
	switch (t) {
	case phase_t::SEARCH:		
		timeSearch_ = 0.0;
		break;	
	case phase_t::PREPROC:	
		timePreproc_ = 0.0;
		break;
	case phase_t::LAST_INCUMBENT:		
		timeIncumbent_ = 0.0;
		break;
	default:
		LOG_ERROR("bizarre timer type, exiting... - com::infoBase::clearTimer");
		LOGG_ERROR("timer type: ", (int)t, " - com::infoBase::clearTimer");
		std::abort();
	}
}

void infoBase::clearAllTimers() {	
	//clearTimer(phase_t::PARSE);
	clearTimer(phase_t::PREPROC);
	clearTimer(phase_t::LAST_INCUMBENT);
	clearTimer(phase_t::SEARCH);
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
	case phase_t::LAST_INCUMBENT:
		timeIncumbent_ = com::_time::toDouble(endTime - startTimeIncumbent_);
		elapsedTime = timeIncumbent_;
		break;

	default:
		LOG_ERROR("bizarre timer type, exiting... - infoBase::read_time");
		LOGG_ERROR("timer type : ", (int)t);
		std::abort();
	}

	return elapsedTime;
}

void infoBase::clear(bool lazy) {
	clearAllTimers();
	if (!lazy) {
		clearGeneralInfo();					//CHECK comment: "manually at the start of every run"
	}
}

















