/*
 * @file tests_analyser.cpp
 * @brief implementation of the TestAnalyser class (tests_analyser.h) to manage benchmarking of graph algorithms
 * @date ?
 * @last_update 20/01/2025
 * @author pss
 */

#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "test_analyser.h"
#include "prec_timer.h"
#include "logger.h"
#include "common.h"

using namespace std;


 ostream& operator << (ostream& o,const TestAnalyser& t){
	try{
		//general information
		if(t.print_mode_ & TestAnalyser::NAME){					//assumes the same instance for all tests
			//o<<left<<setw(30)<<t.arrayOfTests_[0][0].get_name()<<" ";
			o<<left<<setw(30)<<t.arrayOfTests_[0][0].get_name();
		}

		if(t.print_mode_ & TestAnalyser::SIZE){
			//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfTests_[0][0].get_d1()<<" ";  
			o<<right<<"\t"<<t.arrayOfTests_[0][0].get_d1();
		}

		if(t.print_mode_ & TestAnalyser::EDGES){
			//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfTests_[0][0].get_d2()<<" ";  
			o<< right<<"\t"<<t.arrayOfTests_[0][0].get_d2();
		}

		if (t.print_mode_ & TestAnalyser::TIMEOUT) {
			o << right<< setw(10)<< "\t" <<(int)t.arrayOfTests_[0][0].get_tout();
		}

		if (t.print_mode_ & TestAnalyser::ALG) {
			o << right<< "\t" << t.arrayOfTests_[0][0].get_alg();
		}
		
		if (t.print_mode_ & TestAnalyser::SORT) {
			//TODO@decode SORT_TYPE!
			//o << setw(5) << right << setprecision(0) << t.arrayOfTests_[0][0].get_d3() << " ";
			o << right<< "\t" << t.arrayOfTests_[0][0].get_d3();
		}

		//TODO - ADD TIMEOUT (check this comment 20/01/2025)
				
		//information common to all tests
		o.setf(ios::fixed);										
		for(int i=0; i<t.nAlg_; i++){

			if (t.print_mode_ & TestAnalyser::LOWER_BOUND) {
				//o << setw(4) << right << setprecision(2) << t.arrayOfAvLB[i] << " ";
				o << right<< setw(7)<<setprecision(2)<<"\t" << t.arrayOfAvLB[i];

			}

			if (t.print_mode_ & TestAnalyser::SOL) {
				//o << setw(5) << right << setprecision(2) << t.arrayOfAvSol[i] << " ";
				o << right<< setw(7)<<setprecision(2) << "\t" << t.arrayOfAvSol[i];
			}
									
			//if(t.print_mode_& TestAnalyser::STDDEV_SOL){
			//	o<<setw(5)<<right<<setprecision(2)<<t.arrayOfSdSol[i]<<" ";  
			//}

			//if(t.print_mode_ & TestAnalyser::MAX_SOL){
			//	//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfMaxSol[i]<<" ";  
			//	o << setprecision(0) << "\t" << t.arrayOfMaxSol[i];				
			//}
									
			
			if(t.print_mode_ & TestAnalyser::STEPS){
				//o<<setw(15)<<right<<setprecision(0)<<t.arrayOfAvSteps[i]<<" ";  
				o << right<<setw(10)<<setprecision(0) << "\t" << t.arrayOfAvSteps[i];
				
			}
																			
			if(t.print_mode_ & TestAnalyser::TIME){
				//o<<setw(12)<<right<<setprecision(3)<<t.arrayOfAvTimes[i]<<" "; 
				o << right<< setw(7)<<setprecision(3) << "\t" << t.arrayOfAvTimes[i];
				
			}

			if (t.print_mode_ & TestAnalyser::TIMEPRE) {
				//o<<setw(12)<<right<<setprecision(3)<<t.arrayOfAvTimes[i]<<" "; 
				o << right << setw(7) << setprecision(3) << "\t" << t.arrayOfAvPreProcTimes[i];

			}
				
			if (t.print_mode_ & TestAnalyser::NFAIL) {
				//o << setw(5) << t.arrayOfFails[i] << " ";
				o << right<< "\t" << t.arrayOfFails[i];
			}

			if(t.print_mode_ & TestAnalyser::NCONT){
				for(int j=0; j<t.arrayOfCounters[i].size(); j++){
						o<<right<<setw(10)<<setprecision(4)<<t.arrayOfCounters[i][j]<<" ";  
				}
			}		

			
			//separator for diffent algs of same instance
			if(i < (t.nAlg_-1)){				
				o<<"| ";
			}
		}
		o<<endl;
	
	}catch(exception e){
		LOG_ERROR("Test_Analyser::Error when printing data");
		o<<"Test_Analyser::Error when printing data"<<endl;
	}

	return o;
}

