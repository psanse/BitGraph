/**
 * @file common_types.h  
 * @brief set of useful types 
 * @date ?
 * @last_update 03/07/25
 * @author pss
 **/

#ifndef __COMMON_TYPES_H__
#define	__COMMON_TYPES_H__

#include <iostream>
#include <vector>
#include <set>
#include <cassert>

///////////////////////
// useful aliases 

namespace bitgraph {

	using vint = std::vector<int>;
	using sint = std::set<int>;
	using vdob = std::vector<double>;

}

namespace bitgraph {
	
	namespace com {

		////////////////////////
		//
		// struct FixedStack
		//
		// Lightweight fixed-capacity stack backed by a raw C-array.
		// Intended for TRIVIAL types only (no element destructors are ever invoked).
		//
		////////////////////////

		/**
		 * @brief Fixed-capacity stack implemented on top of a contiguous C-array.
		 *
		 * @tparam T Trivial element type (POD-like). The container never runs element
		 *         destructors; removal operations are index-only.
		 *
		 * @details
		 * - Capacity is fixed after allocation (via constructor or reset()).
		 * - Push/pop/erase/truncate perform no allocation/deallocation.
		 * - Some operations use "swap-with-top" semantics for O(1) removal.
		 *
		 * @note
		 * This container assumes @p T is trivial (or at least trivially destructible).
		 * Do NOT use with types that manage resources (std::string, std::vector, RAII handles, ...).
		 */
		template <class T>
		struct FixedStack {

			static_assert(std::is_trivial<T>::value,
				"FixedStack<T> is intended only for trivial types (POD-like).");
			static_assert(std::is_trivially_destructible<T>::value,
				"FixedStack<T> assumes pop/erase/truncate don't run destructors.");

			// Element type stored in the stack.
			using value_type = T;

			///////
			//construction / destruction
			FixedStack() = default;
			explicit FixedStack(std::size_t MAX_SIZE);

			//copy semantics are disallowed
			FixedStack(const FixedStack&) = delete;
			FixedStack& operator = (const FixedStack&) = delete;

			//move semantics are allowed
			FixedStack(FixedStack&&) noexcept = default;
			FixedStack& operator = (FixedStack&&) noexcept = default;

			~FixedStack() { deallocate(); }

			//setters and getters 
			const T& at(std::size_t pos)	const { return stack_[pos]; }

			const T& top() const;
			T& top();
			const T& bottom() const { return stack_[0]; }
			T& bottom() { return stack_[0]; }

			T& at(std::size_t pos) { return stack_[pos]; }
			
			std::size_t size() const noexcept  { return nE_; }
			int size_int() const noexcept { return static_cast<int>(nE_); }								
			std::size_t capacity() const noexcept  { return cap_; }
			
			T* data() noexcept { return stack_.get(); }
			const T* data() const noexcept { return stack_.get(); }
			
			//for range-based for
			T* begin() noexcept { return data(); }
			T* end() noexcept { return data() + nE_; }
			const T* begin() const noexcept { return data(); }
			const T* end() const noexcept { return data() + nE_; }
			const T* cbegin() const noexcept { return data(); }
			const T* cend() const noexcept { return data() + nE_; }

			//safe STL reverse iterators, even  if nE_ == 0. 
			//Better than manually manipulating pointers.
			auto rbegin() noexcept {
				return std::make_reverse_iterator(end());
			}

			auto rend() noexcept {
				return std::make_reverse_iterator(begin());
			}

			auto rbegin() const noexcept {
				return std::make_reverse_iterator(end());
			}

			auto rend() const noexcept {
				return std::make_reverse_iterator(begin());
			}

			////////////
			// allocation

			/**
			 * @brief Reinitializes the stack with capacity @p MAX_SIZE.
			 *
			 * @param MAX_SIZE New capacity.
			 *
			 * @details
			 * - Releases any currently allocated storage.
			 * - Allocates a new underlying array sized @p MAX_SIZE.
			 * - The stack becomes empty after the call.
			 *
			 * @post size() == 0, capacity() == MAX_SIZE.
			 */
			void reset(std::size_t MAX_SIZE);

		private:
			/**
			 * @brief Deallocates underlying storage and resets state.
			 *
			 * @details
			 * - Frees the C-array backing the stack.
			 * - Sets stack_ to nullptr, cap_ to 0, and nE_ to 0.
			 *
			 * @note This is used by the destructor and reset().
			 */
			void deallocate() noexcept;

			//////////////
			//basic operations (no memory management)
		public:

			/**
			 * @brief Pushes @p d to the top of the stack.
			 *
			 * @param d Value to push.
			 *
			 * @pre size() < capacity()
			 *
			 * @details
			 * O(1). No allocation is performed.
			 */
			void push(T d);

