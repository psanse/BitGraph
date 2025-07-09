/**
* @file dimacs_format.h
* @brief header of stateless class DIMACS_READER which implements a number of utilities to read dimacs format
* @details: creation 30/09/2018, ast_update 27/01/2025
* 
* TODO - clean this horror code (08/07/2025)
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
		*  @brief removes empty lines from the input stream - stops at the first non-empty line
		*  @param f (input) stream
		**/
		inline
			void read_empty_lines(std::fstream& f) {
			std::string line;

			streampos oldpos = f.tellg();
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
		
	using gio::dimacs::graph_to_dimacs;

}//end namescape bitgraph	

#endif