void TestAnalyser::clear(){
	arrayOfTests_.clear();				//[nRep][nAlg]
	arrayOfAvTimes.clear();
	arrayOfAvPreProcTimes.clear();
	arrayOfAvSol.clear();				//[nAlg]	
	arrayOfFails.clear();
	arrayOfAvLB.clear();
	arrayOfAvSteps.clear();
	arrayOfCounters.clear();
	arrayOfMaxSol.clear();
	nAlg_ = 0;
	nRep_ = 0;
	print_mode_ = DEFAULT_PRINT_MODE;

	//arrayOfSdTime.clear();			//std dev reports - currently not implemented		
	//arrayOfSdSol.clear();
}

void TestAnalyser::add_test(bool isNewRep, Result res){
			
	if(isNewRep || arrayOfTests_.empty() ){					//new repetition/test

		vres_t v;
		v.push_back(std::move(res));
		arrayOfTests_.push_back(std::move(v));		

	}else{

		arrayOfTests_.back().push_back(std::move(res));		//new result in a current repetition/test

	}
}

int TestAnalyser::analyser(info_t* info){

	//updates nRep_, nAlg_ values / checks consistency
	if(make_consistent() == ERR){
		if (nRep_ <= 0) {
			LOGG_ERROR("Error in number of repetitions: ", nRep_, "TestAnalyser::analyser");
		}
		if(nAlg_ <= 0){
			LOGG_ERROR("Error in number of algorithms: ", nAlg_, "TestAnalyser::analyser");
		}
		return ERR;
	}

	///////////////////////////
	//analysis of the results
	double avSol, avTimes, avPreProcTimes, avSteps, avLB;	
	double maxSol = 0;
	int nFails = 0; 
	
	//main outer loop over algorithms
	vector<double> avnCounters;			
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
		usint nMaxCounters = 0;						
		for(int rep = 0; rep < nRep_; ++rep)
		{
			if (nMaxCounters < arrayOfTests_[rep].at(j).number_of_counters()) {
				nMaxCounters = arrayOfTests_[rep].at(j).number_of_counters();
			}
		}

		//allocates nMaxCounters with initial value 0.0
		avnCounters.assign(nMaxCounters,0.0);	

		////////////////////////////////
		//inner loop repetitions for algorithm j

		for(auto rep = 0; rep < nRep_; ++rep){
			Result res = arrayOfTests_[rep].at(j);				 

			//extracts counter info - always reported independent of TIMEOUT
			for (auto i = 0; i < res.number_of_counters(); i++) {
				avnCounters[i] += res.get_counters()[i];
			}
						
			if(!res.is_time_out())
			{	
				//no time_out
				double sol = res.get_upper_bound();
				avSol += sol;
				if (sol > maxSol) {
					maxSol = sol;
				}
				avTimes += res.get_user_time();
				avPreProcTimes += res.get_pre_time();
				avSteps += res.number_of_steps();
				avLB += res.get_lower_bound();

			}
			else {			
				//time_out
				if(nRep_== 1){									//if only one test, time out results are reported anyway
				
					avSol += res.get_upper_bound();
					maxSol = avSol;			
					avSteps += res.number_of_steps();
					avLB += res.get_lower_bound();
				}

				nFails++;										//if more than one test, time out results are not reported
			}
		}//endFor repetitions for algorithm j

		///////////////////////////
		// Report results for algorithm j
		 
		//counter info (indep. of FAILS)
		for(auto i = 0; i < avnCounters.size(); ++i){
			avnCounters[i] /= nRep_;
		}
		arrayOfCounters.push_back(avnCounters);

	
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
	if(info != nullptr){

		if (com::stl::all_equal(arrayOfAvSol))		{ info -> same_sol = true; }
		if (com::stl::all_equal(arrayOfAvSteps))	{ info -> same_steps = true; }
		if (com::stl::all_equal(arrayOfAvLB))		{ info -> same_lb = true; }
		if (arrayOfAvSteps[0] > arrayOfAvSteps[1])	{ info -> steps_first_greater = true; }
		
		info -> steps_lhs = arrayOfAvSteps[0]; 
		info -> steps_rhs = arrayOfAvSteps[1]; 
	}

	//TODO - STANDARD DEVIATION ANALYSIS
	
return OK;
}