			/**
			 * @brief Inserts @p d at the bottom (index 0) using O(1) swap-with-bottom semantics.
			 *
			 * @param d Value to insert at the bottom.
			 *
			 * @pre size() < capacity()
			 *
			 * @details
			 * O(1). Implements:
			 *  - Move current bottom element to the top (if stack non-empty),
			 *  - Place @p d at the bottom,
			 *  - Increase size by 1.
			 *
			 * This is NOT a stable "shift-right" operation; ordering changes.
			 */
			void push_bottom_swap(T d);

			/**
			* @brief Removes the top element from the stack
			*	     (the element is lost)
			* @details: no deallocation is performed
			**/
			void pop();

			/**
			* @brief Removes the top nb elements from the top of the stack.
			*		 (the elements are lost)
			* @param nb: number of elements to remove
			* @details: no deallocation is performed
			**/
			void pop(std::size_t nb);

			/**
			 * @brief Removes the bottom element using O(1) swap-with-top semantics.
			 *
			 * @pre size() > 0
			 *
			 * @details
			 * O(1). Implements:
			 *  - Move current top element to the bottom (index 0),
			 *  - Decrease size by 1.
			 *
			 * This is NOT a stable "shift-left" operation; ordering changes.
			 */
			void pop_bottom_swap();

			/**
			 * @brief Removes the element at position @p pos using O(1) swap-with-top semantics.
			 *
			 * @param pos Position of the element to remove.
			 *
			 * @pre pos < size()
			 *
			 * @details
			 * O(1). The element at @p pos is replaced with the current top element, then size is decreased by 1.
			 * Ordering is not preserved.
			 */
			void erase(int pos);

			/**
			* @brief Removes all elements from the stack.
			*		 (no deallocation)
			* @details: no deallocation is performed
			**/
			void erase() noexcept { nE_ = 0; }

			/**
			 * @brief Fast rollback to a previous size.
			 *
			 * @param new_size New logical size.
			 *
			 * @pre new_size <= size()
			 *
			 * @details
			 * O(1). Useful for backtracking/rollback. Does not deallocate memory and does not run destructors.
			 */
			void truncate(std::size_t new_size) noexcept {
				assert(new_size <= nE_);
				nE_ = new_size;
			}

			/////////////////
			//boolean operations

			bool empty() const noexcept { return (nE_ == 0); }
			bool full() const noexcept { return nE_ >= cap_; }


			////////////////////
			//I/O

			std::ostream& print(std::ostream& o) const;
			
			template<class U>
			friend std::ostream& operator<< (std::ostream& o, const FixedStack<U>& s);

			/////////////////////
			// data members
				
		private:
			std::size_t nE_ = 0;				//number of elements, points to the next position to fill		
			std::size_t cap_ = 0;				//capacity of the underlying array
			std::unique_ptr<T[]> stack_;		//underlying C-array - TODO: use smart pointer?	(15/01/2026)

		};

	}//end namespace com
			
	using com::FixedStack;
		
}//end namespace bitgraph

namespace bitgraph {
	namespace com {
		template<class U>
		inline
			std::ostream& operator<< (std::ostream& o, const FixedStack<U>& s) { s.print(o); return o; }
	}
}


namespace bitgraph {
	namespace detail
	{

		////////////////////////
		//
		// struct IntRange
		//
		// Interval of positive integers - typically a range of vertices
		// 
		// TODO - under development (17/12/2025)
		///////////////////////

		struct IntRange {
			static constexpr int no_range = -1;

			int vl = no_range;		//lower bound
			int vh = no_range;		//upper bound

			constexpr IntRange(int lh = no_range, int rh = no_range) noexcept :
				vl{lh}, vh{rh}
			{}

			//basic operations
					
			constexpr bool is_unset() const noexcept { return (vl == no_range || vh == no_range); }
			constexpr bool is_set()   const noexcept { return !is_unset(); }
			constexpr bool is_proper() const noexcept { return is_set() && (vl <= vh) && (vl >= 0); }

			//I/O

			/**
			* @brief: streams the range in the format [@vl, @vh]
			**/
			std::ostream& print(std::ostream& os = std::cout) const {
				os << "[" << vl << "," << vh << "]";
				return os;
			}

			friend std::ostream& operator<<(std::ostream& os, const IntRange& r) {
				return os << "[" << r.vl << "," << r.vh << "]";
			}

			friend constexpr bool operator == (const IntRange& lhs, const IntRange& rhs) {
				return (lhs.vl == rhs.vl && lhs.vh == rhs.vh);
			}
		};
				
	}//end namespace detail	

}//end namespace bitgraph





#endif