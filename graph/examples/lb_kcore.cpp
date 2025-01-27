/**
* @file lb_kcore.cpp
* @brief Example of finding an initial solution to the Maximum Clique Problem using
*        a kcore-based heursitic.
**/

#include <iostream>
#include "graph/algorithms/kcore.h"
#include "utils/common.h"
#include "utils/prec_timer.h"
#include "utils/logger.h"

using namespace std;

int main(int argc, char** argv){
	if(argc != 3){
		LOG_ERROR("Incorrect number of parameters");
		LOG_ERROR("Required <filename> <number of iterations / -1 for all possible iterations>");
		LOG_ERROR("exiting...");
		return -1;
	}

	//read params
	string filename (argv[1]);
	auto nIter = atoi(argv[2]);

	//find initial kcore
	sparse_ugraph ug(filename);
	KCore<sparse_ugraph> kc(ug);

	///////////////////
	kc.find_kcore();
	///////////////////

	//find heuristic clique
	vector<int> heurCLQ;
	PrecisionTimer pt;
		
	pt.cpu_tic();
	if(nIter == EMPTY_ELEM)
		/////////////////////////////////////////		
		heurCLQ =kc.find_heur_clique_sparse();
		/////////////////////////////////////////
	else {
		///////////////////////////////////////////////	
		heurCLQ = kc.find_heur_clique_sparse(nIter);
		///////////////////////////////////////////////	
	}
	double time_sec = pt.wall_toc();
	///////////////////////////////

	//I/O
	LOGG_INFO("\n[t:" , time_sec , "," , " Smax:" , heurCLQ.size() , "]\n");
	com::stl::print_collection(heurCLQ);
}

