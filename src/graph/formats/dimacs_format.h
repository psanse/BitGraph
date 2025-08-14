/**
* @file dimacs_format.h
* @brief utilities to parse dimacs format
* @details: creation 30/09/2018, last_update 09/07/2025
**/

#include <fstream>
#include <sstream>
#include <iostream>

#include "utils/file.h"
#include "utils/common.h"

#ifndef	__DIMACS_READER_H__
#define __DIMACS_READER_H__

namespace bitgraph {

	namespace gio {
		/**
		*  @brief portable empty line skipper when parsing from file stream @f
		*		  - stops at the first non-empty line
		*  @param f (input) fstream
		*  @details: is not working properly in combination with std::getline(f, line)
		*			 in some compilers. Currently deprecated. (14/08/2025)
		**/
		inline
			void skip_empty_lines(std::fstream& f) {
			std::string line;

			std::streampos oldpos = f.tellg();
			while (std::getline(f, line)) {

				if (line.empty() || line[0] == '\n' || line[0] == '\r') {
					oldpos = f.tellg();
					continue;							//skip empty lines
				}

				//rewind to the beginning of the line and exit
				f.seekg(oldpos);
				break;
			}
		}

		namespace dimacs {
			
			/**
			* @brief reads DIMACS header (p edge <nV> <nE>) from the input stream
			* @param f input stream
			* @param n output - number of vertices (nV) read
			* @param m ouptut - number if edges (nE) read
			* @returns number of vertices @n if success, -1 if error
			**/
			inline
				int read_dimacs_header(std::fstream& f, int& n, int& m) {

				std::string line;
				n = 0; m = 0;

				////////////////////
				//skip_empty_lines(f);
				////////////////////

				while (std::getline(f, line)) {
					if (line[0] == 'c' ) continue;			//skip comment lines or ...
					if (line.empty() || line[0] == '\n' || line[0] == '\r') continue;
					if (line[0] != 'p') {
						LOG_ERROR("bad DIMACS protocol  - DIMACS_READER::read_dimacs_header");
						LOGG_ERROR("first character of new line is: ", line[0]);
						return -1;
					}

					std::istringstream iss(line);
					std::string p, type;
					if (iss >> p >> type >> n >> m) {
						if (type != "edge") {
							LOGG_ERROR("bad DIMACS header found: expecting 'p edge <nV> <nE>'- found: ", p, " ", type);
							LOG_ERROR("- DIMACS_READER::read_dimacs_header");
							return -1;
						}
						else {
							break;
						}
					}
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
			template<class Graph_t>
			inline	int graph_to_dimacs(Graph_t& g, std::string filename) {
				ofstream f(filename);
				if (f) {
					g.write_dimacs(f);
					f.close();
					return 0;
				}
				return -1;
			}

		} //namespace dimacs

	} //namespace gio
		
	using gio::dimacs::graph_to_dimacs;

}//end namescape bitgraph	

#endif