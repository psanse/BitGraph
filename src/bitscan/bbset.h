/**
 * @file bbset.h
 * @brief header file of the Bitset class from the BITSCAN library.
 *		  Manages bitstrings of any size as an array of bitblocks (64-bit numbers)
 * @author pss
 * @details: Created 2014, last_update 01/02/2025
 * @details: This type has all the abilities except efficient bitscanning,which requires 
 *			 an additional data structure. It does have a basic bitscanning feature.
 * @details  For efficient bitscanning use the BBScan class or the external feature in the
 *			 namespace bbscan.
 * 
 * @todo finish named factories for Bitset creation, merge free factories (17/09/2026)
 **/

#ifndef BITGRAPH_BITSCAN_BBSET_H
#define BITGRAPH_BITSCAN_BBSET_H

#include "bbobject.h"
#include "bitblock.h"
#include "utils/fixed_stack.h"		// FixedStack<T> for bitblock indices
#include "utils/logger.h"

#include <cassert>					
#include <initializer_list>
#include <set>
#include <vector>

namespace bitgraph {

	////////////////////////
	// Forward declarations (friend functions of Bitset)
	////////////////////////
	class Bitset;
	
	// AND masks
	Bitset& AND(const Bitset& lhs, const Bitset& rhs, Bitset& res);
	Bitset AND(Bitset lhs, const Bitset& rhs);
	template<bool Erase>
	Bitset& AND(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res);
	Bitset& AND(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res, bool eraseOutsideRange);
	template<bool Erase>
	Bitset& AND_block(BBObject::block_index_t firstBlock, BBObject::block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs, Bitset& res);
	Bitset AND_block(BBObject::block_index_t firstBlock, BBObject::block_index_t lastBlock, Bitset lhs, const Bitset& rhs);
	
	// OR masks
	Bitset& OR(const Bitset& lhs, const Bitset& rhs, Bitset& res);
	Bitset OR(Bitset lhs, const Bitset& rhs);	
	
	template<bool Erase>
	Bitset& OR(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res);		
	Bitset& OR(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res, bool eraseOutsideRange);
	
	template<bool Erase>
	Bitset& OR_block(BBObject::block_index_t firstBlock, BBObject::block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs, Bitset& res);
	Bitset OR_block(BBObject::block_index_t firstBlock, BBObject::block_index_t lastBlock, Bitset lhs, const Bitset& rhs);
	
	//others
	Bitset& erase_bit(const Bitset& lhs, const Bitset& rhs, Bitset& res);		
	int find_first_common(const Bitset& lhs, const Bitset& rhs);	
	int find_first_common_block(BBObject::block_index_t firstBlock, BBObject::block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs);

	bool operator==	(const Bitset& lhs, const Bitset& rhs);
	bool operator!=	(const Bitset& lhs, const Bitset& rhs);


	// helpers (free factories) - to be moved to named factories in the Bitset class (17/09/2026)
	Bitset make_bitset(int nPop);
	Bitset make_bitset_full(int nPop);
	Bitset make_bitset(int nPop, std::initializer_list<int> lv);
		
	/////////////////////////////////
	//
	// Bitset class 
	//
	// Manages bit strings greater than WORD_SIZE 
	// @details Does not use HW dependent instructions (intrinsics), nor does it cache information for very fast bitscanning
	//
	///////////////////////////////////
	class Bitset : public BBObject {

		using BaseT = BBObject;
		using BaseT::block_index_t;
		using BaseT::bit_indices;
		using BaseT::bit_index_set;
		using BaseT::npos;

		using dense_blocks_t = std::vector<block_t>;
		using DenseBlockVec = dense_blocks_t;				// alias for backward compatibility

	public:
		
		/////////////////////////////
		// Independent operators / masks  
		// comment: do not modify this bitset

		/**
		* @brief AND between lhs and rhs bitsets, stores the result in an existing bitset res
		* @returns reference to the resulting bitstring res
		**/
		friend Bitset& AND(const Bitset& lhs, const Bitset& rhs, Bitset& res);

		/**
		* @brief AND between lhs and rhs bitsets
		* @returns resulting bitset
		**/
		friend Bitset AND(Bitset lhs, const Bitset& rhs) { return lhs &= rhs; }

		/**
		* @brief AND between lhs and rhs bitsets in the CLOSED bit-range [firstBit, lastBit]
		*		 The result is stored in bitset res. The remaining bits of res outside the range
		*		 are set to 0 if the template parameter Erase  is true.
		* @param template<Erase>: if true, the bits of res outside [firstBit, lastBit] are set to 0
		* @param firstBit, lastBit: closed bit-range 0 <= firstBit <= lastBit
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		* @details: The num_blocks of lhs and rhs must be the same.
		*			The num_blocks of res must be greater or equal than lhs / rhs
		* @details: created 06/02/2025
		* @details: GCC does not allow default template parameters in friend functions
		**/
		template<bool Erase>
		friend Bitset& AND(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res);

		/**
		* @brief Wrapper for range AND with explicit runtime policy.
		* @param eraseOutsideRange: if true, sets bits outside [firstBit, lastBit] to 0.
		*/
		friend Bitset& AND(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res, bool eraseOutsideRange) {
			return eraseOutsideRange ? AND<true>(firstBit, lastBit, lhs, rhs, res)
									 : AND<false>(firstBit, lastBit, lhs, rhs, res);
		}

