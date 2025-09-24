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

using namespace bitgraph;


template<class T>
bitgraph::stack<T>::stack() :
	nE_(0), stack_(nullptr), cap_(0)
{
#ifdef DEBUG_STACKS
	MAX_ = 0;
#endif   
}

template<class T>
bitgraph::stack<T>::stack(int MAX_SIZE) : 
	nE_(0), stack_(nullptr), cap_(0)
{
	try {
		stack_ = new T[MAX_SIZE];
		cap_ = static_cast<std::size_t>(MAX_SIZE);
	}
	catch (...) {
		LOGG_ERROR("bad_alloc - stack<T>::stack");
		throw;
	}

#ifdef DEBUG_STACKS
	MAX_ = MAX_SIZE;
#endif
}

template<class T>
bitgraph::stack<T>::stack(bitgraph::stack<T>&& s) noexcept
	: nE_(s.nE_), stack_(s.stack_), cap_(s.cap_)
#ifdef DEBUG_STACKS
	, MAX_(s.MAX_)
#endif
{
	s.stack_ = nullptr;
	s.nE_ = 0;
	s.cap_ = 0;
#ifdef DEBUG_STACKS
	s.MAX_ = 0;
#endif
}

template<class T>
bitgraph::stack<T>& bitgraph::stack<T>::operator = (bitgraph::stack<T>&& s) noexcept
{
	delete[] stack_;
	nE_ = s.nE_;
	stack_ = s.stack_;
	cap_ = s.cap_;
#ifdef DEBUG_STACKS
	MAX = s.MAX_;
#endif
	s.stack_ = nullptr;
	s.nE_ = 0;
	s.cap_ = 0;
#ifdef DEBUG_STACKS
	s.MAX_ = 0;
#endif
	return *this;
}

template<class T>
void bitgraph::stack<T>::reset(int MAX_SIZE) {
	delete[] stack_;
	nE_ = 0;
	cap_ = 0;
	try {
		stack_ = new T[MAX_SIZE];
		cap_ = static_cast<std::size_t>(MAX_SIZE);
	}
	catch (...) {
		LOG_ERROR("bad_alloc - stack<T>::reset");
		throw;
	}

#ifdef DEBUG_STACKS
	MAX_ = MAX_SIZE;
#endif
}

template<class T>
void bitgraph::stack<T>::clear() {
	delete[] stack_;
	stack_ = nullptr;
	nE_ = 0;
	cap_ = 0;
#ifdef DEBUG_STACKS
	MAX_ = 0;
#endif
}

template<class T>
void bitgraph::stack<T>::push(T d) {
	assert(stack_ != nullptr && "stack not initialized. Call reset(MAX_SIZE) or use sized ctor - bitgraph::stack<T>::push");
	assert(nE_ < cap_ && "stack overflow - bitgraph::stack<T>::push");

	/////////////////////////////
	stack_[nE_++] = std::move(d);
	/////////////////////////////

#ifdef DEBUG_STACKS
	if (nE_ > static_cast<std::size_t>MAX_) {
		LOGG_INFO("bizarre stack with size: ", nE_, " and max size: ", MAX_);
		LOG_INFO("-bitgraph::stack<T>::push(T d)");
		LOG_INFO("press any key to continue");
		std::cin.get();
	}
#endif
}

template<class T>
void bitgraph::stack<T>::push_bottom(T d) {
	assert(stack_ != nullptr && "stack not initialized. Call reset(MAX_SIZE) or use sized ctor - bitgraph::stack<T>::push_bottom");
	assert(nE_ < cap_ && "stack overflow - bitgraph::stack<T>::push_bottom");

	//stack empty case
	if (nE_ == 0) {
		stack_[nE_++] = std::move(d);
	}
	else {

		//stack non-empty case, moves bottom element to top
		T temp = stack_[0];
		stack_[0] = std::move(d);
		stack_[nE_++] = std::move(temp);
	}
#ifdef DEBUG_STACKS
	if (nE_ > static_cast<std::size_t>MAX_) {    //*** no checking against N
		LOGG_INFO("bizarre stack with size: ", nE_, " and max size: ", MAX_);
		LOG_INFO("-bitgraph::stack<T>::push_bottom(T d)");
		LOG_INFO("press any key to continue");
		std::cin.get();
	}
#endif
}

template<class T>
const T& bitgraph::stack<T>::top() const
{
	assert(nE_ > 0 && "stack::top on empty stack");
	return stack_[nE_ - 1];
}

template<class T>
T& bitgraph::stack<T>::top()
{
	assert(nE_ > 0 && "stack::top on empty stack");
	return stack_[nE_ - 1];
}

template<class T>
void bitgraph::stack<T>::pop() {
	assert(nE_ > 0 && "stack::pop on empty stack");
	--nE_;
}

template<class T>
void bitgraph::stack<T>::pop(std::size_t nb) {
	assert(nE_ >= nb && "stack::pop(nb) removes more elements than present");
	nE_ -= nb;	
}

template<class T>
void bitgraph::stack<T>::pop_bottom() {
	assert(nE_ > 0 && "stack::pop_bottom on empty stack");
	stack_[0] = std::move(stack_[--nE_]);
}

template<class T>
void bitgraph::stack<T>::erase(int pos) {
	assert(pos >= 0 && static_cast<std::size_t>(pos) < nE_ && "stack::erase out of bounds");
	stack_[pos] = std::move(stack_[--nE_]);
}

//I/O
template<class T>
std::ostream& bitgraph::stack<T>::print(std::ostream& o) const {
	o << "[";
	for (auto i = 0; i < nE_; ++i) {
		o << stack_[i] << " ";
	}
	o << "]" << "[" << nE_ << "]" << std::endl;
	return o;
}

/////////////////////////////////
// declaration of valid types 

template struct bitgraph::stack<int>;
template struct  bitgraph::stack<double>;

/////////////////////////////////








