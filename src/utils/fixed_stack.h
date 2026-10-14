/**
 * @file fixed_stack.h
 * @brief Defines a fixed-capacity contiguous stack for trivial types.
 *
 * FixedStack allocates its storage explicitly and provides constant-time
 * stack, rollback, and unordered-removal operations without reallocating
 * during normal use. It is intended for hot-path and backtracking algorithms.
 *
 * Template implementations are provided in detail/fixed_stack_imp.h.
 *
 * @author Pablo San Segundo
 * @date Last updated: 24/09/2026
 */

#ifndef BITGRAPH_UTILS_FIXED_STACK_H
#define	BITGRAPH_UTILS_FIXED_STACK_H


#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>


namespace bitgraph {
	
	namespace utils {
			

		/**
		 * @brief Fixed-capacity stack backed by a contiguous array.
		 *
		 * Storage is allocated during construction or by reset(). Normal stack
		 * operations do not allocate or deallocate memory. Some removal operations
		 * replace the removed element with another element and therefore do not
		 * preserve ordering.
		 *
		 * @tparam ValueT Trivial element type.
		 *
		 * @note Removal operations modify only the logical size and do not
		 *       individually destroy elements. The element type is therefore
		 *       restricted to trivial types.
		 */
		template <class ValueT>
		class FixedStack {
			static_assert(
				std::is_trivial<ValueT>::value,
				"FixedStack<ValueT> requires a trivial (POD) element type.");
		
		public:	
			using value_type = ValueT;
			using size_type = std::size_t;

			using iterator = value_type*;
			using const_iterator = const value_type*;
			using reverse_iterator =
				std::reverse_iterator<iterator>;
			using const_reverse_iterator =
				std::reverse_iterator<const_iterator>;


			///////
			//construction / destruction

			 /**
			 * @brief Constructs an empty stack with zero capacity.
			 */
			FixedStack() = default;

			/**
			 * @brief Constructs an empty stack with a fixed capacity.
			 *
			 * @param capacity Maximum number of elements.
			 * @throws std::bad_alloc If storage cannot be allocated.
			 */
			explicit FixedStack(size_type capacity);

			// only move semantics are allowed
			FixedStack(const FixedStack&) = delete;
			FixedStack& operator = (const FixedStack&) = delete;

			/**
			 * @brief Move-constructs a stack by transferring ownership of its storage.
			 *
			 * The elements, size, and capacity of @p other are transferred to the new
			 * stack. After the operation, @p other is left as a valid empty stack with
			 * zero capacity.
			 *
			 * @param other Stack whose representation is transferred.
			 *
			 * @post `other.empty() == true`
			 * @post `other.capacity() == 0`
			 */
			FixedStack(FixedStack&& other) noexcept;

			/**
			 * @brief Replaces this stack by move-assigning another stack.
			 *
			 * Any storage currently owned by this stack is released. The storage,
			 * elements, size, and capacity of @p other are then transferred to this
			 * stack. After the operation, @p other is left as a valid empty stack with
			 * zero capacity.
			 *
			 * Self-move assignment leaves this stack unchanged.
			 *
			 * @param other Stack whose representation is transferred.
			 * @return Reference to this stack.
			 *
			 * @post `other.empty() == true`, unless `this == &other`.
			 * @post `other.capacity() == 0`, unless `this == &other`.
			 */
			FixedStack& operator=(FixedStack&& other) noexcept;
	
			~FixedStack() = default;

			////////////////
			//setters and getters 

			/**
			 * @brief Returns the element at a specified position.
			 *
			 * @param position Zero-based element position.
			 * @return const reference to the requested element.
			 *
			 * @pre `position < size()`
			 */
			const value_type& at(std::size_t position)	const 
			{ 
				assert(position < nE_);
				return stack_[position];
			}

			/**
			  * @brief Returns the element at a specified position.
			  *
			  * @param position Zero-based element position.
			  * @return reference to the requested element.
			  *
			  * @pre `position < size()`
			  */
			value_type& at(size_type position){ 
				assert(position < nE_);
				return stack_[position]; 
			}

			/**
			* @brief Returns the top element.
			* @return const reference to the top element.
			* @pre The stack is not empty.
			*/
			const value_type& top() const;

			/**
			 * @brief Returns the last element stored in the stack.
			 *
			 * The top element is the element at logical position `size() - 1`, i.e., the
			 * most recently pushed element.
			 *
			 * @return Reference to the top element.
			 * @pre The stack is not empty.
			 */
			value_type& top();


