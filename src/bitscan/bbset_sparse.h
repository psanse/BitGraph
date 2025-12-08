 /**
  * @file bbset_sparse.h
  * @brief header for sparse class equivalent to the BitSetSp class
  * @author pss
  * @details created 19/12/2015?, @last_update 08/12/2025 (improved design, types etc.)
  *
  * TODO - Improve BitSet class interface (08/12/2025)	
  * TODO - check inlining and header / cpp implementations (08/12/2025) 
  **/

#ifndef __BBSET_SPARSE_H__
#define __BBSET_SPARSE_H__

#include "bitscan/bbobject.h"
#include "bitscan/bitblock.h"
#include "utils/logger.h"
#include "utils/common.h"
#include <vector>	
#include <algorithm>
#include <iterator>

//uncomment #undef NDEBUG in bbconfig.h to enable run-time assertions
#include <cassert>

//aliases
using vint = std::vector<int>; 

namespace bitgraph {
		
	/////////////////////////////////
	//
	// class BitSetSp
	// (manages sparse bit strings, except efficient bitscanning)
	//
	///////////////////////////////////

	class BitSetSp : public BBObject {
				
		using index_t = BBObject::index_t;
	
		static constexpr int DEFAULT_CAPACITY = 2;		//initial allocation of bit blocks for any new sparse bitstring - CHECK efficiency (17/02/2025)

	public:
		
		using BBObject::npos;
		
		struct SparseBlock {
			
			index_t idx_;
			BITBOARD bb_;

			SparseBlock(int idx = npos, BITBOARD bb = 0) : idx_(idx), bb_(bb) {}

			bool operator ==	(const SparseBlock& e)	const { return (idx_ == e.idx_ && bb_ == e.bb_); }
			bool operator !=	(const SparseBlock& e)	const { return (idx_ != e.idx_ || bb_ != e.bb_); }
			bool operator !		()					const { return !bb_; }
			bool is_empty()					const { return !bb_; }
			void clear() { bb_ = 0; }

			std::ostream& print(std::ostream& o = std::cout, bool eofl = true)	const;
			friend  std::ostream& operator<<	(std::ostream& o, const SparseBlock& pB) { pB.print(o); return o; }
		};

		//aliases
		using BlockVec  = std::vector<SparseBlock>;
		using BlockVecIt  = typename std::vector<SparseBlock>::iterator;
		using BlockVecConstIt = typename std::vector<SparseBlock>::const_iterator;
				 

		//functor for sorting -
		struct pBlock_less {
			bool operator()(const SparseBlock& lhs, const SparseBlock& rhs) const {
				return lhs.idx_ < rhs.idx_;
			}
		};

		/////////////////////////////
		// Independent operators / masks  
	public:

		friend bool operator ==			(const BitSetSp& lhs, const BitSetSp& rhs);
		friend bool operator !=			(const BitSetSp& lhs, const BitSetSp& rhs);

		/**
		* @brief AND between lhs and rhs sparse bitsets - stores the result in sparse bitset res
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to res
		* #details: currently two implementations, one optimized for THIS having less population and the other
		*			assuming both THIS and rhs have similar population
		**/
		friend  BitSetSp& AND(const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res);

		/**
		* @brief AND between lhs and rhs bitsets - stores the result in @lhs
		* @returns resulting bitset
		**/
		friend BitSetSp AND(BitSetSp lhs, const BitSetSp& rhs);

		/**
		* @brief AND between lhs and rhs bitsets in the CLOSED block bit-range [firstBit, lastBit]
		*		 The result is stored in bitset res. Outside the range, res is set to 0.
		* @param firstBit, lastBit: closed bit-range 0 <= firstBit <= lastBit <= num_blocks()
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		* @details: The num_blocks of lhs and rhs must be the same.
		*
		* TODO... (25/02/2025)
		**/
		friend BitSetSp& AND(int firstBit, int lastBit, const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res) = delete;

		/**
		* @brief AND between lhs and rhs bitsets in the CLOSED block-range [firstBlock, lastBlock]
		*		 The result is stored in bitset res. Outside the range, res is set to 0.
		* @param firstBit, lastBit: closed bit-range 0 <= firstBit <= lastBit <= num_blocks()
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to the resulting bitstring res
		* @details: The num_blocks of lhs and rhs must be the same.
		**/
		friend  BitSetSp& AND_block(index_t firstBlock, index_t lastBlock, const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res);

		/**
		* @brief OR between lhs and rhs sparse bitsets - stores the result in sparse bitset res
		* @param lhs, rhs: input bitsets
		* @param res: output bitset
		* @returns reference to res
		**/
		friend  BitSetSp& OR(const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res);

		/**
		* @brief Removes 1-bits in the bitstring rhs from the bitstring lhs. Stores
		*		 the result in res
		* @returns reference to the resulting bitstring res
		* @details: set minus operation (res = lhs \ rhs)
		* @details: optimized for the case when lhs is much less dense than rhs (1)
		*
		* TODO - add optimization policy to allow to choose reference bitset (see (1) - 25/02/2025)
		**/
		friend BitSetSp& erase_bit(const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res);


		//TODO - add same interface as BitSet (25/02/2025)

	/////////////////////
	// constructors / destructors

		BitSetSp() noexcept : nBB_(0) {}

		/**
		* @brief Creates an EMPTY bitset given a population size nPop		  
		* @param nPop : population size 
		**/
		explicit BitSetSp(std::size_t nPop);

		/**
		* @brief Creates an EMPTY bitset given a number of 64-blocks
		*		 Population size nBlocks * 64
		* @param nBlocks : number of blocks
		**/
		static BitSetSp from_num_blocks(int nBlocks); 

		/**
		 * @brief Creates a bitset given an initial vector lv of 1-bit elements
		 *		  and a population num_blocks determined by the population maximum size nPop
		 *		  The num_blocks of the bitset is set according to nPop
		 * @param nPop: population size
		 * @param lv : vector of integers representing 1-bits in the bitset
		* @details: fast-fail policy, exception caught inside and the program exits
		 **/
		explicit BitSetSp(std::size_t nPop, const vint& lv);

		/**
		* @brief Creates a bitset with an initial list of 1-bit elements
		*		  and a population size nPop
		*		  The num_blocks of the bitset is set according to nPop
		* @param nPop: population size
		* @param lv : set of integers representing 1-bits in the bitset
		* @details: fast-fail policy, exception caught inside and the program exits
		**/
		explicit  BitSetSp(std::size_t nPop, std::initializer_list<int> lv);

		//Allows copy and move semantics
		BitSetSp(const BitSetSp& bbN) = default;
		BitSetSp(BitSetSp&&)		noexcept = default;
		BitSetSp& operator =			(const BitSetSp&) = default;
		BitSetSp& operator =			(BitSetSp&&)		noexcept = default;

		virtual ~BitSetSp() = default;

		////////////
		//reset / init (heap allocation)

		/**
		* @brief resets the sparse bitset to a new population size if is_popsize is true,
		*	     otherwise the maximum number of bitblocks of the bitset
		* @param nPop: population size
		* @param is_popsize: if true, the population size is set to nPop, otherwise the
		*					  it is the maximum number of bitblocks of the bitset
		* @details: fast-fail policy, exception caught inside and the program exits
		**/
		void reset(std::size_t nPop, bool is_popsize = true) noexcept;

		/**
		* @brief resets the sparse bitset to a new population size with lv 1-bits
		* @param nPop: population size
		* @param lv: vector of 1-bits to set
		* @details: fast-fail policy, exception caught inside and the program exits
		**/
		void reset(std::size_t nPop, const vint& lv) noexcept;

		/**
		* @brief resets the sparse bitset to a new population size, old syntax
		*		 now favoured by reset(...).
		* @details: fast-fail policy, exception caught inside and the program exits
		**/
		void init(std::size_t nPop, bool is_popsize = true) noexcept;

