 /**
  * @file bbutils.h
  * @brief contains composite bitset classes and other utility functions for the BITSCAN library
  * @author pss
  * @details created 2017, last_update 08/12/2025
 **/

#ifndef  __BBUTILS_H__
#define  __BBUTILS_H__

#include "utils/common.h"
#include "utils/logger.h"
#include "bitscan/bbset.h"
#include <array>
#include <functional>

//aliases
namespace bitgraph {
	using bbo = BBObject;
}

namespace bitgraph {
	
	/**
	* @brief Converts a bitset to a vector of integers of size the
	*		 population count of the bitset hierarchy. The bitset is not modified.
	* @details: function next_bit() is used to extract the bits
	**/
	template<class BitSetT>
	std::vector<int> to_vector(BitSetT& bbn);

	/**
	* @brief Creates a random BITBOARD (64-bits) with density p of 1-bits
	*
	* @TODO: Not precise generator (but acceptable since the population is at most 64).
	*		 better to generate a collection of 64 elements with 64*p 1s and shuffle it
	**/
	BITBOARD random_bitblock(double p);

	/**
	* @brief extracts the first k 1-bits in the bitset @bb
	* @param k: the number of bits to find
	* @param bb: the input bitset
	* @param lv: the output vector of integers
	* @returns the number of bits found in lv
	* @details: used in BBMWCP for upper bound computation
	**/
	template<class BitSetT>
	int extract_first_k(int k, BitSetT& bb, std::vector<int>& lv);

}//end namespace bitgraph

namespace bitgraph {
	
	//////////////////////
	// 
	// BitSetWithPC struct
	// 
	// A very simple wrapper for any type of bitset of the BBObject hierarchy with CACHED size 
	// FEATURE: The population count @pc_ cached can differ from the real population size in bb_count()				
	//			For example, in the P-MAX-SAT-BOUND application, color bitsets are reduced by reducing the
	//			the pc_ value, but the bitset itself is not modified.
	/////////////////////

	template <class BitSetT>
	struct BitSetWithPC {

		using bitset_type = BitSetT;
		using count_t = int;

		//construction / destruction
		BitSetWithPC(std::size_t MAX_POP_SIZE) :
			pc_(0),
			bb_(MAX_POP_SIZE)
		{}
		BitSetWithPC() :
			pc_(0)
		{}

		//copy / move semantics disallowed
		BitSetWithPC(const BitSetWithPC& rhs) = delete;
		BitSetWithPC& operator =	(const BitSetWithPC& rhs) noexcept = delete;
		BitSetWithPC(BitSetWithPC&&) noexcept = default;
		BitSetWithPC& operator =	(BitSetWithPC&&) noexcept = default;

		//allocation

		/**
		* @brief Resets the original bitset and allocates new capacity
		**/
		void reset(std::size_t MAX_POP_SIZE) { bb_.reset(MAX_POP_SIZE); pc_ = 0; }

		/**
		* @brief Equivalent to reset, preserved for legacy reasons
		**/
		void init(std::size_t MAX_POP_SIZE) { bb_.reset(MAX_POP_SIZE); pc_ = 0; }

		//setters and getters
		BitSetT& bb() noexcept { return bb_; }
		const BitSetT& bb() const noexcept { return bb_; }
		count_t  count()	const  noexcept { return pc_; }

		//bit twiddling
		void set_bit(int bit) { bb_.set_bit(bit); ++pc_; }
		int  erase_bit(int bit) { bb_.erase_bit(bit); return(--pc_); }

		/**
		* @brief clears all 1-bits. If lazy is true, the bitset is not modified.
		**/
		void erase_bit(bool lazy = false) { if (!lazy) { bb_.erase_bit(); } pc_ = 0; }


		//redefinition of useful operations
		int lsb() { if (pc_ > 0) { return bb_.lsb(); } else return BBObject::noBit; }
		int msb() { if (pc_ > 0) { return bb_.msb(); } else return BBObject::noBit; }

		/**
		* @brief pops the most significant bit. If the bitset is empty, returns BBObject::noBit
		**/
		int pop_msb() {
			if (pc_ > 0) {
				int bit = bb_.msb();
				bb_.erase_bit(bit);
				pc_--; return bit;
			}
			else return BBObject::noBit;
		}

