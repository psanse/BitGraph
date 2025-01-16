/*
 *info_clq.cpp: implementation of result logs of clique and clique-based algorithms
 *@date 12/12/2012
 *@last_update 13/23/2024
 *@dev pss
 */

#include "info_clq.h"
#include "utils/logger.h"
#include <iostream>

using namespace std;
using namespace com;

///////////////////////////
//
// DERIVED CLASS

template<class W_t>
std::ostream& infoCLQ<W_t>::printSummary(std::ostream& o) const
{
	o << "*****************************" << endl;
	o << "w:" << optimum_ << "\tt_par: " << timeParse_ << "\tt_pp: " << timePreproc_ << "\tt_search: " << timeSearch_ << "\t#steps: " << nSteps_ << endl;
	o << "*****************************" << endl;
	return o;
}

template<class W_t>
std::ostream& infoCLQ<W_t>::printReport(std::ostream& o, bool is_endl) const
{
	o << nameInstance_.c_str() << "\t" << N_ << "\t" << M_ << "\t" << TIME_OUT_ << "\t" << TIME_OUT_HEUR_ << "\t" << idAlg_ << "\t"
		<< idSort_ << "\t" << idSortReal_ << "\t"
		<< isDegOrd_ << "\t" << idHeur_ << "\t"
		<< lbRootNoHeur << "\t" << lbRootHeur << "\t"
		<< incumbent_ << "\t" << optimum_ << "\t"
		<< timeParse_ << "\t" << timePreproc_ << "\t" << timeIncumbent_ << "\t" << timeSearch_ << "\t" << isTimeOutReached_ << "\t"
		<< nSteps_ << "\t";
		/*<< nLastIsetCalls_ << "\t" << nsLastIsetCalls_ << "\t" << nLastIsetPreFilterCalls_ << "\t" << nsLastIsetPreFilterCalls_ << "\t"
		<< nCurrIsetCalls_ << "\t" << nsCurrIsetCalls_ << "\t"
		<< nVertexCalls_ << "\t" << nsVertexCalls_ << "\t" << nUBpartCalls_ << "\t" << nsUBpartCalls_ << "\t" << K_
		<< std::endl;*/

		  if (is_endl) {
			  o << std::endl;
		  }

	return o;
}

template<class W_t>
void infoCLQ<W_t>::clearPreprocInfo()
{	
	lbRootNoHeur = 0.0; lbRootHeur = 0.0;
	branchingRootSize_ = 0;
	idSortReal_ = -1;
	isDegOrd_ = false;
		
	clearTimer(com::infoBase::phase_t::PREPROC);
	sol_.clear();									//candidate solution found during preprocessing
}

template<class W_t>
void infoCLQ<W_t>::clearSearchInfo()
{
	incumbent_ = 0;
	optimum_ = 0;
	nSteps_ = 0;
	isTimeOutReached_ = false;
	sol_.clear();

	clearTimer(com::infoBase::phase_t::SEARCH); 
	clearTimer(com::infoBase::phase_t::LAST_INCUMBENT);
}

template<class W_t>
void infoCLQ<W_t>::clear(bool lazy)
{
	clearPreprocInfo();
	clearSearchInfo();

	::com::infoBase::clear(lazy);
}

////////////////////////////////////////////
//list of valid types for infoCLQ to allow generic code in *.cpp files 

template struct  infoCLQ<int>;
template struct  infoCLQ<double>;


/////////////////
// infoCliSAT


void infoCliSAT::clearSearchInfo()
{
	infoCLQ<int>::clearSearchInfo();

	nLastIsetCalls_ = 0;
	nUBpartCalls_ = 0;
	nCurrIsetCalls_ = 0;
	nVertexCalls_ = 0;
	nLastIsetPreFilterCalls_ = 0;
	nsLastIsetCalls_ = 0;
	nsUBpartCalls_ = 0;
	nsCurrIsetCalls_ = 0;
	nsVertexCalls_ = 0;
	nsLastIsetPreFilterCalls_ = 0;
}


std::ostream& infoCliSAT::printReport(std::ostream& o, bool is_endl) const
{
	infoCLQ<int>::printReport(o, false);

	o << nLastIsetCalls_ << "\t" << nsLastIsetCalls_ << "\t" << nLastIsetPreFilterCalls_ << "\t" << nsLastIsetPreFilterCalls_ << "\t"
		<< nCurrIsetCalls_ << "\t" << nsCurrIsetCalls_ << "\t"
		<< nVertexCalls_ << "\t" << nsVertexCalls_ << "\t" << nUBpartCalls_ << "\t" << nsUBpartCalls_ << "\t" << K_;

	if (is_endl) {
		o << std::endl;
	}


	return o;
}


















