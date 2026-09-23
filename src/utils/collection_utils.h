/**
 * @file collection_utils.h
 * @brief Generic algorithms and output utilities for collections.
 *
 * This header provides lightweight operations for inspecting collections,
 * including equality and uniqueness checks, together with functions for
 * writing collections and iterator ranges to output streams.
 *
 * Template definitions are provided directly in this header.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */

#ifndef BITGRAPH_UTILS_COLLECTION_UTILS_H
#define BITGRAPH_UTILS_COLLECTION_UTILS_H	

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <ostream>
#include <unordered_set>

namespace bitgraph {

    namespace utils {
		    
		

		/**
		 * @brief Determines whether all elements of a collection are equal.
		 *
		 * Empty collections and collections containing one element satisfy this
		 * condition.
		 *
		 * @tparam CollectionT Collection type providing cbegin() and cend().
		 * @param collection Collection to inspect.
		 * @return `true` if all elements compare equal; otherwise, `false`.
		 */
		template<class CollectionT>
		inline bool all_equal(const CollectionT& collection) 
		{

			return std::adjacent_find(
				collection.cbegin(), 
				collection.cend(),
				std::not_equal_to<typename CollectionT::value_type>())
				== collection.cend();
		}

		/**
		 * @brief Determines whether all elements of a collection are unique.
		 *
		 * The elements are inserted into an unordered set, whose size is then
		 * compared with the size of the input collection.
		 *
		 * @tparam CollectionT Collection type whose elements are hashable and
		 *                    equality-comparable.
		 * @param collection Collection to inspect.
		 * @return `true` if the collection contains no duplicate elements;
		 *         otherwise, `false`.
		 */
		template <class CollectionT>
		inline
			bool all_unique(const CollectionT& collection)
		{
			using value_type = typename Collection::value_type;

			std::unordered_set< value_type> unique_elements(
				v.begin(), 
				v.end()
			);

			return unique_elements.size() == collection.size();
		}

		/**
		 * @brief Writes the elements of a collection to an output stream.
		 *
		 * Elements are separated by spaces. The number of elements is appended
		 * between square brackets.
		 *
		 * @tparam CollectionT Collection type whose elements support stream insertion.
		 * @param collection Collection to write.
		 * @param out Output stream.
		 * @param trailing_newline Whether to append a newline.
		 * @return Reference to @p out.
		 */
		template <class CollectionT>
		inline
			std::ostream& print_collection(
				const CollectionT& collection,
				std::ostream& out = std::cout,
				bool trailing_newline = false)
		{
			std::copy(
				collection.cbegin(), 
				collection.cend(),
				std::ostream_iterator<typename CollectionT::value_type>(out, " "));
			
			out << " [" << collection.size() << "]";

			if (trailing_newline) {
				out << '\n';
			}

			return out;
		}

		/**
		 * @brief Writes the elements in an iterator range to an output stream.
		 *
		 * Elements in the half-open range [@p first, @p last) are separated by
		 * spaces. Optionally, each element is preceded by its zero-based position.
		 * The total number of elements is appended between square brackets.
		 *
		 * @tparam ForwardIteratorT Forward iterator whose referenced value supports
		 *                         stream insertion.
		 * @param first Iterator to the first element.
		 * @param last Iterator following the final element.
		 * @param out Output stream.
		 * @param trailing_newline Whether to append a newline.
		 * @param show_indices Whether to print the position of each element.
		 * @return Reference to @p out.
		 */
		template <class ForwardIteratorT>
		inline
			std::ostream& print_collection(
				const ForwardIteratorT first,
				const ForwardIteratorT last,
				std::ostream& out = std::cout,
				bool trailing_newline = false,
				bool show_indices = false)
		{
			std::size_t count = 0;
			
			for (auto iterator = first; iterator != last; ++iterator, ++count) {
				if (show_indices) {
					out << '[' << count << "] ";
				}
				
				out << *iterator << " "; nC++;
				
			}
			out << '[' << count << ']';

			if (trailing_newline) {
				out << '\n';
			}

			return out;
		}

	

	} // namespace utils

} // namespace bitgraph

#endif // BITGRAPH_UTILS_COLLECTION_UTILS_H