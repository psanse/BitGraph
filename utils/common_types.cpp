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
	stack_t<T>::stack_t() :nE_(0), stack_(nullptr) {
#ifdef DEBUG_STACKS
		int MAX_ = 0;
#endif   
	}

	template<class T>
	stack_t<T>::stack_t(int MAX_SIZE) : stack_(nullptr) {
		try {
			stack_ = new T[MAX_SIZE];
		}
		catch (std::bad_alloc& ba) {
			LOGG_ERROR("bad_alloc - stack_t<T>::stack_t");
			throw ba;
		}
		nE_ = 0;
#ifdef DEBUG_STACKS
		MAX_ = MAX_SIZE;
#endif
	}

	//template<class T>
	//T stack_t<T>::last() const
	//{
	//	if (nE_ == 0) { 
	//		return stack[0];
	//	}
	//	else {
	//		return stack[nE_ - 1];
	//	}
	//}
		
//	template<class T>
//	void stack_t<T>::init (int MAX_SIZE) {
//		delete[] stack;
//		nE_ = 0;
//
//		try {
//			stack = new T[MAX_SIZE];
//		}
//		catch (std::bad_alloc& ba) {
//			LOGG_ERROR("bad_alloc - stack_t<T>::init");
//			throw ba;
//		}
//
//#ifdef DEBUG_STACKS
//		MAX_ = MAX_SIZE;
//#endif
//	}

	template<class T>
	void stack_t<T>::reset (int MAX_SIZE) {
		nE_ = 0;
		delete[] stack_;
		try {
			stack_ = new T[MAX_SIZE];
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
		delete[] stack_;
		stack_ = nullptr;
		nE_ = 0;
#ifdef DEBUG_STACKS
		MAX_ = 0;
#endif
	}

	template<class T>
	void stack_t<T>::push(T d) {
		stack_[nE_++] = std::move(d);
#ifdef DEBUG_STACKS
		if (nE_ > MAX_) {
			LOG_INFO("bizarre stack with size: ", nE_, " and max size: ", MAX_);
			LOG_INFO("press any key to continue");
			std::cin.get();
		}
#endif
	}

	template<class T>
	void stack_t<T>::push_bottom(T d) {
		if (nE_ == 0) {
			stack_[nE_++] = std::move(d);
		}
		else {
			T temp = stack_[0];
			stack_[0] = d;
			stack_[nE_++] = std::move(temp);
		}
#ifdef DEBUG_STACKS
		if (nE_ > MAX_) {    //*** no checking against N
			LOG_INFO("bizarre stack with size: ", nE_, " and max size: ", MAX_);
			LOG_INFO("press any key to continue");
			std::cin.get();
		}
#endif
	}

	template<class T>
	const T& stack_t<T>::top() const
	{
		if (nE_ == 0) {
			return stack_[0];
		}
		else {
			return stack_[nE_ - 1];
		}		
	}

	template<class T>
	void stack_t<T>::pop() {
		assert(nE_ > 0);
		--nE_;
		//return stack_[--nE_];	
	}

	template<class T>
	void stack_t<T>::pop (std::size_t nb) {
		assert(nE_ >= nb);
		nE_ -= nb;
		//return nb;
	}
		
	template<class T>
	void stack_t<T>::pop_bottom () {
		assert(nE_ > 0);
		stack_[0] = stack_[--nE_];
		//return nE_;

		/*if (nE_ <= 1) { return (nE_ = 0); }
		else {
			stack_[0] = stack_[--nE_];
			return nE_;
		}*/
	}


	template<class T>
	void stack_t<T>::erase(int pos) {
		assert(pos > 0);
		stack_[pos] = stack_[--nE_];
		//if (nE_ > 0) {
		//	stack_[pos] = stack_[--nE_];
		//	//stack_[nE_] = EMPTY_VAL;
		//}
	}

	//I/O
	template<class T>
	std::ostream& stack_t<T>::print(std::ostream& o) const {
		o << "[";
		for (int i = 0; i < nE_; i++) {
			o << stack_[i] << " ";
		}
		o << "]" << "[" << nE_ << "]" << std::endl;
		return o;
	}

}

/////////////////////////////////
// declaration of valid types 

template struct ::com::stack_t<int>;
template struct ::com::stack_t<double>;

/////////////////////////////////








