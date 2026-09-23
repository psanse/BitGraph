/**
 * @file path_utils.cpp
 * @brief Implements filesystem path-string utilities.
 *
 * This translation unit implements lightweight, platform-aware operations for
 * appending directory separators and extracting filename components from path
 * strings.
 *
 * Existing paths may use either forward slashes or backslashes. When a
 * separator must be appended, the platform-preferred separator is used.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */
#include "path_utils.h"


namespace bitgraph {

    namespace utils {


		/**
		 * @brief Ensures that a path ends with a directory separator.
		 *
		 * If @p path already ends in `/` or `\`, it remains unchanged. Otherwise,
		 * the platform-preferred separator is appended: `\` on Windows and `/` on
		 * other supported operating systems.
		 *
		 * An empty path remains unchanged.
		 *
		 * @param path Path string to modify.
		 */
		void append_slash(std::string& path)
		{
			if (path.empty()) {
				return;
			}

			const char last_character = path.back();

			// Accept either separator in an existing path.
			if (last_character == '/' || last_character == '\\') {
				return;
			}

#ifdef _WIN32
			path.push_back('\\');
#else
			path.push_back('/');
#endif
		}


		/**
		 * @brief Extracts the filename component from a path.
		 *
		 * Both `/` and `\` are recognized as directory separators. If @p path does
		 * not contain a separator, the complete input string is returned. If the path
		 * ends in a separator, an empty string is returned.
		 *
		 * @param path Input path string.
		 * @return Filename component following the final directory separator.
		 */
		std::string remove_path(const std::string& path)
		{
			const std::string::size_type separator =
				path.find_last_of("/\\");

			return separator == std::string::npos
				? path
				: path.substr(separator + 1);
		}
		
	
		

	} // namespace utils

} // namespace bitgraph




	