/**
 * @file graph_formats.cpp
 * @brief Converts an undirected graph file to another supported file format.
 *
 * Reads an undirected graph from a file (the input format is detected
 * automatically: DIMACS, Matrix Market or edge list) and writes it back in the
 * output format chosen on the command line.
 *
 * Usage:
 * @code
 *   graph_formats <filename> <output format>
 *   output format: 1 - DIMACS (.clq), 2 - Matrix Market (.mtx), 3 - edge list (.edges)
 * @endcode
 *
 * The output file is created in the current working directory. It keeps the
 * base name of the input file (directory and extension removed) and appends
 * the suffix "_u" plus the extension of the chosen format, e.g. reading
 * "data/brock200_1.clq" with format 2 writes "brock200_1_u.mtx".
 *
 * @created 12/12/2014
 * @updated 27/01/25
 * @author pss
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "graph/graph.h"
#include "utils/logger.h"

using namespace std;
using namespace bitgraph;

/** Output file formats. The numeric values are the ones expected on the command line. */
enum class file_t { DIMACS = 1, MTX, EDGELIST };

int main(int argc, char** argv) {

	// Expect exactly two arguments: input file and output format number
	if (argc != 3) {
		LOG_ERROR("Introduce <filename> <ouput format number [1...3]> 1-DIMACS, 2-MTX, 3-EDGES");
		LOG_ERROR("exiting...");
		return -1;
	}

	/////////////////////////
	// read input params
	string filename = argv[1];
	file_t ftype = static_cast<file_t>(atoi(argv[2]));		// out-of-range values are rejected in the switch below

	// Read the graph: the constructor detects the input format
	LOGG_INFO("reading graph from ", filename);

	ugraph g1(filename);		// alternatively, can use the sparse encoding sparse_ugraph
	g1.print_data();

	///////////////////
	// write to file

	// Build the output name: remove the extension, then the directory path
	// (both '\' and '/' separators are accepted)
	filename = filename.substr(0, filename.find_last_of("."));
	filename = filename.substr(filename.find_last_of("\\ /") + 1);
	LOGG_INFO("writing to:", filename);

	// Write the graph in the requested format
	ofstream f;
	switch (ftype) {
	case file_t::DIMACS:
		f.open(filename + "_u.clq");
		g1.write_dimacs(f);
		break;
	case file_t::MTX:
		f.open(filename + "_u.mtx");
		g1.write_mtx(f);
		break;
	case file_t::EDGELIST:
		f.open(filename + "_u.edges");
		g1.write_EDGES(f);
		break;
	default:
		LOG_ERROR("unknown output graph format type");
		LOG_ERROR("exiting...");
		return -1;
	}

	f.close();
}
