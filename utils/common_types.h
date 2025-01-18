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
	// (simple stack implemented as a std::vector)
	//
	////////////////////////

	template <class T>
	struct stack_t {
	
		///////
		//construction / destruction
		stack_t		();
explicit stack_t	(int MAX_SIZE);

		//TODO other constructors, assignment operations...
		stack_t				(const stack_t& )	= delete;
		stack_t& operator = (const stack_t& )	= delete;
		stack_t				(stack_t&&)			= default;			//CHECK
		stack_t& operator = (stack_t&&)			= default;
			
		~stack_t	()					{ clear(); }

		//setters and getters
	const T& at			(int pos)	const			{ return stack_[pos]; }
	const T& top		()			const;
	const T& bottom		()			const			{ return stack_[0]; }

		 T& at			(int pos)					{ return stack_[pos]; }
		 T& top			();
		 T& bottom		()							{ return stack_[0]; }

		
		//T first		()			const			{ return stack_[0]; }										
		//T last		()			const; 
		
		std::size_t size()							{ return nE_; }

		////////////
		// allocation

		//for backward compatibility
		//void init		(int MAX_SIZE);

		void reset		(int MAX_SIZE);

	private:
		//deallocates memory - use erase to clean the stack without deallocating memory
		void clear		();					
		
		//////////////
		//basic operations
	public:

		//places element at the top of the stack
		void push		(T d);

		//places element at the bottom of the stack, and moves the bottom element to the top 
		void push_bottom(T d);
				
		/* removes the top element from the stack (lost!) */
		void pop		();

		/* removes the top NB elements from the top of the stack (lost!) */
		void pop		(std::size_t nb);

		/* removes the bottom element from the top of the stack (lost!), swaps last elememt */
		void pop_bottom	();
		
		//removes a single element at pos (pos must be grater than 0), moves the last element to pos
		void erase		(int pos);

		//removes all elements from the stack
		void erase		()					{ nE_ = 0; }

		//clears nb elements from the stack-TODO@, REFACTOR
		// void erase_pop (int nb)			{nE_-=nb;}																	

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
		T* stack_;
#ifdef DEBUG_STACKS
		int MAX_;
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