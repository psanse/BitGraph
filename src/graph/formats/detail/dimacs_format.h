/**
 * @file dimacs_format.h
 * @brief Header-only helpers for reading and writing DIMACS edge files.
 *
 * @details
 * This header provides the low-level utilities in
 * @c bitgraph::io::detail::dimacs for the DIMACS graph format. The parser
 * skips blank lines and comment lines beginning with `c`, then validates the
 * `p edge <number_of_vertices> <number_of_edges>` header. The writer delegates
 * graph serialization to a graph type's @c write_dimacs() member function.
 *
 * The functions are inline and use non-throwing error-return conventions at
 * their public boundaries: header parsing returns the declared vertex count
 * on success and `-1` on failure, while file writing returns `0` on success
 * and `-1` if the output file cannot be opened. Errors are reported through
 * the project's logging utilities.
 *
 * @author pss
 * @date Created: 30 September 2018
 * @date Last updated: 9 July 2025
 *
 * @see file_utils.h
 * @warning @c graph_to_dimacs requires its graph type to provide a compatible
 *          @c write_dimacs(std::ostream&) member function.
 */

#include <fstream>
#include <sstream>
#include <iostream>

#include "utils/file_utils.h"
//#include "utils/common.h"

#ifndef	__DIMACS_READER_H__
#define __DIMACS_READER_H__

namespace bitgraph {
	namespace io {
		namespace detail {
			/**
			*  @brief portable empty line skipper when parsing from a @stream
			*		  - stops at the first non-empty line
			*  @param f (input) fstream
			*  @details: assumes any line which does not start with \n or \r is a non-empty line
			**/
			inline
				void skip_empty_lines(std::istream& stream) {
				std::string line;
				char next = 0;

				while (stream.good()) {

					int c = stream.peek();
					if (c == EOF) break;
					else {
						next = static_cast<char>(c);
					}

					if (next == '\n' || next == '\r') {
						std::getline(stream, line);
						continue;
					}

					//non-empty line assumed
					break;
				}
			}

			namespace dimacs {

				/**
				* @brief reads DIMACS header (p edge <nV> <nE>) from the input stream
				* @param input input stream
				* @param n output - number of vertices (nV) read
				* @param m ouptut - number if edges (nE) read
				* @returns number of vertices @n if success, -1 if error
				* @details: no throwing interface - catches exceptions internally
				* @details: removes comment lines (starting with 'c') and empty lines
				**/
				inline
					int read_dimacs_header(std::istream& input, int& n, int& m) noexcept {

					std::string line;
					n = 0; m = 0;

					////////////////////
					//skip_empty_lines(f);
					////////////////////
					try {
						while (std::getline(input, line)) {
							if (line.empty()) continue;
							if (line[0] == 'c' || line[0] == '\n' || line[0] == '\r') continue;
							if (line[0] != 'p') {
								LOG_ERROR("bad DIMACS protocol  - DIMACS_READER::read_dimacs_header");
								LOGG_ERROR("first character of new line is: ", line[0]);
								return -1;
							}

							std::istringstream iss(line);
							std::string p, type;
							if (iss >> p >> type >> n >> m) {
								if (type != "edge") {
									LOGG_ERROR(
										"bad DIMACS header found: expecting 'p edge <nV> <nE>'- found: ",
										p, " ", type);
									LOG_ERROR("- DIMACS_READER::read_dimacs_header");
									return -1;
								}
								else {
									break;
								}
							}
						}
					}
					catch (std::exception& e) {
						LOGG_ERROR("error reading DIMACS header: ", e.what());
						LOG_ERROR("- DIMACS_READER::read_dimacs_header");
						return -1;
					}

					//reaches here if the header is found
					return n;
				}

				/*
				* @brief writes a graph in DIMACS format
				* @param g graph
				* @param filename name of output file stream
				* @return 0 if success, -1 if error
				*/
				template<class GraphT>
				inline	int graph_to_dimacs(GraphT& g, std::string filename) {
					ofstream f(filename);
					if (f) {
						g.write_dimacs(f);
						f.close();
						return 0;
					}
					return -1;
				}

			} //namespace dimacs

		} //namespace detail

	}	// namespace io
	

}//end namescape bitgraph	

#endif
