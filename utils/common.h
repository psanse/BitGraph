/*
 * @file common.h
 * @brief common utilities for the framework
 * @date ?
 * @last_update 17/01/25
 * @author pss
 *
 */

 //comment in release mode - disable checks
#define	NDEBUG
#ifndef NDEBUG
	#define DEBUG_STACKS		//checks stack sizes (important to debug SAT engine in DEBUG mode)
#endif 


#ifndef __COMMON_H__
#define	__COMMON_H__

#include "logger.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "common_types.h"				//common types additional utilities, to be included e

///////////////////////
// aliases 

using vint = std::vector<int>;
using vdob = std::vector<double>;


namespace com {
	

 namespace dir {
	 /**********************
	   *
	   *	append_slash(...)
	   *
	   *	@comments: appends a slash at the end of the path if no slash is present.
	   *			   The type of slash depends on the SO (Linux '/', Windows '\')
	   *
	   *	@param path: string containing a path to be modified 
	   *
	   **********************/
	 void append_slash(std::string & path);

	 /**********************
	   *
	   *	remove_path(...)
	   *
	   *	@returns: substring after the last slash of the path.
	   *			  If no slash, path is returned
	   *
	   *	@param path: input string
	   *
	   **********************/
	 std::string remove_path(const std::string & path);
  }

   namespace stl {
	   	   
	   /**********************
	   *
	   *	all_equal(...)
	   *
	   *	@returns: true if all elements of a collection
	   *			  are equal
	   *
	   *    @comments: empty collection returns true
	   *
	   **********************/
	   template<class Col_t>
	   inline bool all_equal(const Col_t& col) {
		   return (	std::adjacent_find(	col.cbegin(), col.cend(),
										std::not_equal_to<>()	)		
					== col.cend()									);
	   }

	   /**********************
	   *
	   *	print_collection(...)
	   *
	   *	@returns: stream with all the elements of the collection
	   *
	   *    @comments: not valid for C-arrays 
	   *
	   ***********************/
	   template <class Col_t>
	   inline
	   std::ostream& print_collection(const Col_t& c, std::ostream&  o= std::cout, bool with_endl=false){
		   std::copy(c.cbegin(), c.cend(), std::ostream_iterator<typename Col_t::value_type>(o," " ));
		   o<<" ["<<c.size()<<"]";
		   if(with_endl) o<< std::endl;
		   return o;
	   }
	   	   
		/*inline
	   ostream& print_collection(const vector<vint>& c, ostream&  o = cout) {
		   o << "printing " << c.size() << " elements" << endl;
		   for (int i = 0; i < c.size(); i++) {
			   print_collection<vint>(c[i], o, true);			  
		   }
		   return o;
	   }*/
	   	 
	   /**********************
	   *
	   *	print_collection(...)
	   *
	   *	@returns: stream with the elements inside a range
	   *	@param with_index: flag to include the index of each element	
	   *
	   *    @comments: valid for C-arrays as well
	   *
	   **********************/
	   template <class ForwardIterator>
	   inline
		std::ostream& print_collection(	const ForwardIterator begin, const ForwardIterator end,
										std::ostream&  o = cout,
										bool with_endl = false,
										bool with_index = false											)
	   {
		   int nC = 0;
		   for (auto it = begin; it != end; ++it) {
			   if (with_index) {
				   o << "[" << nC << "]" << *it << " "; nC++;
			   }
			   else {
				   o << *it << " "; nC++;
			   }
		   }
		   o << " [" << nC << "]";
		   if (with_endl) o << endl;
		   return o;
	   }
   }
   	
  /////////////////////////////////////////////

	namespace mat {

		template<typename T>
		inline
			T min3(T x, T y, T z) {
			return std::min<T>(std::min<T>(x, y), z);
		}