		/**
		* @brief pops the least significant bit. 
		* @returns the bit popped or BBObject::noBit if the bitset is empty
		**/
		int pop_lsb() {
			if (pc_ > 0) {
				int bit = bb_.lsb();
				bb_.erase_bit(bit);
				pc_--; 
				return bit;
			}
			else return BBObject::noBit;
		}

		/**
		* @brief synchronizes the cached population count @pc_ with the real population count of the bitset
		* @returns the updated population count
		* @details
		*	Does not take into account inference chains, active vertices etc. 
		*	Simply sets pc_ = bb_.count()
		**/
		count_t recompute_pc() {
			pc_ = bb_.count();
			return pc_;
		}

		//bool
		bool is_empty() const { return (pc_ == 0); }
		bool is_sync_pc() const { return (pc_ == bb_.count()); }

		//operators
		friend bool operator ==	(const BitSetWithPC& lhs, const BitSetWithPC& rhs) { return (lhs.pc_ == rhs.pc_) && (lhs.bb_ == rhs.bb_); }
		friend bool operator !=	(const BitSetWithPC& lhs, const BitSetWithPC& rhs) { return !(lhs == rhs); }

		//I/O
		std::ostream& print(std::ostream& o = std::cout, bool show_pc = true, bool eofl = true)	const;

		/////////////////
		// data members

		count_t pc_;								//population count (cached) - can differ from bb_.count()
		BitSetT bb_;								//any bitset type of the BBObject hierarchy	

	}; //end BitSetWithPC class

	/////////////////////////////////////////
	// BitSetWithPC class method external definitions in header

	template <class BitSetT>
	inline
		std::ostream& BitSetWithPC<BitSetT>::print(std::ostream& o, bool show_pc, bool eofl) const {

		bb_.print(o, true, false);
		if (show_pc) { o << "[" << pc_ << "]"; }
		if (eofl) { o << std::endl; }
		return o;
	}
	
} //end namespace bitgraph


namespace bitgraph {

	namespace _impl {

		//////////////////////
		// 
		// BitSetStack class	 
		// 
		// @brief: a very simple wrapper for a bitset combined with a vector to support a stack interface
		//		   the stack contains elements from 0 to the size of the bitset -1, corresponding to the 1-bits in the bitset
		// 
		// @details: created 28/13/17, refactored 27/01/2025, last update 06/12/2025
		// 
		//////////////////////

		template <class BitSetT>
		struct BitSetStack {

			enum print_t { STACK = 0, BITSET };

			//construction / destruction
			BitSetStack(std::size_t MAX_POP_SIZE) : bb_(MAX_POP_SIZE) {}
			BitSetStack() {}

			//move and copy semantics - disabled
			BitSetStack(const BitSetStack& rhs) = delete;
			BitSetStack& operator =	(const BitSetStack& rhs) noexcept = delete;
						
			//allocation

			void reset(int MAX_POP_SIZE);

			//setters and getters
			std::size_t size() { return stack_.size(); }

			//stack interface (bits are added / removed according to the stack)
			/**
			* @brief pushes @bit onto the stack (internally also to the bitset)
			* @param bit: bit to push onto the stack - must be less than the population size of the bitset
			**/
			void push(int bit);

			/**
			* @brief extract the last bit from the stack (internally also removes it from the bitset)
			**/
			int pop();

			///////////
			//basic operations
						
			/**
			* @brief clears all 1-bits in the bitset according to the stack
			* @details: the stack itself remains unchanged
			**/
			void erase_bit();

			//synchro operations bitset / stack 
			
			/**
			* @brief sets the stack according to the internal bitset @bb_, 
			*		 running through all 1-bits in @bb_ in order from LSB to MSB
			* @details: the bitset remains unchanged
			**/
			void sync_stack();
			
			/**
			* @brief sets the internal bitset @bb_ according to the current stack
			* @details: the stack remains unchanged
			**/
			void sync_bitset();

			//boolean
						
