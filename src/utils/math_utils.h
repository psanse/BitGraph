/**
 * @file math_utils.h
 * @brief Provides general-purpose mathematical utilities for BitGraph.
 *
 * This header defines lightweight mathematical helpers, including operations
 * on comparable values and stateful functors for computing descriptive
 * statistics over collections.
 *
 * The statistical accumulators are designed for use with standard algorithms
 * such as std::for_each().
 *
 * @note Template functions and small utility classes are implemented directly
 *       in this header.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */

#ifndef BITGRAPH_UTILS_MATH_UTILS_H
#define BITGRAPH_UTILS_MATH_UTILS_H


#include <cstddef>
#include <cmath>
#include <cassert>

namespace bitgraph {

    namespace utils {
		    

		/**
		 * @brief Returns the smallest of three values.
		 *
		 * @tparam T Comparable value type.
		 * @param x First value.
		 * @param y Second value.
		 * @param z Third value.
		 * @return The smallest input value.
		 */
		template<typename T>
		inline
			T min3(T x, T y, T z) {
			return std::min<T>(std::min<T>(x, y), z);
		}

		/**
		 * @brief Accumulates values and computes their arithmetic mean.
		 *
		 * This functor can be passed to std::for_each(). The resulting functor returned
		 * by std::for_each() contains the accumulated state.
		 *
		 * @pre At least one value must be accumulated before requesting the mean.
		 */
		class MeanValue {		
		public:
			/**
			* @brief Adds a value to the accumulated sample.
			* @param value Value to add.
			*/
			void operator() (double value) noexcept {
				++count_;
				sum_ += value;
			}

			/**
		   * @brief Returns the arithmetic mean of the accumulated values.
		   * @return Arithmetic mean.
		   * @pre At least one value has been accumulated.
		   */
			double value() const noexcept
			{
				assert(count_ > 0);
				return sum_ / static_cast<double>(count_);
			}

			/**
		   * @brief Converts the accumulator to its arithmetic mean.
		   * @return Arithmetic mean.
		   * @pre At least one value has been accumulated.
		   */
			operator double() noexcept{
				return value();
			}

			/** @brief Returns the number of accumulated values. */
			std::size_t count() const noexcept
			{
				return count_;
			}

		private:
			std::size_t count_ = 0;			// number of accumulated values
			double sum_ = 0.0;				// sum of all accumulated values

		};// end of class MeanValue

		/**
		 * @brief Accumulates squared deviations from a supplied mean.
		 *
		 * This functor computes the population standard deviation, using the number
		 * of accumulated values as the denominator.
		 *
		 * @pre At least one value must be accumulated before requesting the standard
		 *      deviation.
		 */
		class StdDevValue {		
		public:

			/**
			* @brief Constructs an accumulator for a known mean.
			* @param mean Arithmetic mean of the collection.
			*/
			explicit StdDevValue(double mean) noexcept : 
				mean_(mean) 
			{}


			/**
		  * @brief Accumulates the squared deviation of one value.
		  * @param value Collection value.
		  */
			void operator() (double value) noexcept {
				++count_;

				const double difference = value - mean_;
				squared_difference_sum_ += difference * difference;
			}

			/**
		   * @brief Returns the population standard deviation.
		   * @return Population standard deviation.
		   * @pre At least one value has been accumulated.
		   */
			double value() const noexcept
			{
				assert(count_ > 0);

				return std::sqrt(
					squared_difference_sum_ /
					static_cast<double>(count_));
			}


			/**
			* @brief Converts the accumulator to its population standard deviation.
			* @return Population standard deviation.
			*/
			operator double() const noexcept {
				return value();
			}

			/** @brief Returns the number of accumulated values. */
			std::size_t count() const noexcept
			{
				return count_;
			}

		private:
			const double mean_;								
			std::size_t count_ = 0;							
			double squared_difference_sum_ = 0;
		};

		/**
		 * @brief Multiplies each supplied value by a fixed scale factor.
		 *
		 * This function object can be used with standard transformation algorithms
		 * such as std::transform().
		 *
		 * @tparam T Value and scale-factor type. The type must support multiplication.
		 */
		template< typename T >
		class Scale {
		public:
			/**
			* @brief Constructs a scaling function.
			* @param factor Multiplicative scale factor.
			*/
			explicit Scale(T s) :
				factor_(s)
			{}

			/**
			 * @brief Scales a value.
			 * @param value Value to scale.
			 * @return Product of @p value and the stored scale factor.
			 */
			T operator()(T value) const { 
				return value * factor_;
			}

		private:
			T factor_;
		};

	} // namespace utils

} // namespace bitgraph

#endif // BITGRAPH_UTILS_MATH_UTILS_H	