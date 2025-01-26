/*
 * @file info_analyser.cpp
 * @brief implementation of the InfoAnalyser class (tests_analyser.h) to manage benchmarking of graph algorithms
 * @date 2013
 * @last_update 20/01/2025
 * @author pss
 */

#include "common.h"
#include "logger.h"
#include "info_analyser.h"
#include "utils/info/info_clq.h"
#include <iomanip>
#include <math.h>
#include <string>


using namespace std;

template<class AlgInfo_t>
std::ostream& operator << (std::ostream& o, const InfoAnalyser<AlgInfo_t>& t) { t.print_analyser_summary(o); return o; }

template<class AlgInfo_t>
void InfoAnalyser< AlgInfo_t>::clear()
{
	arrayOfTests_.clear();				//[nRep][nAlg]
	arrayOfAvTimes.clear();
	arrayOfAvPreProcTimes.clear();
	arrayOfAvSol.clear();				//[nAlg]	
	arrayOfFails.clear();
	arrayOfAvLB.clear();
	arrayOfAvSteps.clear();
	arrayOfCounters.clear();			//[nAlg][nCounters] - currently not used
	arrayOfMaxSol.clear();
	
	print_mode_ = DEFAULT_PRINT_MODE;

	//arrayOfSdTime.clear();			//std dev reports - currently not implemented		
	//arrayOfSdSol.clear();
}

template<class AlgInfo_t>
void InfoAnalyser<AlgInfo_t>::add_test(bool isNewRep, AlgInfo_t res){
			
	if(isNewRep || arrayOfTests_.empty() ){					//new repetition/test

		vInfo_t v;
		v.push_back(std::move(res));
		arrayOfTests_.push_back(std::move(v));	

	}
	else {

		arrayOfTests_.back().push_back(std::move(res));		//new result in a current repetition/test
		
	}
}

template<class AlgInfo_t>
inline
int InfoAnalyser<AlgInfo_t>::number_of_algorithms() const
{ 
	if (number_of_repetitions() > 0) {
		return arrayOfTests_[0].size();
	}
	else {
		return 0;
	}
}