		/**
		* @brief reallocates memory to the number of blocks of the bitset
		**/
		void  shrink_to_fit() { vBB_.shrink_to_fit(); }


		/////////////////////
		//setters and getters (will not allocate memory)

		/**
		* @brief number of non-zero bitblocks in the bitstring
		* @details As opposed to the non-sparse case, it can be zero if there are no 1-bits
		*		   even though the maximum number of bitblocks determined in construction nBB_
		*		   can be anything.
		**/
		std::size_t size()				const noexcept { return vBB_.size(); }

		/**
		* @brief maximum number of bitblocks in the bitset
		* @details the num_blocks is determined by the population size (construction),
		* 		   - comment: NOT the size of @vBB_, which is the non-zero bitblocks in the bitset
		**/
		std::size_t num_blocks()		const noexcept { return nBB_; }


		/**
		* @brief returns the bitblock at position blockID, not the id-th block of the
		*		 equivalent non-sparse bitset in the general case. To find the id-th block
		*		 use find_bitblock function.
		**/
		BITBOARD  block(index_t blockID)	const { return vBB_[blockID].bb_; }
		BITBOARD& block(index_t blockID) { return vBB_[blockID].bb_; }

		SparseBlock  pBlock(index_t blockID)			const { return vBB_[blockID]; }
		SparseBlock& pBlock(index_t blockID) { return vBB_[blockID]; }

		const BlockVec & bitset()						const { return vBB_; }
		BlockVec & bitset() { return vBB_; }

		/**
		* @brief Finds the bitblock corresponding to @blockIDx
		*		 Block not found returns 0 (does not throw)
		* @param blockID: index of the block to find
		* @returns bitblock of the block index @blockID - 0 if the block does not exist
		* @details O(log) complexity
		*		 - does not dissambiguate between non-existing block and existing empty block
		**/
		BITBOARD find_block(index_t blockID)			const;

		/**
		* @brief Finds the bitblock given its block index blockID and its position in the collection. If it does not exists
		*		 provides bitblock closes to blockID with greater index. Depending on the Policy parameter, it can also provide
			*		 the position of the latter bitblock.
			* @param blockID: index of the block
			* @param pos: OUTPUT param which provides I) the position of the block in the collection with index BlockID,
			*			  II) BBObject::noBit depending if the block does not exist or the Policy parameter
			* param ReturnInsertPos: template param which, if true, asks for the position of the block returned
			* @returns: iterator to the closest bitblock with index greater or equal to blocKID
			*			or END() if the block does not exist. Depending on the Policy parameter, it can also provide
			*			the position of the bitblock pointed by the iterator. If the block does not exist, and the Policy iterPos = false
			*			pos = BBObject::noBit.
			* @details O(log) complexity
			* @details two implementations, read only and read-write
			**/
		template<bool ReturnInsertPos = false>
		BlockVecConstIt find_block(index_t blockID, index_t& insert_pos)	const;

		template<bool ReturnInsertPos = false>
		BlockVecIt   find_block(index_t blockID, index_t& pos);

		/**
		* @brief extended version of find_block. The template parameter UseLowerBound determines
		*		 the search policy: lower_bound (true) or upper_bound (false). If true, it searches
		*		 for the block with the same index as blockID, otherwise it searches for the closest
		*		 block with a greater index than blockID.
		*
		*		By default this function uses lower_bound policy, since with upper_bound policy the block
		*		cannot be found even if it exists.
		*
		* @param blockID: input index of the block searched
		* @returns a pair with first:true if the block exists, and second: the iterator to the block (policy = true) or the
		*		  iterator closest to the block with a greater index than blockID (policy=false / policy=true the block does not exist).
		*		  The iterator can return END in the following two cases:
		*		  a) Policy = true - the block does not exist and all the blocks have index less than blockID
		*		  b) Policy = false - all the blocks have index lower than OR EQUAL to blockID
		**/
		template<bool UseLowerBound = true>
		std::pair<bool, BlockVecIt >
			find_block_ext(index_t blockID);

		/**
		* @brief extended version of find_block, returning a const iterator
		*		 (see non-const function)
		**/
		template<bool UseLowerBound = true>
		std::pair<bool, BlockVecConstIt>
			find_block_ext(index_t blockID)			const;

		/**
		* @brief finds the position (index) of the bitblock with index blockID
		* @param blockID: input index of the block searched
		* @returns a pair with first:true if the block exists, and second: the index of the block in the collection
		*		   In the case the block does not exist (first element of the pair is false),
		*		   the second element is the closest block index ABOVE the blockID or -1 if no such block exists
		* @details O(log) complexity
		*
		* TODO - possibly remove - since it can be obained from find_block_ext trivially (21/02/2025)
		**/
		std::pair<bool, index_t>  find_block_pos(index_t blockID)			const;

		/**
		* @brief commodity iterators / const iterators for the bitset
		**/
		BlockVecIt   begin() { return vBB_.begin(); }
		BlockVecIt   end() { return vBB_.end(); }
		BlockVecConstIt cbegin() const { return vBB_.cbegin(); }
		BlockVecConstIt cend() const { return vBB_.cend(); }

		//////////////////////////////
		// Bitscanning (no cache) 

	protected:
		/**
		* @brief Determines the most significant bit in the bitstring
		* @returns index of the most significant bit in the bitstring
		*  @details implemented as a lookup table
		**/
		inline	int msbn64_lup()						const;

		/**
		* @brief Determines the most significant bit in the bitstring
		*		 and returns the bitblock of the last scanned bit
		**/
		inline	int msbn64_intrin(int& block)			const;
		inline  int msbn64_intrin()						const;

	public:
		int msb()	const { return msbn64_intrin(); }
		int msb(int& block)	 const { return msbn64_intrin(block); }
	protected:

		/**
		* @brief Determines the least significant bit in the bitstring
		* @details  implemented as a de Bruijn hashing or a lookup table depending on
		*			an internal switch (see config.h)
		* @returns index of the most significant bit in the bitstring
		**/
		inline	int lsbn64_non_intrin()					const;

		/**
		* @brief Determines the most significant bit in the bitstring
		*		 and returns the bitblock of the last scanned bit
		**/
		inline	int lsbn64_intrin(int& block)			const;
		inline  int lsbn64_intrin()						const;

	public:
		int lsb() const { return lsbn64_intrin(); }
		int lsb(int& block)	const { return lsbn64_intrin(block); }

	public:

		/**
		* @brief Computes the least significant 1-bit in the bitstring AFTER firstBit
		*		 If bit == BBObject::noBit, returns the lest significant bit in the bitstring.
		*
		*		 I.Primitive scanning stateless feature at this level. Require a bit position as argument always.
		*		II. Use bitscanning with state for proper bitscanning (derived class or external feature)
		*
		* @details: SAFE but not efficient. USE ONLY for basic bitscanning.
		* @details: O(log n) worst-case complexity, n is the number of blocks in the bitset
		**/
		inline	int next_bit(int firstBit)			const;

		/**
		* @brief Computes the next most significant 1-bit in the bitstring BEFORE  lastBit
		*		 If bit == BBObject::noBit, returns the most significant bit in the bitstring.
		*
		*		 I.Primitive scanning stateless feature at this level. Require a bit position as argument always.
		*		II. Use bitscanning with state for proper bitscanning (derived class or external feature)
		*
		* @details: SAFE but not efficient. USE ONLY for basic bitscanning.
		* @details: O(n) worst-case complexity, n is the number of blocks in the bitset
		**/
		inline	int prev_bit(int lastBit)			const;

	protected:

		///////////////////////////////////////////
		//DEPRECATED next_bit / prev_bit functions

