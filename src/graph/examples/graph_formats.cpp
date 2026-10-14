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
 * @note The input file is checked before reading it: the GRAPH readers
 *       terminate the program when a file cannot be opened.
 *
 * @created 12/12/2014
 * @updated 14/10/2026
 * @author pss
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "graph/graph.h"

namespace {

	/** Output file formats. The numeric values are the ones expected on the command line. */
	enum class OutputFormat { DIMACS = 1, MTX = 2, EDGELIST = 3 };

	void print_usage()
	{
		std::cerr << "Usage: graph_formats <filename> <output format>\n"
			<< "  output format: 1 - DIMACS (.clq), 2 - Matrix Market (.mtx), 3 - edge list (.edges)\n";
	}

	/**
	 * @brief Name of the output file: base name of the input path (no directory,
	 *        no extension) followed by "_u" and the extension of the chosen format.
	 */
	std::string output_filename(const std::string& input_path, OutputFormat format)
	{
		// remove the directory ('/' and '\' separators are both accepted)
		const std::size_t slash = input_path.find_last_of("/\\");
		std::string name = (slash == std::string::npos) ? input_path : input_path.substr(slash + 1);

		// remove the extension
		const std::size_t dot = name.find_last_of('.');
		if (dot != std::string::npos && dot != 0) {
			name.erase(dot);
		}

		switch (format) {
		case OutputFormat::DIMACS:   return name + "_u.clq";
		case OutputFormat::MTX:      return name + "_u.mtx";
		case OutputFormat::EDGELIST: return name + "_u.edges";
		}
		return name;    // not reached: all formats are handled above
	}

} // anonymous namespace

int main(int argc, char** argv)
{
    if (argc != 3) {
        print_usage();
        return 1;
    }

    const std::string input_path = argv[1];

    // Validate the output format number [1..3]
    const int format_number = std::atoi(argv[2]);
    if (format_number < 1 || format_number > 3) {
        std::cerr << "Unknown output format: " << argv[2] << '\n';
        print_usage();
        return 1;
    }
    const OutputFormat format = static_cast<OutputFormat>(format_number);

    // Check the input file up front (the reader would terminate the program)
    if (!std::ifstream(input_path)) {
        std::cerr << "Cannot open input file: " << input_path << '\n';
        return 1;
    }

    // Read the graph: the constructor detects the input format.
    // The sparse encoding (bitgraph::sparse_ugraph) can be used instead.
    bitgraph::ugraph g(input_path);
    g.print_data();

    // Write the graph in the requested format
    const std::string out_path = output_filename(input_path, format);
    std::ofstream out(out_path);
    if (!out) {
        std::cerr << "Cannot create output file: " << out_path << '\n';
        return 1;
    }

    switch (format) {
    case OutputFormat::DIMACS:
        g.write_dimacs(out);
        break;
    case OutputFormat::MTX:
        g.write_mtx(out);
        break;
    case OutputFormat::EDGELIST:
        g.write_EDGES(out);
        break;
    }

    out.close();
    if (!out) {
        std::cerr << "Error while writing: " << out_path << '\n';
        return 1;
    }

    std::cout << "Written: " << out_path << '\n';
    return 0;
}
