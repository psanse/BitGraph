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

///////////////////////
// useful aliases 

namespace bitgraph {

	using vint = std::vector<int>;
	using vdob = std::vector<double>;

}

namespace bitgraph {
	
	namespace com {

		////////////////////////
		//
		// struct stack_t
		//
		// (very lightweight stack implemented by a C-array)
		//
		////////////////////////

		template <class T>
		struct stack_t {

			///////
			//construction / destruction
			stack_t();
			explicit stack_t(int MAX_SIZE);

			//copy semantics are disallowed
			stack_t(const stack_t&) = delete;
			stack_t& operator = (const stack_t&) = delete;

			//move semantics are allowed
			stack_t(stack_t&&)	 noexcept;
			stack_t& operator = (stack_t&&)	 noexcept;

			~stack_t() { clear(); }

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
			void reset(int MAX_SIZE);

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
			friend std::ostream& operator<< (std::ostream& o, const stack_t<U>& s);

			/////////////////////
			// data members

			std::size_t nE_;								//number of elements, points to the next position to fill
			T* stack_;										//underlying C-array 		
			std::size_t cap_;								//capacity of the underlying array
#ifdef DEBUG_STACKS
			int MAX_;										//capacity	
#endif

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


		///////////////////////
		//
		// class my_array
		//
		// my old C_array wrraper
		// 
		// TODO - deprecated, possibly remove (17(01/25)
		//
		////////////////////////

		/*template<class T>
		class my_array {
			T* pt;
			size_t sz;
		public:
			my_array	(size_t size, T val) :sz(size) { pt = new T[sz];  for (int i = 0; i < sz; i++) { pt[i] = val; } }
			~my_array() { delete[] pt; }
			T& operator [](int idx) { return pt[idx]; }
			const T& operator [](int idx) const { return pt[idx]; }
			T at(int idx) { return pt[idx]; }
			T* get_array() { return pt; };
			size_t size() { return sz; }
		};*/

	}

	//not sure if third party code will use this features (TODO- CHECK)
	using com::range_t;
	using com::stack_t;
	using com::operator==;

}//end namespace bitgraph


namespace bitgraph {
	namespace com {
				
		template<class U>
		inline
		std::ostream& operator<< (std::ostream& o, const stack_t<U>& s) { s.print(o); return o; }
	}

	using com::operator<<;
}


#endif