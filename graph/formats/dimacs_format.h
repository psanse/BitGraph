/*
* @file dimacs_format.h
* @brief header of stateless class DIMACS_READER which implements a number of utilities to read dimacs format
* @creation 30/09/2018
* @last_update 27/01/2025
* @authos pss
*/

#include <fstream>
#include <sstream>
#include <iostream>

#include "utils/file.h"
#include "utils/common.h"

#ifndef	__DIMACS_READER_H__
#define __DIMACS_READER_H__

namespace gio {
	/*
	* @brief removes empty lines from the input stream
	* @param f input stream
	* @return 0 if success, -1 if error (currently only 0)
	*/
	inline
	int read_empty_lines(std::fstream& f) {
		std::string line;
		char c;

		while (true) {
			c = f.peek();
			if (c == '\n') {
				std::getline(f, line);
				continue;
			}
			break;
		}

		return 0;
	}

	namespace dimacs {
		
		/*
		* @brief reads DIMACS header (p edge <nV> <nE>) from the input stream
		* @param f input stream
		* @param n output - number of vertices (nV) read
		* @param m ouptut - number if edges (nE) read
		* @return 0 if success, -1 if error
		*/
		inline
		int read_dimacs_header(std::fstream& f, int& n, int& m) {

			std::string line;
			std::string token;
			char c;
			bool dimacs_header_found = false;

			while (!dimacs_header_found) {

				///////////////
				c = f.peek();
				///////////////

				switch (c) {
				case 'c':							//comment line in DIMACS format
				case '\n':
				case '\r':							//this is not DIMACS format - CHECK
					std::getline(f, line);
					break;

				case 'p':  //header found - read: p edge <nV> <nE> 

					/////////////////////////////////
					f >> token >> token >> n >> m;
					/////////////////////////////////

					if (!f.good() || token != "edge") {
						LOG_ERROR("bad DIMACS header found  ('p' line) - DIMACS_READER::read_dimacs_header");
						return -1;
					}

					dimacs_header_found = true;
					std::getline(f, line);						//remove remaining part of the line
					break;

				default:
					LOG_ERROR("bad DIMACS protocol  - DIMACS_READER::read_dimacs_header");
					LOGG_ERROR("first character of new line is: ", c);
					return -1;
				}

			}

			//returns the number of vertices
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

#endif