bool TestAnalyser::is_consistent_sol(int& num_error){
/////////////////
// Returns TRUE if at least one solution (or just one solution) exists and it is consistent
// Returns num_error: -1 ok, 0 nothing to compare with, 1-N the first algorithm different from the one run first

	num_error=-1;
	bool consistent_sol=true;

	//test emptyness
	if (arrayOfAvSol.empty()){
		LOG_ERROR("no solutions available");
		num_error=0;
		return false;
	}

	consistent_sol = true;
	double firstItem = arrayOfAvSol.front();
	int count=0;
	for (vector<double>::const_iterator it = arrayOfAvSol.begin()+1; it != arrayOfAvSol.end() ; it++) {
		count++;
		if(*it != firstItem) {
			consistent_sol = false;
			num_error=count;
			break;
		}
	}

return consistent_sol;
}

bool TestAnalyser::is_consistent_array_of_tests()
{
	return (	(nRep_ == arrayOfTests_.size())		&& 
				(nAlg_ == arrayOfTests_[0].size())		);

}

////////////////////////
// E/S

void TestAnalyser::print_current_time	(ostream& o){
	o<<PrecisionTimer::local_timestamp();
}

void TestAnalyser::print_line	(const string str, ostream& o){
	o<<str;
}

void TestAnalyser::print_single(ostream & o, int idAlg){
///////////////////////////
// date: 2/7/13
// prints individual results of algorithms 
// nAlg==-1 all algs (DEFAULT)

	//fills nRep_ and nAlg_ appropiately
	make_consistent();
	if(nRep_==0 || nAlg_==0 ){
		LOG_ERROR("empty tests");
		return;
	}

	//bounds parameters
	if(idAlg==-1) idAlg=nAlg_;
	else (idAlg>nAlg_)? idAlg=nAlg_: 1;
	
	o<<"------------------------------------------"<<endl;
	
	for(int r=0; r<nRep_;r++){
		for(int a=0; a<idAlg; a++){
			o<<arrayOfTests_[r][a]<<" ";
		}
		o<<endl;
	}

	o<<"-------------------------------------------------"<<endl;
}

void TestAnalyser::print_single_rep	(ostream & o, int nRep, int idAlg){
///////////////////////////
// date: 2/7/13
// prints individual results results of algorithms up to nAlg in a single repetition
// DEFAULTS: nRep==0 all repetitions (default parameter) idAlg==-1 all algs (DEFAULT)
// 

	//control
	if (nRep < 0) {
		LOG_ERROR("number of repetitions cannot be negative");
		return;
	}
	
	//fills nRep_ and nAlg_ appropiately
	make_consistent();
	if(nRep_==0 || nAlg_==0 ){
		LOG_ERROR("empty tests");
		return;
	}
	

	//bounds parameters
	if(idAlg==-1) idAlg=nAlg_;
	else if(idAlg>nAlg_) idAlg=nAlg_;
	if(nRep > nRep_) nRep = nRep_;
	
	
	//E/S: Exception possible because nRep is not Synchro
	o<<endl;
	
	for(int a=0; a<idAlg; a++){
		try{
			o<<arrayOfTests_[nRep - 1][a]<<" ";
		}catch(exception e){
			/*stringstream sstr("");
			sstr<<"Bad output"<<"Test:"<<a<<" Rep:"<<nRep<<endl;
			LOG_INFO(sstr.str().c_str()<<endl);*/
			LOG_ERROR("Bad output", " Test:", a, " Rep:", nRep);
			break;
		}
	}
}

int TestAnalyser::make_consistent(){
		
	int retVal = OK;

	///////////////////////////////////////////////////////////////
	nRep_ = arrayOfTests_.size();
	(nRep_ > 0)?  nAlg_ = arrayOfTests_[0].size() :  nAlg_=0;
	///////////////////////////////////////////////////////////////	
	
	(nRep_ > 0 && nAlg_ > 0)? retVal = OK : retVal = ERR;
	return retVal;
}