template<class AlgInfo_t>
int InfoAnalyser<AlgInfo_t>::analyser (comp_t* pComp){
	
	auto nRep_ = number_of_repetitions();
	auto nAlg_ = number_of_algorithms();
	if(!check_test_consistency()){ 
		LOGG_ERROR("Error in tests: - InfoAnalyser<AlgInfo_t>::analyser");
		return -1;
	}
		
	///////////////////////////
	//analysis of the results
	double avSol, avTimes, avPreProcTimes, avSteps, avLB;	
	double maxSol = 0;
	int nFails = 0; 
	
	//main outer loop over algorithms
	//vector<double> avnCounters;			
	for(auto j = 0; j < nAlg_; ++j){

		////////////
		//initializes context for current algorithm j
		avSol = 0.0;
		avTimes = 0.0; 
		avPreProcTimes = 0.0;
		avSteps = 0.0;
		avLB = 0.0;
		maxSol = 0.0;
		nFails = 0;
			
		//determines the maximum number of counters of any algorithm and repetition
		//usint nMaxCounters = 0;						
		//for(int rep = 0; rep < nRep_; ++rep)
		//{
		//	if (nMaxCounters < arrayOfTests_[rep].at(j).number_of_counters()) {
		//		nMaxCounters = arrayOfTests_[rep].at(j).number_of_counters();
		//	}
		//}

		////allocates nMaxCounters with initial value 0.0
		//avnCounters.assign(nMaxCounters,0.0);	

		////////////////////////////////
		//inner loop repetitions for algorithm j

		for(auto rep = 0; rep < nRep_; ++rep){
			auto res = arrayOfTests_[rep].at(j);				 

			//extracts counter info - always reported independent of TIMEOUT
			/*for (auto i = 0; i < res.number_of_counters(); i++) {
				avnCounters[i] += res.get_counters()[i];
			}*/
						
			if(!res.is_time_out())
			{	
				//no time_out
				double sol = res.ub();
				avSol += sol;
				if (sol > maxSol) {
					maxSol = sol;
				}
				avTimes += res.search_time();
				avPreProcTimes += res.preprocessing_time();
				avSteps += res.number_of_steps();
				avLB += res.lb();

			}
			else {			
				//time_out
				if(nRep_== 1){									//if only one test, time out results are reported anyway
				
					avSol += res.ub();
					maxSol = avSol;			
					avSteps += res.number_of_steps();
					avLB += res.lb();
				}

				nFails++;										//if more than one test, time out results are not reported
			}
		}//endFor repetitions for algorithm j

		///////////////////////////
		// Report results for algorithm j
		 
		//counter info (indep. of FAILS)
	/*	for(auto i = 0; i < avnCounters.size(); ++i){
			avnCounters[i] /= nRep_;
		}
		arrayOfCounters.push_back(avnCounters);*/

	
		//average info - taking into account FAILS
		arrayOfFails.push_back(nFails);

		if(nFails != nRep_){
			
			auto nonFailedReps = nRep_ - nFails;

			//At least one case in which algorithm j did not time out exists
			arrayOfAvSol.push_back			(avSol   / nonFailedReps);
			arrayOfAvTimes.push_back		(avTimes / nonFailedReps);
			
			arrayOfAvPreProcTimes.push_back	(avPreProcTimes / nonFailedReps);

			arrayOfAvSteps.push_back		(avSteps / nonFailedReps);
			arrayOfAvLB.push_back			(avLB / nonFailedReps);
			arrayOfMaxSol.push_back			(maxSol);
		}
		else
		{ 
			//Algorithm j timed-out in all cases
			arrayOfAvTimes.push_back		(-1.0);	
			arrayOfAvPreProcTimes.push_back	(-1.0);

			if(nRep_ == 1)
			{ 
				//single execution: report values anyway
				arrayOfAvSol.push_back		(avSol);
				arrayOfAvSteps.push_back	(avSteps);
				arrayOfAvLB.push_back		(avLB);
				arrayOfMaxSol.push_back		(maxSol);
			}
			else
			{
				//multiple repetitions: do not show values
				arrayOfAvSol.push_back		(0.0);
				arrayOfAvSteps.push_back	(0.0);
				arrayOfAvLB.push_back		(0.0);
				arrayOfMaxSol.push_back		(0);
			}
		}

	}//endFor algorithms

	//compares algorithms (use for comparison of two algorithms)
	if(pComp != nullptr){

		if (com::stl::all_equal(arrayOfAvSol))		{ pComp-> same_sol = true; }
		if (com::stl::all_equal(arrayOfAvSteps))	{ pComp-> same_steps = true; }
		if (com::stl::all_equal(arrayOfAvLB))		{ pComp-> same_lb = true; }
		if (arrayOfAvSteps[0] > arrayOfAvSteps[1])	{ pComp-> steps_first_greater = true; }
		
		pComp-> steps_lhs = arrayOfAvSteps[0];
		pComp-> steps_rhs = arrayOfAvSteps[1];
	}

	//TODO - STANDARD DEVIATION ANALYSIS
	
return 0;
}

template<class AlgInfo_t>
bool InfoAnalyser<AlgInfo_t>::check_solution_values(int& num_error){

	num_error = -1;
	bool same_sol = true;

	//check no reported solutions
	if (arrayOfAvSol.empty()){
		LOG_ERROR("No reported solutions, possibly all timed -  InfoAnalyser<AlgInfo_t>::is_consistent_sol");
		num_error = 0;	
		return false;
	}

	//iterates over the array of avergaed solutions
	same_sol = true;
	double firstItem = arrayOfAvSol.front();
	for (auto it = arrayOfAvSol.begin() + 1; it != arrayOfAvSol.end() ; it++) {
		
		//finds a different solution
		if(*it != firstItem) {
			same_sol = false;
			num_error = std::distance (arrayOfAvSol.begin(), it);		//CHECK (20/01/2015)
			break;
		}

	}

	return same_sol;
}

