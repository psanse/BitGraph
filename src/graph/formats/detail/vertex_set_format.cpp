/**
 * @file vertex_set_format.cpp
 * @brief Implementation of vertex-set mask file parsing utilities.
 *
 * A vertex mask contains one character per vertex: `0` identifies an
 * interdicted vertex and `1` identifies an unrestricted vertex. Whitespace
 * may be used as a separator and is ignored by the parser.
 */

#include "vertex_set_format.h"
#include <cctype>       // for std::isspace


namespace bitgraph {
	namespace io {
		namespace detail {

			/**
			 * @brief Reads interdicted vertex indices from a mask file.
			 *
			 * The function validates the complete input before replacing the
			 * contents of @p interdicted_vertices. Consequently, a read or format
			 * error leaves the caller's existing vector unchanged.
			 *
			 * @param[in] filename Path to the vertex mask file.
			 * @param[out] interdicted_vertices Receives the zero-based indices of
			 *                                  vertices represented by `0`.
			 * @return `0` when the file is read successfully; `-1` when the file
			 *         cannot be opened, contains an invalid character, or cannot be
			 *         read completely.
			 */
			int read_interdicted_vertices(
				const std::string& filename,
				std::vector<int>& interdicted_vertices)
			{
				// The input file is a vertex mask: a '0' marks an interdicted
				// vertex, while a '1' leaves the vertex unrestricted. Whitespace
				// is accepted so that masks can be formatted across multiple lines.

                try {
                    // Open in binary mode so that the parser handles every
                    // platform's line-ending bytes consistently; whitespace
                    // is filtered explicitly below.
                    std::ifstream input{
                        filename,
                        std::ios::binary | std::ios::in
                    };

                    if (!input) {
                        LOGG_ERROR(
                            "File could not be opened: ",
                            filename,
                            " - read_interdicted_vertices");
                        return -1;
                    }

                    std::vector<int> result;
                    int position = 0;
                    char character = '\0';

                    while (input.get(character)) {
                        const unsigned char value =
                            static_cast<unsigned char>(character);

                        // Separators do not represent vertices and therefore
                        // must not increment the mask position.
                        if (std::isspace(value)) {
                            continue;
                        }

                        // Store the zero-based position of each interdicted
                        // vertex. A '1' is valid but requires no output entry.
                        if (character == '0') {
                            result.push_back(position);
                        }
                        else if (character != '1') {
                            // Reject malformed input instead of silently
                            // producing a partial or ambiguous vertex set.
                            LOGG_ERROR(
                                "Invalid character in interdiction mask file: ",
                                filename,
                                " at mask position ",
                                position,
                                " - read_interdicted_vertices");
                            return -1;
                        }

                        ++position;
                    }

                    if (input.bad()) {
                        LOGG_ERROR(
                            "Error while reading file: ",
                            filename,
                            " - read_interdicted_vertices");
                        return -1;
                    }

                    // Commit only after the complete file has been validated,
                    // preserving the caller's existing vector on failure.
                    interdicted_vertices.swap(result);

                    return 0;
                }
                catch (const std::exception& error) {
                    LOGG_ERROR(
                        "Exception while reading file: ",
                        filename,
                        " - read_interdicted_vertices: ",
                        error.what());
                    return -1;
                }
                catch (...) {
                    LOGG_ERROR(
                        "Unknown error while reading file: ",
                        filename,
                        " - read_interdicted_vertices");
                    return -1;
                }


			}

		} // namespace detail
	} // namespace io
}// namespace bitgraph


