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
bitgraph::FixedStack<T>::FixedStack(std::size_t MAX_SIZE) : 
	nE_(0), stack_(MAX_SIZE ? std::make_unique<T[]>(MAX_SIZE) : nullptr), cap_(MAX_SIZE)
{
	static_assert(std::is_trivially_destructible<T>::value,
				"FixedStack requires trivially destructible T.");	
}

//template<class T>
//bitgraph::FixedStack<T>::FixedStack(bitgraph::FixedStack<T>&& s) noexcept
//	: nE_(s.nE_), stack_(s.stack_), cap_(s.cap_)
//{
//	s.stack_ = nullptr;
//	s.nE_ = 0;
//	s.cap_ = 0;
//}

//template<class T>
//bitgraph::FixedStack<T>& bitgraph::FixedStack<T>::operator = (bitgraph::FixedStack<T>&& s) noexcept
//{
//	if (this == &s) return *this;          // self-move guard
//
//	delete[] stack_;
//	nE_ = s.nE_;
//	stack_ = s.stack_;
//	cap_ = s.cap_;
//
//	s.stack_ = nullptr;
//	s.nE_ = 0;
//	s.cap_ = 0;
//
//	return *this;
//}

template<class T>
void bitgraph::FixedStack<T>::reset(std::size_t MAX_SIZE) {

	// 1) Allocate first (may throw). No state changes yet.
	std::unique_ptr<T[]> new_stack;

	try {
		new_stack = (MAX_SIZE ? std::make_unique<T[]>(MAX_SIZE) : nullptr);
	}
	catch (const std::bad_alloc&) {
		LOGG_ERROR("bad_alloc - FixedStack<T>::reset (MAX_SIZE=", MAX_SIZE, ")");
		throw; 
	}

	stack_ = std::move(new_stack);
	nE_ = 0;
	cap_ = MAX_SIZE;
	
}

template<class T>
void bitgraph::FixedStack<T>::deallocate() noexcept
{
	stack_.reset();
	nE_ = 0;
	cap_ = 0;
}

template<class T>
void bitgraph::FixedStack<T>::push(T d)
{
	assert(stack_ != nullptr && "FixedStack not initialized. Call reset(MAX_SIZE) or use sized ctor - bitgraph::FixedStack<T>::push");
	assert(nE_ < cap_ && "FixedStack overflow - bitgraph::FixedStack<T>::push");

	/////////////////////////////
	stack_[nE_++] = std::move(d);
	/////////////////////////////
}

template<class T>
void bitgraph::FixedStack<T>::push_bottom_swap(T d)
{
	assert(stack_ != nullptr && "FixedStack not initialized. Call reset(MAX_SIZE) or use sized ctor - bitgraph::FixedStack<T>::push_bottom_swap");
	assert(nE_ < cap_ && "FixedStack overflow - bitgraph::FixedStack<T>::push_bottom_swap");

	//FixedStack empty case
	if (nE_ == 0) {
		stack_[nE_++] = std::move(d);
	}
	else {

		//FixedStack non-empty case, moves bottom element to top
		T temp = stack_[0];
		stack_[0] = std::move(d);
		stack_[nE_++] = std::move(temp);
	}
}

template<class T>
const T& bitgraph::FixedStack<T>::top() const
{
	assert(nE_ > 0 && "FixedStack::top on empty FixedStack");
	return stack_[nE_ - 1];
}

template<class T>
T& bitgraph::FixedStack<T>::top()
{
	assert(nE_ > 0 && "FixedStack::top on empty FixedStack");
	return stack_[nE_ - 1];
}

template<class T>
void bitgraph::FixedStack<T>::pop() {
	assert(nE_ > 0 && "FixedStack::pop on empty FixedStack");
	--nE_;
}

template<class T>
void bitgraph::FixedStack<T>::pop(std::size_t nb) {
	assert(nE_ >= nb && "FixedStack::pop(nb) removes more elements than present");
	nE_ -= nb;	
}

template<class T>
void bitgraph::FixedStack<T>::pop_bottom_swap() {
	assert(nE_ > 0 && "FixedStack::pop_bottom_swap on empty FixedStack");
	stack_[0] = std::move(stack_[--nE_]);
}

template<class T>
void bitgraph::FixedStack<T>::erase(int pos) {
	assert(pos >= 0 && static_cast<std::size_t > (pos) < nE_ && "FixedStack::erase out of bounds");
	stack_[pos] = std::move(stack_[--nE_]);
}

//I/O
template<class T>
std::ostream& bitgraph::FixedStack<T>::print(std::ostream& o) const {
	o << "[";
	for (auto i = 0u; i < nE_; ++i) {
		o << stack_[i] << " ";
	}
	o << "]" << "[" << nE_ << "]" << std::endl;
	return o;
}

/////////////////////////////////
// declaration of valid types 

template struct bitgraph::FixedStack<int>;
template struct  bitgraph::FixedStack<double>;

/////////////////////////////////