			/**
			 * @brief Returns the first element stored in the stack.
			 *
			 * The bottom element is the element at logical position zero, i.e., the
			 * oldest element when only regular push() and pop() operations are used.
			 *
			 * @return const reference to the bottom element.
			 * @pre The stack is not empty.
			 */
			const value_type& bottom() const {
				assert(!empty());
				return stack_[0];
			}

			/**
			 * @brief Returns the first element stored in the stack.
			 *
			 * The bottom element is the element at logical position zero, i.e., the
			 * oldest element when only regular push() and pop() operations are used.
			 *
			 * @return Reference to the bottom element.
			 * @pre The stack is not empty.
			 */
			value_type& bottom() { 
				assert(!empty());
				return stack_[0];
			}

			
			/** @brief Returns the number of stored elements. */
			std::size_t size() const noexcept  { 
				return nE_; 
			}

			/**
			 * @brief Returns the number of stored elements as an integer.
			 *
			 * @pre `size() <= std::numeric_limits<int>::max()`
			 */
			int size_int() const noexcept { 

				assert(
					nE_ <= static_cast<size_type>(
						std::numeric_limits<int>::max()));

				return static_cast<int>(nE_);
			}

			/** @brief Returns the maximum number of elements. */
			std::size_t capacity() const noexcept  { 
				return cap_; 
			}
			
			/** @brief Returns a pointer to the underlying storage. */
			value_type* data() noexcept {
				return stack_.get();
			}

			/** @brief Returns a const pointer to the underlying storage. */
			const value_type* data() const noexcept {
				return stack_.get();
			}
			
			// iterators, STL style, for range-based for loops and STL algorithms

			iterator begin() noexcept
			{
				return data();
			}

			iterator end() noexcept
			{
				return nE_ == 0 ? data() : data() + nE_;
			}

			const_iterator begin() const noexcept
			{
				return data();
			}

			const_iterator end() const noexcept
			{
				return nE_ == 0 ? data() : data() + nE_;
			}

			const_iterator cbegin() const noexcept
			{
				return begin();
			}

			const_iterator cend() const noexcept
			{
				return end();
			}

			reverse_iterator rbegin() noexcept
			{
				return reverse_iterator{ end() };
			}

			reverse_iterator rend() noexcept
			{
				return reverse_iterator{ begin() };
			}

			const_reverse_iterator rbegin() const noexcept
			{
				return const_reverse_iterator{ end() };
			}

			const_reverse_iterator rend() const noexcept
			{
				return const_reverse_iterator{ begin() };
			}

			const_reverse_iterator crbegin() const noexcept
			{
				return const_reverse_iterator{ cend() };
			}

			const_reverse_iterator crend() const noexcept
			{
				return const_reverse_iterator{ cbegin() };
			}


			////////////////////
			// operators 

			/**
			 * @brief Returns an element without bounds checking.
			 * @param position Zero-based element position.
			 * @return Const reference to the requested element.
			 */
			const value_type& operator [] (size_type position) const noexcept{
				return stack_[position];
			}		

			/**
		   * @brief Returns an element without bounds checking.
		   * @param position Zero-based element position.
		   * @return Reference to the requested element.
		   */
			value_type& operator [] (size_type position) noexcept {
				return stack_[position];
			}

			////////////
			// allocation

			/**
			 * @brief Reinitializes the stack with a new fixed capacity.
			 *
			 * Allocates new storage before replacing the current representation. After a
			 * successful call, the stack is empty and has capacity for @p capacity
			 * elements.
			 *
			 * @param capacity New stack capacity. A value of zero releases the current
			 *                 storage and leaves the stack empty.
			 *
			 * @throws std::bad_alloc If storage for the requested capacity cannot be
			 *                        allocated.
			 *
			 * @post `size() == 0`
			 * @post `capacity() == capacity`
			 *
			 * @note If allocation fails, the stack remains unchanged.
			 */
			void reset(std::size_t capacity);
				
		
			//////////////
			//basic stack operations (no memory management)
	
			/**
			 * @brief Pushes a value onto the top of the stack.
			 *
			 * @param value Value to push.
			 * @pre `size() < capacity()`
			 *
			 * @note This operation has constant complexity and performs no allocation.
			 */
			void push(value_type value);
			
