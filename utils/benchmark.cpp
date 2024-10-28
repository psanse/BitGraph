#include "benchmark.h"
#include "logger.h"
#include <string>

ostream& operator <<(ostream& o,  Benchmark& b ){
	b.print(o);
	return o;
}

Benchmark::Benchmark(const string& pathname):m_PATH(pathname){
	
	if(pathname.empty()){
		LOG_WARNING("Path for benchmark files empty");
		return;
	}
	LOG_INFO("Path for benchmark files: "<<m_PATH);
}

void Benchmark::add_test(const string str_file){
//test without values
	string str("");
	if(m_PATH.empty()) str=str_file;
	else{
#ifdef _WIN32
		str=m_PATH+"\\"+str_file;		
#elif __GNUC__
		str=m_PATH+"/"+str_file;
#endif
	}
	m_lf.push_back(str);
}

void Benchmark::add_test(const string str_file, int val){
//test with values
	string str("");
	if(m_PATH.empty()) str=str_file;
	else{
#ifdef _WIN32
		str=m_PATH+"\\"+str_file;		
#elif __GNUC__
		str=m_PATH+"/"+str_file;
#endif
	}
			
	m_lf.push_back(str);
	m_mf[str]=val;
}

int Benchmark::get_Value (string filename){
	if(m_mf.count(filename)) return m_mf[filename];
	else return -1;
}

///////////////////
// E/S
void Benchmark::print(ostream& o) {
	 for(int i=0; i<m_lf.size(); i++){
		 o<<m_lf[i];
		 if(m_mf.count(m_lf[i])){
			 o<<":"<<m_mf[m_lf[i]];
		 }
		 o<<endl;
	 }
}