template<class AlgInfo_t>
bool InfoAnalyser<AlgInfo_t>::check_test_consistency()
{
	auto nRep_ = number_of_repetitions();
	auto nAlg_ = number_of_algorithms();
	if (nRep_ <= 0 || nAlg_ <= 0) {
		LOGG_ERROR("Error in tests: - InfoAnalyser<AlgInfo_t>::check_consistency_of_tests");
		return false;
	}

	return true;
}

template<class AlgInfo_t>
ostream& InfoAnalyser<AlgInfo_t>::print_alg (ostream & o, int algID) const{
		
	auto nRep_ = number_of_repetitions();
	auto nAlg_ = number_of_algorithms();
	
	///////////////
	//streams appropiate data
	if  ( (algID == -1) || (algID > nAlg_) ) 	{

		//all data

		algID = nAlg_;
		for (auto a = 0; a < nAlg_; ++a) {
			for (auto r = 0; r < nRep_; ++r) {

				try {
					/////////////////////////////////
					o << (a + 1) << '\t' << r << '\t';
					o << arrayOfTests_[r][a];					//streams full data for each result, adds endl
					/////////////////////////////////
				}
				catch (exception e) {
					LOGG_ERROR("Bad output", " Test:", algID, " Rep:", r, "-InfoAnalyser<AlgInfo_t>::print_alg");
					break;
				}

			}			
		}
		
	}
	else {

		//single algorithm

		for (auto r = 0; r < nRep_; ++r) {

			try {

				/////////////////////////////////
				o << algID << '\t' << r << '\t';
				o << arrayOfTests_[r][algID - 1];						//streams full data for each result, adds endl
				/////////////////////////////////
				
			}
			catch (exception e) {
				LOGG_ERROR("Bad output", " Test:", algID, " Rep:", r, "-InfoAnalyser<AlgInfo_t>::print_single");
				break;
			}

		}
	}
	
	return o;
}

template<class AlgInfo_t>
std::ostream& InfoAnalyser<AlgInfo_t>::print_rep (ostream& o, int nRep, int algID) const {

	auto nRep_ = number_of_repetitions();
	auto nAlg_ = number_of_algorithms();

	//assert / check nRep value
	if (nRep <= 0) { 
		LOG_WARNING("no repetitions available - InfoAnalyser<AlgInfo_t>::print_rep");
		return o; 
	}

	//upper bound nRep_
	if (nRep > nRep_) { nRep = nRep_; }
	

	//streams appropiate data
	if ((algID == -1) || (algID > nAlg_)) {

		//all algorithms - single repetition (index 1-based) 

		for (auto a = 0; a < nAlg_; ++a) {

			try {
				/////////////////////////////////
				o << a << '\t' << nRep << '\t';
				o << arrayOfTests_[nRep - 1][a];					//streams full data for each result, adds endl
				/////////////////////////////////
			}
			catch (exception e) {
				LOGG_ERROR("Bad output", " Test:", algID, " Rep:", nRep, "-InfoAnalyser<AlgInfo_t>::print_rep");
				break;
			}
		}

	}
	else {

		//single algorithm, single repetition (jndex 1-based)

		try {
			/////////////////////////////////
			o << algID << '\t' << nRep << '\t';
			o << arrayOfTests_[nRep - 1][algID];							//streams full data for each result, adds endl
			/////////////////////////////////
		}
		catch (exception e) {
			LOGG_ERROR("Bad output", " Test:", algID, " Rep:", nRep, "-InfoAnalyser<AlgInfo_t>::print_single");
		}

	}

	return o;
}

