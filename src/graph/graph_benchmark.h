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
		class GraphBenchmark {

			friend std::ostream& operator<<	(std::ostream& o, GraphBenchmark& b) {
				return b.print(o);
			}

		public:
		
			using filename_list = std::vector<std::string>;
			using value_map = std::map<std::string, int>;

			// alias for backward compatibility
			using vstr_t = filename_list;
			using mstri_t = value_map;
						
						
			GraphBenchmark(const GraphBenchmark& b) = delete;
			GraphBenchmark& operator=	(const GraphBenchmark& b) = delete;
			GraphBenchmark(GraphBenchmark&& b) = delete;
			GraphBenchmark& operator=	(GraphBenchmark&& b) = delete;
						
			virtual ~GraphBenchmark() {}
							

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
			int get_value(const std::string& filename) const;
						
			
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
			 * @param value Reference value, typically a known optimum or bound.
			 */
			virtual void add_test(const std::string&, int value);

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
			explicit GraphBenchmark(std::string path_name);

			const std::string path_;   ///< Common path for all instances.
			filename_list filenames_;  ///< Registered instance filenames.
			value_map values_;         ///< Reference values indexed by filename.			
		};
	

}//end namespace bitgraph


namespace bitgraph {

	// Backward-compatible alias. New code should use GraphBenchmark.
	using Benchmark = GraphBenchmark;

} // namespace bitgraph

#endif // BITGRAPH_GRAPH_BENCHMARK_H