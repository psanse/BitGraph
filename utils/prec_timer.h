//A simple precision timer for Linux and Windows platforms


#ifndef __PRECISION_TIMER__
#define __PRECISION_TIMER__

#include <iostream>
#include <sstream>
using namespace std;

#ifdef _MSC_VER
#pragma once
#include <Windows.h>
#elif __GNUC__
#include <sys/time.h>
#endif

class PrecisionTimer{
public:
	PrecisionTimer():wall_time(0.0), cpu_time(0.0){}

	//local time stamp
	static string local_timestamp(bool date=true){
		stringstream sstr;
#ifdef _MSC_VER
		SYSTEMTIME st;

		//get current local time
		GetLocalTime(&st);	
		if(date){
			sstr<<st.wDay<<"/"<<st.wMonth<<"/"<<st.wYear<<" ";
		}
		sstr<<st.wHour<<"h:"<<st.wMinute<<"m:"<<st.wSecond<<"s"<<endl;
		return sstr.str();

#elif __GNUC__
		time_t t=time(&t);
		sstr<<ctime(&t);		
		return sstr.str();
#endif
	}

	//timers
	void wall_tic(){wall_time=get_wall_time();}
	double wall_toc(){return get_wall_time()-wall_time;};
	void cpu_tic(){cpu_time=get_cpu_time();}
	double cpu_toc(){return get_cpu_time()-cpu_time;};

private:
	double get_wall_time(){
#ifdef _MSC_VER
		LARGE_INTEGER time,freq;
		if (!QueryPerformanceFrequency(&freq)){
			//  Handle error
			return 0;
		}
		if (!QueryPerformanceCounter(&time)){
			//  Handle error
			return 0;
		}
		return (double)time.QuadPart / freq.QuadPart;
	
#elif __GNUC__
		struct timeval time;
		if (gettimeofday(&time,NULL)){
			//  Handle error
			return 0;
		}
		return (double)time.tv_sec + (double)time.tv_usec * .000001;
				
#endif
		//error for other OS
	}

	double get_cpu_time(){
#ifdef _MSC_VER
		FILETIME a,b,c,d;
		if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
			//  Returns total user time.
			//  Can be tweaked to include kernel times as well.
			return
				(double)(d.dwLowDateTime |
				((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
		}else{
			//  Handle error
			return 0;
		}
#elif __GNUC__
	  return (double)clock() / CLOCKS_PER_SEC;

#endif
	//error for other OS
	}

private:
	double wall_time;
	double cpu_time;
};

#endif