		/**
		* @brief Computes the least significant 1-bit in the bitstring AFTER firstBit
		*		 If bit == BBObject::noBit, returns the lest significant bit in the bitstring.
		*
		*		 I.Basic scanning which caches the block to continue scanning. Requires a bit position as argument always.
		*		II.Use BBScanSp class specialized in bitscanning which caches block and last bit scanned for maximun efficiency
		* @details: Cached block information is a static variable. May only be used in a single-threaded environment,
		*			and only one sparse bitset can be scanned simultaneously (DO NOT USE)
		**/
		//inline	int next_bit			(int firstBit) = delete;

		/**
		* @brief Computes the most significant 1-bit in the bitstring BEFORE lastBit
		*		 If bit == BBObject::noBit, returns the lest significant bit in the bitstring.
		*
		*		 I.Basic scanning which caches the block to continue scanning. Requires a bit position as argument always.
		*		II.Use BBScanSp class specialized in bitscanning which caches block and last bit scanned for maximun efficiency.
		* @details: Cached block information is a static variable. May only be used in a single-threaded environment,
		*			and only one sparse bitset can be scanned simultaneously (DO NOT USE).
		**/
		//inline	int prev_bit			(int lastBit)  = delete;

		//////////////////////////////////////////////////


	public:
		/////////////////
		// Popcount

		/**
		* @brief Number of 1-bits in the bitstring
		*
		* @details alias to popcn64, calls the function
		* @details To be used instead of popcn64 (12/02/2025)
		**/
		int count()								const { return popcn64(); }
		int count(int firstBit, int lastBit)	const { return popcn64(firstBit, lastBit); }

	protected:
		/**
		* @brief number of 1-bits in THIS
		* @details implemented as a lookup table
		* @details implementation depends of POPCN64 switch in bbconfig.h
		*		   By default - intrinsic HW assembler instructions
		**/
		virtual inline	 int popcn64()						const;

	protected:
		/**
		* @brief Returns the number of 1-bits in the bitstring
		*	 	 in the closed range [firstBit, lastBit]
		*		 If lastBit == -1, the range is [firstBit, endOfBitset)
		*
		* @details efficiently implemented as a lookup table or with HW instructions
		*			depending  on an internal switch (see config.h)
		**/
		virtual	inline int popcn64(int firstBit, int lastBit)		const;

		/**
		* @brief number of 1-bits in the range [nBit, END(
		* @details implementation depends of POPCN64 switch in bbconfig.h
		*		   By default - intrinsic HW assembler instructions
		**/
		virtual inline	 int popcn64(int firstBit)					const;


	public:
		/////////////////////
		//Setting (ordered insertion) / Erasing bits  

		/**
		* @brief Sets THIS to the singleton bit
		**/
		inline BitSetSp& reset_bit(int bit);

		/**
		* @brief Sets THIS to 1-bits in the	 range [firstBit, lastBit]
		**/
		BitSetSp& reset_bit(int firstBit, int lastBit);

		/**
		* @brief Sets THIS to rhs in the range [0, lastBit]
		* @details: more efficient than using the more general reset_bit in the
		*			range [firstBit=0, lastBit]
		**/
	protected:
		inline	BitSetSp& reset_bit(int lastBit, const BitSetSp& rhs);

		/**
		* @brief Sets THIS to rhs in the closed range [firstBit, lastBit]
		**/
	public:
		inline	BitSetSp& reset_bit(int firstBit, int lastBit, const BitSetSp& rhs);

		/**
		* @brief Sets bit in THIS. If bit is outside the num_blocks of the bitset
		*		 it is ignored (feature)
		* @param bit: position of the bit to set
		* @returns 0 if the bit was set, -1 if error
		* @details emplaces pBlock in the bitstring or changes an existing bitblock
		* @detials O (log n) to determine if the block constaining firstBit is present
		* @details O (log n) to insert the new block according to index
		*		   ( + n potential shifts since it is a vector)
		**/
		inline	BitSetSp& set_bit(int bit);

		/**
		* @brief sets bits in the closed range [firstBit, lastBit]
		* @params firstBit, lastBit: range
		* @returns 0 if the bits were set, -1 if error
		* @details only one binary search is performed for the lower block
		* @details  I.  O(n)  to update/add blocks, where n = lastBit - firstBit
		* @details  II. O(log n) to determine the closest block to
		*				the first block of the range
		* @details  III.O(n log n) average case for sorting (if required)
		**/
		BitSetSp& set_bit(int firstBit, int lastBit);

		/**
		* @brief Adds the bits from the bitstring bitset in the population
		*		 range of *THIS (bitblocks are copied).
		*
		*		 I. Both bitsets MUST have the SAME num_blocks (number of blocks).
		*		II. Should have the same expected maximum population size
		*
		* @details  Equivalent to OR operation / set union
		* @details  Does not use iterators - not necessary to allocate memory a priori
		* @returns  reference to the modified bitstring
		**/
		BitSetSp& set_bit(const BitSetSp& bitset);
		BitSetSp& set_bit(int firstBit, int lastBit, const BitSetSp& bitset) = delete;	 //TODO

		/**
		* @brief Adds 1-bits in bitset in the closed range [firstBlock, lastBlock] to *this
		*		 (0 <= firstBlock <= lastBlock < the number of bitblocks in the bitset).
		* @param bitset input bitstring to be copied
		* @param firstBlock, lastBlock: input closed range of bitset
		* @returns reference to the modified bitstring
		* @details Does not require to allocate memory a priori
		**/
		BitSetSp& set_block(int firstBlock, int lastBlock, const BitSetSp& bitset);

	protected:
		/**
		* @brief Bitwise OR operator with bitset in the semi open range [firstBlock, END)
		* @details apply for set union
		*
		* TODO - CHECK, possible OR operation is missing (23/02/2025)
		**/
		inline BitSetSp& set_block(int firstBlock, const BitSetSp& bitset);

	public:
		/**
		* @brief sets bit to 0 in the bitstring
		* @param  bit: position of the 1-bit to set (>=0)
		* @returns reference to the modified bitstring
		* @details: zero blocks are not removed
		**/
		inline	BitSetSp& erase_bit(int bit);

		/**
		* @brief sets the bits in the closed range [firstBit, lastBit] to 0 in the bitstring
		* @param firstBit:, lastBit: 0 <= firstBit <= lastBit
		* @details: last_update 19/02/25
		* @details: zero blocks are not removed
		**/
		inline BitSetSp& erase_bit(int firstBit, int lastBit);

		/**
		 * @brief sets bit to 0 in the bitstring and returns t
		 * @returns the iterator to the block where the bit was set to 0, if it exists, otherwise
		 *			the iterator to the block with the closest greater index than the bit (can be END).
		 * @details oriented to basic bitscanning
		 * @details: zero blocks are not removed
		 *
		 * TODO -  improve or remove (19/02/2025)
		 **/
		inline	BlockVecIt   erase_bit(int bit, BlockVecIt  from_it);

		/**
		* @brief sets all bits to 0
		* @details: As opposed to the non-sparse case, this is an O(1) operation
		**/
		void  erase_bit() { vBB_.clear(); }

		/**
		* @brief Removes the 1-bits from bitset (inside *this num_blocks).
		*
		*		 I. bitset must have a maximum population greater or equal than the bitstring (CHECK - 19/02/2025)
		*
		* @details Equivalent to a set minus operation
		* @details: zero blocks are not removed
		* @returns reference to the modified bitstring
		**/
		BitSetSp& erase_bit(const BitSetSp& bitset);

		/**
		* @brief erase operation which effectively removes the zero blocks
		* @details EXPERIMENTAL - does not look efficient,
		*
		* TODO - refactor, actually used elsewhere in GRAPH. CHECK (19/02/2025)
		**/
		int	 clear_bit(int firstBit, int lastBit);

