/**
 * @file common_types.cpp
 * @brief implementation of common types in common_types.h
 * @date ?
 * @last_update 17/01/25
 * @author pss
 **/

#include "common_types.h"
#include "logger.h"
#include <cassert>
#include <string>
#include <vector>

namespace com {
	

	template<class T>
	stack_t<T>::stack_t() :pt_(0), stack(nullptr) {
#ifdef DEBUG_STACKS
		int MAX_ = 0;
#endif   
	}

	template<class T>
	stack_t<T>::stack_t(int MAX_SIZE) :stack(nullptr) {
		try {
			stack = new T[MAX_SIZE];
		}
		catch (std::bad_alloc& ba) {
			LOGG_ERROR("bad_alloc - stack_t<T>::stack_t");
			throw ba;
		}
		pt_ = 0;
#ifdef DEBUG_STACKS
		MAX_ = MAX_SIZE;
#endif
	}

	template<class T>
	T stack_t<T>::last() const
	{
		if (pt_ == 0) { 
			return stack[0];
		}
		else {
			return stack[pt_ - 1];
		}
	}
		
	template<class T>
	void stack_t<T>::init(int MAX_SIZE) {
		delete[] stack;
		pt_ = 0;

		try {
			stack = new T[MAX_SIZE];
		}
		catch (std::bad_alloc& ba) {
			LOGG_ERROR("bad_alloc - stack_t<T>::init");
			throw ba;
		}

#ifdef DEBUG_STACKS
		MAX_ = MAX_SIZE;
#endif
	}

	template<class T>
	void stack_t<T>::reset(int MAX_SIZE) {
		pt_ = 0;
		delete[] stack;
		try {
			stack = new T[MAX_SIZE];
		}
		catch (std::bad_alloc& ba) {
			LOGG_ERROR("bad_alloc - stack_t<T>::reset");
			throw ba;
		}

#ifdef DEBUG_STACKS
		MAX_ = MAX_SIZE;
#endif
	}

	template<class T>
	void stack_t<T>::clear() {
		delete[] stack;
		stack = nullptr;
		pt_ = 0;
#ifdef DEBUG_STACKS
		MAX_ = 0;
#endif
	}

	template<class T>
	void stack_t<T>::push_back(T d) {
		stack[pt_++] = d;
#ifdef DEBUG_STACKS
		if (pt_ > MAX_) {
			LOG_INFO("bizarre stack with size: ", pt_, " and max size: ", MAX_);
			LOG_INFO("press any key to continue");
			std::cin.get();
		}
#endif
	}

	template<class T>
	void stack_t<T>::push_front(T d) {
		if (pt_ == 0) {
			stack[pt_++] = d;
		}
		else {
			T t = stack[0];
			stack[0] = d;
			stack[pt_++] = t;
		}
#ifdef DEBUG_STACKS
		if (pt_ > MAX_) {    //*** no checking against N
			LOG_INFO("bizarre stack with size: ", pt_, " and max size: ", MAX_);
			LOG_INFO("press any key to continue");
			std::cin.get();
		}
#endif
	}

	template<class T>
	T stack_t<T>::pop() {
		assert(pt_ > 0);
		return stack[--pt_];	
	}
		
	template<class T>
	int stack_t<T>::pop_swap () {
		assert(pt_ > 0);
		stack[0] = stack[--pt_];
		return pt_;

		/*if (pt_ <= 1) { return (pt_ = 0); }
		else {
			stack[0] = stack[--pt_];
			return pt_;
		}*/
	}

	template<class T>
	int stack_t<T>::pop (std::size_t nb) {
		assert(pt_ >= nb);
		pt_ -= nb;
		return nb;
	}

	template<class T>
	void stack_t<T>::erase(int pos) {
		assert(pos > 0);
		stack[pos] = stack[--pt_];
		//if (pt_ > 0) {
		//	stack[pos] = stack[--pt_];
		//	//stack[pt_] = EMPTY_VAL;
		//}
	}

	//I/O
	template<class T>
	std::ostream& stack_t<T>::print(std::ostream& o) const {
		o << "[";
		for (int i = 0; i < pt_; i++) {
			o << stack[i] << " ";
		}
		o << "]" << "[" << pt_ << "]" << std::endl;
		return o;
	}

}

/////////////////////////////////
// declaration of valid types 

template struct ::com::stack_t<int>;
template struct ::com::stack_t<double>;

/////////////////////////////////








