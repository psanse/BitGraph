/**
 * @file common.h
 * @brief common utilities for the framework
 * @date ?
 * @last_update 17/01/25
 * @dev: main pss, others ff
 **/

#ifndef BITGRAPH_UTILS_COMMON_H
#define	BITGRAPH_UTILS_COMMON_H


 //comment in release mode - disable checks
 //current impact on stack_t in common_types.h

#ifndef NDEBUG
//#define DEBUG_STACKS		//checks stack sizes (important to debug SAT engine in DEBUG mode) TODO - legacy code, remove (06/10/2025)
#endif 

#include "logger.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <regex>
#include <unordered_set>				//for all_unique

#include "common_types.h"				//common types additional utilities, to be included e
#include "random_utils.h"
#include "prec_timer.h"

namespace bitgraph {

	namespace utils {

		

		

		

		namespace _file {

			/**
			* @brief writes a set of vertices in a collection to file in the format:
			*		size <SIZE>
			*		elem1
			*		elem2 ...
			* @param filename: name of the output file
			* @param nodes: collection of elements to be written
			* @param plus_one: flag to add 1 to each element before writing
			* @returns 0 if success, -1 if error (non-throwing interface)
			**/
			template<class Col_t>
			inline
				int WRITE_SET_OF_VERTICES(const char* filename, const Col_t& nodes, bool plus_one = true) noexcept
			{
				std::ofstream f(filename, std::ofstream::out);
				if (!f) {
					LOGG_ERROR("Could not open file: ", filename, "_file::WRITE_SET_OF_VERTICES");
					return -1;
				}

				auto SIZE = nodes.size();
				f << "size " << SIZE << std::endl;
				for (auto i = 0; i < SIZE; ++i) {
					if (!(f << ((plus_one) ? nodes[i] + 1 : nodes[i]) << std::endl)) {
						LOGG_ERROR("Error writing to file: ", filename, " at element: ", i, "_file::WRITE_SET_OF_VERTICES");
						f.close();
						return -1;
					}					
				}

				f.close();
				return 0;
			}

			/**
			 * @brief reads a mask of 0s and 1s from a file and provides the position of the 0s
			 * @param interdicted_nodes: output vector of integers to store the positions of the 0s
			 * @returns 0 if success, -1 if error (non-throwing interface)
			 **/
			int READ_SET_OF_INTERDICTED_VERTICES(const char* filename, std::vector<int>& interdicted_nodes) noexcept;

		}

		
	}

	//////////////////////////////
	//
	// FUNCTORS for sorting
	// (TODO substitute by lambdas inside the code whenever possible - 18/01/2025)
	//
	//////////////////////////////

	namespace utils {

		/**
		* @brief Functor to sort a collection of elements by a criterion
		*
		*		 I. Enable type is for SFINAE 
		*
		**/
		template<class T, class ColCrit_t, bool Greater, typename Enable = void >
		struct has_val {
			explicit has_val(const ColCrit_t& c) : crit(c) {}

			bool operator()(const T& a, const T& b) const {
				if (Greater) {
					return crit[a] > crit[b];
				}
				else {
					return crit[a] < crit[b];
				}
			}

			const ColCrit_t& crit;
		};

		/**
		* @brief functor to sort a collection of pointers by a criterion
		*		 (specialization)
		**/
		template<class T, class ColCrit_t, bool Greater >
		struct has_val<T*, ColCrit_t, Greater, typename std::enable_if< std::is_pointer<T>::value>::type > {
			explicit has_val(const ColCrit_t& c) : crit(c) {}

			bool operator()(const T* a, const T* b) const {
				if (Greater) {
					return crit[*a] > crit[*b];
				}
				else {
					return crit[*a] < crit[*b];
				}
			}

			const ColCrit_t& crit;
		};

		/**
		* @brief functor to sort a collection according to a product of value and criterion
		* @details: if contexpr (C++17) removed for C++11 compatibility
		*
		**/
		template<class T, class ColCrit_t, bool Greater>
		struct has_val_prod {
			explicit has_val_prod(const ColCrit_t& c) : crit(c) {}

			bool operator()(const T& a, const T& b) const {
				auto prod_a = crit[a] * a;
				auto prod_b = crit[b] * b;
				if (Greater) {							 //if contexpr in C++17 removed for C++11 compatibility
					return prod_a > prod_b;
				}
				else {
					return prod_a < prod_b;
				}
			}

			const ColCrit_t& crit;
		};

		/**
		* @brief functor to sort a collection according to a difference of value and criterion
		*
		**/
		template<class T, class ColCrit_t, bool Greater>
		struct has_val_diff {
			explicit has_val_diff(const ColCrit_t& c) : crit(c) {}

