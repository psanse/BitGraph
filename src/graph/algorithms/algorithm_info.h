/**
 *@file algorithm_info.h
 *@details: created 12/12/2024, last_update 12/09/2025
 *@author pss
 **/

 /**
  * @file algorithm_info.h
  * @brief Common configuration, timing, and reporting infrastructure for graph algorithms.
  *
  * Defines the base parameter and information classes shared by graph,
  * clique, and clique-based algorithms. Algorithm-specific information classes
  * may derive from AlgorithmInfo to add counters, solution data, and custom
  * reporting behavior.
  *
  * Generic time measurement and conversion facilities are provided by the
  * UTILS library.
  *
  * @author Pablo San Segundo
  * @date created 12/12/2024 (and moved to this location 24/09/2026)
  * @date last_update 24/09/2026
  */

#ifndef BITGRAPH_GRAPH_ALGORITHMS_ALGORITHM_INFO_H
#define BITGRAPH_GRAPH_ALGORITHMS_ALGORITHM_INFO_H


#include "utils/prec_timer.h"
#include "utils/time_utils.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>
#include <string>
#include <utility>


namespace bitgraph {

		/**
		* @brief Common configuration parameters for graph algorithms.
		*
		* Stores instance information and execution settings shared by graph,
		* clique, and clique-based algorithms. Derived parameter classes may add
		* algorithm-specific configuration fields and override reset() and print().
		*/
		struct AlgorithmParameters {

			using time_point_type = PrecisionTimer::timepoint_t;
			using clock_type = PrecisionTimer::clock_t;
			using time_point_type = clock_type::time_point;

			std::string name;                ///< Instance name.
			std::size_t N = 0;				 ///< Number of graph vertices.
			std::size_t M = 0;				///< Number of graph edges.

			/// Maximum execution time, in seconds.
			double time_out = std::numeric_limits<double>::max();

			/// Maximum heuristic execution time, in seconds.
			double heuristic_time_out = std::numeric_limits<double>::max();
			
			int num_threads = 1;										
			
			/**
		   * @brief Legacy loop-unrolling option.
		   * @deprecated Retained for compatibility with existing algorithms.
		   */
			bool unrolled = false;											

			time_point_type start_time{};    ///< Starting point of the auxiliary timer.
			double elapsed_time = 0.0;       ///< Last measured auxiliary duration.							


			AlgorithmParameters() = default;
			virtual ~AlgorithmParameters() = default;

			AlgorithmParameters(const AlgorithmParameters&) = default;
			AlgorithmParameters& operator=(const AlgorithmParameters&) = default;
			AlgorithmParameters(AlgorithmParameters&&) noexcept = default;
			AlgorithmParameters& operator=(AlgorithmParameters&&) noexcept = default;

			
			/**
			 * @brief Starts the auxiliary parameter timer.
			 */
			void start_timer() noexcept
			{
				start_time = clock_type::now();
			}

			/**
			* @brief Reads the auxiliary parameter timer.
			*
			* Updates elapsed_time with the time elapsed since start_timer() was
			* called.
			*
			* @return Elapsed time in seconds.
			*/
			double read_timer() noexcept
			{
				elapsed_time = utils::elapsed_time(start_time);
				return elapsed_time;				
			}						

			/**
			* @brief Restores all common parameters to their default values.
			*
			* Derived implementations should reset their additional fields and invoke
			* AlgorithmParameters::reset().
			*/
			virtual void reset();

			/**
			 * @brief Writes the common algorithm parameters.
			 *
			 * Derived implementations may extend the output after invoking
			 * AlgorithmParameters::print().
			 *
			 * @param out Destination stream.
			 * @param trailing_newline Whether to append a newline.
			 * @return Reference to @p out.
			 */
			virtual std::ostream& print(
				std::ostream& out = std::cout,
				bool trailing_newline = true) const;		
		
		};  // end struct AlgorithmParameters

	

		// for backwards compatibility with previous code
		using paramBase = AlgorithmParameters;


		//////////////////////
		//
		//	infoBase
		// 
		//  @brief base struct to report results of graph algorithms.
		// 
		//  Supports basic configuration parameters and timers.
		//  
		//  TODO- conceived as a struct initially (all data members are public), 
		//  added getters/setters later, possibly convert to a CLASS (31/08/2025)
		//
		///////////////////////
		template <class ParametersT = AlgorithmParameters>
		class BasicAlgorithmInfo {
			static_assert(
				std::is_base_of<AlgorithmParameters, ParametersT>::value,
				"ParametersT must derive from AlgorithmParameters - BasicAlgorithmInfo ");
		public:
			using parameters_type = ParametersT;
			using time_point_type = PrecisionTimer::timepoint_t;
			using clock_type = PrecisionTimer::clock_t;
			using time_point_type = clock_type::time_point;
					