		/**
		* @brief Deletes the 1-bits from the bitstring bb_del in the closed range [firstBlock, lastBlock].
		*		 If lastBlock == -1, the range is [firstBlock, END).
		* @details:  0 <= firstBlock <= lastBlock < the number of bitblocks in the
		* @details:  Equivalent to a set minus operation
		*
		* @param firstBlock:, lastBlock: input closed range of bitset
		* @param bitset: input bitstring whose 1-bits are to be removed from *this
		* @returns reference to the modified bitstring
		**/
		inline  BitSetSp& erase_block(int firstBlock, int lastBlock, const BitSetSp& bitset);
	
	protected:
		/**
		* @brief Deletes the 1-bits from the bitstring bb_del in the SEMI_OPEN range [firstBlock, min{rhs.nBB_, *this->nBB_})
		*		 (set minus operation)
		* @details: can be substituted by the call erase_block(firstBLock, *this->nBB_), but should be more efficient
		*
		* @param firstBlock: initial block of the reange
		* @param bitset: input bitstring whose 1-bits are to be removed from *this
		* @returns reference to the modified bitstring
		**/
		inline BitSetSp& erase_block(int firstBlock, const BitSetSp& bitset);

	public:
		/**
		* @brief erase_block in the range [position of the block in the bitset, END)
		* @details weird function, referring to the position in the bitset and not the
		*		   block index.
		*
		* TODO - remove after checking calls  (22/02/2025)
		**/
		BitSetSp& erase_block_pos(int firstBlockPos, const BitSetSp& rhs) = delete;

		////////////////////////
		//Operators (member functions)

		/**
		* @brief Bitwise AND operator with rhs
		* @details apply for set intersection
		**/
		BitSetSp& operator &=				(const BitSetSp& rhs);

		/**
		* @brief Bitwise OR operator with rhs
		* @details set union operation
		**/
		BitSetSp& operator |=				(const BitSetSp& rhs);

		/**
		* @brief Bitwise XOR operator with rhs
		* @details set symmetric difference operation
		**/
		BitSetSp& operator ^=				(const BitSetSp& rhs);

		/**
		* @brief Bitwise AND operator with rhs in the range [firstBlock, END)
		* @details set intersection operation
		* @details Capacities of THIS and bitset should be the same.
		*
		* TODO - check semantics when the num_blocks of bitset is greater than the num_blocks of THIS (23/02/2025)
		**/
		inline BitSetSp& AND_block(index_t firstBlock, const BitSetSp& rhs);

		/////////////////////////
		//TODO - (19/02/2025)

		BitSetSp& AND_block(int firstBlock, int lastBlock, const BitSetSp& bitset) = delete;

		/////////////////////////////
		//Boolean functions

		 /**
		 * @brief TRUE if there is a 1-bit in the position bit
		 **/
		inline	bool is_bit(int bit)				const;

		/**
		* @brief TRUE if the bitstring has all 0-bits
		* @details O(num_bitblocks) complexity. Assumes it is possible
		*			that there are blocks with all 0-bits, so all of them need
		*			to be checked.
		**/
		inline	bool is_empty()						const;

		/**
		* @brief TRUE if this bitstring has no bits in common with rhs
		**/
		inline	bool is_disjoint(const BitSetSp& bb)	const;

		/**
		* @brief TRUE if this bitstring has no bits in common with rhs
		*		 in the closed range [first_block, last_block]
		**/
		inline	bool is_disjoint_block(index_t first_block, index_t last_block, const BitSetSp& bb)   const;

		////////////////////////
		 //Other operations 

		void sort() { std::sort(vBB_.begin(), vBB_.end(), pBlock_less()); }

		/////////////////////
		//I/O 

		/**
		* @brief streams bb and its size to @o  with format
		*		 [bit1 bit2 bit3 ... <(pc)>] - (if empty: [ ])
		* @param o: output stream
		* @param show_pc: if true, shows popcount
		* @param endl: if true, adds a new line at the end
		* @returns output stream
		* @details uses basic bitscanning implementation to enumerate the bits
		**/
		std::ostream& print(std::ostream& o = std::cout, bool show_pc = true, bool endl = true) const override;

		/////////////////////
		//Conversions

		/**
		* @brief converts the bitstring to a string of 1-bits with format
		*        [bit1 bit2 bit3 ... <(pc)>] - (if empty: [ ])
		* @details same as print, but returns a string
		* @returns string representation of the bitstring
		*
		* TODO implement - explicit cast operator for convenience (24/02/2025)
		**/
		std::string to_string()	const;

		/**
		* @brief Converts the bitstring to a std::vector of non-negative integers.
		*		 The size of the vector is the number of bits in the bitstring.
		* @param lb: output vector
		**/
		void extract(std::vector<int>& lb) const;

		/**
		* @brief Casts the bitstring to a vector of non-negative integers
		*		 (convenience function for the API)
		* @details calls to_vector
		**/
		operator vint () const;

		/////////////////////
		//data members

	protected:
		BlockVec  vBB_;				//a vector of sorted pairs of a non-empty bitblock and its index in a non-sparse bitstring
		int nBB_;					//maximum number of bitblocks

	}; //end BitSetSp class


}//end namespace bitgraph



/////////////////////////////////////////
//
// INLINE FUNCTIONS - necessary implementations of BitSetSP in the header

namespace bitgraph {

	bool BitSetSp::is_bit(int bit)	const {

		int bb = WDIV(bit);

		/////////////////////////////////////////////////////////////////////////////////////
		auto it = lower_bound(vBB_.cbegin(), vBB_.cend(), SparseBlock(bb), pBlock_less());
		/////////////////////////////////////////////////////////////////////////////////////

		return (it != vBB_.cend() &&
			it->idx_ == bb &&
			(it->bb_ & bblock::MASK_BIT(bit - WMUL(bb))));
	}


	bool BitSetSp::is_empty()	const {

		if (!vBB_.empty()) {

			//check if all bitblocks are empty - assumes it is possible, since erase operations allow it
			for (auto i = 0u; i < vBB_.size(); ++i) {
				if (vBB_[i].bb_) {
					return false;
				}
			}
		}

		return true;
	}


	bool BitSetSp::is_disjoint(const BitSetSp& rhs) const {

		auto itL = vBB_.begin();
		auto itR = rhs.vBB_.begin();

		//main loop
		while (itL != vBB_.end() && itR != rhs.vBB_.end())
		{
			if (itL->idx_ < itR->idx_) {
				itL++;
			}
			else if (itL->idx_ > itR->idx_) {
				itR++;
			}
			else {
				if (itL->bb_ & itR->bb_) {
					return false;				//bits in common	
				}
				++itL;
				++itR;
			}
		}

		return true;
	}


	bool BitSetSp::is_disjoint_block(index_t first_block, index_t last_block, const BitSetSp& rhs)   const {

		///////////////////////////////////////////////////////////////////////////////////
		assert(first_block >= 0 && first_block <= last_block && (last_block < rhs.num_blocks()));
		///////////////////////////////////////////////////////////////////////////////////

		auto posL = npos;
		auto posR = npos;
		auto itL = find_block(first_block, posL);
		auto itR = rhs.find_block(first_block, posR);

		//main loop
		while (itL != vBB_.end() &&
			itL->idx_ <= last_block &&
			itR != rhs.vBB_.end() &&
			itR->idx_ <= last_block)
		{

			if (itL->idx_ < itR->idx_) {
				++itL;
			}
			else if (itL->idx_ > itR->idx_) {
				++itR;

			}
			else {
				//same block index
				if (itL->bb_ & itR->bb_) {
					//////////////
					return false;					//bits in common	
					//////////////
				}
				++itL;
				++itR;
			}

		}//end while


		return true;
	}

	///////////////////
	// Bit updates


