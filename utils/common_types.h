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
	// (my simple stack)
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
		T get_elem	(int pos)	const			{ return stack[pos]; }
		T first		()			const			{ return stack[0]; }										
		T last		()			const; 
		

		unsigned int size()				{ return pt_; }

		////////////
		// allocation

		//for backward compatibility
		void init		(int MAX_SIZE);

		void reset		(int MAX_SIZE);

	private:
		void clear		();					
		
		//////////////
		//basic operations
	public:
		void push_back	(T d);
		void push_front	(T d);

		T pop			();

		/* removes the last NB elements from the stack (lost!) */
		int pop			(std::size_t nb);

		/* removes the first element from the stack (lost!), swaps last elememt */
		int pop_swap	();
		
		//removes a single element at pos (pos must be grater than 0)
		void erase		(int pos);

		//clears the stack
		void erase		()					{ pt_ = 0; }

		//clears nb elements from the stack-TODO@, REFACTOR
		// void erase_pop (int nb)			{pt_-=nb;}																	

		/////////////////
		//boolean operations
		bool empty		()		const		{ return (pt_ == 0); }

		/////
		//I/O

		std::ostream& print(std::ostream& o) const;
		friend std::ostream& operator << (std::ostream& o, const stack_t& s)	{ s.print(o); return o; }
			

		/////////////////////
		// data members
		
		//static const int EMPTY_VAL = -1;
		int pt_;							//stack pointer[0, MAX_SIZE-1], always points to a free position (top of the stack) 
		T* stack;
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