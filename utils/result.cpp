//result.cpp: implementation of the result class (result.h)

#include "result.h"
#include "logger.h"
#include "common.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>

using namespace std;

ostream& operator<< (ostream& o, const Result& r){
	 o<<setw(30)<<left<<r.m_name<<setw(15)<<r.m_steps<<setw(10)<<r.m_UB<<setw(10)<<r.m_LB<<fixed<<setw(12)<<setprecision(3)<<right<<r.m_uT;
	
	 //shows active counters
	o<<setw(8)<<right;
	for(int i=0; i<r.number_of_counters(); i++){
			o<<" "<<r.get_counters()[i]<<" ";
	}
	
	o<<endl;
	return o;
}

string Result::get_current_local_time(){
	return PrecisionTimer::local_timestamp();  
}


void Result::clear(){
	m_steps=0;
	m_UB=0;							
	m_LB=0;						
	m_is_sol=false;				
	m_is_tout=false;				
	m_uT = 0;
	m_pT = 0;
	m_name.clear();
	m_lc.clear();
	m_lsol.clear();
	m_flag=false;
	m_d1=0;
	m_d2=0;
}


void Result::tic(bool preproc){
	if(preproc){
		m_pT=0;
	}else{
		m_uT=0;
	}
	m_rt.wall_tic();
}


bool Result::add_solution(vector<usint> v){
//////////////////////////
// stores a solution as a set of numbers
// TRUE if solution added

	if(m_lsol.size()<MAX_NUM_SOL){
		m_lsol.push_back(v);
	}else{
#ifdef	print_result_logs
		LOG_DEBUG("Result::add_solution()-cannot store more solutions");
#endif
		return false;
	}
return true;				//TRUE if solution added
}

bool Result::add_solution(size_t size, usint sol[]){
//////////////////////////
// stores a solution as a set of numbers
// TRUE if solution added

	if(m_lsol.size()>=MAX_NUM_SOL){
#ifdef	print_result_logs
		LOG_DEBUG("Result::add_solution()-cannot store more solutions");
#endif
		return false;
	}
	vector<usint> v;
	copy(sol, sol+size, back_inserter<vector<usint> >(v));
	m_lsol.push_back(v);
		
return true;				
}

double Result::toc(bool preproc){
	if(preproc){
		m_pT=m_rt.wall_toc();
		return m_pT;
	}

	m_uT=m_rt.wall_toc();
	return m_uT;
}

void Result::inc_number_of_steps(int n){
	m_steps+=n;
}

usint Result::inc_counter(usint index, double num){
//////////////////////
// creates new counter if not allocated
	if(index>=m_lc.size()){
		m_lc.push_back(num);		//starts a new counter with value num
		if(index!=m_lc.size()-1){
#ifdef	print_result_logs
			LOG_INFO("Result:: new counter at index "<<m_lc.size()-1<<" not "<<index);
#endif
		}
		return m_lc.size()-1;			//returns new index number
	}else if(m_lc.size()<index){
		LOG_ERROR("Result:: Trying to increment an inconsistent counter");
		return -1;
	}
	
	// Suma num unidades al contador actual
	try{
		m_lc[index]+=num;
	}catch(exception e){
		cout<<e.what();
		LOGG_ERROR("Result:: error when incrementing counter: ", index);
		return -1;
	}

	return -1;  //should not reach here
}

void Result::print_first_sol(std::ostream& o){
	for(int i=0; i<m_lsol.front().size(); i++){
		o<<m_lsol.front()[i]<<" ";
	}
	o<<"["<<m_lsol.front().size()<<"]";
}

void Result::print_last_sol(std::ostream& o ){
	for(int i=0; i<m_lsol.back().size(); i++){
		o<<m_lsol.back()[i]<<" ";
	}
	o<<"["<<m_lsol.back().size()<<"]";
}

void Result::print_all_sol (ostream& o){
	for(int i=0; i<m_lsol.size(); i++){
		com::stl::print_collection(m_lsol[i]);
		o<<endl;
	}
	o<<endl;
	
}

void  Result::operator = (const Result& res){
	m_steps=res.m_steps;
	m_UB=res.m_UB;			
	m_LB=res.m_LB;			
	m_is_sol=res.m_is_sol;				
	m_is_tout=res.m_is_tout;						
	m_uT=res.m_uT;
	m_name.assign(res.get_name());
	m_lc=res.m_lc;
	m_lsol=res.m_lsol;			
}