	BitSetSp&
		BitSetSp::erase_bit(int bit) {

		auto bb = WDIV(bit);

		//find closest block to blockID greater or equal
		auto it = lower_bound(vBB_.begin(), vBB_.end(), SparseBlock(bb), pBlock_less());

		if (it != vBB_.end() && it->idx_ == bb) {

			//removes the bit
			it->bb_ &= ~bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */);
		}

		return *this;
	}


	BitSetSp::BlockVecIt 
		BitSetSp::erase_bit(int bit, BitSetSp::BlockVecIt  from_it) {

		int bb = WDIV(bit);

		//iterator to the block of the bit if it exists or the closest non-empty block with greater index
		auto it = lower_bound(from_it, vBB_.end(), SparseBlock(bb), pBlock_less());

		if (it != vBB_.end() && it->idx_ == bb) {

			//erase bit if the block exists
			it->bb_ &= ~bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */);
		}

		return it;
	}


	BitSetSp&
		BitSetSp::set_bit(int bit) {

		auto bb = WDIV(bit);

		//////////////////////
		assert(bb < nBB_);
		//////////////////////

		//find closest block to bb in THIS
		auto pL = find_block_ext(bb);

		if (pL.first) {
			//bb exists - overwrite
			pL.second->bb_ |= bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */);
		}
		else {
			if (pL.second == vBB_.end()) {
				//there are not blocks with higher index
				vBB_.emplace_back(SparseBlock(bb, bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */)));
			}
			else {
				//there are blocks with higher index, insert to avoid sorting
				vBB_.insert(pL.second, SparseBlock(bb, bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */)));
			}
		}

		return *this;
	}




	BitSetSp&
		BitSetSp::reset_bit(int bit) {

		vBB_.clear();

		auto bb = WDIV(bit);
		vBB_.push_back(SparseBlock(bb, bblock::MASK_BIT(bit - WMUL(bb) /*WMOD(bit)*/)));

		return *this;
	}


	int BitSetSp::next_bit(int firstBit)  const {

		//special case - first bitscan
		if (firstBit == BBObject::noBit) {
			return lsb();
		}

		//find the block containing nBit or closest one with less index 
		int bbL = WDIV(firstBit);

		for (auto i = 0u; i < vBB_.size(); ++i) {

			//block bbL exists - find msb
			if (vBB_[i].idx_ == bbL) {

				int pos = bblock::lsb(vBB_[i].bb_ & Tables::mask_high[firstBit - WMUL(bbL)]);

				//exits if a bit is found
				if (pos != BBObject::noBit) {
					return (WMUL(bbL) + pos);
				}
				else {
					continue;		//next block
				}
			}

			//first non-zero block with greater index than bbL
			if (vBB_[i].bb_ && vBB_[i].idx_ > bbL) {
				return bblock::lsb(vBB_[i].bb_) + static_cast<int>(WMUL(vBB_[i].idx_));
			}
		}

		return BBObject::noBit;
	}


	int BitSetSp::prev_bit(int lastBit) const {

		//special case - first bitscan
		if (lastBit == BBObject::noBit) {
			return msb();
		}

		//find the block containing nBit or closest one with less index 
		int bbL = WDIV(lastBit);
		const auto size = static_cast<int>(vBB_.size());

		for (int i = size - 1; i >= 0; --i) {

			//block bbL exists - find msb
			if (vBB_[i].idx_ == bbL) {

				int pos = bblock::msb(vBB_[i].bb_ & Tables::mask_low[lastBit - WMUL(bbL)]);
				if (pos != BBObject::noBit) {
					return (WMUL(bbL) + pos);
				}
				continue;
			}

			//first block with less index than bbL
			if (vBB_[i].bb_ && vBB_[i].idx_ < bbL) {
				return bblock::msb(vBB_[i].bb_) + static_cast<int>(WMUL(vBB_[i].idx_));
			}
		}

		return BBObject::noBit;
	}


	int BitSetSp::msbn64_intrin(int& block)	const {

		Ul posInBB;
		const auto size = static_cast<int>(vBB_.size());

		for (int i = size - 1; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {
				block = i;
				return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}

		return BBObject::noBit;

		//old lookup version
		/*register union u {
			U16 c[4];
			BITBOARD b;
		}val;


		for (int i = vBB_.size() - 1; i >= 0; --i)
		{
			val.b = vBB_[i].bb_;
			if(val.b){
				block = i;
				if(val.c[3]) return (Tables::msba[3][val.c[3]] + WMUL(vBB_[i].idx_));
				if(val.c[2]) return (Tables::msba[2][val.c[2]] + WMUL(vBB_[i].idx_));
				if(val.c[1]) return (Tables::msba[1][val.c[1]] + WMUL(vBB_[i].idx_));
				if(val.c[0]) return (Tables::msba[0][val.c[0]] + WMUL(vBB_[i].idx_));
			}
		}*/

		return BBObject::noBit;		//should not reach here
	}


	int BitSetSp::msbn64_lup() const {

		union u {
			U16 c[4];
			BITBOARD b;
		}val{};

		const auto size = static_cast<int>(vBB_.size());
		for (int i = size; i >= 0; --i) {
			val.b = vBB_[i].bb_;
			if (val.b) {
				if (val.c[3]) return (Tables::msba[3][val.c[3]] + static_cast<int>(WMUL(vBB_[i].idx_)));
				if (val.c[2]) return (Tables::msba[2][val.c[2]] + static_cast<int>(WMUL(vBB_[i].idx_)));
				if (val.c[1]) return (Tables::msba[1][val.c[1]] + static_cast<int>(WMUL(vBB_[i].idx_)));
				if (val.c[0]) return (Tables::msba[0][val.c[0]] + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}

		return BBObject::noBit;		//should not reach here
	}


	int BitSetSp::msbn64_intrin() const
	{
		Ul posInBB;
		const auto size = static_cast<int>(vBB_.size());

		for (int i = size - 1; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {
				return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}

		return BBObject::noBit;
	}

	int BitSetSp::lsbn64_intrin(int& block) const {

		Ul posInBB;

		for (auto i = 0u; i < vBB_.size(); ++i) {
			if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {
				block = i;
				return(posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}


		//old lookup version
	//#ifdef DE_BRUIJN
	//	for(int i = 0; i < vBB_.size(); ++i){
	//		if(vBB_[i].bb_){
	//			block = i;
	//#ifdef ISOLANI_LSB
	//			return(Tables::indexDeBruijn64_ISOL[((vBB_[i].bb_ & -vBB_[i].bb_) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(vBB_[i].idx_));	
	//#else
	//			return(Tables::indexDeBruijn64_SEP[((vBB_[i].bb_ ^ (vBB_[i].bb_ - 1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >>
	//														bblock::DEBRUIJN_MN_64_SHIFT] + WMUL(vBB_[i].idx_)							);
	//#endif
	//		}
	//	}
	//#elif LOOKUP
	//	union u {
	//		U16 c[4];
	//		BITBOARD b;
	//	}val;
	//
	//	for(int i = 0; i < vBB_.size(); i++){
	//		val.b = vBB_[i].bb_;
	//		block = i;
	//		if(val.b){
	//			if(val.c[0]) return (Tables::lsba[0][val.c[0]] + WMUL(vBB_[i].idx_));
	//			if(val.c[1]) return (Tables::lsba[1][val.c[1]] + WMUL(vBB_[i].idx_));
	//			if(val.c[2]) return (Tables::lsba[2][val.c[2]] + WMUL(vBB_[i].idx_));
	//			if(val.c[3]) return (Tables::lsba[3][val.c[3]] + WMUL(vBB_[i].idx_));
	//		}
	//	}
	//
	//#endif

		return BBObject::noBit;
	}


	int BitSetSp::lsbn64_non_intrin() const {

#ifdef DE_BRUIJN
		for (auto i = 0u; i < vBB_.size(); ++i) {
			if (vBB_[i].bb_) {
#ifdef ISOLANI_LSB
				return(Tables::indexDeBruijn64_ISOL[((vBB_[i].bb_ & -vBB_[i].bb_) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT] + WMUL(vBB_[i].idx_));
#else
				return(Tables::indexDeBruijn64_SEP[((vBB_[i].bb_ ^ (vBB_[i].bb_ - 1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >>
					bblock::DEBRUIJN_MN_64_SHIFT] + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
#endif
		}
#elif LOOKUP
		register union u {
			U16 c[4];
			BITBOARD b;
		}val;

		for (int i = 0; i < vBB_.size(); ++i) {
			val.b = vBB_[i].bb_;
			if (val.b) {
				if (val.c[0]) return (Tables::lsba[0][val.c[0]] + static_cast<int>(WMUL(vBB_[i].idx_)));
				if (val.c[1]) return (Tables::lsba[1][val.c[1]] + static_cast<int>(WMUL(vBB_[i].idx_)));
				if (val.c[2]) return (Tables::lsba[2][val.c[2]] + static_cast<int>(WMUL(vBB_[i].idx_))));
				if (val.c[3]) return (Tables::lsba[3][val.c[3]] + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}

#endif
		return EMPTY_ELEM;
	}

	int BitSetSp::lsbn64_intrin() const
	{
		Ul posInBB;

		for (auto i = 0u; i < vBB_.size(); ++i) {
			if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {
				return(posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}

		return BBObject::noBit;
	}


	int BitSetSp::popcn64() const {

		BITBOARD pc = 0;

		for (auto i = 0u; i < vBB_.size(); ++i) {
			pc += bblock::popc64(vBB_[i].bb_);
		}

		return (int)pc;
	}


	int BitSetSp::popcn64(int firstBit) const {

		auto bbL = WDIV(firstBit);
		auto it = lower_bound(vBB_.begin(), vBB_.end(), SparseBlock(bbL), pBlock_less());
		BITBOARD pc = 0;

		if (it != vBB_.end()) {

			if (it->idx_ == bbL) {
				pc += bblock::popc64(it->bb_ & bblock::MASK_1_HIGH(firstBit - WMUL(bbL)));
				it++;
			}

			//counts the population of the rest of bitblocks
			for (; it != vBB_.end(); ++it) {
				pc += bblock::popc64(it->bb_);
			}
		}

		return (int)pc;
	}


	int BitSetSp::popcn64(int firstBit, int lastBit) const
	{
		////////////////////////////////
		if (lastBit == -1) {
			return popcn64(firstBit);
		}
		/////////////////////////////////

		auto bbL = WDIV(firstBit);
		auto it = lower_bound(vBB_.begin(), vBB_.end(), SparseBlock(bbL), pBlock_less());
		BITBOARD pc = 0;

		if (it != vBB_.end()) {

			if (it->idx_ == bbL) {
				pc += bblock::popc64(it->bb_ & bblock::MASK_1_HIGH(firstBit - WMUL(bbL)));
				it++;
			}

			//counts the population of the rest of bitblocks
			for (; it != vBB_.end() && it->idx_ <= lastBit; ++it) {
				pc += bblock::popc64(it->bb_);
			}
		}

		return (int)pc;
	}



	BitSetSp& BitSetSp::AND_block(index_t firstBlock, const BitSetSp& rhs) {

		//determine the closes block to firstBlock
		auto posL = npos;
		auto itL = this->find_block(firstBlock, posL);				//*this
		auto posR = npos;
		auto itR = rhs.find_block(firstBlock, posR);				//rhs

		while (itL != vBB_.end() && itR != rhs.vBB_.end())
		{
			//update before either of the bitstrings has reached its end
			if (itL->idx_ < itR->idx_)
			{
				/////////////////////
				itL->bb_ = 0;
				/////////////////////
				++itL;

			}
			else if (itL->idx_ > itR->idx_)
			{
				++itR;

			}
			else {

				//blocks with same index

				///////////////////////////////////
				itL->bb_ &= itR->bb_;
				///////////////////////////////////

				++itL;
				++itR;
			}
		}

		//fill with zeros from last block in *this onwards if rhs has been examined
		if (itR == rhs.vBB_.end()) {

			for (; itL != vBB_.end(); ++itL) {

				///////////////////////
				itL->bb_ = ZERO;
				////////////////////////
			}
		}

		return *this;
	}


	BitSetSp& BitSetSp::set_block(int firstBlock, const BitSetSp& rhs) {

		//determine the closes block to firstBlock
		auto  pairTHIS = this->find_block_ext(firstBlock);		//*this
		auto  pairR = rhs.find_block_ext(firstBlock);			 //rhs	


		while (pairTHIS.second < vBB_.end() && pairR.second < rhs.vBB_.end())
		{
			//update before either of the bitstrings has reached its end
			if (pairTHIS.second->idx_ < pairR.second->idx_)
			{
				++pairTHIS.second;
			}
			else if (pairR.second->idx_ < pairTHIS.second->idx_)
			{
				//OR - add block from rhs
				++pairR.second;
			}
			else {

				//blocks with same index

				///////////////////////////////////
				pairTHIS.second->bb_ |= pairR.second->bb_;
				///////////////////////////////////

				++pairTHIS.second;
				++pairR.second;
			}
		}

		//rhs unfinished with index below the last block of *this
		if (pairTHIS.second == vBB_.end()) {

			for (; pairR.second < rhs.vBB_.end(); ++pairR.second) {

				//////////////////////////////////
				vBB_.emplace_back(*pairR.second);
				///////////////////////////////////
			}

		}

		return *this;
	}



	BitSetSp& BitSetSp::erase_block(int firstBlock, int lastBlock, const BitSetSp& rhs)
	{
		//special case until the end of the bitset
		if (lastBlock == -1) {
			return erase_block(firstBlock, rhs);
		}

		//////////////////////////////////////////////////////////////////////////////////
		assert(firstBlock >= 0 && firstBlock <= lastBlock && lastBlock < rhs.num_blocks());
		//////////////////////////////////////////////////////////////////////////////////

		//determine the closest block in the range for both bitstrings
		auto pL = find_block_ext(firstBlock);
		auto pR = rhs.find_block_ext(firstBlock);

		while (pL.second != vBB_.end() &&
			pR.second != rhs.vBB_.end() &&
			pL.second->idx_ <= lastBlock &&
			pR.second->idx_ <= lastBlock)
		{

			//update before either of the bitstrings has reached its end
			if (pL.second->idx_ < pR.second->idx_)
			{
				++pL.second;
			}
			else if (pL.second->idx_ > pR.second->idx_)
			{
				++pR.second;
			}
			else {  //both indexes must be equal

				//////////////////////////////////
				pL.second->bb_ &= ~pR.second->bb_;			//set minus operation
				//////////////////////////////////

				++pL.second;
				++pR.second;
			}

		}//end while

		return *this;
	}


	BitSetSp& BitSetSp::erase_block(int firstBlock, const BitSetSp& rhs) {


		//determine the closest block in the range for both bitstrings
		auto pL = find_block_ext(firstBlock);
		auto pR = rhs.find_block_ext(firstBlock);

		//iteration
		while (pL.second != vBB_.end() &&
			pR.second != rhs.vBB_.end())
		{

			//update before either of the bitstrings has reached its end
			if (pL.second->idx_ < pR.second->idx_)
			{
				++pL.second;
			}
			else if (pL.second->idx_ > pR.second->idx_)
			{
				++pR.second;
			}
			else {  //both indexes must be equal

				//////////////////////////////////
				pL.second->bb_ &= ~pR.second->bb_;			//set minus operation
				//////////////////////////////////

				++pL.second;
				++pR.second;
			}

		}

		return *this;
	}


	BitSetSp& BitSetSp::erase_bit(int firstBit, int lastBit) {

		//////////////////////////////////////////////
		assert(firstBit >= 0 && firstBit <= lastBit);
		//////////////////////////////////////////////

		auto bbh = WDIV(lastBit);
		auto bbl = WDIV(firstBit);

		/////////////////////////
		assert(bbh < num_blocks());
		/////////////////////////

		auto offsetl = firstBit - WMUL(bbl);
		auto offseth = lastBit - WMUL(bbh);

		//determines the block in bbl or the closest one with greater index 
		auto it = lower_bound(vBB_.begin(), vBB_.end(), SparseBlock(bbl), pBlock_less());

		//special case - no bits to erase in the closed range
		if (it == vBB_.end()) {
			return *this;
		}

		//lower block
		if (it->idx_ == bbl)				//lower block exists
		{

			if (bbh == bbl)					//case singleton range
			{
				it->bb_ &= bblock::MASK_0(offsetl, offseth);

				//////////////
				return *this;
				/////////////
			}

			//update lower block
			it->bb_ &= bblock::MASK_0_HIGH(offsetl);

			//next block for processing
			++it;
		}

		//rest of pBlocks
		for (; it != vBB_.end(); ++it) {

			if (it->idx_ >= bbh)			//exit condition
			{
				if (it->idx_ == bbh)
				{
					//extra processing if the end block exists

					if (bbh == bbl) {

						it->bb_ &= bblock::MASK_0(offsetl, offseth);

					}
					else {
						//add last block and trim
						it->bb_ &= bblock::MASK_0_LOW(offseth);
					}

				}

				//////////////
				return *this;
				/////////////
			}

			//Deletes intermediate block
			it->bb_ = ZERO;

		}//end for - pBlock iterations

		return *this;
	}


	BitSetSp& BitSetSp::reset_bit(int lastBit, const BitSetSp& rhs) {

		auto bbh = WDIV(lastBit);

		////////////////////////
		assert(bbh < nBB_);
		///////////////////////

		vBB_.clear();

		//finds block bbh, or closest with higher index than bbh in rhs
		auto pR = rhs.find_block_ext(bbh);

		//special case - no bits to set in the closed range
		if (pR.second != rhs.cend()) {

			//copy up to and excluding bbh
			std::copy(rhs.cbegin(), pR.second, std::insert_iterator<BlockVec>(vBB_, vBB_.begin()));

			//deal with last block bbh
			if (pR.first) {
				vBB_.emplace_back(SparseBlock(bbh, pR.second->bb_ & bblock::MASK_1_LOW(lastBit - WMUL(bbh))));

			}
		}

		return *this;
	}


	BitSetSp& BitSetSp::reset_bit(int firstBit, int lastBit, const BitSetSp& rhs) {

		//////////////////////////////
		//special case - range starts from the beginning, called specialized case
		if (firstBit == 0) {
			return reset_bit(lastBit, rhs);
		}
		///////////////////////////////

		auto bbl = WDIV(firstBit);
		auto bbh = WDIV(lastBit);

		////////////////////////////////////////////////////////////////////////////////////
		assert((bbl >= 0) && (bbl <= bbh) && (bbh < nBB_) && (bbh < rhs.num_blocks()));
		///////////////////////////////////////////////////////////////////////////////////

		vBB_.clear();

		//finds bbl or closest block with greater indx
		auto it = lower_bound(rhs.cbegin(), rhs.cend(), SparseBlock(bbl), pBlock_less());

		//special case - no bits to set in the closed range
		if (it == rhs.cend()) {
			return *this;
		}

		//first block exists
		auto offsetl = firstBit - WMUL(bbl);
		auto offseth = lastBit - WMUL(bbh);
		if (it->idx_ == bbl)
		{
			//special case, the range is a singleton, exit condition
			if (bbh == bbl)
			{
				vBB_.emplace_back(SparseBlock(bbl, it->bb_ & bblock::MASK_1(offsetl, offseth)));

				return *this;
			}
			else {

				//add lower block
				vBB_.emplace_back(SparseBlock(bbl, it->bb_ & bblock::MASK_1_HIGH(offsetl)));

				//next block
				++it;
			}
		}

		//main loop copies blocks (bbl, bbh]
		//alternative implementation - find lower bound for bbh and copy (as in reset(lastBit))
		while (it != rhs.cend() && it->idx_ < bbh) {
			vBB_.emplace_back(*it);
			++it;
		}

		//upate due to bbh if it exists
		if (it != rhs.cend() && it->idx_ == bbh) {

			//add and trim last block
			vBB_.push_back(SparseBlock(bbh, it->bb_ & bblock::MASK_1_LOW(offseth)));

		}

		return *this;
	}

	template<bool ReturnInsertPos>
	inline
		BitSetSp::BlockVecConstIt
		BitSetSp::find_block(index_t blockID, index_t& pos) const
	{

		////////////////////////////////////////////////////////////////////////////////////////////
		auto it = lower_bound(vBB_.cbegin(), vBB_.cend(), SparseBlock(blockID), pBlock_less());
		////////////////////////////////////////////////////////////////////////////////////////////

		if (it != vBB_.end() && (it->idx_ == blockID || ReturnInsertPos)) {
			pos = static_cast<index_t>(it - vBB_.begin());
		}
		else {
			pos = npos;
		}

		return it;
	}

	template<bool ReturnInsertPos>
	inline
		BitSetSp::BlockVecIt 
		BitSetSp::find_block(index_t blockID, index_t& pos)
	{

		////////////////////////////////////////////////////////////////////////////////////////////
		auto it = lower_bound(vBB_.begin(), vBB_.end(), SparseBlock(blockID), pBlock_less());
		////////////////////////////////////////////////////////////////////////////////////////////

		if (it != vBB_.end() && (it->idx_ == blockID || ReturnInsertPos)) {
			pos = static_cast<int>(it - vBB_.begin());
		}
		else {
			pos = npos;
		}

		return it;
	}

	template<bool UseLowerBound>
	std::pair<bool, BitSetSp::BlockVecIt >
		BitSetSp::find_block_ext(index_t blockID)
	{
		std::pair<bool, BlockVecIt > res;

		if (UseLowerBound) {
			////////////////////////////////////////////////////////////////////////////////////////////
			res.second = lower_bound(vBB_.begin(), vBB_.end(), SparseBlock(blockID), pBlock_less());
			////////////////////////////////////////////////////////////////////////////////////////////

			res.first = (res.second != vBB_.end()) && (res.second->idx_ == blockID);
		}
		else {
			////////////////////////////////////////////////////////////////////////////////////////////
			res.second = upper_bound(vBB_.begin(), vBB_.end(), SparseBlock(blockID), pBlock_less());
			////////////////////////////////////////////////////////////////////////////////////////////

			res.first = false;			//the same block cannot be found with upper_bound, only the closest with greater index
		}

		return res;
	}

	template<bool UseLowerBound>
	std::pair<bool, BitSetSp::BlockVecConstIt>
		BitSetSp::find_block_ext(index_t blockID) const
	{
		std::pair<bool, BlockVecConstIt>res;

		if (UseLowerBound) {
			////////////////////////////////////////////////////////////////////////////////////////////
			res.second = lower_bound(vBB_.cbegin(), vBB_.cend(), SparseBlock(blockID), pBlock_less());
			////////////////////////////////////////////////////////////////////////////////////////////

			res.first = (res.second != vBB_.end()) && (res.second->idx_ == blockID);
		}
		else {
			////////////////////////////////////////////////////////////////////////////////////////////
			res.second = upper_bound(vBB_.cbegin(), vBB_.cend(), SparseBlock(blockID), pBlock_less());
			////////////////////////////////////////////////////////////////////////////////////////////

			res.first = false;			//the same block cannot be found with upper_bound, only the closest with greater index
		}

		return res;
	}

}//end namespace bitgraph


///////////////////////////
// friend functions of BitSetSp

namespace bitgraph {

	inline
		bool operator == (const BitSetSp& lhs, const BitSetSp& rhs) {
		return((lhs.nBB_ == rhs.nBB_) &&
			(lhs.vBB_ == rhs.vBB_));
	}

	inline
		bool operator != (const BitSetSp& lhs, const BitSetSp& rhs) { return !(lhs == rhs); }


	inline
		BitSetSp& AND(const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res) {


		/////////////////////////////////////////////////////////////
		res.reset((int)lhs.num_blocks(), false);
		res.vBB_.reserve(std::min(lhs.vBB_.size(), rhs.vBB_.size()));
		/////////////////////////////////////////////////////////////

		//special case
		if (lhs.is_empty() || rhs.is_empty()) {
			return res;
		}

		auto itR = rhs.cbegin();
		auto itL = lhs.cbegin();

		//////////////////
		//A) General purpose code assuming no a priori knowledge about population size in lhs and rhs
		do {
			if (itL->idx_ < itR->idx_) {
				itL++;
			}
			else if (itL->idx_ > itR->idx_) {
				itR++;
			}
			else {
				////////////////////////////////////////////////////////////////////////
				res.vBB_.push_back(BitSetSp::SparseBlock(itL->idx_, itL->bb_ & itR->bb_));
				/////////////////////////////////////////////////////////////////////////
				itL++;
				itR++;
			}
		} while (itL != lhs.vBB_.end() && itR != rhs.vBB_.end());


		/////////////////	
		//B) Optimization for the case lhs has less 1-bits than rhs

		//for (auto& blockL : lhs.vBB_) {

		//	//finds closest
		//	while (itR != rhs.vBB_.end() && itR->idx_ < blockL.idx_) { itR++; }

		//	//exit condition - rhs has been examined
		//	if (itR == rhs.vBB_.end()) { break;	}

		//	//blocks remain to be examined in both bitsets
		//	if (blockL.idx_ == itR->idx_) {
		//		res.vBB_.push_back(BitSetSp::SparseBlock(blockL.idx_, blockL.bb_ & itR->bb_));
		//	}
		//	
		//}

		return res;
	}

	inline
		BitSetSp AND(BitSetSp lhs, const BitSetSp& rhs) { return lhs &= rhs; }

	inline
		BitSetSp& OR(const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res) {

		/////////////////////////////////////////////////////////////
		res.reset((int)lhs.num_blocks(), false);
		res.vBB_.reserve(lhs.vBB_.size() + rhs.vBB_.size());
		/////////////////////////////////////////////////////////////

		auto itR = rhs.cbegin();
		auto itL = lhs.cbegin();

		while (itL != lhs.vBB_.end() && itR != rhs.vBB_.end()) {

			if (itL->idx_ < itR->idx_) {
				res.vBB_.push_back(BitSetSp::SparseBlock(itL->idx_, itL->bb_));
				res.print(std::cout, true);
				itL++;
			}
			else if (itL->idx_ > itR->idx_) {
				res.vBB_.push_back(BitSetSp::SparseBlock(itR->idx_, itR->bb_));
				res.print(std::cout, true);
				itR++;
			}
			else {

				////////////////////////////////////////////////////////////////////////
				res.vBB_.push_back(BitSetSp::SparseBlock(itL->idx_, itL->bb_ | itR->bb_));
				/////////////////////////////////////////////////////////////////////////

				itL++;
				itR++;
			}
		}

		//add rest of non-examined blocks 
		if (itR != rhs.vBB_.end()) {
			res.vBB_.insert(res.vBB_.end(), itR, rhs.vBB_.end());
		}
		else if (itL != lhs.vBB_.end()) {
			res.vBB_.insert(res.vBB_.end(), itL, lhs.vBB_.end());
		}


		return res;
	}


	inline
		BitSetSp& AND_block(BitSetSp::index_t firstBlock, BitSetSp::index_t lastBlock, const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res) {

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		assert(firstBlock >= 0 && firstBlock <= lastBlock && lastBlock < rhs.num_blocks() && lastBlock < lhs.num_blocks());
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		res.reset((int)lhs.num_blocks(), false);
		res.vBB_.reserve(std::min(lhs.vBB_.size(), rhs.vBB_.size()));
		//////////////////////////////////////////////////////////////

		auto posL = BitSetSp::npos;
		auto posR = BitSetSp::npos;
		auto itL = lhs.find_block(firstBlock, posL);
		auto itR = rhs.find_block(firstBlock, posR);

		//special case - out of range
		if (itL == lhs.vBB_.end() || itR == rhs.vBB_.end()) { return res; }

		//main loop	
		while (itL != lhs.vBB_.end() &&
			itL->idx_ <= lastBlock &&
			itR != rhs.vBB_.end() &&
			itR->idx_ <= lastBlock)
		{

			//update before either of the bitstrings has reached its end
			if (itL->idx_ < itR->idx_) {
				itL++;
			}
			else if (itL->idx_ > itR->idx_) {
				itR++;
			}
			else {
				//blocks with same index
				///////////////////////////////////
				res.vBB_.push_back(BitSetSp::SparseBlock(itL->idx_, itL->bb_ & itR->bb_));
				///////////////////////////////////
				++itL;
				++itR;
			}

		}//end while

		return res;
	}

	inline
		BitSetSp& erase_bit(const BitSetSp& lhs, const BitSetSp& rhs, BitSetSp& res) {

		//special case - rhs empty bitstring
		if (rhs.is_empty()) {
			return (res = lhs);
		}

		///////////////////////////////////
		res.reset(static_cast<int>(lhs.num_blocks()), false);
		res.vBB_.reserve(lhs.vBB_.size());
		///////////////////////////////////

		auto itR = rhs.cbegin();
		auto itL = lhs.cbegin();

		//optimized races based on the assumption that lhs has less 1-bits than rhs
		for (auto& blockL : lhs.vBB_) {

			//finds closest
			while (itR != rhs.vBB_.end() && itR->idx_ < blockL.idx_) { itR++; }

			//exit condition - rhs has been examined
			if (itR == rhs.vBB_.end()) {
				res.vBB_.insert(res.vBB_.end(), itL, lhs.vBB_.end());
				break;
			}

			//blocks remain to be examined in both bitsets
			if (blockL.idx_ == itR->idx_) {
				res.vBB_.push_back(BitSetSp::SparseBlock(blockL.idx_, blockL.bb_ & ~itR->bb_));
			}
			else {
				res.vBB_.push_back(BitSetSp::SparseBlock(blockL.idx_, blockL.bb_));
			}
		}

		return res;
	}

	
	/*using _impl::operator==;
	using _impl::operator!=;
	using _impl::AND;
	using _impl::AND_block;
	using _impl::OR;
	using _impl::erase_bit;*/

}//end namespace bitgraph






//////////////////////////
//
// DEPRECATED
//
/////////////////////////

//inline
//BitSetSp&  BitSetSp::erase_block_pos (int first_pos_of_block, const BitSetSp& rhs ){
///////////////////////
//// erases bits from a starting block in *this (given as the position in the bitstring collection, not its index) till the end of the bitstring, 
//
//	int i2 = 0;							//all blocks in rhs are considered
//	const int MAX = rhs.vBB_.size()-1;
//	
//	//optimization which works if rhs has less 1-bits than *this
//	for (int i1 = first_pos_of_block; i1 < vBB_.size(); ++i1){
//		for (; i2 < MAX && rhs.vBB_[i2].idx_ < vBB_[i1].idx_; ++i2) {
//
//			//update before either of the bitstrings has reached its end
//			if (vBB_[i1].idx_ == rhs.vBB_[i2].idx_) {
//				vBB_[i1].bb_ &= ~rhs.vBB_[i2].bb_;
//			}
//		}
//	}
//
//	return *this;
//}


#endif
