/**
 * @file path_utils.h
 * @brief Filesystem path-string manipulation utilities.
 *
 * This header provides lightweight functions for manipulating path strings,
 * including appending platform-preferred directory separators and extracting
 * filename components.
 *
 * Both forward slashes and backslashes are recognized when analyzing existing
 * paths, allowing paths originating from different operating systems to be
 * processed consistently.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */


#ifndef BITGRAPH_UTILS_PATH_UTILS_H
#define BITGRAPH_UTILS_PATH_UTILS_H


#include <string>

namespace bitgraph {

    namespace utils {
		    		
			/**
			  * @brief Appends a slash at the end of the path if no slash is present.
			  *		   The type of slash depends on the SO (Linux '/', Windows '\')
			  * @param path: string containing a path to be modified
			  *
			  **/
			void append_slash(std::string& path);

			/**
			  * @brief	removes the path from a filename
			  * @param path: input string
			  *
			  * @returns: substring after the last slash of the filename.
			  *		   (if no slash the filename is returned)
			  **/
			std::string remove_path(const std::string& path);
		
			

	} // namespace utils

} // namespace bitgraph



#endif // BITGRAPH_UTILS_PATH_UTILS_H		