/**
 * @file fixed_stack_imp.h
 * @brief Provides template implementations for the FixedStack container.
 *
 * This file contains the out-of-class definitions of FixedStack member
 * functions. It is included automatically at the end of fixed_stack.h and
 * should not normally be included directly by client code.
 *
 * All definitions belong to bitgraph::utils.
 *
 * @author Pablo San Segundo
 * @date Last updated: 24/09/2026
 */

#ifndef BITGRAPH_UTILS_FIXED_STACK_IMP_H
#define	BITGRAPH_UTILS_FIXED_STACK_IMP_H


namespace bitgraph {
	
	namespace utils {
			

		template<class ValueT>
		FixedStack<ValueT>::FixedStack(size_type capacity)
		{
			reset(capacity);
		}

		template<class ValueT>
		FixedStack<ValueT>::FixedStack(FixedStack&& other) noexcept
			: nE_(std::exchange(other.nE_, 0)),
			cap_(std::exchange(other.cap_, 0)),
			stack_(std::move(other.stack_))
		{
		}

		template<class ValueT>
		FixedStack<ValueT>& FixedStack<ValueT>::operator=(FixedStack&& other) noexcept
		{
			if (this != &other) {
				stack_ = std::move(other.stack_);
				nE_ = std::exchange(other.nE_, 0);
				cap_ = std::exchange(other.cap_, 0);
			}

			return *this;
		}
				

		template<class ValueT>
		void bitgraph::FixedStack<ValueT>::reset(std::size_t capacity)
		{

			std::unique_ptr<value_type[]> new_storage =
				capacity == 0
				? nullptr
				: std::make_unique<value_type[]>(capacity);
			
			// Commit only after allocation succeeds.
			stack_ = std::move(new_storage);
			nE_ = 0;
			cap_ = capacity;
		}
			

		template<class ValueT>
		void bitgraph::FixedStack<ValueT>::push(value_type value)
		{
			assert(
				nE_ < cap_ &&
				"FixedStack overflow in FixedStack::push().");

			stack_[nE_] = value;
			++nE_;
		}

		template<class ValueT>
		void bitgraph::FixedStack<ValueT>::push_bottom_swap(value_type value)
		{
			assert(
				nE_ < cap_ &&
				"FixedStack overflow in FixedStack::push_bottom_swap().");

			//FixedStack empty case
			if (nE_ == 0) {
				stack_[0] = value;
				nE_ = 1;
				return;
			}

			// Move the previous bottom into the new top position.
			stack_[nE_] = stack_[0];

			// Install the new bottom.
			stack_[0] = value;

			++nE_;
		}

		template<class ValueT>
		const ValueT& bitgraph::FixedStack<ValueT>::top() const
		{
			assert (
				!empty() 
				&& "FixedStack::top on empty FixedStack");
			return stack_[nE_ - 1];
		}

		template<class ValueT>
		ValueT& bitgraph::FixedStack<ValueT>::top() 
		{
			assert(
				!empty()
				&& "FixedStack::top on empty FixedStack");
			return stack_[nE_ - 1];
		}

		template<class ValueT>
		void bitgraph::utils::FixedStack<ValueT>::pop() noexcept
		{
			assert(
				nE_ > 0 &&
				"FixedStack::pop() called on an empty stack.");

			--nE_;
		}

		template<class ValueT>
		void bitgraph::FixedStack<ValueT>::pop(size_type count) noexcept 
		{
			assert(
				count <= nE_ &&
				"FixedStack::pop(count) removes more elements than the stack contains.");

			nE_ -= count;
		}

		template<class ValueT>
		void bitgraph::utils::FixedStack<ValueT>::pop_bottom_swap() noexcept
		{
			assert(
				nE_ > 0 &&
				"FixedStack::pop_bottom_swap() called on an empty stack.");

			--nE_;

			if (nE_ != 0) {
				// Replace the removed bottom with the previous top.
				stack_[0] = stack_[nE_];
			}
		}

		template<class ValueT>
		void bitgraph::utils::FixedStack<ValueT>::erase_swap(size_type position) noexcept
		{
			assert(
				position < nE_ &&
				"FixedStack::erase_swap() position is out of bounds.");

			--nE_;

			if (position != nE_) {
				// Replace the removed element with the previous top.
				stack_[position] = stack_[nE_];
			}
		}
			
			

		template<class ValueT>
		std::ostream& bitgraph::FixedStack<ValueT>::print(
			std::ostream& out) const
		{
			out << '[';

			for (size_type position = 0; position < nE_; ++position) {
				if (position != 0) {
					out << ' ';
				}

				out << stack_[position];
			}

			out << "] [" << nE_ << ']';

			return out;
		}


	}//end namespace utils
		
		
}//end namespace bitgraph





#endif  // BITGRAPH_UTILS_FIXED_STACK_IMP_H