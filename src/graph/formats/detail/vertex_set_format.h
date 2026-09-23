/**
 * @file vertex_set_io.h
 * @brief Input and output utilities for graph vertex sets.
 *
 * This header provides functions for writing collections of vertex indices
 * and reading vertex-interdiction masks.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 * 
 */

// TODO: add unit tests (23/09/2026)

#ifndef BITGRAPH_GRAPH_IO_VERTEX_SET_IO_H
#define BITGRAPH_GRAPH_IO_VERTEX_SET_IO_H

#include <cstddef>
#include <vector>
#include <exception>
#include <fstream>
#include <string>
#include "utils/logger.h"


 /**
  * @brief Writes a collection of vertex indices to a file.
  *
  * The first line has the form `size N`. Each subsequent line contains one
  * vertex index.
  *
  * @tparam Collection Iterable collection whose elements support integer
  *                     addition and stream insertion.
  * @param filename Output filename.
  * @param vertices Vertex indices to write.
  * @param one_based Whether to add one to each vertex index before writing.
  * @return `0` on success; `-1` if the file cannot be opened or written.
  */
namespace bitgraph {
	namespace io {
		namespace detail {


			/**
			 * @brief Writes a collection of vertex indices to a file.
			 *
			 * The first line has the form `size N`. Each subsequent line contains one
			 * vertex index.
			 *
			 * @tparam Collection Iterable collection whose elements support integer
			 *                     addition and stream insertion.
			 * @param filename Output filename.
			 * @param vertices Vertex indices to write.
			 * @param one_based Whether to add one to each vertex index before writing.
			 * @return `0` on success; `-1` if an error occurs.
			 *
			 * @note This function provides a non-throwing interface. All exceptions are
			 *       caught internally and reported as an error return value.
			 */
			template<class CollectionT>
			inline
				int write_vertex_set(
					const std::string& filename,
					const CollectionT& vertices,
					bool one_based = true) noexcept;


			/**
			 * @brief Reads a binary mask and collects the positions containing zero.
			 *
			 * The input file must contain only the characters `0`, `1`, and whitespace.
			 * Both contiguous masks, such as `0101`, and whitespace-separated masks,
			 * such as `0 1 0 1`, are accepted.
			 *
			 * Positions are numbered from zero. The output collection is replaced only
			 * when the complete file has been read successfully.
			 *
			 * @param filename Input filename.
			 * @param interdicted_vertices Output vector receiving the positions whose
			 *                             mask value is zero.
			 * @return `0` on success; `-1` if the file cannot be opened, contains an
			 *         invalid character, or cannot be read completely.
			 *
			 * @note This function provides a non-throwing interface. All exceptions are
			 *       caught internally and reported through the return value.
			 */
			int read_interdicted_vertices(
				const std::string& filename,
				std::vector<int>& interdicted_vertices);


		} // namespace detail
	} // namespace io
}// namespace bitgraph


/////////////////////////////
// Header template implementations

namespace bitgraph {
	namespace io {
		namespace detail {

			template<class CollectionT>
			inline
				int write_vertex_set(
					const std::string& filename,
					const CollectionT& vertices,
					bool one_based) noexcept
			{
				try {

					std::ofstream output{ filename };

					if (!output) {
						LOGG_ERROR(
							"Could not open file: ",
							filename,
							" - write_vertex_set");
						return -1;
					}

					output << "size " << vertices.size() << '\n';

					std::size_t position = 0;

					for (const auto& vertex : vertices) {
						output << (one_based ? vertex + 1 : vertex) << '\n';

						if (!output) {
							LOGG_ERROR(
								"Error writing file: ",
								filename,
								" at element ",
								position,
								" - write_vertex_set");
							return -1;
						}

						++position;
					}

					return output ? 0 : -1;

				}
				catch (const std::exception& error) {
					LOGG_ERROR(
						"Exception while writing file: ",
						filename,
						" - write_vertex_set: ",
						error.what());

					return -1;
				}
				catch (...) {
					LOGG_ERROR(
						"Unknown error while writing file: ",
						filename,
						" - write_vertex_set");

					return -1;
				}

			}

		} // namespace detail
	} // namespace io
}// namespace bitgraph


#endif // BITGRAPH_GRAPH_IO_VERTEX_SET_IO_H