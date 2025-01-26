/**
 * @file benchmark.cpp
 * @brief implementation of Benchmark class in benchmark.h
 * @date ?
 * @last_update 17/01/25
 * @author pss
 **/

#include "benchmark.h"
#include "logger.h"
#include <string>
#include "utils/common.h"


using namespace std;

Benchmark::Benchmark(const string& pathname):
	PATH_(pathname)
{
	if(pathname.empty()){
		LOG_WARNING("Path for benchmark files empty - Benchmark::Benchmark");
	}
	else {
		LOGG_DEBUG("Path for benchmark files: ", PATH_.c_str(), "-Benchmark::Benchmark");
	}
}

void Benchmark::add_test(const string str_file){
	
	string str("");

	if (PATH_.empty()) {
		str = str_file;
	}
	else {								//appends slash to PATH_ if not present		
		str = PATH_;
		com::dir::append_slash(str);
		str += str_file;

		//#ifdef _WIN32
		//		str=PATH_+"\\"+str_file;		
		//#elif __GNUC__
		//		str=PATH_+"/"+str_file;
		//#endif
	}

	lf_.push_back(str);
}

void Benchmark::add_test(const string str_file, int val){

	string str("");

	if (PATH_.empty()) {
		str = str_file;
	}
	else{								//appends slash to PATH_ if not present		
		str = PATH_;
		com::dir::append_slash(str);
		str += str_file;	

//#ifdef _WIN32
//		str=PATH_+"\\"+str_file;		
//#elif __GNUC__
//		str=PATH_+"/"+str_file;
//#endif
	}
				
	lf_.push_back(str);
	mf_[str]=val;
}

int Benchmark::get_value (string filename){

	if (mf_.count(filename)) {
		return mf_[filename];
	}
	
	return -1;
	
}

///////////////////
// E/S
std::ostream& Benchmark::print(ostream& o) {
	 for(auto i = 0; i < lf_.size(); ++i){
		 o << lf_[i];
		 if(mf_.count(lf_[i])){
			 o << ":" << mf_[lf_[i]];
		 }
		 o << endl;
	 }

	 return o;
}
