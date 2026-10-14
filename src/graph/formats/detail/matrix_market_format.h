/**
 * @file matrix_market_format.h
 * @brief Header-only Matrix Market coordinate reader for graph adjacency data.
 *
 * @details
 * This header defines the templated @c bitgraph::io::detail::MMI reader. It
 * uses the Matrix Market I/O helpers from @c mmio.h to load sparse pattern
 * matrices into a graph-like object. The input must describe a square matrix;
 * one-based matrix coordinates are converted to BitGraph's zero-based vertex
 * indices and each coordinate becomes a graph edge.
 *
 * By default the reader accepts sparse pattern matrices and, when
 * @c ALLOW_DIRECTED_GRAPHS is enabled, does not require a symmetric Matrix
 * Market banner. Self-loops are reported and passed to the graph object, and
 * failures are logged before returning `-1`. The graph is resized to the
 * matrix dimension and receives the input filename as its name.
 *
 * @tparam T Graph type providing @c reset(), @c add_edge(), and @c set_name()
 *           operations compatible with the reader.
 *
 * @author pss
 * @date Created: 6/10/2014
 * @date Last updated: 30/06/2025 (added bitgraph namespace)
 *
 * @see mmio.h
 * @see graph.h
 * @warning This reader is intentionally limited to sparse pattern matrices
 *          and square adjacency matrices; unsupported Matrix Market types or
 *          non-square inputs are rejected.
 */

#include "mmio.h"
#include "utils/logger.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>

#ifndef BITGRAPH_GRAPH_MATRIX_MARKET_FORMAT_H
#define BITGRAPH_GRAPH_MATRIX_MARKET_FORMAT_H

#define ALLOW_DIRECTED_GRAPHS	//does not look for "symmetric" header

namespace bitgraph {
	namespace io {
		namespace detail {

			template<class T>
			class MMI {
			public:
				MMI(T& gout) :g(gout) {}
				int read(const string& filename);

			private:
				void what(int error_code, ostream & = cerr);
				MM_typecode matcode;			//4 char code which represents the format type
				T& g;
			};

		}// end namespace _impl
	} // end namespace io
}//end namespace bitgraph

////////////////////////////////////
// Necessary implementation of template methods in header file

namespace bitgraph {
	namespace io {
		namespace detail {

			template<class T>
			int MMI<T>::read(const string& filename) {
				int M, N, nz, v, w;

				//open file
				FILE* f = fopen(filename.c_str(), "r");
				if (f == NULL) {
					LOGG_ERROR(filename.c_str(), " not found - MMI<T>::read");
					return -1;
				}

				//read banner	
				int ret_code = 0;
				if ((ret_code = mm_read_banner(f, &matcode)) != 0) {

					LOG_ERROR("Could not process Matrix Market banner-MMI<T>::read");
					what(ret_code);
					fclose(f);
					return -1;
				}

				//filter out everything except MCPS
#ifdef ALLOW_DIRECTED_GRAPHS

				if ((!mm_is_sparse(matcode)) || (!mm_is_pattern(matcode)) /*|| (!mm_is_symmetric(matcode))*/) {

					LOG_ERROR("Sorry, this application does not support Market Market type -MMI<T>::read");

					stringstream sstr("Market Market type: ");
					sstr << mm_typecode_to_str(matcode) << endl;
					LOGG_ERROR(sstr.str());

					fclose(f);
					return -1;
				}

#else

				if ((!mm_is_sparse(matcode)) || (!mm_is_pattern(matcode)) || (!mm_is_symmetric(matcode))) {
					cout << "Sorry, this application does not support ";
					stringstream sstr("Market Market type: ");
					sstr << mm_typecode_to_str(matcode) << endl;
					fclose(f);
					return -1;
				}

#endif

				//find out size of sparse matrix 
				if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) != 0) {
					what(ret_code);
					fclose(f);
					return -1;
				}

				//check it is a square matrix
				if (M != N) {
					LOG_ERROR("non-square adjacency matrix-MMI<T>::read");
					fclose(f);
					return -1;
				}

				//init size
				g.reset(N);

				//read edges
				for (auto i = 0; i < nz; ++i) {
					if (fscanf(f, "%d %d", &v, &w) != 2) {
						LOG_ERROR("bad reading protocol-MMI<T>::read");
						fclose(f);
						return -1;
					}

					if (v == w) {
						LOGG_ERROR("loops found in vertex: ", v, " -MMI<T>::read");
					}

					g.add_edge(v - 1, w - 1);		//0 based
				}

				//name (remove path)
				g.set_name(filename);


				fclose(f);
				return 0;
			}

			template<class T>
			void MMI<T>::what(int error_code, ostream& o) {
				switch (error_code) {
				case MM_COULD_NOT_READ_FILE:
					o << "could not read file" << endl;
					break;
				case MM_PREMATURE_EOF:
					o << "premature EOF" << endl;
					break;
				case MM_NOT_MTX:
					o << "no matrix format" << endl;
					break;
				case MM_NO_HEADER:
					o << "incorrect header" << endl;
					break;
				case MM_UNSUPPORTED_TYPE:
					o << "not supported type" << endl;
					break;
				case MM_LINE_TOO_LONG:
					o << "line too long" << endl;
					break;
				case MM_COULD_NOT_WRITE_FILE:
					o << "could not write to file" << endl;		///this should not occur
					break;
				default:
					o << "unknown error type" << endl;

				}
			}

		}// end namespace detail
	}// end namespace io

}//end namespace bitgraph

#endif // BITGRAPH_GRAPH_MATRIX_MARKET_FORMAT_H
