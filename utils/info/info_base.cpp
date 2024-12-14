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

ostream& operator<<(ostream& o, const info_base& info) {
	return info.printTable(o);
}

std::ostream& infoBase::printParams(std::ostream& o) const
{
	LOG_INFO("");
	LOG_INFO("*****************************");
	LOG_INFO("DATA:" << nameInstance_.c_str() << "\t N:" << N_ << "\t M:" << M_ << "\t D:" << 2 * M_ / (float)((N_ - 1) * N_));
	LOG_INFO("TIME_LIMIT:" << TIME_OUT_);
	LOG_INFO("TIME_LIMIT_HEUR:" << TIME_OUT_HEUR_);
	LOG_INFO("ALG:" << idAlg_);
	LOG_INFO("SORTING:" << idSort_);
	LOG_INFO("AMTS:" << idHeur_);
	if (this->K_ != 0) {
		LOG_INFO("MAX UB:" << K);
	}
	LOG_INFO("*****************************");
	return o;
}
std::ostream& infoBase::printTimers(std::ostream& o) const
{
	LOG_INFO("");
	LOG_INFO("*****************************");
	//LOG_INFO("start_time_parse:" << startTimeParse_ / (float)CLOCKS_PER_SEC);
	//LOG_INFO("start_time_preproc:" << startTimePreproc_ / (float)CLOCKS_PER_SEC);
	//LOG_INFO("start_time_incumbent:" << startTimeIncumbent_ / (float)CLOCKS_PER_SEC);
	//LOG_INFO("start_time_search:" << startTimeSearch_ / (float)CLOCKS_PER_SEC);
	LOG_INFO("time_parse:" << timeParse_);
	LOG_INFO("time_preproc:" << timePreproc_);
	LOG_INFO("time_incumbent:" << timeIncumbent_);
	LOG_INFO("time_search:" << timeSearch_);
	LOG_INFO("TIME_LIMIT:" << TIME_OUT_);
	LOG_INFO("TIME_LIMIT_HEUR:" << TIME_OUT_HEUR_);
	LOG_INFO("*****************************");
	return o;
}


std::ostream& infoBase::printTable(std::ostream& o) const
{
	  o	<< nameInstance_.c_str() << "\t" << N_ << "\t" << M_ << "\t" << TIME_OUT_ << "\t" << TIME_OUT_HEUR_ << "\t" << idAlg_ << "\t"
		<< idSort_ << "\t" << idSortReal_ << "\t"
		<< idHeur_ << "\t"			
		<< timeParse_ << "\t" << timePreproc_ << "\t" << timeIncumbent_ << "\t" << timeSearch_ << "\t" 
		<< std::endl;

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
		LOG_ERROR("bizarre timer type, exiting... - infoCLQ::start_timer");
		cout << "timer type: " << (int)t << endl;
		exit(-1);
	}	
}


void infoBase::clearTimer(phase_t t) {
	switch (t) {
	case phase_t::SEARCH:
		startTimeSearch_ = startTimeSearch_();
		timeSearch_ = 0.0;
		break;
	case phase_t::PARSE:
		startTimeParse_ = startTimeParse_();
		timeParse_ = 0.0;
		break;
	case phase_t::PREPROC:
		startTimePreproc_ = startTimePreproc_();
		timePreproc_ = 0.0;
		break;
	case phase_t::LAST_INCUMBENT:
		startTimeIncumbent_ = startTimeIncumbent_();
		timeIncumbent_ = 0.0;
		break;	
	default:
		LOG_ERROR("bizarre timer type, exiting...-infoCSP::clear_time");
		cout << "timer type: " << (int)t << endl;
		exit(-1);
	}
}

void infoBase::clearTimers() {
	clearTimer(SEARCH);
	clearTimer(PARSE);
	clearTimer(PREPROC);
	clearTimer(LAST_INCUMBENT);
}

double infoBase::readTimer(phase_t t)
{
	double elapsedTime;
	tpoint_t endTime = PrecisionTimer::clock_t::now();
	
	switch (t) {
	case phase_t::SEARCH:
		timeSearch_ =  com::time::toDouble(endTime - startTimeSearch_);
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
		exit(-1);
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



///////////////////////////
//
// DERIVED CLASS

std::ostream& infoCLQ::printResults(std::ostream& o) const
{
	LOG_INFO("*****************************");
	LOG_INFO("w:" << optimum_ << "\tt_par: " << timeParse_ << "\tt_pp: " << timePreproc_ << "\tt_search: " << timeSearch_ << "\t#steps: " << nSteps_);
	LOG_INFO("*****************************");
	return o;
}

std::ostream& infoCLQ::printTable(std::ostream& o) const
{
	  o	<< nameInstance_.c_str() << "\t" << N_ << "\t" << M_ << "\t" << TIME_OUT_ << "\t" << TIME_OUT_HEUR_ << "\t" << idAlg_ << "\t"
		<< idSort_ << "\t" << idSortReal_ << "\t"
		<< isDegOrd_ << "\t" << idHeur_ << "\t"
		<< LB_0_no_AMTS_ << "\t" << LB_0_AMTS_ << "\t"
		<< incumbent_ << "\t" << optimum_ << "\t"
		<< timeParse_ << "\t" << timePreproc_ << "\t" << timeIncumbent_ << "\t" << timeSearch_ << "\t" << isTimeOutReached_ << "\t"
		<< nSteps_ << "\t"
		<< nLastIsetCalls_ << "\t" << nsLastIsetCalls_ << "\t" << nLastIsetPreFilterCalls_ << "\t" << nsLastIsetPreFilterCalls_ << "\t" 
		<< nCurrIsetCalls_ << "\t" << nsCurrIsetCalls_ << "\t"
		<< nVertexCalls_ << "\t" << nsVertexCalls_ << "\t" << nUBpartCalls_ << "\t" << nsUBpartCalls_ << "\t" << K_
		<< std::endl;

	return o;
}

void infoCLQ::resetPreprocInfo()
{
	LB_0_no_AMTS_ = 0.0; LB_0_AMTS_ = 0.0;
	branchingRootSize_ = 0;
	idSortReal_ = -1;
	isDegOrd_ = false;
		
	resetTimer(PREPROC);
	sol_.clear();								//candidate solution found during preprocessing
}

void infoCLQ::resetSearchInfo() override
{
	incumbent_ = 0;
	optimum_ = 0;
	nSteps_ = 0;
	timeOutReached_ = false;
	sol_.clear();

	nLastIsetCalls = 0; nUBpartCalls = 0; nCurrIsetCalls = 0; nVertexCalls = 0; nLastIsetPreFilterCalls = 0;
	nsLastIsetCalls = 0;  nsUBpartCalls = 0; nsCurrIsetCalls = 0; nsVertexCalls = 0; nsLastIsetPreFilterCalls = 0;
	resetTimer(SEARCH); resetTimer(LAST_INCUMBENT);
}












