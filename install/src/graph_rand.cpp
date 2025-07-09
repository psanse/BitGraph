/**
* @file graph_rand.cpp
* @brief Example of GRAPH, part of the BITGRAPH lib.
* @details: a target path is required in the command line for the uniform random benchmark
* @details created 02/2025, last updated 04/07/2025
**/


#include <iostream>
#include "BitGraph/graph/graph.h"							//header file for main types of GRAPH lib
#include "BitGraph/graph/algorithms/graph_gen.h"			//uniform random graph generator

using namespace std;
//using namespace bitgraph;									//alternatively, to avoid typing "bitgraph::" before each type



//UNIT TESTS
int main(int argc, char* argv[]) {

	if (argc != 2){
		LOG_ERROR("please enter a valid output directory...exiting");
		std::exit(-1);
	}

	//////////////
	//generate single uniform random graph
	cout << "*******************************" << endl;
	cout << "Generating random graph..." << endl;
	bitgraph::ugraph ug;

	///////////////////////////////////////////////
	bitgraph::RandomGen<bitgraph::ugraph>::create_graph(ug, 100, .1);
	///////////////////////////////////////////////

	ug.print_data();
	cout << endl;

	/////////////
	//generate uniform random graph benchmark
	cout << "*******************************" << endl;
	cout << "Generating random graph benchmark..." << endl;

	
	//declare appropiate random data for the generator
	/**
	 *range of sizes : [10, 15]
	 *range of densities: [0.5, 0.6]
	 *number of instances: 2 of each type
	 *increment size: 1
	 *increment density: 0.1	
	**/
	bitgraph::random_attr_t rt(10, 15, 0.5, 0.6, 2, 1, 0.1);

	string output_path = argv[1];

	////////////////////////////////////////////////////////////////////////////////
	bitgraph::RandomGen<bitgraph::ugraph>::create_graph_benchmark(output_path, rt);
	////////////////////////////////////////////////////////////////////////////////


}