		/**
		*  @brief Functor to compute the mean of a collection
		*		   (use a for-each loop)
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
		*		   given its mean (use a for-each loop)
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
	
   namespace fileproc{
		  
	   int READ_EMPTY_LINES(std::fstream& f);
	   int READ_SET_OF_INTERDICTED_NODES(const char* filename, std::vector<int>& interdicted_nodes);

	   template<class Col_t>
	   inline
		   int WRITE_SET_OF_NODES(const char* filename, const Col_t& nodes, bool plus_one = true) {
		   //writes a set of elements to file in the format:
		   //size <SIZE>
		   //elem1
		  //elem2 ...
		   std::ofstream f(filename, std::ofstream::out);
		   if (!f) {
			   LOG_ERROR("Could not open file: " << filename);
			   return -1;
		   }
		   int SIZE = nodes.size();
		   f << "size " << SIZE << endl;
		   for (int i = 0; i < SIZE; i++) {
			   f << ((plus_one) ? nodes[i] + 1 : nodes[i]) << endl;
		   }
		   f.close();
		   return 0;
	   }

   }

    namespace counting{
		inline
		int number_of_words(std::string str){
			int word_count( 0 );
			std::stringstream ss(str);
			std::string word;
			while (ss >> word) { ++word_count; }
			return word_count;

		}
	}
}

//////////////////////////////
//
// Useful FUNCTORS
//
// (simplified with copilot suggestions :-))
//
//////////////////////////////

namespace com {

	//functors which use a global external critera which evaluates to a number
	template<class T, class ColCrit_t, bool Greater, typename Enable = void>
	struct has_val {
		explicit has_val(const ColCrit_t& c) : crit(c) {}

		bool operator()(const T& a, const T& b) const {
			if constexpr (Greater) {
				return crit[a] > crit[b];
			}
			else {
				return crit[a] < crit[b];
			}
		}

		const ColCrit_t& crit;
	};

	// Especialización para punteros
	template<class T, class ColCrit_t, bool Greater>
	struct has_val<T*, ColCrit_t, Greater, typename std::enable_if< std::is_pointer<T>::value>::type > {
		explicit has_val(const ColCrit_t& c) : crit(c) {}

		bool operator()(const T* a, const T* b) const {
			if constexpr (Greater) {
				return crit[*a] > crit[*b];
			}
			else {
				return crit[*a] < crit[*b];
			}
		}

		const ColCrit_t& crit;
	};

	// Para comparaciones de productos
	template<class T, class ColCrit_t, bool Greater>
	struct has_val_prod {
		explicit has_val_prod(const ColCrit_t& c) : crit(c) {}

		bool operator()(const T& a, const T& b) const {
			auto prod_a = crit[a] * a;
			auto prod_b = crit[b] * b;
			if constexpr (Greater) {
				return prod_a > prod_b;
			}
			else {
				return prod_a < prod_b;
			}
		}

		const ColCrit_t& crit;
	};

	// Para comparaciones de diferencias
	template<class T, class ColCrit_t, bool Greater>
	struct has_val_diff {
		explicit has_val_diff(const ColCrit_t& c) : crit(c) {}

		bool operator()(const T& a, const T& b) const {
			auto diff_a = crit[a] - a;
			auto diff_b = crit[b] - b;
			if constexpr (Greater) {
				return diff_a > diff_b;
			}
			else {
				return diff_a < diff_b;
			}
		}

		const ColCrit_t& crit;
	};

	// Alias para simplificar la creación de instancias
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

	   
	//functors which use two (1.main, 2.tiebreak) global external critera which evaluates to a number
	template<class T, class ColCrit_t, bool Greater>
	struct has_val_with_tb {
		explicit has_val_with_tb(const ColCrit_t& ref, const ColCrit_t& tb)
			: crit1(ref), crit2(tb) {}

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

	// Alias para simplificar la creación de estructuras específicas
	template<class T, class ColCrit_t>
	using has_smaller_val_with_tb = has_val_with_tb<T, ColCrit_t, false>;

	template<class T, class ColCrit_t>
	using has_greater_val_with_tb = has_val_with_tb<T, ColCrit_t, true>;


	//functors for collections of elements 
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
namespace com {
	namespace sort {
		enum { FIRST_TO_LAST = 0, LAST_TO_FIRST };		
		enum { NEW_TO_OLD = 0, OLD_TO_NEW };			//format of the ordering - old-to-new in the space of the original graph, new-to-old in the space of the new graph
 		
		/********************************************************************************************************************************
		*
		*		 Sorting of n items (integer) according to non-increasing values of their scores (double)
		*
		********************************************************************************************************************************/
		void SORT_NON_INCR(int *item, double *score, int n);
		
		
		/********************************************************************************************************************************
		*
		*		Sorting of n items (integer) according to non-increasing values of their scores (double)
		*
		********************************************************************************************************************************/
		void SORT_NON_DECR(int *item, double *score, int n);
		
		
		/********************************************************************************************************************************
		*
		*	Insert an element into n-SORTED items (type T) according to non-decreasing values of their scores (type T)
		*	After insertion, there are n sorted items
		*
		*	@returns: the position of insertion
		*
		********************************************************************************************************************************/
		template<class T>
		inline
			int INSERT_ORDERED_SORT_NON_INCR(T* item, T* score, int n, T target, T target_val) {
			/********************************************************************************************************************************/

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

	}
}


//////////////////////////////
//
// RANDOM generation
//
//////////////////////////////
namespace com {
	namespace rand {
		
	   //////////////////
	   //
	   // class RandomUniformGen
	   //
	   // (uses global random engine and dist.)
	   //
	   //////////////////
		
		template<	typename D = std::uniform_int_distribution<int>,
					typename RE = std::mt19937									>	//std::default_random_engine in this machine
			class RandomUniformGen {
			public:
				using result_type = typename D::result_type;
				using dist_type = D;
				using rgen_type = RE;

				constexpr static unsigned int FIXED_RANDOM_SEED = 10000;		//default seed for randomness

				result_type a() { return dist_.param().a(); }
				result_type b() { return dist_.param().b(); }

				static void set_range(result_type min, result_type max) {
					dist_.param(D::param_type{ min, max });
				}

				static void seed(std::size_t seed) {							//external seed
					seed_ = seed;
					re_.seed(seed_);
				}
				static void seed() {											//internal seed- 
					std::random_device rd;

					///////////////////////////////////////
					if (rd.entropy())											//checks if the source of randomness in HW is valid
						seed_ = rd();
					else
						seed_ = static_cast<decltype(seed_)>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
					/////////////////////////////////////////

					re_.seed(seed_);
				}