			bool operator()(const T& a, const T& b) const {
				auto diff_a = crit[a] - a;
				auto diff_b = crit[b] - b;
				if (Greater) {						 //if contexpr in C++17 is the point - removed for C++11 compatibility
					return diff_a > diff_b;
				}
				else {
					return diff_a < diff_b;
				}
			}

			const ColCrit_t& crit;
		};

		//useful aliases binders
		template<class T, class ColCrit_t>
		using has_greater_val = has_val<T, ColCrit_t, true>;

		template<class T, class ColCrit_t>
		using has_smaller_val = has_val<T, ColCrit_t, false>;

		template<class T, class ColCrit_t>
		using has_greater_val_prod = has_val_prod<T, ColCrit_t, true>;

		template<class T, class ColCrit_t>
		using has_smaller_val_prod = has_val_prod<T, ColCrit_t, false>;

		template<class T, class ColCrit_t>
		using has_greater_val_diff = has_val_diff<T, ColCrit_t, true>;

		template<class T, class ColCrit_t>
		using has_smaller_val_diff = has_val_diff<T, ColCrit_t, false>;


		/**
		* @brief functor to sort a collection according to two criteria
		*		  (1.main, 2.tiebreak)
		*
		**/
		template<class T, class ColCrit_t, bool Greater>
		struct has_val_with_tb {
			explicit has_val_with_tb(const ColCrit_t& ref, const ColCrit_t& tb)
				: crit1(ref), crit2(tb) {
			}

			bool operator()(const T& a, const T& b) const {
				// Compara usando crit1 primero
				if (Greater ? crit1[a] > crit1[b] : crit1[a] < crit1[b]) {
					return true;
				}
				if (Greater ? crit1[a] < crit1[b] : crit1[a] > crit1[b]) {
					return false;
				}
				// Tiebreak con crit2
				return Greater ? crit2[a] > crit2[b] : crit2[a] < crit2[b];
			}

			const ColCrit_t& crit1;
			const ColCrit_t& crit2;  // Tiebreak
		};

		//useful aliases binders
		template<class T, class ColCrit_t>
		using has_smaller_val_with_tb = has_val_with_tb<T, ColCrit_t, false>;

		template<class T, class ColCrit_t>
		using has_greater_val_with_tb = has_val_with_tb<T, ColCrit_t, true>;


		//functors for sorting sets of collections 
		template< typename Col_t >
		struct has_bigger_size {
			bool operator()(const Col_t& lhs, const Col_t& rhs) const { return lhs.size() > rhs.size(); }
		};

		template<typename Col_t	>
		struct has_smaller_size {
			bool operator()(const Col_t& lhs, const Col_t& rhs) const { return lhs.size() < rhs.size(); }
		};		

	}
	////////////////////////////////////////////


	//////////////////////////////
	//
	// C-Sorting procedures
	//
	// @dev: Fabio Furini (20/07/17)
	//
	//////////////////////////////
	namespace utils {
		namespace _sort {
	
			/**
			* @brief Sorting of n items (integer) according to non-increasing values of their scores (double)
			*
			**/
			void SORT_NON_INCR(int* item, double* score, int n);


			/**
			* @brief Sorting of n items (integer) according to non-decreasing values of their scores (double)
			*
			**/
			void SORT_NON_DECR(int* item, double* score, int n);


			/**
			* @brief Inserts an element into n-SORTED items (type T) according to non-decreasing values of their scores (type T)
			*		 After insertion, there are n sorted items
			* @returns: the position of insertion
			*
			**/
			template<class T>
			inline
				int INSERT_ORDERED_SORT_NON_INCR(T* item, T* score, int n, T target, T target_val) {

				int i = n - 1;
				for (; i >= 1; i--) {
					// if( target_val>score[item[i-1]] ){
					if (target_val > score[i - 1]) {
						item[i] = item[i - 1];
						//score[item[i]]=score[item[i-1]];	
						score[i] = score[i - 1];
					}
					else break;
				}
				item[i] = target;				/* note target can be written in item[n-1] */
				//score[item[i]]=target_val;
				score[i] = target_val;
				return i;
			}

			/*
			* @brief fills Collection with numbers [0..NV-1] - similar to std::iota
			*/
			template<typename Col_t>
			void fill_vertices(Col_t& lv, std::size_t NV) {
				lv.clear();
				lv.reserve(NV);
				for (int i = 0; i < NV; i++) {
					lv.emplace_back(i);
				}
			}

		} // end namespace _sort

	} // end namespace utils
				
		
	using namespace utils::_sort;
	

}//end namespace bitgraph


#endif // BITGRAPH_UTILS_COMMON_H