			/**
			* @brief checks if the contents of the stack and bitset are the same 
			* @returns TRUE if the stack and bitset are the same
			* @details: the order in which the bits are placed in the stack is not relevant
			**/
			bool is_sync();

			bool is_empty() { return (stack_.empty()); }

			//I/O
			std::ostream& print(print_t t = STACK, std::ostream& o = std::cout, bool eofl = true);

			/////////////////
			// data members

			BitSetT bb_;
			std::vector<int> stack_;

		}; //end BitSetStack class

		//////////////////////
		// 
		// BitSetArray class 
		// 
		// @brief  a very simple wrapper for a collection of bitsets of a fixed size
		// @details: created 9/8/17 for MWCP upper bound computation, refactored 27/02/2025
		// 
		// TODO - legacy code, check usage and refactor / remove if necessary (06/12/2025)
		// 
		///////////////////////

		template <class BitSetT, int SIZE>
		struct BitSetArray {

			using basic_type = BitSetT;
			using type = BitSetArray<BitSetT, SIZE>;

			//contructors / destructors
			BitSetArray(int popCount) {
				reset(popCount);
			}

			//TODO move and copy semantics...

		//allocation
			void reset(int popCount) {
				try {
					for (int i = 0; i < SIZE; i++) {
						bb_[i].reset(popCount);
					}
				}
				catch (std::exception& e) {
					LOG_ERROR("%s", e.what());
					LOG_ERROR("BitSetArray::BitSetArray()");
					std::abort();
				}
			}

			//setters and getters
			int number_of_bitblocks(int bitsetID) { return bb_[bitsetID].num_blocks(); }
			int size(int bitsetID) { return bb_[bitsetID].count(); }
			int capacity() { return SIZE; }

			//basic operations
			BitSet& set_bit(int bitsetID, int bit) { return std::ref((bb_[bitsetID].set_bit(bit))); }

			/**
			* @brief sets the bit in the position pos of the bitset at bitsetID
			*		 in the collection
			* @param bitsetID: the position of the bitset in the array
			* @param bit: the bit to set
			* @param is_first_bit: true if bit is now the first bit of the bitset
			* @returns a reference to the bitset modified
			**/
			BitSet& set_bit(int bitsetID, int bit, bool& is_first_bit);

			BitSet& erase_bit(int bitsetID, int bit) { return std::ref((bb_[bitsetID].erase_bit(bit))); }

			/**
			* @brief clears all 1-bits in the bitset bb_[bitsetID]
			**/
			BitSet& erase_bit(int bitsetID) { return std::ref((bb_[bitsetID].erase_bit())); }

			//bool
			bool is_bit(int pos, int bit) { return bb_[pos].is_bit(bit); }


			//I/O
			std::ostream& print(std::ostream& o = std::cout, bool show_pc = true, bool eofl = true)	 const;
			//TODO... others


		/////////////
		// data members
			static const int nBB_ = SIZE;
			std::array<BitSetT, SIZE> bb_;
		};

	}//end namespace _impl

}//end namespace bitgraph

///////////////////////////////////////////////////////////
// Necessary header implementations for generic code


namespace bitgraph {

	using _impl::Tables;

	template <class BitSetT, int SIZE>
	inline
		BitSet& _impl::BitSetArray<BitSetT, SIZE>::set_bit(int bitsetID, int bit, bool& is_first_bit) {

		//adds bit
		bb_[bitsetID].set_bit(bit);

		//checks if the bit added is the first bit of the bitset
		is_first_bit = (bit == bb_[bitsetID].lsb());

		//returns a reference to the modified bitset
		return 	std::ref(bb_[bitsetID]);
	}


	template <class BitSetT, int SIZE>
	inline
		std::ostream& _impl::BitSetArray<BitSetT, SIZE>::print(std::ostream& o, bool show_pc, bool eofl)  const {
		for (auto i = 0; i < bb_.size(); ++i) {
			if (!bb_[i].is_empty()) {
				bb_[i].print(o, show_pc, true);
			}
		}
		if (eofl) { o << std::endl; }
		return o;
	}

