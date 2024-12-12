/*
 *info_base.cpp:		implementation of infoCLQ_Base class for result logs of clique and clique-based algorithms
 *@date 12/12/2012
 *@dev pss
 */

#include "info_base.h"
#include "utils/logger.h"

#include <iostream>
using namespace std;

//ostream& operator<<(ostream& o, const infoCLQ_Base& k) {
//	o << k.name.c_str()<<"\t" <<k.N <<"\t"<<k.M<< "\t"<< k.TIME_LIMIT<< "\t" << k.TIME_LIMIT_HEUR << "\t" << k.id_alg << "\t"
//	  << k.id_sorting_alg_called <<"\t" << k.id_sorting_alg_real << "\t" 
//	  << k.is_degree_ordering << "\t" << k.id_AMTS << "\t"
//	  << k.LB_0_no_AMTS<<"\t" <<k.LB_0_AMTS<<"\t"  
////	  << k.UB_0_LupMCKP << "\t" << k.UB_0_dantzig_MT << "\t" << k.UB_0_alfa << "\t" << k.UB_0_cplex_lp << "\t"
// 	  << k.incumbent << "\t" << k.optimum << "\t" 
//	  << k.time_parse<<"\t"<<k.time_preproc << "\t" << k.time_incumbent << "\t" << k.time_search << "\t" << k.time_limit_reached << "\t"
//	  << k.nSteps << "\t"
//	  << k.nLastIsetCalls << "\t" << k.nsLastIsetCalls << "\t" << k.nLastIsetPreFilterCalls << "\t" << k.nsLastIsetPreFilterCalls << "\t"<< k.nCurrIsetCalls << "\t" << k.nsCurrIsetCalls << "\t"
//	  << k.nVertexCalls << "\t" << k.nsVertexCalls << "\t" << k.nUBpartCalls << "\t" << k.nsUBpartCalls << "\t" << k.K
//	  << std::endl;	
//	return o;
//}

//std::ostream& infoCLQ_Base::print_params(std::ostream& o) {
//	LOG_INFO("");
//	LOG_INFO("*****************************");
//	LOG_INFO("DATA:" << name.c_str() << "\t N:" << N << "\t M:" << M << "\t D:" << 2 * M / (float)((N - 1) * N));
//	LOG_INFO("TIME_LIMIT:" << TIME_LIMIT);
//	LOG_INFO("TIME_LIMIT_HEUR:" << TIME_LIMIT_HEUR);
//	LOG_INFO("ALG:" << id_alg);
//	LOG_INFO("SORTING:" << id_sorting_alg_called);
//	LOG_INFO("AMTS:" << id_AMTS);
//	if (this->K != -1) {
//		LOG_INFO("MAX UB:" << K);
//	}
//	LOG_INFO("*****************************");
//	return o;
//}
//
//std::ostream& infoCLQ_Base::print_results(std::ostream& o) {
//	LOG_INFO("*****************************");
//	LOG_INFO("w:" << optimum << "\tt_par: " << time_parse << "\tt_pp: " << time_preproc << "\tt_search: " << time_search << "\t#steps: " << nSteps);
//	LOG_INFO("*****************************");
//	return o;
//}
//
//std::ostream& infoCLQ_Base::print_timers(std::ostream& o) {
//	LOG_INFO("");
//	LOG_INFO("*****************************");
//	LOG_INFO("start_time_parse:" << start_time_parse / (float)CLOCKS_PER_SEC);
//	LOG_INFO("start_time_preproc:" << start_time_preproc / (float)CLOCKS_PER_SEC);
//	LOG_INFO("start_time_incumbent:" << start_time_incumbent / (float)CLOCKS_PER_SEC);
//	LOG_INFO("start_time_search:" << start_time_search / (float)CLOCKS_PER_SEC);
//	LOG_INFO("time_parse:" << time_parse);
//	LOG_INFO("time_preproc:" << time_preproc);
//	LOG_INFO("time_incumbent:" << time_incumbent);
//	LOG_INFO("time_search:" << time_search);
//	LOG_INFO("TIME_LIMIT:" << TIME_LIMIT);
//	LOG_INFO("TIME_LIMIT_HEUR:" << TIME_LIMIT_HEUR);
//	LOG_INFO("*****************************");
//	return o;
//}