			enum class phase_t { SEARCH = 0, PREPROC, LAST_INCUMBENT, PARSE };
			enum class report_t { VERBOSE = 0, TABLE = 1 };
									

			///////////////////////
			//constructors / destructor

			BasicAlgorithmInfo() = default;
			explicit BasicAlgorithmInfo(const parameters_type& parameters)
				: data_(parameters)
			{}

			virtual ~BasicAlgorithmInfo() = default;

			/////////////////////
			// setters / getters

			const parameters_type& parameters() const noexcept
			{
				return parameters_;
			}
						
			const std::string& name() const noexcept { return data_.name; }
			std::size_t num_vertices() const noexcept { return data_.N; }
			std::size_t num_edges() const noexcept { return data_.M; };
			double time_out() const noexcept { return data_.time_out; };
			double time_out_heur() const noexcept { return data_.heuristic_time_out; }
			int number_of_threads() const  noexcept { return data_.num_threads; }
			std::size_t recursion_calls_per_tout_check() const noexcept { return numStepsTimeOutCheck; }

			double parsing_time() const noexcept { return data_.elapsed_time; }
			double preprocessing_time() const  noexcept { return timePreproc_; }
			double search_time() const noexcept { return timeSearch_; }
			double incumbent_time() const  noexcept { return timeIncumbent_; }

			//////////////////////
			//setters - only for general info, timers should not be set manually

			void name(std::string name) noexcept { data_.name = std::move(name); }
			void num_vertices(std::size_t N)  noexcept { data_.N = N; }
			void num_edges(std::size_t m) noexcept { data_.M = m; }
			void time_out(double t)  noexcept { (t == -1) ? data_.time_out = std::numeric_limits<double>::max() : data_.time_out = t; }
			void time_out_heur(double t) noexcept { (t == -1) ? data_.heuristic_time_out = std::numeric_limits<double>::max() : data_.heuristic_time_out = t; }
			void number_of_threads(int n) noexcept { data_.num_threads = n; }
			void recursion_calls_per_tout_check(uint32_t n) noexcept { numStepsTimeOutCheck = n; }

			//timers
			/*
			* @brief sets initial time in timer @t (previously set with startTimer(...))
			* @param t - phase_t enum
			*/
			void startTimer(phase_t t);

			/*
			* @brief reads time in timer @t (previously set with startTimer(...))
			* @param t - phase_t enum
			*/
			double readTimer(phase_t t);

			/*
			* @brief clears appropiate time duration (concerning phase_t @t)
			*/
			void clearTimer(phase_t t);

			/**
			* @brief clears all timers
			**/
			void clearAllTimers();

			/*
			* @brief resets to default values
			* @param lazy - if true general info is NOT cleared, only timers
			*/
			virtual void clear(bool lazy = false);

		protected:
			/**
			* @brief clears general info - virtual since derived classes might have more general info to clear
			**/
			virtual void clearGeneralInfo() { data_.reset(); }


			//I/O
		public:
			friend std::ostream& operator<<	(std::ostream&, const infoBase&);

			/*
			* @brief streams all info
			* @param o: output stream
			* @param is_endl: if true adds endl at the end
			* @returns output stream
			*
			* TODO Add @K_ to ouput conditionally
			*/
			virtual std::ostream& printReport(std::ostream& o = std::cout, report_t r = report_t::TABLE, bool is_endl = false) const;

			/*
			* @brief streams gereral info
			* @param o: output stream
			* @returns output stream
			*/
			virtual	std::ostream& printParams(std::ostream& o = std::cout) const;

			/*
			* @brief streams timer info
			* @param o output stream
			* @returns output stream
			*/
			std::ostream& printTimers(std::ostream& o = std::cout) const;

		protected:

			parameters_type data_;						// general metadata and configuration parameters

			// timers			
			time_point_type startTimePreproc_;
			double timePreproc_ = 0;					//preprocessing time(in seconds)
			time_point_type startTimeSearch_;
			double timeSearch_ = 0;						//search time (in seconds)
			time_point_type startTimeIncumbent_;
			double timeIncumbent_ = 0;					//time when last new incumbent was found (in seconds)

			std::uint32_t numStepsTimeOutCheck = 100;	//number of recursions before timeout is checked


		}; // end class infoBase




}//end namespace bitgraph


#endif //  BBITGRAPH_GRAPH_ALGORITHMS_ALGORITHM_INFO_H






