#include "vertex_set_format.h"
#include <cctype>       // for std::isspace


namespace bitgraph {
	namespace io {
		namespace detail {

			int read_interdicted_vertices(
				const std::string& filename,
				std::vector<int>& interdicted_vertices)
			{

                try {
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

                        // Whitespace does not represent a mask position.
                        if (std::isspace(value)) {
                            continue;
                        }

                        if (character == '0') {
                            result.push_back(position);
                        }
                        else if (character != '1') {
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

                    // Commit the result only after the complete file is validated.
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


