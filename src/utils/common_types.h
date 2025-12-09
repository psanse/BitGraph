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
		// struct stack
		//
		// (very lightweight stack implemented by a C-array)
		//
		////////////////////////

		template <class T>
		struct stack {

			using value = T;

			///////
			//construction / destruction
			stack();
			explicit stack(std::size_t MAX_SIZE);

			//copy semantics are disallowed
			stack(const stack&) = delete;
			stack& operator = (const stack&) = delete;

			//move semantics are allowed
			stack(stack&&)	 noexcept;
			stack& operator = (stack&&)	 noexcept;

			~stack() { clear(); }

			//setters and getters 
			const T& at(int pos)	const { return stack_[pos]; }
			const T& top()			const;
			const T& bottom()		const { return stack_[0]; }

			T& at(int pos) { return stack_[pos]; }
			T& top();
			T& bottom() { return stack_[0]; }

			std::size_t size() { return nE_; }
			std::size_t capacity() const { return cap_; }
			bool full() const { return nE_ >= cap_; }

			////////////
			// memory

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
			**/
			void push(T d);

			/**
			* @brief Places element at the bottom of the stack
			*	     (first position in the underlying array)
			*
			*		 I. The bottom element is moved to the top of the stack			
			*
			* TODO - possibly change names (push_front, push_back idioms better) (06/10/2025)
			**/
			void push_bottom(T d);

			/**
			* @brief Removes the top element from the stack
			*	     (the element is lost)
			**/
			void pop();

			/**
			* @brief Removes the top nb elements from the top of the stack.
			*		 (the elements are lost)
			* @param nb: number of elements to remove
			*
			**/
			void pop(std::size_t nb);

			/**
			* @brief Replaces the bottom element of the stack with the top element of the stack.
			*		 (the bottom element is lost)
			*
			*		 I. The top element is moved to the bottom of the stack
			**/
			void pop_bottom();

			/**
			* @brief Removes the element at position pos form the underlying array.
			*		 (the element is lost)
			*
			*		 I. The top element is moved to position pos
			* @param pos: position of the element to remove
			**/
			void erase(int pos);

			/**
			* @brief Removes all elements from the stack.
			*		 (no deallocation)
			**/
			void erase() { nE_ = 0; }

			/////////////////
			//boolean operations

			bool empty()		const { return (nE_ == 0); }

			////////////////////
			//I/O

			std::ostream& print(std::ostream& o) const;
			
			template<class U>
			friend std::ostream& operator<< (std::ostream& o, const stack<U>& s);

			/////////////////////
			// data members

			std::size_t nE_ = 0;								//number of elements, points to the next position to fill
			T* stack_ = nullptr;								//underlying C-array 		
			std::size_t cap_ = 0;								//capacity of the underlying array

		};

		////////////////////////
		//
		// struct range_t
		//
		// (interval of positive integers - typically a range of vertices)
		///////////////////////

		struct range_t {
			static const int noRange = -1;
			int vl, vh;								//vl: low vertex/value, vh: high vertex/value

			range_t(int lh = noRange, int rh = noRange) :
				vl{ lh }, vh{ rh }
			{
			}

			//boolean operations

			/**
			* @brief: determines an empty range
			* @returns: True if the range has at least one NO_RANGE value False otherwise
			**/
			bool is_empty() const { return (vl == noRange || vh == noRange); }

			//I/O

			/**
			* @brief: streams the range in the format [@vl, @vh]
			**/
			std::ostream& print(std::ostream& os = std::cout) const {
				os << "[" << vl << "," << vh << "]";
				return os;
			}
		};

		inline
			bool operator == (const range_t& lhs, const range_t& rhs) {
			return (lhs.vl == rhs.vl && lhs.vh == rhs.vh);
		}
		

	}

	//not sure if third party code will use this features (TODO- CHECK)
	using com::range_t;
	using com::stack;
	using com::operator==;

}//end namespace bitgraph


namespace bitgraph {
	namespace com {
				
		template<class U>
		inline
		std::ostream& operator<< (std::ostream& o, const stack<U>& s) { s.print(o); return o; }
	}

	using com::operator<<;
}


#endif