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

#ifndef BITGRAPH_GRAPH_ALGORITHM_INFO_H
#define BITGRAPH_GRAPH_ALGORITHM_INFO_H


#include "utils/prec_timer.h"
#include "utils/time_utils.h"
#include "utils/logger.h"

#include <cstddef> // for std::size_t
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>
#include <string>

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
			
			// Instance metadata
			std::string name;               
			std::size_t N = 0;				
			std::size_t M = 0;				

			// Execution configuration
			double time_out = std::numeric_limits<double>::max();			
			double heuristic_time_out = std::numeric_limits<double>::max();	

			int num_threads = 1;			
			bool unrolled = false;		// legacy loop-unrolling option, retained for compatibility with existing algorithms									

			// Parsing timing
			time_point_type parsing_start_time{};		
			double parsing_time = 0.0;											


			AlgorithmParameters() = default;
			virtual ~AlgorithmParameters() = default;

		protected:
			/**
		    * @name Copy and move support for derived parameter types
		    *
		    * These operations are protected to allow automatically generated
		    * copy/move operations in derived classes while preventing clients from
		    * copying or moving objects through AlgorithmParameters and accidentally
		    * slicing their derived state.
		    * @{
		    */
			AlgorithmParameters(const AlgorithmParameters&) = default;
			AlgorithmParameters& operator=(const AlgorithmParameters&) = default;
			AlgorithmParameters(AlgorithmParameters&&) noexcept = default;
			AlgorithmParameters& operator=(AlgorithmParameters&&) noexcept = default;
			/** @} */

		public:	
			/**
			 * @brief Starts the auxiliary parameter timer.
			 */
			void start_parsing_timer() noexcept
			{
				parsing_start_time = clock_type::now();
			}

			/**
			* @brief Stops the parsing timer and stores the elapsed duration.
			* @return Parsing time in seconds.
			*/
			double read_parsing_timer() noexcept
			{
				parsing_time =
					utils::elapsed_time(parsing_start_time);

				return parsing_time;
			}

			/**
			* @brief Clears the recorded parsing time.
			*/
			void clear_parsing_timer() noexcept
			{
				parsing_start_time = time_point_type{};
				parsing_time = 0.0;
			}

			/**
			 * @brief Restores the common algorithm parameters to their defaults.
			 */
			void reset_base();

			/**
			* @brief Restores algorithm-specific parameters to their defaults.
			*
			* The base implementation has no algorithm-specific fields to reset.
			*/
			virtual void reset_derived()
			{ // No algorithm-specific fields to reset in the base class
			}

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


		/**
		 * @brief Base information class for graph-algorithm executions.
		 *
		 * Stores a concrete parameter object together with common execution timers
		 * and reporting facilities. The parameter type may extend AlgorithmParameters
		 * with algorithm-specific configuration fields.
		 *
		 * Derived information classes may add execution results such as solutions,
		 * bounds, counters, and search statistics. Such classes should override
		 * clearResults() to reset their result fields; the public clear() function
		 * controls the complete clearing sequence.
		 *
		 * Public access to the stored parameters is read-only. Derived information
		 * classes may modify them through the protected parameters() overload.
		 *
		 * @tparam ParametersT Concrete parameter type associated with the algorithm.
		 *         The type must be AlgorithmParameters or derive from it.
		 *
		 * @note The complete ParametersT object is stored by value, so derived
		 *       parameter fields are preserved without object slicing.
		 *
		 * @note This class implements the Template Method pattern: clear() defines
		 *       the clearing procedure and invokes the virtual clearResults() hook
		 *       for algorithm-specific result data.
		 */
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
				: parameters_(parameters)
			{}

			virtual ~BasicAlgorithmInfo() = default;

			BasicAlgorithmInfo(const BasicAlgorithmInfo&) = default;
			BasicAlgorithmInfo& operator=(const BasicAlgorithmInfo&) = default;
			BasicAlgorithmInfo(BasicAlgorithmInfo&&) noexcept = default;
			BasicAlgorithmInfo& operator=(BasicAlgorithmInfo&&) noexcept = default;

			// Parameter observers

			 /**
			 * @brief Returns the complete concrete parameter object.
			 */
			const parameters_type& parameters() const noexcept
			{
				return parameters_;
			}

			const std::string& name() const noexcept
			{
				return parameters_.name;
			}

			std::size_t num_vertices() const noexcept
			{
				return parameters_.N;
			}

			std::size_t num_edges() const noexcept
			{
				return parameters_.M;
			}

			double time_out() const noexcept
			{
				return parameters_.time_out;
			}

			double heuristic_time_out() const noexcept
			{
				return parameters_.heuristic_time_out;
			}

			int number_of_threads() const noexcept
			{
				return parameters_.num_threads;
			}


			// Runtime information observers
			
			std::uint32_t
				recursion_calls_per_timeout_check() const noexcept
			{
				return recursion_calls_per_timeout_check_;
			}

			double parsing_time() const noexcept
			{
				return parameters_.parsing_time;
			}

			double preprocessing_time() const noexcept
			{
				return preproc_time_;
			}

			double search_time() const noexcept
			{
				return search_time_;
			}

			double incumbent_time() const noexcept
			{
				return incumbent_time_;
			}

			/**
			 * @brief Sets the number of recursive calls between timeout checks.
			 *
			 * @param count Number of recursive calls. A larger value reduces
			 *        checking overhead but may delay timeout detection.
			 */
			void recursion_calls_per_timeout_check(
				std::uint32_t count) noexcept
			{
				recursion_calls_per_timeout_check_ = count;
			}

			// Timer operations
	

			/*
			* @brief sets initial time in timer @t (previously set with startTimer(...))
			* @param t - phase_t enum
			*/
			void start_timer(phase_t phase) noexcept;

			/*
			* @brief reads time in timer @t (previously set with startTimer(...))
			* @param t - phase_t enum
			*/
			double read_timer(phase_t phase)  noexcept;

			/*
			* @brief clears appropiate time duration (concerning phase_t @t)
			*/
			void clear_timer(phase_t phase) noexcept;

			/**
			* @brief clears all execution timers
			*/
			void clear_execution_timers() noexcept;
						
			/**
			 * @brief Resets the algorithm information object.
			 *
			 * Clears all execution timers and invokes the virtual clearResults() hook to
			 * reset algorithm-specific results stored by derived information classes. It
			 * also resets the parameters defined by the concrete derived parameter type.
			 *
			 * Common parameters inherited from AlgorithmParameters, including the
			 * instance name, graph dimensions, timeout limits, and parsing information,
			 * are cleared only when @p preserve_base_parameters is false.
			 *
			 * @param preserve_base_parameters If true, preserves the common parameters
			 *        inherited from AlgorithmParameters. If false, restores the complete
			 *        information object to its default state.
			 *
			 * @note Derived information classes should override clearResults(), rather
			 *       than clear(), to reset their algorithm-specific result fields.
			 *
			 * @note The parsing timer is considered part of the common parameter state
			 *       and is therefore preserved when @p preserve_base_parameters is true.
			 *
			 * @see clearResults()
			 */
			 void clear(
				bool preserve_base_parameters = false);
						
			// output
			
			/**
			 * @brief Writes the complete algorithm report.
			 *
			 * In table mode, values are written as tab-separated fields. In verbose mode,
			 * each field is written on a labeled line. Floating-point values use fixed
			 * notation with four decimal places.
			 *
			 * The original formatting state of the output stream is restored before the
			 * function returns.
			 *
			 * @param out Destination stream.
			 * @param format Output report format.
			 * @param trailing_newline Whether to append a newline.
			 * @return Reference to @p out.
			 */
			virtual std::ostream& print_report(
				std::ostream& out = std::cout, 
				report_t format = report_t::TABLE,
				bool trailing_newline = false) const;

			/**
			 * @brief Writes the general algorithm parameters to an output stream.
			 *
			 * Writes the instance metadata and common execution configuration stored in
			 * the associated parameter object, such as the instance name, graph size,
			 * timeout limits, and number of threads.
			 *
			 * Derived information classes may override this function to append
			 * algorithm-specific parameters.
			 *
			 * @param out Destination output stream.
			 * @return Reference to @p out.
			 */
			virtual	std::ostream& print_params(
				std::ostream& o = std::cout) const;

			/**
			 * @brief Writes the recorded phase durations and timeout limits.
			 *
			 * Floating-point values are written in fixed notation with four decimal
			 * places. The stream's original formatting state is restored before return.
			 *
			 * @param out Destination stream.
			 * @return Reference to @p out.
			 */
			std::ostream& print_timers(
				std::ostream& o = std::cout) const;


			/**
			 * @brief Writes the complete algorithm report.
			 */
			friend std::ostream& operator<<(
				std::ostream& out,
				const BasicAlgorithmInfo& information)
			{
				return information.print_report(out);
			}

		protected:
			/**
			* @brief Returns mutable access to the concrete parameter object.
			*
			* This overload is available only to this class and derived information
			* classes.
			*/
			parameters_type& mutable_parameters() noexcept
			{
				return parameters_;
			}

			/**
			 * @brief Clears algorithm-specific result information.
			 *
			 * Derived information classes should override this function to clear
			 * counters, solutions, bounds, and other execution results.
			 */
			virtual void clear_results() noexcept
			{}
				
			
			// Concrete parameter object
			parameters_type parameters_;
							
			time_point_type preproc_start_time_;
			double preproc_time_ = 0.0;									//preprocessing time(in seconds)
			time_point_type search_start_time_;
			double search_time_ = 0.0;									//search time (in seconds)
			time_point_type incumbent_start_time_;
			double incumbent_time_ = 0.0;								//time when last new incumbent was found (in seconds)

			std::uint32_t recursion_calls_per_timeout_check_ = 100;		//number of recursions before timeout is checked

		}; // end class BasicAlgorithmInfo

} //end namespace bitgraph

////////////////////////////////
// template implementation

#include "detail/algorithm_info_imp.h"


#endif //  BITGRAPH_GRAPH_ALGORITHMS_INFO_H






