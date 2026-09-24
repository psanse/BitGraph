/**
 * @file batch.h
 * @brief Defines a generic container for managing batches of test configurations.
 *
 * This header provides the Batch class template, which stores parameter
 * configurations used to define a sequence of tests or algorithm executions.
 *
 * Consumer code is given selective read-only access to the stored
 * configurations through indexed access, size queries, and constant
 * iterators. Mutating operations are protected so that derived batch classes
 * can control how configurations are created, added, modified, or removed.
 *
 * @note Batch does not execute tests. It only manages their parameter
 *       configurations.
 *
 * @author Pablo San Segundo
 * @date Last updated: 24/09/2026
 */

#ifndef BITGRAPH_UTILS_BATCH_H
#define BITGRAPH_UTILS_BATCH_H

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace bitgraph {

	namespace utils {


		/**
		 * @brief Stores parameter configurations for a batch of tests.
		 *
		 * @tparam ParameterT Type containing the parameters of one test.
		 */
		template <class ParameterT>
		class Batch {
		public:

			using parameter_type = ParameterT;
			using container_type = std::vector<parameter_type>;
			using size_type = typename container_type::size_type;
			using const_iterator = typename container_type::const_iterator;

			// no need for virtual destructor since this class is not intended to be used polymorphically
			// this class is intended to provide implementation reuse for derived classes,
			// not to be used through base class pointers.
			~Batch() = default;			
							
			//getters and setters

			/**
			 * @brief Returns the number of registered test configurations.
			 * @return Number of test configurations.
			 */
			size_t number_of_tests() const noexcept {
				return tests_.size(); 			
			}

			/**
		   * @brief Returns a test configuration.
		   *
		   * @param position Zero-based position of the configuration.
		   * @return Const reference to the requested configuration.
		   * @pre `position < number_of_tests()`
		   */
			const parameter_type& test(size_type position) const {

				assert(position < tests_.size());
				return tests_[position];
			}

			/**
			 * @brief Returns a mutable test configuration.
			 *
			 * @param position Zero-based position of the configuration.
			 * @return Reference to the requested configuration.
			 * @pre `position < number_of_tests()`
			 */
			parameter_type& test(size_type position)
			{
				assert(position < tests_.size());
				return tests_[position];
			}

			/**
			 * @brief Constructs and appends a test configuration in place.
			 *
			 * Forwards @p args directly to a constructor of parameter_type, avoiding the
			 * need to create a separate temporary configuration object.
			 *
			 * @tparam Args Types of the constructor arguments.
			 * @param args Arguments forwarded to the parameter_type constructor.
			 *
			 * @throws std::bad_alloc If the underlying container must grow and allocation
			 *                        fails.
			 * @throws Any exception thrown by the parameter_type constructor or while
			 *         relocating existing configurations.
			 *
			 * @note References and iterators to existing configurations may be invalidated
			 *       if the underlying container reallocates.
			 */
			template<class... Args>
			void emplace_test(Args&&... args)
			{
				tests_.emplace_back(
					std::forward<Args>(args)...);
			}

			// iterators - avoid client user access to the underlying container type

			const_iterator begin() const noexcept
			{
				return tests_.begin();
			}

			const_iterator end() const noexcept
			{
				return tests_.end();
			}

			const_iterator cbegin() const noexcept
			{
				return tests_.cbegin();
			}

			const_iterator cend() const noexcept
			{
				return tests_.cend();
			}

			/**
			 * @brief Removes all test configurations.
			 *
			 * The logical size becomes zero, but allocated vector capacity may be
			 * retained for reuse.
			 */
			void clear() noexcept
			{
				tests_.clear();
			}

			/** @brief Determines whether the batch contains no configurations. */
			bool empty() const noexcept
			{
				return tests_.empty();
			}
					

		protected:

			//construction / destruction
			Batch() = default;


			Batch(const Batch&) = default;
			Batch& operator=(const Batch&) = default;
			Batch(Batch&&) noexcept = default;
			Batch& operator=(Batch&&) noexcept = default;

			/////////////////////////
			// basic mutable operations

			/**
			 * @brief Adds a test configuration by copying it.
			 * @param parameters Configuration to add.
			 */
			void add_test(const parameter_type& parameters)
			{
				tests_.push_back(parameters);
			}
			
			/**
			 * @brief Adds a test configuration by moving it.
			 * @param parameters Configuration to add.
			 */
			void add_test(parameter_type&& parameters)
			{
				tests_.push_back(std::move(parameters));
			}

			/**
			* @brief Adds a test configuration by copying it.
			* @param parameters Configuration to add.
			* @return Reference to this batch.
			*/
			Batch& operator+=(const parameter_type& parameters)
			{
				add_test(parameters);
				return *this;
			}

			/**
			* @brief Adds a test configuration by moving it.
			* @param parameters Configuration to add.
			* @return Reference to this batch.
			*/
			Batch& operator+=(parameter_type&& parameters)
			{
				add_test(std::move(parameters));
				return *this;
			}

			/**
			 * @brief Returns the stored test configurations.
			 * @return Const reference to the underlying configuration collection.
			 * @note: iterator access is desired, i.e., for-range loops,
			 *		   so this function is not part of the public API
			 */
			const container_type& tests() const noexcept
			{
				return tests_;
			}

			
		private:
			container_type tests_;
		};

	}//end namespace utils

	// for backward compatibility, we keep the Batch class in the bitgraph namespace
	using utils::Batch;

}//end namespace bitgraph

#endif // BITGRAPH_UTILS_BATCH_H





