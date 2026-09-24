/**
 * @file benchmark.h
 * @brief Defines the GraphBenchmark base class for graph test datasets.
 *
 * GraphBenchmark manages a collection of graph instance filenames sharing a common
 * base path. Each instance may also have an associated integer value, typically
 * a known optimum or bound used by an algorithm-testing framework.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */

#ifndef BITGRAPH_GRAPH_BENCHMARK_H
#define BITGRAPH_GRAPH_BENCHMARK_H

#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "utils/logger.h"
#include "utils/path_utils.h"



namespace bitgraph {
	
		/**
		 * @brief Base class for collections of graph benchmark instances.
		 *
		 * Benchmark stores graph instance filenames, a common base path, and optional
		 * integer reference values associated with individual instances.
		 *
		 * The protected constructor prevents direct construction while allowing
		 * concrete benchmark datasets to derive from this class.
		 */
		
		template <class ValueT>
		class BasicGraphBenchmark {
		public:
		
			using value_type = ValueT;
			using filename_list = std::vector<std::string>;
			using value_map = std::map<std::string, value_type>;

			// alias for backward compatibility
			using vstr_t = filename_list;
			using mstri_t = value_map;
						
						
			BasicGraphBenchmark(const BasicGraphBenchmark& b) = delete;
			BasicGraphBenchmark& operator=	(const BasicGraphBenchmark& b) = delete;
			BasicGraphBenchmark(BasicGraphBenchmark&& b) = delete;
			BasicGraphBenchmark& operator=	(BasicGraphBenchmark&& b) = delete;
						
			virtual ~BasicGraphBenchmark() {}
							

			///////////
			// setters and getters

			 /**
			 * @brief Returns the number of registered graph instances.
			 * @return Number of instance filenames.
			 */
			std::size_t number_of_instances() const noexcept
			{
				return filenames_.size();
			}

			/**
			 * @brief Replaces the current list of graph instance filenames.
			 * @param filenames New filename list.
			 */
			void set_filenames(filename_list filenames)
			{
				filenames_ = std::move(filenames);
			}

			/*void setArrayOfFilenames(std::vector<std::string> list) { 
				filenames_ = list; 
			}*/

			/**
			 * @brief Returns the reference value associated with an instance.
			 *
			 * @param filename Instance filename.
			 * @return Associated value, or `-1` if the filename is not registered.
			 *
			 * @note This interface assumes that `-1` is not a valid reference value.
			 */
			ValueT get_value(const std::string& filename) const;
						
			
			const std::string& path() const noexcept
			{ 
				return path_; 
			}

			/**
			 * @brief Returns the registered instance filenames.
			 * @return Const reference to the filename list.
			 */
			const filename_list& filenames() const noexcept
			{
				return filenames_;
			}		

			/**
			* @brief Returns the filename-to-value mapping.
			* @return Const reference to the value map.
			*/
			const value_map& values() const noexcept
			{
				return values_;
			}
						
			/////////////
			// Context

			/**
			 * @brief Removes all registered benchmark instances.
			 *
			 * Clears both the instance filename list and the associated reference values.
			 * The benchmark base path remains unchanged.
			 */
			void clear_instances() noexcept
			{
				filenames_.clear();
				values_.clear();
			}


			//////////////
			//GraphBenchmark operations

			/**
			 * @brief Adds a graph instance and its associated reference value.
			 *
			 * The benchmark base path is prepended to @p filename. A platform-preferred
			 * directory separator is inserted when necessary.
			 *
			 * @param filename Instance filename, relative to the benchmark base path.
			 * @tparam value Reference value, typically a known optimum or bound.
			 */
			virtual void add_test(const std::string&, ValueT value);

			/**
			 * @brief Adds a graph instance without an associated reference value.
			 *
			 * The benchmark base path is prepended to @p filename. A
			 * platform-preferred directory separator is inserted when necessary.
			 *
			 * @param filename Instance filename, relative to the benchmark base path.
			 *
			 * @pre @p filename does not begin with a directory separator.
			 */
			virtual void add_test(const std::string&);

			/**
			* @brief Determines whether the benchmark contains no instances.
			* @return `true` if no instances are registered.
			*/
			bool is_empty() const noexcept { 
				return filenames_.empty(); 
			}


			/////////////
			// I/O
			/**
			 * @brief Writes the benchmark contents to an output stream.
			 * @param out Output stream.
			 * @return Reference to @p out.
			 */
			std::ostream& print(std::ostream & = std::cout) const;
						
		protected:
	
			/**
			 * @brief Constructs a benchmark for a dataset path.
			 * @param path Common path for all benchmark instances.
			 */
			explicit BasicGraphBenchmark(std::string path_name);

			const std::string path_;   ///< Common path for all instances.
			filename_list filenames_;  ///< Registered instance filenames.
			value_map values_;         ///< Reference values indexed by filename.

		}; // end class BasicGraphBenchmark


		/**
		 * @brief Writes a graph benchmark to an output stream.
		 *
		 * @tparam ValueT Type of the values associated with benchmark instances.
		 * @param out Output stream.
		 * @param benchmark Benchmark to write.
		 * @return Reference to @p out.
		 */
		template<class ValueT>
		std::ostream& operator<<(
			std::ostream& out,
			const BasicGraphBenchmark<ValueT>& benchmark)
		{
			return benchmark.print(out);
		}
	

}//end namespace bitgraph


namespace bitgraph {
	
	// convenient aliases
	using GraphBenchmark = 
		BasicGraphBenchmark<int>;

	using WeightedGraphBenchmark =
		BasicGraphBenchmark<double>;

	template<class WeightT>
	using WeightedGraphBenchmarkT =
		BasicGraphBenchmark<WeightT>;

	// Backward-compatible alias. New code should use GraphBenchmark.
	using Benchmark = GraphBenchmark;

} // namespace bitgraph

///////////////////////////////////////
// Necessary header implementation for template class

namespace bitgraph {

	template<class ValueT>
	BasicGraphBenchmark<ValueT>::BasicGraphBenchmark(std::string path_name)
		:path_(std::move(path_name))
	{
		if (path_.empty()) {
			LOG_DEBUG(
				"GraphBenchmark base path is empty; filenames will be used as supplied. - GraphBenchmark::GraphBenchmark");
		}
	}

	template<class ValueT>
	void BasicGraphBenchmark<ValueT>::add_test(const std::string& filename)
	{
		std::string full_filename = path_;

		if (!full_filename.empty()) {
			utils::append_slash(full_filename);
		}

		full_filename += filename;
		filenames_.push_back(std::move(full_filename));
	}

	template<class ValueT>
	void BasicGraphBenchmark<ValueT>::add_test(
		const std::string& filename,
		ValueT value)
	{
		std::string full_filename = path_;

		if (!full_filename.empty()) {
			utils::append_slash(full_filename);
		}

		full_filename += filename;

		filenames_.push_back(full_filename);
		values_[full_filename] = value;
	}

	template<class ValueT>
	ValueT BasicGraphBenchmark<ValueT>::get_value(const std::string& filename) const
	{
		const auto iterator = values_.find(filename);

		return iterator != values_.end()
			? iterator->second
			: -1;
	}

	///////////////////
	// I/O

	template<class ValueT>
	std::ostream& BasicGraphBenchmark<ValueT>::print(std::ostream& out) const
	{
		for (const auto& filename : filenames_) {
			out << filename;

			const auto iterator = values_.find(filename);

			if (iterator != values_.end()) {
				out << ':' << iterator->second;
			}

			out << '\n';
		}

		return out;
	}

}


#endif // BITGRAPH_GRAPH_BENCHMARK_H