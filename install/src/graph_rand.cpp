/**
* @file graph_rand.cpp
* @brief GRAPH uniform random graphs example - linking to GRAPH lib.
* @details created 02/2025, last updated 02/05/2025
**/
using namespace std;

#include <iostream>
#include "BitGraph/graph/graph.h"							//header file for main types of GRAPH lib
#include "BitGraph/graph/algorithms/graph_gen.h"			//uniform random graph generator

//comment in release mode - to disable assertions
//#undef NDEBUG
#include <cassert>

using namespace std;


//UNIT TESTS
int main() {

	//////////////
	//generate single uniform random graph
	cout << "*******************************" << endl;
	cout << "Generating random graph..." << endl;
	ugraph ug;

	///////////////////////////////////////////////
	RandomGen<ugraph>::create_graph(ug, 100, .1);
	///////////////////////////////////////////////

	ug.print_data();
	cout << endl;

	/////////////
	//generate uniform random graph benchmark
	cout << "*******************************" << endl;
	cout << "Generating random graph benchmark..." << endl;


	//declare appropiate random data for the generator
	//range of sizes: [10, 15]
	//range of densities: [0.5, 0.6]
	//number of instances: 2 of each type
	//increment size: 1
	//increment density: 0.1
	random_attr_t rt(10, 15, 0.5, 0.6, 2, 1, 0.1);

	string output_path = "./benchmark";

	////////////////////////////////////////////////////////////////////////////////
	RandomGen<ugraph>::create_graph_benchmark(output_path, rt);
	////////////////////////////////////////////////////////////////////////////////


}