/**
 * @file common_types.h  
 * @brief set of useful types 
 * @date ?
 * @last_update 29/01/2026
 * @author pss
 **/

#ifndef BITGRAPH_UTILS_COMMON_TYPES_H
#define	BITGRAPH_UTILS_COMMON_TYPES_H

#include <iostream>
#include <vector>
#include <set>
#include <memory>
#include <cassert>


namespace bitgraph {

	namespace utils {

		namespace detail {

			////////////////////////
			//
			// struct IntRange
			//
			// Interval of positive integers - typically a range of vertices
			// 
			// TODO - under development (17/12/2025)
			///////////////////////

			struct IntRange {
				static constexpr int no_range = -1;

				int vl = no_range;		//lower bound
				int vh = no_range;		//upper bound

				constexpr IntRange(int lh = no_range, int rh = no_range) noexcept :
					vl{ lh }, vh{ rh }
				{}

				// basic operations

				constexpr bool is_unset() const noexcept { return (vl == no_range || vh == no_range); }
				constexpr bool is_set()   const noexcept { return !is_unset(); }
				constexpr bool is_proper() const noexcept { return is_set() && (vl <= vh) && (vl >= 0); }

				// I/O

				/**
				* @brief: streams the range in the format [@vl, @vh]
				**/
				std::ostream& print(std::ostream& os = std::cout) const {
					os << "[" << vl << "," << vh << "]";
					return os;
				}

				friend std::ostream& operator<<(std::ostream& os, const IntRange& r) {
					return os << "[" << r.vl << "," << r.vh << "]";
				}

				friend constexpr bool operator == (const IntRange& lhs, const IntRange& rhs) {
					return (lhs.vl == rhs.vl && lhs.vh == rhs.vh);
				}
			};


		}//end namespace detail

	} //end namespace utils

}//end namespace bitgraph


#endif  // BITGRAPH_UTILS_COMMON_TYPES_H