			/**
			 * @brief Inserts a value at the bottom in constant time.
			 *
			 * The new value is stored at logical position zero. If the stack was not
			 * empty, the previous bottom element is moved to the new top position at the
			 * old `size()` index. All intermediate elements remain unchanged.
			 *
			 * This operation increases the logical size by one but does not preserve the
			 * original stack ordering.
			 *
			 * @param value Value to insert at the bottom.
			 *
			 * @pre `size() < capacity()`
			 *
			 * @post `size()` is increased by one.
			 * @post `bottom() == value`
			 *
			 * @note This operation has constant complexity and performs no allocation.
			 */
			void push_bottom_swap(value_type value);


			/**
			 * @brief Removes the top element.
			 *
			 * Decreases the logical size by one. The removed value remains physically
			 * present in the underlying storage but is no longer part of the stack.
			 *
			 * @pre The stack is not empty.
			 *
			 * @post `size()` is decreased by one.
			 *
			 * @note This operation has constant complexity, performs no deallocation, and
			 *       does not destroy the removed element individually.
			 */
			void pop() noexcept;

			/**
			 * @brief Removes a number of elements from the top of the stack.
			 *
			 * Reduces the logical size by @p count. The removed values remain physically
			 * present in the underlying storage but are no longer part of the stack.
			 *
			 * @param count Number of elements to remove.
			 *
			 * @pre `count <= size()`
			 *
			 * @post `size()` is reduced by @p count.
			 *
			 * @note Passing zero has no effect.
			 * @note This operation has constant complexity, performs no deallocation, and
			 *       does not destroy removed elements individually.
			 */
			void pop(size_type count) noexcept;
		
			/**
			 * @brief Removes the bottom element in constant time.
			 *
			 * If the stack contains more than one element, the current top element is
			 * moved to logical position zero, replacing the removed bottom element. The
			 * logical size is then reduced by one.
			 *
			 * This operation does not preserve element order.
			 *
			 * @pre The stack is not empty.
			 *
			 * @post `size()` is decreased by one.
			 *
			 * @note This operation has constant complexity, performs no deallocation, and
			 *       does not destroy the removed element individually.
			 */
			void pop_bottom_swap() noexcept;

			/**
			 * @brief Removes an element using swap-with-top semantics.
			 *
			 * The element at @p position is replaced by the current top element, after
			 * which the logical size is decreased.
			 *
			 * @param position Position of the element to remove.
			 * @pre `position < size()`
			 * @note This operation does not preserve element order.
			 */
			void erase_swap(size_type position) noexcept;

			/**
			* @brief Removes all elements without releasing storage.
			*/
			void clear() noexcept { 
				nE_ = 0; 
			}

			/**
			 * @brief Reduces the logical size of the stack.
			 *
			 * This operation supports constant-time rollback in backtracking
			 * algorithms.
			 *
			 * @param new_size New logical size.
			 * @pre `new_size <= size()`
			 *
			 * @note Storage is not released and individual elements are not destroyed.
			 */
			void truncate(size_type new_size) noexcept {
				assert(new_size <= nE_);
				nE_ = new_size;
			}

			/////////////////
			//boolean operations

			 /** @brief Returns whether the stack contains no elements. */
			bool empty() const noexcept { 
				return (nE_ == 0);
			}

			/** @brief Returns whether the stack has reached its capacity. */
			bool full() const noexcept { 
				return nE_ >= cap_;
			}

			////////////////////
			//I/O

			/**
			 * @brief Writes the stack elements and logical size to an output stream.
			 *
			 * Elements are written from bottom to top between square brackets, followed
			 * by the number of stored elements in a second pair of brackets.
			 *
			 * No trailing newline is written.
			 *
			 * @param out Output stream.
			 * @return Reference to @p out.
			 */
			std::ostream& print(std::ostream& o) const;
						
			/////////////////////
			// data members
				
		private:
			std::size_t nE_ = 0;				//number of elements, points to the next position to fill		
			std::size_t cap_ = 0;				//capacity of the underlying array
			std::unique_ptr<ValueT[]> stack_;		//underlying C-array 

		}; // end struct FixedStack

		template<class ValueT>
		inline
			std::ostream& operator<< (
				std::ostream& o, 
				const FixedStack<ValueT>& stack) {
			return stack.print(o);			
		}


	}//end namespace utils
	

	// for backwards compatibility, FixedStack is also available in the bitgraph namespace
	using utils::FixedStack;
		
}//end namespace bitgraph

///////////////////////////////////
// Necessary implementation headers

#include "detail/fixed_stack_imp.h"




#endif  // BITGRAPH_UTILS_FIXED_STACK_H