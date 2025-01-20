// result.h: interface for the Result class, a general purpose class for tests in search problems 
//			 where solutions are lists of integers 
// author: jorge artieda
// last update: 1/3/15
//
// Comments: includes fields for multiple soltions as well as multiple counters to keep track of search
//////////////////////////////////////////////////////////////////////

#ifndef __RESULT_H__
#define	__RESULT_H__

#include "prec_timer.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std; 

typedef unsigned long long ull;	
typedef int usint;							//user int
typedef double bound_t;						

///////////
//switches
//#define print_result_logs					//DEF-ON but switch off for clique enumeration

constexpr usint DEFAULT_MAX_NUM_SOL = 5000;		//* default: relevant for Clique Partition / Enumeration routines */

//#define DEFAULT_MAX_NUM_SOL		5000		//* default: relevant for Clique Partition / Enumeration routines */
//#define DEFAULT_MAX_NUM_SOLs	200	
//////////////

class Result {
	friend ostream& operator<< (std::ostream& o, const Result& r);

public:
	static string get_current_local_time();
	const usint MAX_NUM_SOL;	

	Result(usint max_sol = DEFAULT_MAX_NUM_SOL) : MAX_NUM_SOL(max_sol){clear();}
	virtual ~Result(){}

	void  operator =								(const Result& res);			
////////////
// setters and getters

	//getters
	ull	 number_of_steps							()	const {return m_steps;}
	bound_t get_lower_bound							()	const {return m_LB;}
	bound_t get_upper_bound							()	const {return m_UB;}
	bool is_sol_found								()	const {return m_is_sol;}
	double get_user_time							()  const {return m_uT;}
	double get_pre_time								()  const {return m_pT;}
	bool is_time_out								()	const {return m_is_tout;}
	string get_name									()	const {return m_name;}	
	double get_tout									()  const {return m_tout; }
	int get_alg										()  const {return m_alg; }

	int	 get_d1										()	const {return m_d1;}	
	int	 get_d2										()	const {return m_d2;}	
	int	 get_d3										()	const { return m_d3; }
	int& get_d3										()  { return m_d3; }

	usint get_number_of_solutions					()	const {return m_lsol.size();}
	usint number_of_counters						()	const {return m_lc.size();}
	bool get_flag									()	const {return m_flag;}
	vector<usint> get_first_solution				()	const {if(!m_lsol.empty())return m_lsol.front(); else return vector<usint>(); }
	vector<usint> get_last_solution					()	const {if(!m_lsol.empty())return m_lsol.back(); else return vector<usint>();}
	const vector<vector<usint> >& get_all_solutions	()	const {return m_lsol;}
		 vector<vector<usint> >& get_all_solutions	() {return m_lsol;}
	vector<vector<usint> > copy_all_solutions		()	const {return m_lsol;}		
	vector<double> get_counters						()	const {return m_lc;}
	usint get_counter_value							(usint counter)	const { return m_lc[counter];}
	usint get_MAX_NUM_SOL							(){return MAX_NUM_SOL;}	

	vector<usint>& get_first_solution				()	{return m_lsol.front();}		/* will crash if vector is empty */

	//erase
	void clear										();
	void clear_name									() {m_name.clear();}
	void clear_steps								() {m_steps=0;}
	void clear_all_solutions						() {m_lsol.clear();}		
	void clear_last_solution						() {m_lsol.pop_back();}
	
	//setters
	void set_name									(string name) {m_name=name;}
	void set_is_tout								(bool is_tout){ m_is_tout=is_tout;}
	void set_user_time								(float time) {m_uT=time;}
	void set_pre_time								(float time) {m_pT=time;}
	void set_number_of_steps						(ull steps) {m_steps=steps;}
	void set_LB										(bound_t lb) {m_LB=lb;}
	void set_UB										(bound_t ub) {m_UB=ub;}				  
	void set_flag									(bool b) {m_flag=b;}

	void set_d1										(int dout) {m_d1=dout;}
	void set_d2										(int dout) {m_d2=dout;}
	void set_d3										(int dout) {m_d3 = dout; }
	void set_tout									(int tout) {m_tout = tout; }
	void set_alg									(int alg)  {m_alg = alg; }

	bool add_solution								(vector<usint> sol);
	bool add_solution								(size_t size, usint[]);
	void inc_number_of_steps						(int n=1);
	usint inc_counter								(usint counter=0, double num=1.0);			//(creates a counter if it doesn´t exist)

	
	//time
	void tic										(bool preproc=false);					//added preproc timer 23/11/16	
	double toc										(bool preproc=false);			

////////
//E/S
	void print_first_sol							(ostream& o = cout);
	void print_last_sol								(ostream& o = cout);
	void print_all_sol								(ostream& o = cout);
////////////////////////
// Data members
private:
	ull m_steps;
	bound_t m_UB;							//maximization problem may hold current solution here
	bound_t m_LB;							//minimization problem may hold current solution here
	bool m_is_sol;							//true->m_UB is solution 
	bool m_is_tout;	
	bool m_flag;							//to communicate an internal condition
	double m_uT;							//running time interval in seconds with prec. up to milliseconds (user time)
	double m_pT;							//running time interval in seconds with prec. up to milliseconds (pre-processing time)
	double m_tout;
	int  m_alg;								//algorithm variant	

	string m_name;			
	int m_d1;								//extra data 1
	int m_d2;								//extra data 2
	int m_d3;								//extra data 3			//TODO@vector!
	//vector<usint> m_lc;					//list of counters 
	vector<double> m_lc;					//list of additional metrics
	vector <vector<usint> > m_lsol;			//list of candidate solutions
	PrecisionTimer m_rt;					//timer object 	
	vector<int> m_data;						//extra_data
};


#endif