template<class AlgInfo_t>
std::ostream& InfoAnalyser<AlgInfo_t>::print_analyser_summary(std::ostream& o) const
{
	auto nRep_ = number_of_repetitions();
	auto nAlg_ = number_of_algorithms();

	try {
		

		//information common to all tests
		o.setf(ios::fixed);
		for (auto a = 0; a < nAlg_; ++a) {

			//algorithm ID
			o << a << '\t';

			////////////////////////
			//general information
			if (print_mode_ & InfoAnalyser<AlgInfo_t>::NAME) {					//assumes the same instance for all tests
				//o<<left<<setw(30)<<t.arrayOfTests_[0][0].get_name()<<" ";
				o << left << setw(30) << arrayOfTests_[0][0].name();
			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::SIZE) {
				//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfTests_[0][0].get_d1()<<" ";  
				o << right << "\t" << arrayOfTests_[0][0].number_of_vertices();
			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::EDGES) {
				//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfTests_[0][0].get_d2()<<" ";  
				o << right << "\t" << arrayOfTests_[0][0].number_of_edges();
			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::TIMEOUT) {
				o << right << setw(10) << "\t" << (int)arrayOfTests_[0][0].time_out();
			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::ALG) {
				o << right << "\t" << arrayOfTests_[0][0].search_algorithm();
			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::SORT) {
				//TODO@decode SORT_TYPE!
				//o << setw(5) << right << setprecision(0) << t.arrayOfTests_[0][0].get_d3() << " ";
				o << right << "\t" << arrayOfTests_[0][0].sorting_algorithm();
			}
			
			////////////////////////////
			//specific information
			if (print_mode_ & InfoAnalyser<AlgInfo_t>::LOWER_BOUND) {
				//o << setw(4) << right << setprecision(2) << t.arrayOfAvLB[i] << " ";									
				o << right << setw(7) << setprecision(2) << "\t" << arrayOfAvLB.at(a);
			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::SOL) {
				//o << setw(5) << right << setprecision(2) << t.arrayOfAvSol[i] << " ";
				o << right << setw(7) << setprecision(2) << "\t" << arrayOfAvSol.at(a);
			}

			//if(print_mode_& InfoAnalyser::STDDEV_SOL){
			//	o<<setw(5)<<right<<setprecision(2)<<t.arrayOfSdSol[i]<<" ";  
			//}

			//if(print_mode_ & InfoAnalyser::MAX_SOL){
			//	//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfMaxSol[i]<<" ";  
			//	o << setprecision(0) << "\t" << t.arrayOfMaxSol[i];				
			//}


			if (print_mode_ & InfoAnalyser<AlgInfo_t>::STEPS) {
				//o<<setw(15)<<right<<setprecision(0)<<t.arrayOfAvSteps[i]<<" ";  
				o << right << setw(10) << setprecision(0) << "\t" << arrayOfAvSteps.at(a);

			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::TIME) {
				//o<<setw(12)<<right<<setprecision(3)<<t.arrayOfAvTimes[i]<<" "; 
				o << right << setw(7) << setprecision(3) << "\t" << arrayOfAvTimes.at(a);

			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::TIMEPRE) {
				//o<<setw(12)<<right<<setprecision(3)<<t.arrayOfAvTimes[i]<<" "; 
				o << right << setw(7) << setprecision(3) << "\t" << arrayOfAvPreProcTimes.at(a);

			}

			if (print_mode_ & InfoAnalyser<AlgInfo_t>::NFAIL) {
				//o << setw(5) << t.arrayOfFails[i] << " ";
				o << right << "\t" << arrayOfFails.at(a);
			}

			/*if(print_mode_ & InfoAnalyser<AlgInfo_t>::NCONT){
				for(auto j = 0; j < t.arrayOfCounters[i].size(); ++j){
					o << right << setw(10) << setprecision(4) << t.arrayOfCounters[i][j]<<" ";
				}
			}	*/

			o << endl;
		}
		o << endl;

	}
	catch (const std::ios::failure& ex) {
		LOG_ERROR("Error when streaming data", ex.what(), "- Test_Analyser::print_analyser_summary ");
		o << "Error when streaming data" << ex.what() << "- Test_Analyser::print_analyser_summary " << endl;
	}
	catch (...) {
		LOG_ERROR("Error when streaming data - check if analyser has been called - Test_Analyser::print_analyser_summary ");
		o << "Error when streaming data, check if analyser has been called - Test_Analyser::print_analyser_summary " << endl;
	}

	return o;

}


/////////////////////////////////
// declaration of valid types 

template class InfoAnalyser< com::infoCLQ<int> >;
template class InfoAnalyser< com::infoCLQ<double> >;

template std::ostream& operator << (std::ostream& o, const InfoAnalyser<com::infoCLQ<int>>& t);
template std::ostream& operator << (std::ostream& o, const InfoAnalyser<com::infoCLQ<double>>& t);

/////////////////////////////////