		/**
		* @brief AND between lhs and rhs bitsets in the closed block- range [firstBlock, lastBlock].
		*		 Stores the result in res. The remaining bits of res outside the range
		*		 are set to 0 if the template parameter Erase is true.
		*		 If lastBock == -1, the range is till the end of the bitset, i.e., [firstBlock, num_blocks())
		*
		*		I.  The num_blocks of lhs and rhs must be the same.
		*		II. The num_blocks of res must be at least the same as lhs nand rhs
		* @param template<Erase>: if true, the bits of res outside [firstBit, lastBit] are set to 0
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		* @details: GCC does not allow default template parameters in friend functions
		**/
		template<bool Erase>
		friend Bitset& AND_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs, Bitset& res);

		/**
		* @brief AND between lhs and rhs bitsets in the closed block-range [firstBlock, lastBlock].
		*		 If lastBock==-1, the range is the full bitset. The bits outside the range are set to 0.
		* @returns the new resulting bitset
		**/
		friend Bitset AND_block(block_index_t firstBlock, block_index_t lastBlock, Bitset lhs, const Bitset& rhs) {
			return lhs.AND_EQUAL_block<true>(firstBlock, lastBlock, rhs);
		}

		/**
		* @brief OR between lhs and rhs bitsets, stores the result in an existing bitset res
		* @returns reference to the resulting bitstring res
		**/
		friend Bitset& OR(const Bitset& lhs, const Bitset& rhs, Bitset& res);

		/**
		* @brief OR between lhs and rhs bitsets
		* @returns resulting bitset
		**/
		friend Bitset OR(Bitset lhs, const Bitset& rhs) { return lhs |= rhs; }

		/**
		* @brief OR between lhs and rhs bitsets in the CLOSED bit-range [firstBit, lastBit]
		*		 The result is stored in bitset res. The remaining bits of res outside the range
		*		 are set to 0 if the template parameter Erase is true, else res is not modified.
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		* @details: The num_blocks of lhs and rhs must be the same.
		*			The num_blocks of res must be greater or equal than lhs / rhs
		* @details: created 06/02/2025
		* @details: GCC does not allow default template parameters in friend functions
		**/
		template<bool Erase>
		friend Bitset& OR(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res);

		/**
		* @brief Wrapper for range OR with explicit runtime policy.
		* @param eraseOutsideRange: if true, sets bits outside [firstBit, lastBit] to 0.
		*/
		friend Bitset& OR(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res, bool eraseOutsideRange) {
			return eraseOutsideRange ? OR<true>(firstBit, lastBit, lhs, rhs, res)
									: OR<false>(firstBit, lastBit, lhs, rhs, res);
		}