void infoCLQ_Base::startTimer(phase_t t)
{
	//TODO -here: reset data for search and preprocessing selectively in this function (i.e. reset_preproc_info(); reset_search_info(); reset_bound_info())....

	switch (t) {
	case phase_t::SEARCH:
		start_time_search = clock();
		reset_search_info();					//CHECK()?
		break;
	case phase_t::PREPROC:
		start_time_preproc = clock();
		reset_preproc_info();					//CHECK()?
		break;
	case phase_t::PARSE:
		start_time_parse = clock();
		break;
	case phase_t::LAST_INCUMBENT:
		start_time_incumbent = clock();
		break;

	default:
		LOG_ERROR("bizarre timer type, exiting... - infoCLQ::start_timer");
		cout << "timer type: " << (int)t << endl;
		exit(-1);
	}	
}


void infoCLQ_Base::resetTimer(phase_t t) {
	switch (t) {
	case phase_t::SEARCH:
		start_time_search = 0.0;
		time_search = 0.0;
		break;
	case phase_t::PARSE:
		start_time_parse = 0.0;
		time_parse = 0.0;
		break;
	case phase_t::PREPROC:
		start_time_preproc = 0.0;
		time_preproc = 0.0;
		break;
	case phase_t::LAST_INCUMBENT:
		start_time_incumbent = 0.0;
		time_incumbent = 0.0;
		break;	
	default:
		LOG_ERROR("bizarre timer type, exiting...-infoCSP::clear_time");
		cout << "timer type: " << (int)t << endl;
		exit(-1);
	}
}

void infoCLQ_Base::resetTimers() {
	resetTimer(SEARCH);
	resetTimer(PARSE);
	resetTimer(PREPROC);
	resetTimer(LAST_INCUMBENT);
}


double infoCLQ_Base::readTimer(phase_t t)
{
	double elapsedTime;
	clock_t endTime = clock();

	switch (t) {
	case phase_t::SEARCH:
		time_search = (double)(endTime - startTimeSearch_) / (double)CLOCKS_PER_SEC;
		elapsed_time = time_search;
		break;
	case phase_t::PREPROC:
		time_preproc = (double)(endTime - startTimePreproc_) / (double)CLOCKS_PER_SEC;
		elapsed_time = time_preproc;
		break;
	case phase_t::PARSE:
		time_parse = (double)(endTime - startTimeParse_) / (double)CLOCKS_PER_SEC;
		elapsed_time = time_parse;	
		break;
	case phase_t::LAST_INCUMBENT:
		time_incumbent = (double)(endTime - startTimeIncumbent_) / (double)CLOCKS_PER_SEC;
		elapsed_time = timeIncumbent_;
		break;

	default:
		LOG_ERROR("bizarre timer type, exiting...-infoCLQ::read_time");
		cout << "timer type: " << (int)t << endl;
		exit(-1);
	}

	return elapsed_time;
}

double infoCLQ_Base::elapsedTime(clock_t start_time)
{
	clock_t end_time = clock();	
	return  (double)((end_time - start_time) / (double)CLOCKS_PER_SEC);
}


void infoCLQ_Base::resetGeneralInfo() {
	TIME_LIMIT = DBL_MAX;
	name.clear();
	name_log.clear();
	N = 0;
	M = 0;
	K = 0;
	id_alg = -1;
	id_AMTS = -1;
	id_sorting_alg_called = -1;
	time_parse = 0.0;

	//....
}


///////////////////////////
//
// DERIVED CLASS

void infoCLQ::resetSearchInfo() {
	optimum = 0;  incumbent = 0; nSteps = 0;
	nLastIsetCalls = 0; nUBpartCalls = 0; nCurrIsetCalls = 0; nVertexCalls = 0; nLastIsetPreFilterCalls = 0;
	nsLastIsetCalls = 0;  nsUBpartCalls = 0; nsCurrIsetCalls = 0; nsVertexCalls = 0; nsLastIsetPreFilterCalls = 0;
	time_incumbent = 0.0; time_search = 0.0; time_limit_reached = false;
}

void infoCLQ::resetPreprocInfo() {
	id_sorting_alg_real = -1;
	is_degree_ordering = false; branching_root_size = 0;
	sol.clear();
	LB_0_no_AMTS = 0.0; LB_0_AMTS = 0.0;
	time_preproc = 0.0;
}










