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
#include "random.h"
#include "prec_timer.h"

namespace bitgraph {

	namespace utils {

		namespace _dir {
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
		}

		namespace _stl {

			/**
			*
			* @brief Returns true if all elements of a collection are equal
			*        or the collection is empty
			*
			**/
			template<class Col_t>
			inline bool all_equal(const Col_t& col) {
				return (std::adjacent_find(col.cbegin(), col.cend(),
					std::not_equal_to<typename Col_t::value_type>())			//std::not_equal_to<> is more modern C++
					== col.cend());
			}

			/**
			* @brief checks if an std collection contains unique elements
			* @details: adds the elements to an unordered_set and compares sizes
			**/
			template <class Col_t>
			inline
				bool all_unique(const Col_t& v) {
				std::unordered_set<typename Col_t::value_type> s(v.begin(), v.end());
				return s.size() == v.size();
			}

			/**
			* @brief  Streams all the elements of an STL collection
			*
			* @param c: input collection
			* @param	o: output stream
			* @param	eofl: flag to include a new line at the end of the stream
			* @returns: stream with all the elements of the collection
			**/
			template <class Col_t>
			inline
				std::ostream& print_collection(const Col_t& c, std::ostream& o = std::cout, bool eofl = false)
			{
				std::copy(c.cbegin(), c.cend(), std::ostream_iterator<typename Col_t::value_type>(o, " "));
				o << " [" << c.size() << "]";

				if (eofl) { o << std::endl; }
				return o;
			}

			/**
			 * @brief  Streams all the elements of an STL collection inside a range
			 *
			 * @param begin, end: iterators to the beginning and end of the collection
			 * @param o: output stream
			 * @param with_endl: flag to include a new line at the end of the stream
			 * @returns: stream with all the elements of the collection
			 **/
			template <class ForwardIterator>
			inline
				std::ostream& print_collection(const ForwardIterator begin, const ForwardIterator end,
					std::ostream& o = std::cout,
					bool eofl = false,
					bool index = false)
			{
				int nC = 0;
				for (auto it = begin; it != end; ++it) {
					if (index) {
						o << "[" << nC << "]" << *it << " "; nC++;
					}
					else {
						o << *it << " "; nC++;
					}
				}
				o << " [" << nC << "]";
				if (eofl) o << std::endl;
				return o;
			}
						
		}

		/////////////////////////////////////////////

		namespace _mat {

			template<typename T>
			inline
				T min3(T x, T y, T z) {
				return std::min<T>(std::min<T>(x, y), z);
			}

			/**
			*  @brief Functor to compute the mean of a collection
			*		   (use in a for-each loop)
			**/
			class MeanValue {
			private:
				std::size_t num = 0;		// number of elements
				double sum = 0;				// sum of all element values
			public:

				void operator() (double elem) {
					num++;
					sum += elem;
				}

				// return mean value (implicit cast)
				operator double() {
					return sum / static_cast<double>(num);
				}
			};

			/**
			*  @brief Functor to compute the standard deviation of a collection
			*		   given its mean (use in a for-each loop)
			**/
			class StdDevValue {
			private:
				const double MEAN;					//given mean of the collection
				std::size_t num = 0;				//number of elements
				double sumSqrDiff = 0;
			public:
				StdDevValue(double mean_out) : MEAN(mean_out) {}

				void operator() (double elem) {
					num++;
					sumSqrDiff += (MEAN - elem) * (MEAN - elem);
				}

				// return std diff value (implicit cast)
				operator double() {
					return std::sqrt(sumSqrDiff / static_cast<double>(num));
				}
			};

		}
		//////////////////////

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

		namespace _count {

			/**
			* @brief counts the number of words in a string
			**/
			inline
				int number_of_words(const std::string& str)
			{
				static const std::regex word_regex(R"(\b\w+\b)");
				return (int)std::distance(
					std::sregex_iterator(	str.begin(), str.end(), word_regex),
											std::sregex_iterator()				);

				//OLD CODE
				/*auto word_count = 0;
				std::stringstream sstr(str);
				std::string word;
				while (sstr >> word) {
					++word_count;
				}

				return word_count;*/
			}
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

		//scale functor
		template< typename T >
		struct scale {
			scale(T s) : scale_(s) {}
			T operator()(T x) const { return x * scale_; }
			T scale_;
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
				

	// for backward compatibility
	using namespace utils::_stl;
	//using namespace utils::_time;
	using namespace utils::_sort;
	//using namespace utils::_rand;	
	using namespace utils::_dir;


}//end namespace bitgraph


#endif // BITGRAPH_UTILS_COMMON_H