		/**
		* @brief OR between lhs and rhs bitsets in the closed block- range [firstBlock, lastBlock].
		*		 Stores the result in res. The remaining bits of res outside the range
		*		 are set to 0 if the template parameter Erase is true, else res is not modified.
		*		 If lastBock==-1, the range is til the end of the bitset, i.e., [firstBlock, num_blocks())
		*
		*		I.  The num_blocks of lhs and rhs must be the same.
		*		II. The num_blocks of res must be at least the same as lhs nand rhs
		* @param template<Erase>: if true, the bits of res outside [firstBit, lastBit] are set to 0
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		**/
		template<bool Erase>
		friend Bitset& OR_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs, Bitset& res);

		/**
		* @brief OR between lhs and rhs bitsets in the closed block-range [firstBlock, lastBlock].
		*		 If lastBock==-1, the range is the full bitset. The bits outside the range are set to 0.
		* @returns the new resulting bitset
		**/
		friend Bitset OR_block(block_index_t firstBlock, block_index_t lastBlock, Bitset lhs, const Bitset& rhs) {
			return lhs.OR_EQUAL_block<true>(firstBlock, lastBlock, rhs);
		}

		/**
		* @brief Removes the 1-bits in the bitstring rhs from the bitstring lhs. Stores
		*		 the result in res.
		* @returns reference to the resulting bitstring res
		**/
		friend Bitset& erase_bit(const Bitset& lhs, const Bitset& rhs, Bitset& res);										//removes rhs from lhs

		/**
		* @brief Determines the first bit of the itersection between bitsets lhs and rhs
		* @param lhs, rhs: input bitsets
		* @returns the first BIT of the intersection or BBObject::noBit if the sets are disjoint
		**/
		friend int find_first_common(const Bitset& lhs, const Bitset& rhs);

		/**
		* @brief Determines the first bit of the itersection between bitsets lhs and rhs
		*		 in the closed block-range [firstBlock, lastBlock].
		*		 If lastBock == -1, the range [firstBlock, END OF BITSET)
		* @param lhs, rhs: input bitsets
		* @returns the first BIT of the intersection or BBObject::noBit if the sets are disjoint
		**/
		friend int find_first_common_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs);

		////////////
		//construction / destruction 

		Bitset() : nBB_(0) {}

		/**
		 * @brief Constructs a bitset with capacity for at least @p nPop bits.
		 *
		 * Storage is allocated in complete bitblocks, so the resulting physical
		 * capacity may be greater than @p nPop.
		 *
		 * @param nPop Minimum number of bit positions required.
		 * @param value Initial value assigned to every physically allocated bit.
		 * @warning This constructor follows a fail-fast policy. If construction
	     *     fails, the program terminates.
		 */
		explicit  Bitset(std::size_t nPop, bool value = false);

		/**
		 * @brief Constructs a bitset containing the specified set-bit positions.
		 *
		 * The required capacity is inferred from the greatest position in @p positions.
		 * An empty collection produces an empty bitset.
		 *
		 * @param bits Positions of the bits to set.
		 *
		 * @pre Every position must be nonnegative.
		 * @warning This constructor follows a fail-fast policy. If construction
	     *     fails, the program terminates.
		 */
		explicit  Bitset(const bit_indices& bits);

		/**
		 * @brief Constructs a bitset with the specified set-bit positions.
		 *
		 * Storage is allocated for at least @p requestedBits bit positions, and every
		 * position contained in @p positions is set.
		 *
		 * @tparam Collection Collection type providing begin() and end().
		 * @param nPop Minimum number of bit positions required.
		 * @param bits Positions of the bits to set.
		 *
		 * @pre Every position must be nonnegative and smaller than
		 *      @p requestedBits.
		 * @warning This constructor follows a fail-fast policy. If construction
	     *     fails, the program terminates.
		 */
		template<class ColT>
		explicit  Bitset(std::size_t nPop, const ColT& bits);

		/**
		 * @brief Constructs a bitset from an initializer list of set-bit positions.
		 *
		 * @param nPop Minimum number of bit positions required.
		 * @param bits Positions of the bits to set.
		 *
		 * @pre Every position must be nonnegative and smaller than
		 *      @p requestedBits.
		 * @warning This constructor follows a fail-fast policy. If construction
	     *     fails, the program terminates.
		 */
		explicit  Bitset(std::size_t nPop, std::initializer_list<int> bits);


		////////
		// Named factories 
		
		static Bitset empty(std::size_t nPop)
		{
			return Bitset(nPop, false);
		}

		static Bitset full(std::size_t nPop)
		{
			return Bitset(nPop, true);
		}

		template<class It>
		static Bitset from_positions(
			std::size_t nPop,
			It first,
			It last) = delete;

		template<class Collection>
		static Bitset from_positions(
			std::size_t nPop,
			const Collection& positions)
		{
			return Bitset(nPop, positions);
		}

		static Bitset from_set_bits(
			std::size_t nPop,
			std::initializer_list<int> positions)
		{
			return Bitset(nPop, positions);
		}

				
		////////
		// Copy and move semantics allowed

		Bitset(const Bitset&) = default;
		Bitset(Bitset&&) noexcept = default;

		Bitset& operator=(const Bitset&) = default;
		Bitset& operator=(Bitset&&) noexcept = default;

		~Bitset() override = default;

		////////////
		//Reset / init (memory allocation)
		
		/**
		* @brief Resets this bitset given to a vector lv of 1-bit elements
		 *		  and a population size nPop.
		 *
		 *		  I. The num_blocks of the bitset is set according to nPop.
		 *		  II. Memory is deallocated and reallocated as required
		 * @param nPop: population size
		 * @param lv : vector of integers representing 1-bits in the bitset
		 * @warning: this function follows a fail-fast policy. If the function fails, the program terminates.
		**/
		void reset(std::size_t nPop, const bit_indices& lv) noexcept;

		/**
		* @brief Resets this bitset to an EMPTY BITSET given to a population size nPop.
		*
		*		  I. The num_blocks of the bitset is set according to nPop.
		*		  II. Memory is deallocated and reallocated as required
		*
		* @param nPop: population size
		* @warning: this function follows a fail-fast policy. If the function fails, the program terminates.
		**/
		void reset(std::size_t nPop) noexcept;

		/* substituted by reset - preserved for backward compatibility */
		void init(std::size_t nPop) noexcept { reset(nPop); }
		void init(std::size_t nPop, const bit_indices& bits) noexcept { reset(nPop, bits); }

		/**
		* @brief reallocates memory to the number of blocks of the bitset
		**/
		void  shrink_to_fit() { vBB_.shrink_to_fit(); }

		/////////////////////
		//setters and getters (will not allocate memory)

		/**
		* @brief returns the number of blocks allocated for the bitset
		* @returns number of blocks allocated for the bitset - integer type
		* @details: recommended for internal use
		**/
		int num_blocks() const noexcept { return nBB_; }

		/**
		* @brief returns the number of blocks allocated for the bitset
		* @returns number of blocks allocated for the bitset - std::size_ type
		* @details: recommended for consumer code
		**/
		std::size_t size() const noexcept { return vBB_.size(); }
		std::size_t bit_capacity() const noexcept {
			return static_cast<std::size_t>(num_blocks()) * WORD_SIZE;
		}

		DenseBlockVec& bitset()  noexcept { return vBB_; }
		const DenseBlockVec& bitset() const  noexcept { return vBB_; }

		BITBOARD block(block_index_t blockID)	const {
			assert(blockID < static_cast<block_index_t>(vBB_.size()));
			return vBB_[blockID];
		}
		BITBOARD& block(block_index_t blockID) {
			assert(blockID < static_cast<block_index_t>(vBB_.size()));
			return vBB_[blockID];
		}

		//////////////////////////////
		// Bitscanning (no HW operations)

		/**
		* @brief returns the index of the most significant bit in the bitstring
		* @details implemented as a lookup table
		**/
	protected:
		int msbn64_lup()	const;
		int msbn64_intrin()	const;

	public:
		int msb()	const { return msbn64_intrin(); }

		/**
		* @brief returns the index of the least significant bit in the bitstring
		* @details  implemented as a de Bruijn hashing or a lookup table depending on
		*			an internal switch (see config.h)
		**/
	protected:
		int lsbn64_non_intrin()	const;
		int lsbn64_intrin()	const;

	public:
		int lsb()	const { return lsbn64_intrin(); }

		/**
		* @brief Computes the next least significant 1-bit in the bitstring after bit
		*		 If bit == BBObject::noBit, returns the lest significant bit in the bitstring.
		*
		*		 I.Primitive scanning stateless feature at this level. Require a bit position as argument always.
		*		II. Use bitscanning with state for proper bitscanning (derived class or external feature)
		*
		* @param bit: position from which to start the search
		* @returns the next 1-bit in the bitstring after bit, BBObject::noBit if there are no more bits
		* @details: no internal state is used, NOT EFFICIENT since it has to compute the offset and
		*		    current bitblock of bit in each call. It does not cache the last bit found.
		* @details: Uses a DeBruijn implementation for lsb()
		* @details: DEPRECATED in favour of the bitscanning with state of BBIntrinsic class
		**/
		int next_bit(bit_index_t bit)	const noexcept;

		/**
		* @brief Computes the next most significant  1-bit in the bitstring after bit
		*		 If bit == BBObject::noBit, returns the most significant bit in the bitstring
		*
		*		 I. Primitive scanning stateless feature at this level. Require a bit position as argument always.
		*		II. Use bitscanning with state for proper bitscanning (derived class or external feature)
		*
		* @param bit: position from which to start the search
		* @returns the next msb 1-bit in the bitstring after bit, BBObject::noBit if there are no more bits
		* @details: no internal state is used, NOT EFFICIENT since it has to compute the offset and
		*		    current bitblock of bit in each call. It does not cache the last bit found.
		* @details: Uses a lookup table implementation for msb()
		* @details: Not recommended. DEPRECATED in favour of the bitscanning with state of BBIntrinsic class
		**/
		int prev_bit(bit_index_t bit)	const noexcept;

		/////////////////
		// Popcount

		/**
		* @brief Returns the number of 1-bits in the bitstring
		*
		* @details alias to popcn64, calls the function
		* @details Only function for population count since 12/02/2025
		* @details implementation depends of POPCN64 switch in bbconfig.h
		*		   By default - intrinsic HW assembler instructions
		**/
		int	count()									const { return popcn64(); }
		int	count(bit_index_t firstBit,
					bit_index_t lastBit = BBObject::noBit)	const { 
			return popcn64(firstBit, lastBit);
		}

		/**
		* @brief returns the number of 1-bits in the bitstring
		**/
	protected:
		virtual	int popcn64() const noexcept;

		/**
		* @brief Returns the number of 1-bits in the bitstring
		*	 	 in the closed range [firstBit, lastBit]
		*		 If lastBit == -1, the range is [firstBit, endOfBitset)
		*
		* @details efficiently implemented as a lookup table or with HW instructions
		*			depending  on an internal switch (see config.h)
		**/
		virtual	int popcn64(bit_index_t firstBit, bit_index_t lastBit = BBObject::noBit)	const noexcept;

		/////////////////////
		//Setting / Erasing bits 
	public:

		/**
		* @brief sets a 1-bit in the bitstring
		* @param  bit: position of the 1-bit to set (nBit >= 0)
		* @returns reference to the modified bitstring
		**/
		Bitset& set_bit(bit_index_t bit) noexcept;

		/**
		* @brief API core alias for set_bit(bit_index_t bit)
		* @param  bit: position of the 1-bit to set (nBit >= 0)
		* @returns reference to the modified bitstring
		**/
		Bitset& set(bit_index_t bit) noexcept { return set_bit(bit); }

		/**
		* @brief sets the bits in the closed range [firstBit, lastBit] to 1 in the bitstring
		* @params firstBit, lastBit:  0 <= firstBit <= lastBit
		* @date 22/9/14
		* @last_update 01/02/25
		**/
		Bitset& set_bit(bit_index_t firstBit, bit_index_t lastBit) noexcept;

		/**
		* @brief API core alias for set_bit(bit_index_t firstBit, bit_index_t lastBit)
		* @params firstBit, lastBit:  0 <= firstBit <= lastBit
		* @created 18/09/26
		**/
		Bitset& set_range(bit_index_t firstBit, bit_index_t lastBit) noexcept { return set_bit(firstBit, lastBit); }

		/**
		* @brief Adds the bits from the bitstring bb_add in the population
		*		 range of the bitstring (bitblocks are copied).
		*
		*		 Note: The bitblock size of bb_add must be at least as large as the bitstring.
		*
		* @details  Equivalent to OR operation / set union
		* @returns reference to the modified bitstring
		**/
		Bitset& set_bit(const Bitset& bb_add);

		/**
		* @brief Overwrites this bitstring with @bb_add (equivalent to operator=)
		*
		*		 Note: The bitblock size of bb_add must be at least as large as this bitstring.
		*
		* @param bb_add: input bitstring whose bits are copied
		* returns reference to the modified bitstring
		**/
		Bitset& assign_bit(const Bitset& bb_add);

		/**
		* @brief Adds the bits from the bitstring bb_add in the range [0, lastBit]
		* @param lastBit : the last bit in the range to be copied
		* @returns reference to the modified bitstring
		**/
		Bitset& set_bit(bit_index_t lastBit, const Bitset& bb_add);

		/**
		* @brief Adds elements from a vector of non-negative integers lv as 1-bit
		*	    up to the maximum num_blocks of the bitstring. Values greater than the
		*		maximum num_blocks of the biset are ignored.
		*
		* @param bits: vector of non-negative integers
		* @returns reference to the modified bitstring
		* @details negative elements will cause an assertion if NDEBUG is not defined,
		*		  else the behaviour is undefined.
		**/
		Bitset& set_bit(const bit_indices& bits);

		/**
		* @brief sets bit number bit to 0 in the bitstring
		* @param  bit: position of the 1-bit to set (>=0)
		* @returns reference to the modified bitstring
		**/
		Bitset& erase_bit(bit_index_t bit) noexcept;

		/**
		* @brief API core for erase_bit(bit). sets bit number bit to 0 in the bitstring
		* @param  bit: position of the 1-bit to set (>=0)
		* @returns reference to the modified bitstring
		**/
		Bitset& clear(bit_index_t bit) noexcept { return erase_bit(bit); }	

		/**
		* @brief sets the bits in the closed range [firstBit, lastBit] to 0 in the bitstring
		*		 If lastBit == -1, the range is [firstBit, endOfBitset)
		* @params firstBit, lastBit: 0 <= firstBit <= lastBit
		* @created 22/9/14
		* @details last_update 01/02/25
		**/
		Bitset& erase_bit(bit_index_t firstBit, bit_index_t lastBit) noexcept;
		
		/**
		* @brief API core for erase_bit(firstBit, lastBit). sets the bits in the closed range [firstBit, lastBit] to 0 in the bitstring
		*		 If lastBit == -1, the range is [firstBit, endOfBitset)
		* @params firstBit, lastBit: 0 <= firstBit <= lastBit
		* @created 18/09/26
		**/
		Bitset& clear_range(bit_index_t firstBit, bit_index_t lastBit) noexcept { return erase_bit(firstBit, lastBit); }

		/**
		* @brief sets all bits to 0
		* @returns reference to the modified bitstring
		* @details: complexity O(nBlocks), does not deallocate memory.
		**/
		Bitset& erase_bit() noexcept;

		/**
		 * @brief Clears every bit in the bitset.
		 * Legacy explicit alias for erase_bit().
		 * 
		 * @return Reference to this modified bitset.
		 * @note No memory is deallocated.
		 */
		Bitset& erase_all_bits() noexcept {	return erase_bit(); }						
		
		/**
		 * @brief Clears every bit in the bitset.
		 * This is the preferred core API alias for erase_bit().
		 * 
		 * @return Reference to this modified bitset.		
		 * @note The current storage allocation and bit capacity are preserved.
		 */
		Bitset& clear_all(bit_index_t bit) noexcept { return erase_bit(bit); }				

		/**
		* @brief Removes the bits from the bitstring @bitset inside the population range.
		*
		*		 I. bitset must have a maximum population
		*			greater or equal than the bitstring.
		*
		* @details  Equivalent to a set minus operation
		* @returns reference to the modified bitstring
		**/
		Bitset& erase_bit(const Bitset& bitset);
		Bitset & andnot_eq(const Bitset & bitset) noexcept { return this->erase_bit(bitset); }

		/**
		* @brief Removes the bits  from the bitstring @bitset inside the
		*		 closed range [firstBit, lastBit].
		*		 If lastBit == -1, the range is [firstBit, endOfBitset)
		*
		*		 I. bitset must have a maximum population
		*			greater or equal than the bitstring.
		*
		* @details  Equivalent to a set minus operation
		* @returns reference to the modified bitstring
		**/
		Bitset& erase_bit(bit_index_t firstBit, bit_index_t lastBit, const Bitset& bitset) noexcept;

		/**
		* @brief Removes the 1-bits from both input bitstrings (their union)
		*		inside the population range. The sizes of both input bitstrings
		*		must be greater than the destination bitstring (*this)
		* @param lhs, rhs: input bitstrings
		* @returns reference to the modified bitstring
		* @created: 30/7/2017  for the MWCP
		* @last_update: 02/02/2025
		**/
		Bitset& erase_bit(const Bitset& lhs, const Bitset& rhs);

		/////////////////////
		//BitBlock operations 

		/**
		* @brief ORs the 1-bits from the bitstring @bb_add in the closed range [firstBlock, lastBlock]
		*		 If lastBlock = npos, the range is [firstBlock, nBB_]
		*
		*		 0 <= FirstBlock <= LastBLock < the number of bitblocks in the bitstring
		*
		* @param bb_add: input bitstring whose bits are added
		* @param FirstBlock: the first bitblock to be modified
		* @param LastBLock: the last bitblock to be modified
		* @returns reference to the modified bitstring
		**/
		Bitset& set_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_add);

		/**
		* @brief Overwrites / Copies the blocks of 1-bits from the bitstring @bb_add
		*		 in the closed range [firstBlock, lastBlock] to this bitstring.
		*		 If LastBlock = npos, the range is [firstBlock, nBB_]
		*		 0 <= FirstBlock <= LastBLock < the number of bitblocks in the bitstring
		* @param bb_add: input bitstring whose bits are copied
		* @param FirstBlock: the first bitblock to be modified
		* @param LastBLock: the last bitblock to be modified
		* returns reference to the modified bitstring
		**/
		Bitset& assign_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_add);

		/**
		* @brief Deletes the 1-bits from the bitstring bb_del in the closed range [firstBlock, lastBlock]
		*		 If lastBlock = npos, the range is the whole bitstring.
		*
		*		 0 <= firstBlock <= lastBlock < the number of bitblocks in the bitstring
		*
		* @param bb_del: input bitstring whose 1-bits are to be removed
		* @param firstBlock: the first bitblock to be modified
		* @param lastBlock: the last bitblock to be modified
		* @returns reference to the modified bitstring
		**/
		Bitset& erase_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_del);

		/**
		* @brief Removes the 1-bits from both input bitstrings (their union)
		*		in the closed range of bitblocks [firstBlock, lastBlock]
		*		If lastBlock = npos, the range is [firstBlock, nBB_]
		*
		* @param firstBlock: the first bitblock to be modified
		* @param bb_del_lhs, bb_del_rhs : bitstrings whose 1-bits are to be removed
		* @returns reference to the modified bitstring
		* @date: 02/02/2025 during a refactorization of BITSCAN
		**/
		Bitset& erase_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_del_lhs, const Bitset& bb_del_rhs);

		////////////////////////
		// operators

		/**
		* @brief Bitwise AND operator with bbn
		* @details For set intersection
		**/
		Bitset& operator &=				(const Bitset& bbn) noexcept;
		Bitset & and_eq					(const Bitset & rhs) noexcept { return *this &= rhs; }

		/**
		* @brief Bitwise OR operator with bbn
		* @details For set union
		**/
		Bitset& operator |=				(const Bitset& bbn) noexcept;
		Bitset & or_eq					(const Bitset & rhs) noexcept { return *this |= rhs; }

		/**
		* @brief Bitwise XOR operator with bbn
		* @details For symmetric_difference
		**/
		Bitset& operator ^=				(const Bitset& bbn) noexcept;

		friend bool operator ==			(const Bitset& lhs, const Bitset& rhs);
		friend bool operator !=			(const Bitset& lhs, const Bitset& rhs);

		////////////////////////
		// Basic operations

		/**
		* @brief flips 1-bits to 0 and 0-bits to 1
		**/
		Bitset& flip() noexcept;

		/**
		* @brief flips 1-bits to 0 and 0-bits to 1 in the
		*		 closed block range [firstBlock, lastBlock]
		**/
		Bitset& flip_block(block_index_t firstBlock, block_index_t lastBlock) noexcept;

		/**
		* @brief AND between rhs and caller bitstring in the closed range of bitblocks [firstBlock, lastBlock]
		*		 If lastBlock == npos the range is [firstBlock, nBB_]
		*
		* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_).
		* @param rhs: bitstring
		* @returns reference to the modified bitstring
		* @date: 04/02/2025 during a refactorization of BITSCAN
		**/
		template<bool Erase = false>
		Bitset& AND_EQUAL_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs);

		/**
		* @brief Wrapper example for AND_EQUAL_block with explicit runtime policy.
		* @param eraseOutsideRange: if true, sets caller bits outside [firstBlock,lastBlock] to 0.
		*/
		Bitset& AND_EQUAL_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs, bool eraseOutsideRange) {
			return eraseOutsideRange ? AND_EQUAL_block<true>(firstBlock, lastBlock, rhs)
									 : AND_EQUAL_block<false>(firstBlock, lastBlock, rhs);
		}

		/**
		* @brief OR between rhs and caller bitstring in the closed range of bitblocks [firstBlock, lastBlock]
		*		 If lastBlock == npos the range is [firstBlock, nBB_]
		*
		* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_)
		* @param rhs: bitstring
		* @returns reference to the modified bitstring
		**/
		template<bool Erase = false>
		Bitset& OR_EQUAL_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs);

		/**
		* @brief Wrapper example for OR_EQUAL_block with explicit runtime policy.
		* @param eraseOutsideRange: if true, sets caller bits outside [firstBlock,lastBlock] to 0.
		*/
		Bitset& OR_EQUAL_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs, bool eraseOutsideRange) {
			return eraseOutsideRange ? OR_EQUAL_block<true>(firstBlock, lastBlock, rhs)
									 : OR_EQUAL_block<false>(firstBlock, lastBlock, rhs);
		}

		/**
		* @brief Determines the lowest bit (least-significant) in common between rhs and this bitstring
		* @param rhs: input bitstring
		* @returns the first 1-bit in common, -1 if they are disjoint
		* @created 7/17
		* @last_update 02/02/2025
		**/
		int find_first_common(const Bitset& rhs)	const;

		/**
		* @brief Determines if the bitstring has a single 1-bit in the closed range [firstBit, lastBit]
		*		 in singleton_bit
		* @param firstBit, lastBit: closed range of bits (0<=firstBit<=lastBit)
		* @param bit: contains the singleton bit if it exists or -1
		* @returns  0 if range is empty, 1 if singleton, -1 if more than one bit exists in the range
		*			IMPORTANT: if any bitset is empty in the range, the result is 0 (disjoint), , bit = -1
		**/
		int  find_singleton(bit_index_t firstBit, bit_index_t lastBit, bit_index_t& bit) const;

		/**
		* @brief Determines the single 1-bit common to both this and rhs bitstring.			*
		* @param rhs: input bitstring
		* @param bit:  1-bit index or -1 if not single disjoint
		* @returns 0 if disjoint,  1 if intersection is a single bit, -1 otherwise (more than 1-bit in common)
		*		   IMPORTANT: if any bitset is empty, the result is 0 (disjoint), bit = -1
		**/
		int	find_common_singleton(const Bitset& rhs, bit_index_t& bit)	const;

		/**
		* @brief  Determines the single 1-bit common to both this and rhs bitstring in the
		*		  closed range [firstBlock, lastBlock].
		*		  If lastBlock == npos the range is [firstBlock, nBB_]
		*
		* @param bit:  1-bit index or -1 if not single disjoint
		* @returns 0 if disjoint, 1 if intersection is a single bit, -1 otherwise (more than 1-bit in common)
		* @created 14/8/16
		* @last_update 04/02/2025
		**/
		int	find_common_singleton_block(block_index_t firstBlock, block_index_t lastBlock,
			const Bitset& rhs, int& bit) const;

		/**
		* @brief Determines the single 1-bit in this bitstring of to the set difference
		*		 bitset this \ rhs.
		* @param bit:  1-bit index or -1 if the set difference is not a single bit
		* @returns 0 if the set difference is empty, 1 if a singleton and -1 otherwise (more than 1-bit)
		* @created 27/7/16
		* @last_update 04/02/2025
		**/
		int	find_diff_singleton(const Bitset& rhs, bit_index_t& bit) const;

		/**
		* @brief Determines the pair of bits bit1 and bit2 the set difference  bitset this \ rhs.
		*		 If the set difference is a singleton, bit1 = singleton bit and bit2 = -1.
		*		 If the set diference is empty or larger than 2 bits, bit1 = bit2 = -1.
		*
		* @param bit1, bit2: output pair of bits of the set difference.
		*
		* @returns 0 if the set difference is empty, 1 if it is a singleton, 2 a pair
		*		  and -1 otherwise (more than 1-bit)
		* @details: created  27/7/16, last_update 04/02/2025
		**/
		int find_diff_pair(const Bitset& rhs, bit_index_t& bit1, bit_index_t& bit2) 	const;

		/////////////////////////////
		//Boolean functions 

		/**
		 * @brief Tests whether a bit is set.
		 *
		 * @param bit Position of the bit to test.
		 * @return `true` if @p bit is set; otherwise, `false`.
		 *
		 * @pre @p bit must be a valid bit position.
		 */
		inline bool is_bit(bit_index_t bit)	const noexcept;
		
		/**
		 * @brief API core alias for is_bit(bit_index_t).
		 *
		 * @param bit Position of the bit to test.
		 * @return `true` if @p bit is set; otherwise, `false`.
		 *
		 * @pre @p bit must be a valid bit position.
		 */
		inline bool test(bit_index_t bit) const noexcept { return is_bit(bit); }

		/**
		* @brief TRUE if the bitstring has all 0-bits
		**/
		virtual bool is_empty() const noexcept;

		/**
		* @brief Determines if the bitstring has all 0-bits in the closed range [firstBlock, lastBlock]
		*		  If lastBlock == npos the range is [firstBlock, nBB_]
		*
		* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_).
		* @returns TRUE if the bitstring has all 0-bits in the given range
		* @details optimized for non-sparse bitsets - early exit
		**/
		virtual bool is_empty_block(block_index_t firstBlock, block_index_t lastBlock) const;

		/**
		* @brief TRUE if caller bitstring has a single 1-bit
		* @returns 1 if singleton, 0 if empty, -1 if more than one bit
		* @created (15/3/17)
		* @details optimized for non-sparse bitsets - early exit
		**/
		int is_singleton() const;

		/**
		* @brief Determines if the caller bitstring has a single 1-bit in the
		*		 closed bit-range [firstBit, lastBit]
		*
		* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_).
		* @returns 1 if singleton, 0 if empty, -1 if more than one bit in the specifed range
		* @details optimized for non-sparse bitsets - early exit
		**/
		int  is_singleton(bit_index_t firstBit, bit_index_t	lastBit)						const;

		/**
		* @brief TRUE if caller bitstring has a single 1-bit in the closed range [firstBlock, lastBlock]
		*		  If lastBlock == npos the range is [firstBlock, nBB_]
		*
		* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_).
		* @returns 1 if singleton, 0 if empty, -1 if more than one bit.
		* @details optimized for non-sparse bitsets - early exit
		**/
		int is_singleton_block(block_index_t firstBlock, block_index_t lastBlock) const;

		/**
		 * @brief Tests whether this bitset is disjoint from another bitset.
		 *
		 * Two bitsets are disjoint when they have no set-bit positions in common.
		 *
		 * @param other Bitset to compare with this bitset.
		 * @return `true` if the bitsets have no set bits in common; otherwise,
		 *         `false`.
		 *
		 * @pre Both bitsets must have the same number of bitblocks.
		 */
		bool is_disjoint(const Bitset& bitset)	const noexcept;

		/**
		 * @brief API core for testing intersection of two bitsets.
		 *
		 * Two bitsets intersect when they have at least one set-bit position in
		 * common.
		 *
		 * @param other Bitset to compare with this bitset.
		 * @return `true` if the bitsets have at least one set bit in common;
		 *         otherwise, `false`.
		 *
		 * @pre Both bitsets must have the same number of bitblocks.
		 */
		bool intersects(const Bitset & bitset) const noexcept { return !is_disjoint(bitset); }

		/**
		* @brief TRUE if this bitstring has no bits in common with rhs
		*		 in the closed range [firstBlock, lastBlock].
		*
		*		If lastBlock == npos, the range is [firstBlock, nBB_]
		**/
		bool is_disjoint_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs)	const;
		/**
		* @brief TRUE if this bitstring has no bits in common with neither lhs NOR rhs bitstrings
		* @details Currently not available for sparse bitsets
		**/
		bool is_disjoint(const Bitset& lhs, const  Bitset& rhs)	const;

		/////////////////////
		// I/O 

		/**
		* @brief streams bb and its popcount to the output stream o
		* @details format example [...000111 (3)]
		* @param o: output stream
		* @param show_pc: if true, shows popcount
		* @returns output stream
		**/
		std::ostream& print(std::ostream& o = std::cout,
							bool show_pc = true, bool endl = true)	const override;
	

		///////////////////////
		//Conversions / Casts

		/**
		* @brief converts bb and its popcount to a readable string
		* @details format example [...000111 (3)]
		* @returns string
		*
		* TODO implement - cast operator	(24/02/2025)
		**/
		std::string to_string();

		/**
		* @brief Fills std::vector lb with the 1-bits of the bitset.
		* @param lb: output vector of integers (external ownership)
		*
		* TODO - loop unrolling for efficiency (18/11/2025)
		**/
		void extract(bit_indices& lb) const;
		void extract_set(bit_index_set& lb)	const;

		/**
		 * @brief Creates a sequence containing the indices of all set bits.
		 *
		 * The indices are returned in increasing order.
		 *
		 * @return Sequence containing the positions of the set bits.
		 */
		explicit operator bit_indices()	const;
		
		/**
		 * @brief Creates an ordered set containing the indices of all set bits.
		 *
		 * @return Set containing the positions of the set bits.
		 */
		explicit operator bit_index_set() const;
		
		/**
		* @brief Fills stack @s
		* @param s: output stack object (external ownership)
		**/
		void extract_stack(utils::FixedStack<bit_index_t>& s) const;
		/**
		* @brief Fills array @lv and sets @size
		* @param lv: must point to an array of capacity >= population size of this bitset (external ownership)
		* @param size: output size of the array, should be the same as this->count()
		* @param rev: if true, the array is filled in reverse order
		**/
		void extract_array(int* lv, std::size_t& size, bool rev = false);


		///////////////////////////////
		// handlers

		[[noreturn]]
		static void bitset_initialization_error() noexcept
		{
			std::fputs("Bitset initialization failed: invalid bit position\n", stderr);
			std::terminate();
		}

		////////////////////////
		//data members

	protected:
		int nBB_;							// number of bitblocks (redundant to vBB.size(), cached for efficiency)
		DenseBlockVec vBB_;					// vector of fixed size bitblocks 
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// DEPRECATED friend operations, TO BE REMOVED. NOT CHECKED!! (06/02/2025)

		/**
		* @brief AND between lhs and rhs bitsets in the SEMI-OPEN range [0, last_vertex)
		*
		*		 I. Sets to 0 the remaining bits after, and including lastBit of the bitblock lbBLOCK containing lastBit
		*		 II.If template Erase is true, the rest of the bitblocks after  lbBLOCK are also set to 0
		*
		* @param lastBit:  position that determines the range of the AND mask
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		* @details: The num_blocks of lhs and rhs must be the same.
		*			The num_blocks of res must be greater or equal than lhs / rhs
		*
		* TODO: slightly weird behaviour, check if it is necessary
		* TODO: add firstBit logic  (06/02/2025)
		* TODO: deprecated - remove after checking optimization algorithms (06/02/2025)
		**/
		//template<bool Erase>
		//friend Bitset& AND(int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res);

		/**
		* @brief AND between lhs and rhs bitsets in the SEMI-OPEN range [0, last_vertex)
		*		 For C-array compatibility, the result is stored in bitset res
		* @returns C-array of integers representing the resulting bitstring res, array size is stored in size
		* @details: Experimental, not efficient
		* TODO: deprecated - REMOVE after checking optimization algorithms (06/02/2025)
		**/
		//friend int* AND(int lastBit, const Bitset& lhs, const Bitset& rhs, int bitset[], int& size);

		/**
		* @brief OR between lhs and rhs bitsets in the SEMI-OPEN range [firstBit, END).
		*		 Stores the result in bitset res.
		*		 Outside the range the bits are set to lhs.
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		**/
		//friend Bitset& OR(int firstBit, const Bitset& lhs, const Bitset& rhs, Bitset& res);

		// lhs OR rhs - ranges (rhs [v, END[ if left = TRUE,  rhs [0, v[ if left = false)
		// date@26/10/19
		//friend Bitset& OR(int bit, bool left /* to */, const Bitset& lhs, const Bitset& rhs, Bitset& res);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////

	}; //end Bitset class

	
}//end namespace bitgraph

#include "detail/bbset_imp.h"


#endif // BITGRAPH_BITSCAN_BBSET_H
