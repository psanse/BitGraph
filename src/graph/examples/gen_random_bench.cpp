/**
* @file gen_random.cpp
* @brief Generates a benchmark of simple uniform random undirected graphs
* @details created 24/05/2015,  last_update 02/05/2025
* @author pss
**/

#include <iostream>
#include <sstream>
#include <string>
#include "graph/algorithms/graph_gen.h"
#include "graph_excep_hand.h"
#include "utils/common.h"
#include "utils/logger.h"

//comment in release mode - to disable assertions
//#undef NDEBUG
#include <cassert>

using namespace std;
using namespace bitgraph;

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
		o << "RANGE SIZE: " << "[" << i.nLB << "," << i.nUB << "]" << endl
		  << "RANGE DENSITY: " << "[" << i.pLB << "," << i.pUB << "]" << endl
		  << "NUM_REP: " << i.nRep <<endl
		  << "INC_SIZE: " << i.incN << endl
		  << "INC_DENSITY: " << i.incP << endl;

		return o;
	}

};

class parserRB {
private:
	int argc_;
	
public:
	input_t info_;
	string path_benchmark_;

public:
	parserRB(int argc,  char* argv[]) :
		argc_{ argc },
		info_{}
	{
		if (argc_ != 9) {
			LOG_ERROR("Enter <range of sizes> <range of densities> <num of instances> <inc size> <inc density> <output dir>");
			LOG_ERROR("exiting...");
			std::exit(- 1);
		}	

		try {
			////////////////
			parse(argv);
			////////////////
		}
		catch (const GraphParseError& e) {
			LOG_ERROR(e.what());
			LOG_ERROR("exiting...");
			std::exit(-1);
		}
		
	}

	void parse(char* argv[]) {

		stringstream sstr;
	
		sstr << argv[1];
		sstr >> info_.nLB;
		
		sstr = std::stringstream();
		sstr << argv[2];
		sstr >> info_.nUB;
		if (info_.nLB > info_.nUB || info_.nLB <= 0) {
			throw GraphParseError("Invalid range of sizes - parserRB::parse");
								
		}
		//assert( (info_.nLB < info_.nUB) || info_.nLB > 0 );

		sstr = std::stringstream();
		sstr << argv[3];
		sstr >> info_.pLB;

		sstr = std::stringstream();
		sstr << argv[4];
		sstr >> info_.pUB;
		if( info_.pLB > info_.pUB || info_.pLB <= 0) {
			throw GraphParseError("Invalid range of densities - parserRB::parse");
		}
		//assert( (info_.pLB < info_.pUB) || info_.pLB > 0);

		sstr = std::stringstream();
		sstr << argv[5];
		sstr >> info_.nRep;
		if (info_.nRep <= 0) {
			throw GraphParseError("Invalid number of repetitions - parserRB::parse");
		}
		//assert(info_.nRep > 0);

		sstr = std::stringstream();
		sstr << argv[6];
		sstr >> info_.incN;
		if (info_.incN <= 0) {
			throw GraphParseError("Invalid number of size increment - parserRB::parse");
		}
		//assert(info_.incN > 0);

		sstr = std::stringstream();
		sstr << argv[7];
		sstr >> info_.incP;
		if (info_.incP <= 0) {
			throw GraphParseError("Invalid number of density increment - parserRB::parse");
		}
		//assert(info_.incP > 0);

		path_benchmark_ = argv[8];		
	}

};

int main(int argc,  char* argv[]) {

	parserRB parser(argc, argv);
		
	//////////////
	//generate single uniform random graph
	cout << "*******************************" << endl;
	cout << "Generating random graph..." << endl;
	ugraph ug;	

	///////////////////////////////////////////////
	RandomGen<ugraph>::create_graph(ug,100,.1);
	///////////////////////////////////////////////

	ug.print_data();
	cout << endl;

	/////////////
	//generate uniform random graph benchmark
	cout << "*******************************" << endl;
	cout << "Generating random graph benchmark..." << endl;	
	cout << parser.info_;	
	cout << "TARGET_PATH: " << parser.path_benchmark_ << endl;
	cout << "*******************************" << endl;

	//declare appropiate random data for the generator
	random_attr_t rt(parser.info_.nLB, parser.info_.nUB, parser.info_.pLB, parser.info_.pUB, parser.info_.nRep, parser.info_.incN, parser.info_.incP);
	
	////////////////////////////////////////////////////////////////////////////////
	RandomGen<ugraph>::create_graph_benchmark(parser.path_benchmark_.c_str(), rt);	
	////////////////////////////////////////////////////////////////////////////////

}