	template <class BitSetT>
	inline
		std::ostream& _impl::BitSetStack<BitSetT>::print(print_t t, std::ostream& o, bool eofl) {

		switch (t) {
		case STACK:
			o << "[";
			for (auto i = 0; i < stack_.size(); ++i) {
				o << stack_[i] << " ";
			}
			o << "]" << std::endl;
			break;
		case BITSET:
			bb_.print(o, true, false);		//size info and no end of line
			break;
		default:
			; //error
		}

		if (eofl) { o << std::endl; }
		return o;
	}

	template <class BitSetT>
	inline
		bool _impl::BitSetStack<BitSetT>::is_sync() {

		//same population count
		if (bb_.count() != static_cast<int>(stack_.size())) {
			return false;
		}

		//same bits in the stack  
		for (auto i = 0u; i < stack_.size(); ++i) {
			if (!bb_.is_bit(stack_[i])) {
				return false;
			}
		}

		return true;
	}

	template <class BitSetT>
	inline
		void _impl::BitSetStack<BitSetT>::sync_bitset() {

		bb_.erase_bit();
		for (auto i = 0; i < stack_.size(); i++) {
			bb_.set_bit(stack_[i]);
		}
	}

	template <class BitSetT>
	inline
		void _impl::BitSetStack<BitSetT>::sync_stack() {

		//cleans stack
		stack_.clear();

		//bitscanning with nested data structure
		typename BitSetT::scan sc(bb_);
		if (sc.init_scan() != -1) {
			int bit = BBObject::noBit;
			while ((bit = bb_.next_bit()) != BBObject::noBit) {
				stack_.emplace_back(bit);
			}
		}

	}

	template <class BitSetT>
	inline
		void  _impl::BitSetStack<BitSetT>::reset(int MAX_POP_SIZE) {

		//cleans stack
		stack_.clear();

		//allocates memory	
		try {
			/////////////////////////////
			bb_.reset(MAX_POP_SIZE);
			stack_.clear();
			////////////////////////////
		}
		catch (std::exception& e) {
			LOG_ERROR("%s", e.what());
			LOG_ERROR("BitSetStack<BitSetT>::-reset()");
			std::abort();
		}

	}; //end struct


	template <class BitSetT>
	inline
		void _impl::BitSetStack<BitSetT>::push(int bit) {

		if (!bb_.is_bit(bit)) {
			bb_.set_bit(bit);
			stack_.emplace_back(bit);
		}
	}

	template <class BitSetT>
	inline
		int _impl::BitSetStack<BitSetT>::pop() {

		if (stack_.size() > 0) {
			int bit = stack_.back();
			stack_.pop_back();
			bb_.erase_bit(bit);
			return bit;
		}
		else return BBObject::noBit;
	}

	template <class BitSetT>
	inline
		void _impl::BitSetStack<BitSetT>::erase_bit() {
		for (int i = 0; i < stack_.size(); i++) {
			bb_.erase_bit(stack_[i]);
		}
	}


	template<class BitSetT>
	inline
		std::vector<int> to_vector(BitSetT& bbn) {

		std::vector<int> res;
		res.reserve(bbn.count());

		//uses primitive bitscanning which is compatible for all bitsets in the hierarchy
		int v = BBObject::noBit;
		while ((v = bbn.next_bit(v)) != BBObject::noBit) {
			res.emplace_back(v);
		}

		return res;
	}


	inline
		BITBOARD random_bitblock(double p) {

		BITBOARD bb = 0;

		for (auto i = 0; i < WORD_SIZE; i++) {
			if (com::_rand::uniform_dist(p)) {
				bb |= Tables::mask[i];
			}
		}
		return bb;
	}

	template<class BitSetT>
	inline
		int extract_first_k(int k, BitSetT& bb, std::vector<int>& lv) {

		lv.clear();

		///////////////////////////////////////////////////////////////
		if (bb.init_scan(bbo::NON_DESTRUCTIVE) == -1) { return 0; }
		///////////////////////////////////////////////////////////////

		int nBits = 0;
		int bit = BBObject::noBit;
		while ((bit = bb.next_bit()) != BBObject::noBit || nBits < k) {
			lv.emplace_back(bit);
			nBits++;
		}

		//number of bits found (ideally the first-k)
		return nBits;
	}

}//end namespace bitgraph


#endif

