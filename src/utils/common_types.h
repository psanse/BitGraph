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
		// (lightweight fixed size stack implemented by a fixed size array)
		//
		////////////////////////

		template <class T>
		struct FixedStack {

			using value_type = T;

			///////
			//construction / destruction
			FixedStack() = default;
			explicit FixedStack(std::size_t MAX_SIZE);

			//copy semantics are disallowed
			FixedStack(const FixedStack&) = delete;
			FixedStack& operator = (const FixedStack&) = delete;

			//move semantics are allowed
			FixedStack(FixedStack&&)	 noexcept;
			FixedStack& operator = (FixedStack&&)	 noexcept;

			~FixedStack() { clear(); }

			//setters and getters 
			const T& at(std::size_t pos)	const { return stack_[pos]; }
			const T& top()			const;
			T& top();
			const T& bottom()		const { return stack_[0]; }
			T& bottom() { return stack_[0]; }

			T& at(std::size_t pos) { return stack_[pos]; }
			
			std::size_t size() const noexcept  { return nE_; }
			int size_int() const noexcept { return static_cast<int>(nE_); }								
			std::size_t capacity() const noexcept  { return cap_; }
			
			

			////////////
			// allocation

			/**
			* @brief Resets the stack to a new size, previos elements are destroyed.
			*        The new elements are constructed with the default constructor
			**/
			void reset(std::size_t MAX_SIZE);

		private:
			/**
			* @brief Deallocates memory (used by the destructor)
			*	     Use erase to clean the stack without deallocating memory
			**/
			void clear();

			//////////////
			//basic operations (no memory management)
		public:

			/**
			* @brief Places element at the top of the stack
			*	     (last position in the underlying array)
			* @details: no allocation is performed, stack must have enough capacity
			**/
			void push(T d);

			/**
			* @brief Places element at the bottom of the stack
			*	     (first position in the underlying array)
			*
			*		 I. The bottom element is moved to the top of the stack			
			* @details: no allocation is performed, stack must have enough capacity
			* TODO - possibly change names (push_front, push_back idioms better) (06/10/2025)
			**/
			void push_bottom(T d);

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
			* @brief Replaces the bottom element of the stack with the top element of the stack.
			*		 (the bottom element is lost)
			*
			*		 I. The top element is moved to the bottom of the stack
			* @details: no deallocation is performed
			**/
			void pop_bottom();

			/**
			* @brief Removes the element at position pos form the underlying array.
			*		 (the element is lost)
			*
			*		 I. The top element is moved to position pos
			* @param pos: position of the element to remove
			* @details: no deallocation is performed
			**/
			void erase(int pos);

			/**
			* @brief Removes all elements from the stack.
			*		 (no deallocation)
			* @details: no deallocation is performed
			**/
			void erase() noexcept { nE_ = 0; }

			/**
			* @brief Provides fast-rollback to a previous mark.
			* @details: no deallocation is performed
			**/
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
			std::size_t nE_ = 0;								//number of elements, points to the next position to fill
			T* stack_ = nullptr;								//underlying C-array 		
			std::size_t cap_ = 0;								//capacity of the underlying array

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