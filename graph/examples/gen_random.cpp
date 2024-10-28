//gen_random.cpp: an example to generate a benchmark of simple undirected middle-size graphs  
//author:pablo san segundo
//inital_date@24/05/2015
//last_update@20/12/22 -- changed to a proper random generator executable

#include <iostream>
#include <string.h>
#include <string>
#include "../graph.h"
#include "../graph_gen.h"

using namespace std;

struct input_t{
	int nLB;
	int nUB;
	double pLB;
	double pUB;
	int nRep;
	int incN;
	double incP;

	friend ostream & operator<<(ostream& o, const input_t& i) {
		o << "RANGE N: " << "[" << i.nLB << "," << i.nUB << "]" << endl
		  << "RANGE P: " << "[" << i.pLB << "," << i.pUB << "]" << endl
		  << "NREP: " << i.nRep <<endl
		  << "INCN: " << i.incN << endl
		  << "INCP: " << i.incP << endl;

		return o;
	}

}input;

int main(int argc, char** argv){
	stringstream sstr;
	string str_path_benchmark;
	Logger::SetInformationLevel(LOGGER_INFO);

	if (argc != 9) {
		LOG_ERROR("please enter the range of sizes [nLB, nUB], the range of densities [pLB, pUB], the number of instances per G(N,P), the size increment, the density increment and the path, exiting...");
		exit(-1);
	}
	////////////////////////////////////////////////////////////////////////
	//READ INPUT DATA
	sstr = std::stringstream();
	sstr << argv[1];
	sstr >> input.nLB;
	sstr = std::stringstream();
	sstr << argv[2];
	sstr >> input.nUB;
	sstr = std::stringstream();
	sstr << argv[3];
	sstr >> input.pLB;
	sstr = std::stringstream();
	sstr << argv[4];
	sstr >> input.pUB;
	sstr = std::stringstream();
	sstr << argv[5];
	sstr >> input.nRep;
	sstr = std::stringstream();
	sstr << argv[6];
	sstr >> input.incN;
	sstr = std::stringstream();
	sstr << argv[7];
	sstr >> input.incP;

	str_path_benchmark = argv[8];

	//ASSERT
	if (input.nLB > input.nUB || input.nLB < 0 ) {
		LOG_ERROR("please enter different values the range of sizes [nLB, nUB] (note that both ends are included, exiting...");
		exit(-1);
	}
	if (input.pLB > input.pUB || input.pLB < 0) {
		LOG_ERROR("please enter different values the range of densities [nLB, nUB] (note that both ends are included, exiting...");
		exit(-1);
	}
	if (input.nRep <=0 ) {
		LOG_ERROR("number of repetitions invalid: "<< input.nRep<< " exiting...");
		exit(-1);
	}
	if (input.incN <= 0) {
		LOG_ERROR("size of  graphs increment invalid: "<<input.incN<< " exiting...");
		exit(-1);
	}
	if (input.incP <= 0) {
		LOG_ERROR("density of  graphs increment invalid: "<< input.incP<<" exiting...");
		exit(-1);
	}

	LOG_INFO("*******************************");
	LOG_INFO("Generating random graph benchmark");
	LOG_INFO("PATH: " << str_path_benchmark);
	sstr = std::stringstream();
	sstr << input;
	LOG_INFO(sstr.str());	
	LOG_INFO("*******************************");
	/////////////////////////////////////////////////////////////////////////
		
	//create the benchmark
	random_attr_t rt(input.nLB, input.nUB, input.pLB, input.pUB, input.nRep, input.incN, input.incP);
	RandomGen<ugraph>::create_ugraph_benchmark(str_path_benchmark.c_str(), rt);


	//////////////////////////////////
	//create a single graph
	//ugraph g;
	//RandomGen<ugraph>::create_ugraph(g,100,.1);
	//g.print_data();
	//////////////////////////////////

}

