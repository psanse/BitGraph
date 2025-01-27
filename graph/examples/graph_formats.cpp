/**
* @file graph_formats.cpp
* @brief writes a simple udirected graph to file in different formats
* @created 12/12/2014
* @updated 27/01/25
* @author pss
*/

#include <iostream>
#include <string.h>
#include <string>
#include "graph/graph.h"
#include "utils/logger.h"

using namespace std;

enum class file_t{DIMACS=1, MTX, EDGELIST};

int main(int argc, char** argv){
	if(argc != 3){
		LOG_ERROR("Introduce <filename> <ouput format number [1...3]> 1-DIMACS, 2-MTX, 3-EDGES");
		LOG_ERROR("exiting...");
		return -1;
	}

	/////////////////////////
	//read input params
	string filename = argv[1];
	file_t ftype = (file_t) atoi(argv[2]);

	//parse file with format
	LOGG_INFO("reading graph from ", filename);

	sparse_ugraph g1(filename);
	g1.print_data();

	///////////////////
	//write to file

	//extract filename (remove path and extension)
	filename = filename.substr(0,filename.find_last_of("."));	
	filename = filename.substr(filename.find_last_of("\\ /") + 1);
	LOGG_INFO("writing to:", filename);

	ofstream f;
	switch(ftype){
	case file_t::DIMACS:
		f.open(filename + "_u.clq");
		g1.write_dimacs(f);		
		break;
	case file_t::MTX:
		f.open(filename+"_u.mtx");
		g1.write_mtx(f);	
		break;
	case file_t::EDGELIST:
		f.open(filename+"_u.edges");
		g1.write_EDGES(f);		
		break;
	default:
		LOG_ERROR("unknown output graph format type");
		LOG_ERROR("exiting...");
		return -1;
	}

	f.close();

}