				result_type operator()() {
					return dist_(re_);
				}

				result_type operator()(result_type min, result_type max) {
					return dist_(re_, D::param_type{ min, max });
				}

				//////////////////////////////////////////////////////////////////
				RandomUniformGen() = default;
				RandomUniformGen(const RandomUniformGen&) = delete;
				RandomUniformGen& operator = (const RandomUniformGen&) = delete;
				//////////////////////////////////////////////////////////////////

			private:
				static RE re_;										//random generator
				static D dist_;										//uniform distribution
				static std::size_t seed_;							//default seed
		};

		////////////////////////////////////////////////////////////////
		template  < typename D = std::uniform_int_distribution<int>,
					typename RE = std::mt19937							>
		using ugen = RandomUniformGen< D, RE >;

		using iugen = RandomUniformGen< std::uniform_int_distribution<int>, std::mt19937 >;
		using rugen = RandomUniformGen< std::uniform_real_distribution<double>, std::mt19937 >;
		////////////////////////////////////////////////////////////////

	
		
		inline
			bool uniform_dist(double p) {
			//returns true with prob p, 0 with 1-p

			//c- windows generator (deprecated)
		   /* double n_01=std::rand()/(double)RAND_MAX;
			return (n_01<=p);*/

			/////////////////////////////
			rugen r;
			return r(0.0, 1.0) <= p;
			////////////////////////////
		}


	}
}



//////////////////////////////
//
// TIME UTILITIES 
//
//////////////////////////////
namespace com {
	namespace time {
		//////////////////////////////////////
		//
		// I/O for durations
		// (shows ratio and ticks)
		//
		/////////////////////////////////////

		template <typename V, typename R>
		inline
			std::ostream& operator << (std::ostream& s, const std::chrono::duration<V, R>& d)
		{
			s << "[" << d.count() << " of "
				<< R::num << "/"
				<< R::den << "]";
			return s;
		}



		//////////////////////////////////////
		//
		// toDouble (duration)
		//
		// returns the duration in seconds 
		//
		/////////////////////////////////////

		template <typename T, typename R>
		inline
			double toDouble(const std::chrono::duration<T, R>& d) noexcept
		{
			return std::chrono::duration_cast<std::chrono::duration<double>>(d).count();
		}

		//////////////////////////////////////
		//
		// tp2string
		//
		// std::chrono::timepoint to string
		//
		/////////////////////////////////////

		template<typename TP_t>
		inline std::string tp2string(const TP_t& tp, bool date = true) {
			std::time_t t = TP_t::clock::to_time_t(tp); // Convert to POSIX system time
			std::stringstream sstr;

			if (date) {
				sstr << std::put_time(std::localtime(&t), "%H:%M:%S --- %d/%b/%Y"); // HH:MM:SS --- DD/Mon/YYYY
			}
			else {
				sstr << std::put_time(std::localtime(&t), "%H:%M:%S"); // HH:MM:SS
			}

			return sstr.str();
		}
			   		 
		//template<typename TP_t>
		//inline
		//	std::string tp2string(const TP_t& tp, bool date = true)
		//{
		//	std::time_t t = TP_t::clock::to_time_t(tp);				// convert to POSIX system time
		//	std::string tstr;



		//	if (date) {
		//		auto formattedTimeLocal = std::put_time(std::localtime(&t), "%H:%M:%S --- %d/%b/%Y");		// HH:MM:SS - 01/Nov/2024 localtime
		//		//auto formattedTimeUtc = std::put_time(std::gmtime(&t)	,	"%H:%M:%S");					// HH:MM:SS - - 01/Nov/2024 UTC (universal time)

		//		std::stringstream sstr;
		//		sstr << formattedTimeLocal << std::endl;
		//		tstr = sstr.str();

		//		//////////////////////////////////////////////////////////////////
		//		// Deprecated POSIX style code

		//		//tstr = std::ctime(&t);									// convert to calendar time (takes local time zone into account)
		//		//std::string ts = std::asctime(gmtime(&t));				// conversion to UTC (universal)	
		//		//tstr.resize(tstr.size() - 1);								// skip trailing newline
		//		///////////////////////////////////////////////////////////////////
		//	}
		//	else {
		//		auto formattedTimeLocal = std::put_time(std::localtime(&t), "%H:%M:%S");		// HH:MM:SS localtime
		//		//auto formattedTimeUtc = std::put_time(std::gmtime(&t), "%H:%M:%S");			// HH:MM:SS UTC (universal)

		//		std::stringstream sstr;
		//		sstr << formattedTimeLocal << std::endl;
		//		tstr = sstr.str();
		//	}

		//	return tstr;
		//}

		//////////////////////////////////////
		//
		// makeTimePoint(...)
		// 
		// converts calendar time to timepoint of system clock
		//
		/////////////////////////////////////
		std::chrono::system_clock::time_point
		makeTimePoint(	int year, int mon, int day,
						int hour, int min, int sec = 0		);
	
	}
}


#endif