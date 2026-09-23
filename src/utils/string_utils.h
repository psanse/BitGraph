/**
 * @file string_utils.h
 * @brief String inspection and token-counting utilities.
 *
 * This header provides lightweight functions for analyzing textual data,
 * including counting word-like sequences in strings.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */
#ifndef BITGRAPH_UTILS_STRING_UTILS_H
#define BITGRAPH_UTILS_STRING_UTILS_H


#include <cstddef>
#include <regex>
#include <string>

namespace bitgraph {

    namespace utils {
		    		

		/**
		 * @brief Counts word-like sequences in a string.
		 *
		 * A word is defined by the regular expression `\b\w+\b`. Consequently,
		 * sequences containing letters, digits, or underscores are counted as words.
		 *
		 * @param text Input string.
		 * @return Number of word-like sequences found in @p text.
		 *
		 * @note The interpretation of `\w` depends on the regular-expression
		 *       implementation and locale and may not provide full Unicode support.
		 */
		inline
			int number_of_words(const std::string& text)
		{

			static const std::regex word_expression{ R"(\b\w+\b)" };

			return static_cast<std::size_t>(
				std::distance(
					std::sregex_iterator{
						text.begin(), text.end(), word_expression },
					std::sregex_iterator{})	);
		
		}		
		

	} // namespace utils

} // namespace bitgraph

#endif // BITGRAPH_UTILS_STRING_UTILS_H		