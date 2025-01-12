//tests_analyser.cpp: implementation of test_analyser in test_analyser.h


#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "test_analyser.h"
#include "prec_timer.h"
#include "logger.h"
#include "common.h"


 ostream& operator <<(ostream& o,const TestAnalyser& t){
	try{
		//general information
		if(t.m_print_mode & TestAnalyser::NAME){					//assumes the same instance for all tests
			//o<<left<<setw(30)<<t.arrayOfTests[0][0].get_name()<<" ";
			o<<left<<setw(30)<<t.arrayOfTests[0][0].get_name();
		}

		if(t.m_print_mode & TestAnalyser::SIZE){
			//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfTests[0][0].get_d1()<<" ";  
			o<<right<<"\t"<<t.arrayOfTests[0][0].get_d1();
		}

		if(t.m_print_mode & TestAnalyser::EDGES){
			//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfTests[0][0].get_d2()<<" ";  
			o<< right<<"\t"<<t.arrayOfTests[0][0].get_d2();
		}

		if (t.m_print_mode & TestAnalyser::TIMEOUT) {
			o << right<< setw(10)<< "\t" <<(int)t.arrayOfTests[0][0].get_tout();
		}

		if (t.m_print_mode & TestAnalyser::ALG) {
			o << right<< "\t" << t.arrayOfTests[0][0].get_alg();
		}
		
		if (t.m_print_mode & TestAnalyser::SORT) {
			//TODO@decode SORT_TYPE!
			//o << setw(5) << right << setprecision(0) << t.arrayOfTests[0][0].get_d3() << " ";
			o << right<< "\t" << t.arrayOfTests[0][0].get_d3();
		}

		//ADD TIMEOUT
				
		//information common to all tests
		o.setf(ios::fixed);										
		for(int i=0; i<t.m_nAlg; i++){

			if (t.m_print_mode & TestAnalyser::LOWER_BOUND) {
				//o << setw(4) << right << setprecision(2) << t.arrayOfAvLB[i] << " ";
				o << right<< setw(7)<<setprecision(2)<<"\t" << t.arrayOfAvLB[i];

			}

			if (t.m_print_mode & TestAnalyser::SOL) {
				//o << setw(5) << right << setprecision(2) << t.arrayOfAvSol[i] << " ";
				o << right<< setw(7)<<setprecision(2) << "\t" << t.arrayOfAvSol[i];
			}
									
			//if(t.m_print_mode& TestAnalyser::STDDEV_SOL){
			//	o<<setw(5)<<right<<setprecision(2)<<t.arrayOfSdSol[i]<<" ";  
			//}

			//if(t.m_print_mode & TestAnalyser::MAX_SOL){
			//	//o<<setw(5)<<right<<setprecision(0)<<t.arrayOfMaxSol[i]<<" ";  
			//	o << setprecision(0) << "\t" << t.arrayOfMaxSol[i];				
			//}
									
			
			if(t.m_print_mode & TestAnalyser::STEPS){
				//o<<setw(15)<<right<<setprecision(0)<<t.arrayOfAvSteps[i]<<" ";  
				o << right<<setw(10)<<setprecision(0) << "\t" << t.arrayOfAvSteps[i];
				
			}
																			
			if(t.m_print_mode & TestAnalyser::TIME){
				//o<<setw(12)<<right<<setprecision(3)<<t.arrayOfAvTimes[i]<<" "; 
				o << right<< setw(7)<<setprecision(3) << "\t" << t.arrayOfAvTimes[i];
				
			}

			if (t.m_print_mode & TestAnalyser::TIMEPRE) {
				//o<<setw(12)<<right<<setprecision(3)<<t.arrayOfAvTimes[i]<<" "; 
				o << right << setw(7) << setprecision(3) << "\t" << t.arrayOfAvPreProcTimes[i];

			}
				
			if (t.m_print_mode & TestAnalyser::NFAIL) {
				//o << setw(5) << t.arrayOfFails[i] << " ";
				o << right<< "\t" << t.arrayOfFails[i];
			}

			if(t.m_print_mode & TestAnalyser::NCONT){
				for(int j=0; j<t.arrayOfCounters[i].size(); j++){
						o<<right<<setw(10)<<setprecision(4)<<t.arrayOfCounters[i][j]<<" ";  
				}
			}		

			
			//separator for diffent algs of same instance
			if(i < (t.m_nAlg-1)){				
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


TestAnalyser::TestAnalyser(void){
	clear();
	m_print_mode=DEFAULT_PRINT_MODE;
}

void TestAnalyser::clear(){
	arrayOfTests.clear();				//[Nª rep][Algorithm]
	arrayOfAvTimes.clear();
	arrayOfAvPreProcTimes.clear();
	arrayOfAvSol.clear();				//nAlg
//	arrayOfSdTime.clear();		
	arrayOfSdSol.clear();	
	arrayOfFails.clear();
	arrayOfAvLB.clear();
	arrayOfAvSteps.clear();
	arrayOfCounters.clear();
	arrayOfMaxSol.clear();
	m_nAlg=0;
	m_nRep=0;
	m_print_mode=DEFAULT_PRINT_MODE;
}

void TestAnalyser::add_test(bool isNewRep, Result res){
////////////////////
//Appends new repetition/test (isNewRep=TRUE) or inserts value of a new algorithm with current repetition/test (isNewRep=FALSE)
		
	if(isNewRep||arrayOfTests.empty() ){
		vres_t v;
		v.push_back(res);
		arrayOfTests.push_back(v);				//new repetition/test
	}else{
		arrayOfTests.back().push_back(res);		//new alg result in current repetition/test
	}
}

int TestAnalyser::analyser(info_t* info){
////////////////////////////////
// Analyses average results of all algorithms (average values for each repetition etc.)
// RETURN 0 OK, -1 ERROR
//
// TODO: Standard deviation analysis
//
// COMMENTS
// 1-A test is each repetition of an algorithm
// 2-All algorithms are assumed to have the same number of counters for each test
// 3-Averages only consider NOT FAILED tests (if there is only one test results will be shown)
// 4-Obvioulsy if all tests FAIL no results are shown

	//updates m_nRep, m_nAlg
	if(update_sizes()==ERR){
		if(m_nRep<=0)
			LOG_ERROR("TestAnalyser::Error in number of repetitions: ",m_nRep);
		if(m_nAlg<=0){
			LOG_ERROR("TestAnalyser::Error en number of algorithms: ",m_nAlg);
		}
		return ERR;
	}

	double avSol, avTimes, avPreProcTimes, avSteps, avLB;		
	int nFails=0; double maxSol=0;
	
	//main outer loop over algorithms
	vector<double> avnCounters;			
	for(int j=0; j<m_nAlg; j++){
		nFails=0, maxSol=0.0, avSol=0.0, avTimes=0.0, avPreProcTimes=0.0, avSteps=0.0, avLB=0.0;

		/////////////////////////////
		//initiliazes counter results for current algorithm
		avnCounters.clear();
		usint nMaxCounters=0;
		for(int rep=0; rep<m_nRep; rep++){
			if(nMaxCounters<arrayOfTests[rep].at(j).number_of_counters())
				nMaxCounters=arrayOfTests[rep].at(j).number_of_counters();
		}
		//assigns initial value cero to avnCounters
		avnCounters.assign(nMaxCounters,0.0);	

		////////////////////////////////
		//inner loop of repetitions
		for(int rep=0; rep<m_nRep; rep++){
			Result res=arrayOfTests[rep].at(j);

			//counters are always shown independent of FAIL
			for(int i=0; i<res.number_of_counters(); i++)
				avnCounters[i]+=res.get_counters()[i];

			if(!res.is_time_out()){
				double sol=res.get_upper_bound();
				avSol+=sol;
				if(sol>maxSol)
					maxSol=sol;
				avTimes+=res.get_user_time();
				avPreProcTimes += res.get_pre_time();
				avSteps+=res.number_of_steps();
				avLB+=res.get_lower_bound();

			}else{
				//timeout
				if(m_nRep==1){
					//si no hay mas que un ensayo se muestran los resultados
					avSol+=res.get_upper_bound();
					maxSol=avSol;
					//arrayOfMaxSol[j]=avSol;
					avSteps+=res.number_of_steps();
					avLB+=res.get_lower_bound();
				}
				nFails++;
			}
		}//end of test analysis for current algorithm

		///////////////////////////
		//Computes Counters (indep. of FAILS)
		for(int i=0; i<avnCounters.size(); i++){
			avnCounters[i]/=m_nRep;
		}
		arrayOfCounters.push_back(avnCounters);

		////////////////////////////
		//Computes averages taking into account FAILS
		arrayOfFails.push_back(nFails);
		if(nFails!=m_nRep){
			//Existe al menos un caso no fail
			arrayOfAvSol.push_back(avSol/(float)(m_nRep-nFails));
			arrayOfAvTimes.push_back(avTimes/(m_nRep-nFails));	
			
			arrayOfAvPreProcTimes.push_back(avPreProcTimes / (m_nRep - nFails));

			arrayOfAvSteps.push_back(avSteps/(m_nRep-nFails));
			arrayOfAvLB.push_back(avLB/(m_nRep-nFails));
			arrayOfMaxSol.push_back(maxSol);
		}else{ //Todos los intentos con time_out 
			arrayOfAvTimes.push_back(-1.0);	
			arrayOfAvPreProcTimes.push_back(-1.0);

			if(m_nRep==1){ //single execution: show values
				arrayOfAvSol.push_back(avSol);
				arrayOfAvSteps.push_back(avSteps);
				arrayOfAvLB.push_back(avLB);
				arrayOfMaxSol.push_back(maxSol);
			}else{	//multiple repetitions: do not show values
				arrayOfAvSol.push_back(0.0);
				arrayOfAvSteps.push_back(0.0);
				arrayOfAvLB.push_back(0.0);
				arrayOfMaxSol.push_back(0);
			}
		}
	}
	
	//analyse similitude between algorithms (specially for comparison of two algorithms)
	if(info!=nullptr){
		if(com::stl::all_equal(arrayOfAvSol)) info->same_sol=true;
		if(com::stl::all_equal(arrayOfAvSteps))	info->same_steps=true;
		if(com::stl::all_equal(arrayOfAvLB)) info->same_lb=true;
		if(arrayOfAvSteps[0]>arrayOfAvSteps[1])	info->steps_first_greater=true;
		info->steps_lhs=arrayOfAvSteps[0]; 
		info->steps_rhs=arrayOfAvSteps[1]; 
	}

	//***STANDARD DEVIATION ****

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

////////////////////////
// E/S

void print_current_time	(ostream& o){
	o<<PrecisionTimer::local_timestamp();
}

void print_line	(const string str, ostream& o){
	o<<str;
}

void TestAnalyser::print_single(ostream & o, int idAlg){
///////////////////////////
// date: 2/7/13
// prints individual results of algorithms 
// nAlg==-1 all algs (DEFAULT)

	//fills m_nRep and m_nAlg appropiately
	update_sizes();
	if(m_nRep==0 || m_nAlg==0 ){
		LOG_ERROR("empty tests");
		return;
	}

	//bounds parameters
	if(idAlg==-1) idAlg=m_nAlg;
	else (idAlg>m_nAlg)? idAlg=m_nAlg: 1;
	
	o<<"------------------------------------------"<<endl;
	
	for(int r=0; r<m_nRep;r++){
		for(int a=0; a<idAlg; a++){
			o<<arrayOfTests[r][a]<<" ";
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
	if (nRep<0) {
		LOG_ERROR("number of repetitions cannot be negative");
		return;
	}
	
	//fills m_nRep and m_nAlg appropiately
	update_sizes();
	if(m_nRep==0 || m_nAlg==0 ){
		LOG_ERROR("empty tests");
		return;
	}
	

	//bounds parameters
	if(idAlg==-1) idAlg=m_nAlg;
	else if(idAlg>m_nAlg) idAlg=m_nAlg;
	if(nRep>m_nRep) nRep=m_nRep;
	
	
	//E/S: Exception possible because nRep is not Synchro
	o<<endl;
	
	for(int a=0; a<idAlg; a++){
		try{
			o<<arrayOfTests[nRep-1][a]<<" ";
		}catch(exception e){
			/*stringstream sstr("");
			sstr<<"Bad output"<<"Test:"<<a<<" Rep:"<<nRep<<endl;
			LOG_INFO(sstr.str().c_str()<<endl);*/
			LOG_ERROR("Bad output", " Test:", a, " Rep:", nRep);
			break;
		}
	}
}



int TestAnalyser::update_sizes(){
//updates number of repetitions and number of algorithms based on array of tests	
	int retVal=OK;
	m_nRep=arrayOfTests.size();
	(m_nRep>0)?  m_nAlg=arrayOfTests[0].size() :  m_nAlg=0;
	(m_nRep>0 && m_nAlg>0)? retVal=OK : retVal=ERR;
return retVal;
}
