/**
* @file gen_random.cpp
* @brief Generates a benchmark of simple uniform random undirected graphs
* @created 24/05/2015
* @tagOne 20/12/22 -- changed to a proper random generator executable
* @last_update 27/01/25
**/

#include <iostream>
#include <string.h>
#include <string>
#include "graph/algorithms/graph_gen.h"
#include "utils/logger.h"

//comment in release mode - to disable assertions
//#undef NDEBUG
#include <cassert>

using namespace std;

struct input_t{
	int nLB;
	int nUB;
	double pLB;
	double pUB;
	int nRep;
	int incN;
	double incP;

	//I/O
	friend ostream & operator<<(ostream& o, const input_t& i) {
		o << "RANGE N: " << "[" << i.nLB << "," << i.nUB << "]" << endl
		  << "RANGE P: " << "[" << i.pLB << "," << i.pUB << "]" << endl
		  << "NREP: " << i.nRep <<endl
		  << "INCN: " << i.incN << endl
		  << "INCP: " << i.incP << endl;

		return o;
	}

}info;

int main(int argc, char** argv){
		
	if (argc != 9) {
		LOG_ERROR("Enter <range of sizes> <range of densities> <num of instances> <inc size> <inc density> <output dir>"						);
		LOG_ERROR("exiting...");
		return -1;
	}

	//////////////////////////
	//parse input params
	stringstream sstr;
	string str_path_benchmark;
		
	sstr = std::stringstream();
	sstr << argv[1];
	sstr >> info.nLB;
	sstr = std::stringstream();
	sstr << argv[2];
	sstr >> info.nUB;
	sstr = std::stringstream();
	sstr << argv[3];
	sstr >> info.pLB;
	sstr = std::stringstream();
	sstr << argv[4];
	sstr >> info.pUB;
	sstr = std::stringstream();
	sstr << argv[5];
	sstr >> info.nRep;
	sstr = std::stringstream();
	sstr << argv[6];
	sstr >> info.incN;
	sstr = std::stringstream();
	sstr << argv[7];
	sstr >> info.incP;

	str_path_benchmark = argv[8];

	//assertions
	assert(info.nLB > info.nUB || info.nLB < 0);
	assert(info.pLB > info.pUB || info.pLB < 0);
	assert(info.nRep <= 0);
	assert(info.incN <= 0);
	assert(info.incP <= 0);


	//I/O
	LOG_INFO("*******************************");
	LOG_INFO("Generating random graph benchmark");
	LOGG_INFO("PATH: ", str_path_benchmark);
	sstr = std::stringstream();
	sstr << info;
	LOGG_INFO(sstr.str());	
	LOG_INFO("*******************************");
	
	/////////////////////////////	
	//create the benchmark
	random_attr_t rt(info.nLB, info.nUB, info.pLB, info.pUB, info.nRep, info.incN, info.incP);

	/////////////////////////////////////////////////////////////////////////////
	RandomGen<ugraph>::create_graph_benchmark(str_path_benchmark.c_str(), rt);
	/////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////
	//create a single graph
	//ugraph g;
	//RandomGen<ugraph>::create_ugraph(g,100,.1);
	//g.print_info();
	//////////////////////////////////

}

