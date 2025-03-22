/**
 * @file common_types.h  
 * @brief set of useful types 
 * @date ?
 * @last_update 17/01/25
 * @author pss
 **/

#ifndef __COMMON_TYPES_H__
#define	__COMMON_TYPES_H__

#include <iostream>

namespace com {

	////////////////////////
	//
	// struct stack_t
	//
	// (simple stack implemented by a C-array)
	//
	////////////////////////

	template <class T>
	struct stack_t {
	
		///////
		//construction / destruction
		stack_t		();
explicit stack_t	(int MAX_SIZE);

		//copy semantics are disallowed
		stack_t				(const stack_t& )	= delete;
		stack_t& operator = (const stack_t& )	= delete;

		//move semantics are allowed
		stack_t				(stack_t&&)	 noexcept;
		stack_t& operator = (stack_t&&)	 noexcept;
			
		~stack_t	()					{ clear(); }

		//setters and getters 
	const T& at			(int pos)	const			{ return stack_[pos]; }
	const T& top		()			const;
	const T& bottom		()			const			{ return stack_[0]; }

		 T& at			(int pos)					{ return stack_[pos]; }
		 T& top			();
		 T& bottom		()							{ return stack_[0]; }
		 	
		std::size_t size()							{ return nE_; }

		////////////
		// memory
		
		/**
		* @brief Resets the stack to a new size, previos elements are destroyed. 
		*        The new elements are constructed with the default constructor
		**/
		void reset		(int MAX_SIZE);

	private:
		/**
		* @brief Deallocates memory (used by the destructor)
		*	     Use erase to clean the stack without deallocating memory
		**/
		void clear		();					
		
		//////////////
		//basic operations (no memory management)
	public:
				
		/**
		* @brief Places element at the top of the stack
		*	     (last position in the underlying array)
		**/
		void push		(T d);
				
		/**
		* @brief Places element at the bottom of the stack
		*	     (first position in the underlying array)
		* 
		*		 I. The bottom element is moved to the top of the stack
		**/
		void push_bottom(T d);
					
		/**
		* @brief Rremoves the top element from the stack 
		*	     (the element is lost)
		**/
		void pop		();
						
		/**
		* @brief Removes the top nb elements from the top of the stack.
		*		 (the elements are lost)
		* @param nb: number of elements to remove
		*	   
		**/
		void pop		(std::size_t nb);
			
		/**
		* @brief Removes the bottom element of the stack.  from the top of the stack.
		*		 (the element is lost)
		*
		*		 I. The top element is moved to the bottomof the stack
		**/
		void pop_bottom	();
				
		/**
		* @brief Removes the element at position pos form the underlying array.
		*		 (the element is lost)
		*
		*		 I. The top element is moved to position pos
		* @param pos: position of the element to remove
		**/
		void erase		(int pos);
					
		/**
		* @brief Removes all elements from the stack.
		*		 (no deallocation)
		**/
		void erase		()					{ nE_ = 0; }									

		/////////////////
		//boolean operations
		
		bool empty		()		const		{ return (nE_ == 0); }

		/////
		//I/O

		std::ostream& print(std::ostream& o) const;
		friend std::ostream& operator << (std::ostream& o, const stack_t& s)	{ s.print(o); return o; }
	
		/////////////////////
		// data members
		
		std::size_t nE_;								//number of elements, used as internal index
		T* stack_;										//underlying C-array (TODO - raw pointer ¿change to a std::vector?)		
#ifdef DEBUG_STACKS
		int MAX_;										//maximum number of elements	
#endif

	};
	

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





#endif