/**  
 * @file bitboardn.h file 
 * @brief header file of the BitBoardN class from the BITSCAN library.
 *		  Manages bitstrings of any size as an array of bitblocks (64-bit numbers)
 * @author pss
 * @details: created 2014, last_update 01/02/2025
 * 
 * TODO - refactoring and testing (31/01/2025)
 *
 **/

#ifndef __BITBOARDN_H__
#define __BITBOARDN_H__

#include "bbobject.h"
#include "bitboard.h"	
#include <vector>	
#include "utils/common.h"			//stack

#include <cassert>					//uncomment NDEBUG in config.h to enable run-time assertions

//useful alias
using vint = std::vector<int>;
using vbset  = std::vector<BITBOARD>;	

/////////////////////////////////
//
// BitBoardN class 
//
// Manages bit strings greater than WORD_SIZE 
// @details Does not use HW dependent instructions (intrinsics), nor does it cache information for very fast bitscanning
//
///////////////////////////////////
class BitBoardN:public BBObject{
public:	

/////////////////////////////
// Independent operators / masks  
// comment: do not modify this bitset
	
	/**
	* @brief AND between lhs and rhs bitsets, stores the result in an existing bitset res
	* @returns reference to the resulting bitstring res
	**/
	friend BitBoardN&  AND			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);	
	
	/**
	* @brief AND between lhs and rhs bitsets
	* @returns resulting bitset
	**/
	friend BitBoardN   AND			(BitBoardN lhs, const BitBoardN& rhs)									{ return lhs &= rhs; }
			
	/**
	* @brief AND between lhs and rhs bitsets in the CLOSED bit-range [firstBit, lastBit]
	*		 The result is stored in bitset res. The remaining bits of res outside the range
	*		 are set to 0 if the template parameter Erase  is true.
	* @param template<Erase>: if true, the bits of res outside [firstBit, lastBit] are set to 0
	* @param firstBit, lastBit: closed bit-range 0 < firstBit <= lastBit
	* @param lhs, rhs: input bitsets
	* @param res: output bitset
	* @returns reference to the resulting bitstring res
	* @details: The capacity of lhs and rhs must be the same.
	*			The capacity of res must be greater or equal than lhs / rhs
	* @details: created 06/02/2025
	**/
	template<bool Erase>
	friend BitBoardN& AND			(int firstBit, int lastBit, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res);

	/**
	* @brief AND between lhs and rhs bitsets in the closed block- range [firstBlock, lastBlock]. 
	*		 Stores the result in res. The remaining bits of res outside the range
	*		 are set to 0 if the template parameter Erase is true.
	*		 If lastBock==-1, the range is til the end of the bitset, i.e., [firstBlock, capacity())
	*		 
	*		I.  The capacity of lhs and rhs must be the same. 
	*		II. The capacity of res must be at least the same as lhs nand rhs
	*
	* @returns reference to the resulting bitstring res
	**/
	template<bool Erase>
	friend BitBoardN&  AND_block	(int firstBlock, int lastBlock, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	
	/**
	* @brief AND between lhs and rhs bitsets in the closed block-range [firstBlock, lastBlock]. 
	*		 If lastBock==-1, the range is the full bitset. The bits outside the range are set to 0.
	* @returns the new resulting bitset
	**/	
	friend BitBoardN AND_block		(int firstBlock, int lastBlock, BitBoardN lhs, const BitBoardN& rhs);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DEPRECATED AND, TO BE REMOVED (06/02/2025)
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
	* @details: The capacity of lhs and rhs must be the same.
	*			The capacity of res must be greater or equal than lhs / rhs
	*
	* TODO: slightly weird behaviour, check if it is necessary
	* TODO: add firstBit logic  (06/02/2025)
	* TODO: deprecated - remove after checking optimization algorithms (06/02/2025)
	**/
	template<bool Erase>
	friend BitBoardN&  AND			(int lastBit, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res);
		
	/**
	* @brief AND between lhs and rhs bitsets in the SEMI-OPEN range [0, last_vertex)
	*		 For C-array compatibility, the result is stored in bitset res
	* @returns C-array of integers representing the resulting bitstring res, array size is stored in size
	* @details: Experimental, not efficient
	* TODO: deprecated - REMOVE after checking optimization algorithms (06/02/2025)
	**/
	friend int*	       AND			(int lastBit, const BitBoardN& lhs, const BitBoardN& rhs, int bitset[], int& size);				
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief OR between lhs and rhs bitsets, stores the result in an existing bitset res
	* @returns reference to the resulting bitstring res
	**/
	friend BitBoardN&  OR			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);

	/**
	* @brief OR between lhs and rhs bitsets
	* @returns resulting bitset
	**/
	friend BitBoardN   OR			(BitBoardN lhs, const BitBoardN& rhs)								{ return lhs |= rhs; }

	/**
	* @brief OR between lhs and rhs bitsets in the CLOSED bit-range [firstBit, lastBit]
	*		 The result is stored in bitset res. The remaining bits of res outside the range
	*		 are set to 0.
	* @param lhs, rhs: input bitsets
	* @param res: output bitset
	* @returns reference to the resulting bitstring res
	* @details: The capacity of lhs and rhs must be the same.
	*			The capacity of res must be greater or equal than lhs / rhs
	* @details: created 06/02/2025
	**/
	friend BitBoardN&  OR			(int firstBit, int lastBit, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DEPRECATED OR, TO BE REMOVED (06/02/2025)
		
	// lhs OR rhs in semiopen range (rhs [from, END)
	friend BitBoardN&  OR			(int firstBit, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);	
	
	// lhs OR rhs - ranges (rhs [v, END[ if left = TRUE,  rhs [0, v[ if left = false)
	// date@26/10/19
	friend BitBoardN&  OR			(int bit, bool left /* to */, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	//bit deletion
	friend BitBoardN&  ERASE		(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);										//removes rhs from lhs
	
	//first elem in common
	friend int FIRST_SHARED			(const BitBoardN& lhs, const BitBoardN& rhs);
	friend int FIRST_SHARED			(int firstBlock, const BitBoardN& lhs, const BitBoardN& rhs);	

////////////
//construction / destruction 

	 BitBoardN						(): nBB_(0)				{};	

	 /**
	 * @brief Constructor of an EMPTY bitset given a population size nPop
	 *		  The capacity of the bitset is set according to the population size
	 * @param nBits : population size		
	 **/
	 explicit  BitBoardN			(int nBits);

	 /**
	 * @brief Constructor of an EMPTY bitset given an initial vector lv of 1-bit elements 
	 *		  The population size is the maximum value of lv
	 *		  The capacity of the bitset is set according to the population size
	 * @param lv : vector of integers representing 1-bits in the bitset
	 **/
	explicit  BitBoardN				(const vint& lv);

	/**
	 * @brief Constructor of an EMPTY bitset given an initial vector lv of 1-bit elements
	 *		  and a population size nPop
	 *		  The capacity of the bitset is set according to nPop
	 * @param nPop: population size
	 * @param lv : vector of integers representing 1-bits in the bitset
	 **/
	explicit  BitBoardN				(int nPop, const vint& v);
		 	 
	 //Move and copy semantics allowed
	BitBoardN						(const BitBoardN& bbN)				= default;
	BitBoardN						(BitBoardN&&)			noexcept	= default;
	BitBoardN& operator =			(const BitBoardN&)					= default;
	BitBoardN& operator =			(BitBoardN&&)			noexcept	= default;

virtual	~BitBoardN					()									= default;

////////////
//Reset / init (memory allocation)
	void init						(int popsize);										
	void init						(int popsize, const vint& );
	
	/**
	* @brief Resets this bitset given to a vector lv of 1-bit elements
	 *		  and a population size nPop.
	 * 
	 *		  I. The capacity of the bitset is set according to nPop.
	 *		  II. Memory is deallocated and reallocated as required	
	 * @param nPop: population size
	 * @param lv : vector of integers representing 1-bits in the bitset	
	**/
	void reset						(int nPop, const vint& lv);

	/**
	* @brief Resets this bitset to an EMPTY BITSET given to a population size nPop.
	*
	*		  I. The capacity of the bitset is set according to nPop.
	*		  II. Memory is deallocated and reallocated as required
	* 
	* @param nPop: population size
	**/
	void reset						(int nPop);
			

/////////////////////
//setters and getters (will not allocate memory)
	
	int number_of_bitblocks()					const					{ return nBB_; }
	
	/**
	* @brief alternative syntax for number_of_bitblocks
	**/
	int capacity()								const					{ return nBB_; }

	vbset& bitstring				()									{ return vBB_; }
const vbset& bitstring				()			const					{ return vBB_; }
	
const BITBOARD bitblock				(int block) const					{ return vBB_[block]; }
	BITBOARD& bitblock				(int block)							{ return vBB_[block]; }

//////////////////////////////
// Bitscanning (no HW operations)
		
	/**
	* @brief returns the index of the most significant bit in the bitstring
	* @details implemented as a lookup table
	**/
inline virtual int msbn64			()	const;		//lookup 

	/**
	* @brief returns the index of the least significant bit in the bitstring
	* @details  implemented as a de Bruijn hashing or a lookup table depending on 
	*			an internal switch (see config.h)
	**/
inline virtual int lsbn64			()	const; 		//de Bruijn	/ lookup								
	
	/**
	* @brief Computes the next least significant 1-bit in the bitstring after bit
	*		 If bit == EMPTY_ELEM, returns the lest significant bit in the bitstring
	* 
	* @param bit: position from which to start the search
	* @returns the next 1-bit in the bitstring after bit, EMPTY_ELEM if there are no more bits
	* @details: no internal state is used, NOT EFFICIENT since it has to compute the offset and
	*		    current bitblock of bit in each call. It does not cache the last bit found.
	* @details: Uses a DeBrijn implementation for lsbn64()
	* @details: Deprecated in favour of the bitscanning with state of BBIntrinsic class
	**/
inline int next_bit					(int bit)	const;					

////////////////////////////
// Returns next bit assuming, when used in a loop, that the last bit
// scanned is deleted prior to the call
	/**
	* @brief Computes the least significant 1-bit in the bitstring starting from the
	*		 bitblock that contains the input bit. 
	*		 If bit == EMPTY_ELEM, returns the lest significant bit in the bitstring.
	* 
	* @param bit: position of the bitblock where to start the search
	* @returns The lsb starting from the bitblock of bit, EMPTY_ELEM if there are no more bits
	* 
	* @details To be used inside a bitscanning loop, where the last bit scanned 
	*		   is deleted prior to the call.
	* @details Uses a De Bruijn hashing implementation for lsbn64()
	* @details Preliminary attempt for bitscanning with state information
	**/
inline int next_bit_if_del			(int bit)	const;					
	
	/**
	* @brief Computes the next most significant  1-bit in the bitstring after bit
	*		 If bit == EMPTY_ELEM, returns the most significant bit in the bitstring
	*
	* @param bit: position from which to start the search
	* @returns the next msb 1-bit in the bitstring after bit, EMPTY_ELEM if there are no more bits
	* @details: no internal state is used, NOT EFFICIENT since it has to compute the offset and
	*		    current bitblock of bit in each call. It does not cache the last bit found.
	* @details: Uses a lookup table implementation for msbn64()
	* @details: Deprecated in favour of the bitscanning with state of BBIntrinsic class
	**/
inline int prev_bit					(int bit)	const;					
	
/////////////////
// Popcount

	/**
	* @brief returns the number of 1-bits in the bitstring
	* @details alias to popcn64, calls the function
	**/
std::size_t	size					()							const		{return (std::size_t) popcn64();}

	/**
	* @brief returns the number of 1-bits in the bitstring
	* @details implemented as a lookup table	
	**/
virtual	inline int popcn64			()							const;		 

	/**
	* @brief returns the number of 1-bits in the bitstring
	*	 	in the closed range [firstBit, lastBit]
	* @details efficiently implemented as a lookup table or
	*		   with HW instructions depending  on an internal switch (see config.h)
	**/
virtual	inline int popcn64			(int firstBit, int lastBit)	const;

/////////////////////
//Setting / Erasing bits 
			
	/**
	* @brief sets a 1-bit in the bitstring
	* @param  bit: position of the 1-bit to set (nBit >= 0)
	* @returns reference to the modified bitstring
	**/
		
	template<bool EraseAll = false>
	BitBoardN&  set_bit				(int bit);

	/**
	* @brief sets the bits in the closed range [firstBit, lastBit] to 1 in the bitstring
	* @params firstBit, lastBit:  0 < firstBit <= lastBit
	* @date 22/9/14
	* @last_update 01/02/25
	**/
		template<bool EraseAll = false>
inline  BitBoardN&	 set_bit		(int firstBit, int lastBit);

	/**
	* @brief sets all bits to 1
	**/
inline  BitBoardN&  set_bit			();
	 	
	/**
	* @brief Adds the bits from the bitstring bb_add in the population
	*		 range of the bitstring (bitblocks are copied).
	* 
	*		 I. The bitblock size of bb_add must be at least as large as the bitstring.
	*		
	* @details  Equivalent to OR operation / set union
	* @returns reference to the modified bitstring
	**/
inline	BitBoardN& set_bit			(const BitBoardN& bb_add);
		
	/**
	* @brief Adds the bits from the bitstring bb_add in the range [0, lastBit] 
	* @param lastBit : the last bit in the range to be copied
	* @returns reference to the modified bitstring
	**/
inline BitBoardN& set_bit			(int lastBit, const BitBoardN& bb_add);
	 
	/**
	* @brief Adds elements from a vector of non-negative integers lv as 1-bit
	*	    up to the maximum capacity of the bitstring. Values greater than the
	*		maximum population size are ignored.
	* 
	* @param lv: vector of non-negative integers
	* @param EraseAll <template>: if true, deletes the rest of bits in the bitstring
	* @returns reference to the modified bitstring
	* @details negative elements will cause an assertion if NDEBUG is not defined, 
	*		  else the behaviour is undefined.
	**/
	template<bool EraseAll = false>	
	BitBoardN& set_bit				(const vint& lv);

	/**
	* @brief sets bit number bit to 0 in the bitstring
	* @param  bit: position of the 1-bit to set (>=0)
	* @returns reference to the modified bitstring
	**/
inline	BitBoardN& erase_bit		(int bit);

	/**
	* @brief sets the bits in the closed range [firstBit, lastBit] to 0 in the bitstring
	* @params firstBit, lastBit: 0 < firstBit <= lastBit
	* @created 22/9/14
	* @last_update 01/02/25
	**/
inline BitBoardN& erase_bit			(int firstBit, int lastBit);

	/**
	* @brief sets all bits to 0
	* @returns reference to the modified bitstring
	**/
inline BitBoardN& erase_bit			();

	/**
	* @brief Removes the bits from the bitstring bb_del inside the population range.
	*
	*		 I. bb_del must have a maximum population
	*			greater or equal than the bitstring.
	*
	* @details  Equivalent to a set minus operation
	* @returns reference to the modified bitstring
	**/
inline	BitBoardN& erase_bit		(const BitBoardN& bb_del);	

	/**
	* @brief Removes the 1-bits from both input bitstrings (their union) 
	*		inside the population range. The sizes of both input bitstrings
	*		must be greater than the destination bitstring (*this)
	* @param lhs, rhs: input bitstrings  
	* @returns reference to the modified bitstring
	* @created: 30/7/2017  for the MWCP
	* @last_update: 02/02/2025
	**/
inline	BitBoardN& erase_bit		(const BitBoardN& lhs, const BitBoardN& rhs);
						
/////////////////////
//BitBlock operations 

	/**
	* @brief Copies the 1-bits from the bitstring bb_add in the closed range [firstBlock, lastBlock]
	*		 If LastBlock == -1, the range is [firstBlock, nBB_]
	* 
	*		 0 <= FirstBlock <= LastBLock < the number of bitblocks in the bitstring
	* 
	* @param bb_add: input bitstring to be copied
	* @param FirstBlock: the first bitblock to be modified
	* @param LastBLock: the last bitblock to be modified
	* @returns reference to the modified bitstring
	**/
	BitBoardN& set_block			(int firstBlock, int lastBlock, const BitBoardN& bb_add);
	
	/**
	* @brief Deletes the 1-bits from the bitstring bb_del in the closed range [firstBlock, lastBlock]
	*		 If lastBlock == -1, the range is the whole bitstring.
	*
	*		 0 <= firstBlock <= lastBlock < the number of bitblocks in the bitstring
	*
	* @param bb_del: input bitstring whose 1-bits are to be removed	
	* @param firstBlock: the first bitblock to be modified
	* @param lastBlock: the last bitblock to be modified
	* @returns reference to the modified bitstring
	**/
	inline	BitBoardN& erase_block	(int firstBlock, int lastBlock, const BitBoardN& bb_del);

	/**
	* @brief Removes the 1-bits from both input bitstrings (their union)
	*		in the closed range of bitblocks [firstBlock, lastBlock]
	*		If lastBlock == -1, the range is [firstBlock, nBB_]
	* 
	* @param firstBlock: the first bitblock to be modified
	* @param bb_del_lhs, bb_del_rhs : bitstrings whose 1-bits are to be removed
	* @returns reference to the modified bitstring
	* @date: 02/02/2025 during a refactorization of BITSCAN
	**/
	inline	BitBoardN& erase_block (int firstBlock, int lastBlock, const BitBoardN& bb_del_lhs, const BitBoardN& bb_del_rhs);

////////////////////////
// operators
	
	/**
	* @brief Bitwise AND operator with bbn
	* @details For set intersection
	**/
	BitBoardN& operator &=			(const BitBoardN& bbn);													
	
	/**
	* @brief Bitwise OR operator with bbn
	* @details For set union
	**/
	BitBoardN& operator |=			(const BitBoardN& bbn);
	
	/**
	* @brief Bitwise XOR operator with bbn
	* @details For symmetric_difference
	**/
	BitBoardN& operator ^=			(const BitBoardN& bbn);

	
	friend bool operator ==			(const BitBoardN& lhs, const BitBoardN& rhs);
	friend bool operator !=			(const BitBoardN& lhs, const BitBoardN& rhs);


////////////////////////
// Basic operations

	/**
	* @brief flips 1-bits to 0 and 0-bits to 1
	**/
	BitBoardN& flip					();

	/**
	* @brief flips 1-bits to 0 and 0-bits to 1 in the 
	*		 closed block range [firstBlock, lastBlock]
	**/
	BitBoardN& flip_block			(int firstBlock, int lastBlock);

	/**
	* @brief AND between rhs and caller bitstring in the closed range of bitblocks [firstBlock, lastBlock]
	*		 If lastBlock == -1 the range is [firstBlock, nBB_]
	*
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_). 
	* @param rhs: bitstring 
	* @returns reference to the modified bitstring
	* @date: 04/02/2025 during a refactorization of BITSCAN
	**/
	BitBoardN&  AND_EQUAL_block		(int firstBlock, int lastBlock, const BitBoardN& rhs );								
	
	/**
	* @brief OR between rhs and caller bitstring in the closed range of bitblocks [firstBlock, lastBlock]
	*		 If lastBlock == -1 the range is [firstBlock, nBB_]
	*
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_)
	* @param rhs: bitstring
	* @returns reference to the modified bitstring
	**/
	BitBoardN&  OR_EQUAL_block			(int firstBlock, int lastBlock, const BitBoardN& rhs );								
		
	/**
	* @brief Determines the lowest bit (least-significant) in common between rhs and this bitstring
	* @param rhs: input bitstring
	* @returns the first bit in common, -1 if they are disjoint
	* @created 7/17
	* @last_update 02/02/2025
	**/
	inline int find_first_common_bit	(const BitBoardN& rhs)						const;	
		
	/**
	* @brief Determines if the bitstring has a single 1-bit in the closed range [firstBit, lastBit]	
	*		 in singleton_bit
	* @param firstBit, lastBit: closed range of bits (0<=firstBit<=lastBit)
	* @param bit: contains the singleton bit if it exists or -1
	* @returns  0 if range is empty, 1 if singleton, -1 if more than one bit exists in the range
	**/
	inline  int  find_singleton			(int firstBit, int lastBit, int& bit)		const;

	/**
	* @brief Determines the single 1-bit common to both this and rhs bitstring
	* @param rhs: input bitstring
	* @param bit:  1-bit index or -1 if not single disjoint
	* @returns 0 if disjoint, 1 if intersection is a single bit, -1 otherwise (more than 1-bit in common)
	**/
	inline int	find_common_singleton	(const BitBoardN& rhs, int& bit)			const;
		
	/**
	* @brief  Determines the single 1-bit common to both this and rhs bitstring in the 
	*		  closed range [firstBlock, lastBlock].
	*		  If lastBlock == -1 the range is [firstBlock, nBB_]
	* 
	* @param bit:  1-bit index or -1 if not single disjoint
	* @returns 0 if disjoint, 1 if intersection is a single bit, -1 otherwise (more than 1-bit in common)
	* @created 14/8/16
	* @last_update 04/02/2025
	**/
	inline	int	find_common_singleton_block		(int first_block, int last_block,
													const BitBoardN& rhs, int& bit)		const;
	
	/**
	* @brief Determines the single 1-bit in this bitstring of to the set difference
	*		 bitset this \ rhs.
	* @param bit:  1-bit index or -1 if the set difference is not a single bit
	* @returns 0 if the set difference is empty, 1 if a singleton and -1 otherwise (more than 1-bit)
	* @created 27/7/16
	* @last_update 04/02/2025
	**/
	inline	int	find_diff_singleton				(const BitBoardN& rhs, int& bit)		const;					
	
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
	inline  int find_diff_pair					(const BitBoardN& rhs, 
														int& bit1, int& bit2	)		const;

/////////////////////////////
//Boolean functions - CHECK UNIT TESTS

	inline bool is_bit					(int bit)										const;

	/**
	* @brief TRUE if the bitstring has all 0-bits
	**/
	inline virtual bool is_empty		()												const;	

	/**
	* @brief Determines if the bitstring has all 0-bits in the closed range [firstBlock, lastBlock]
	*		  If lastBlock == -1 the range is [firstBlock, nBB_]
	*
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_).
	* @returns TRUE if the bitstring has all 0-bits in the given range
	* @details optimized for non-sparse bitsets - early exit
	**/	
	inline virtual bool is_empty_block	(int firstBlock, int lastBlock)					const;
	
	/**
	* @brief TRUE if caller bitstring has a single 1-bit
	* @returns 1 if singleton, 0 if empty, -1 if more than one bit
	* @created (15/3/17) 
	* @details optimized for non-sparse bitsets - early exit
	**/
	inline int is_singleton				()												const;	
			
	/**
	* @brief Determines if the caller bitstring has a single 1-bit in the 
	*		 closed bit-range [firstBit, lastBit]
	*
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_).
	* @returns 1 if singleton, 0 if empty, -1 if more than one bit in the specifed range
	* @details optimized for non-sparse bitsets - early exit
	**/
	inline int  is_singleton			(int firstBit, int lastBit)						const;					

	/**
	* @brief TRUE if caller bitstring has a single 1-bit in the closed range [firstBlock, lastBlock]
	*		  If lastBlock == -1 the range is [firstBlock, nBB_]
	* 
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<nBB_). 
	* @returns 1 if singleton, 0 if empty, -1 if more than one bit.
	* @details optimized for non-sparse bitsets - early exit
	**/
	inline int is_singleton_block		(int firstBlock, int lastBlock)					const;

	/**
	* @brief TRUE if this bitstring has no bits in common with rhs
	**/
	inline bool is_disjoint				(const BitBoardN& rhs)							const;
		
	/**
	* @brief TRUE if this bitstring has no bits in common with rhs
	*		 in the closed range [firstBlock, lastBlock].
	*
	*		If lastBlock == -1, the range is [firstBlock, nBB_]
	**/
	inline bool is_disjoint_block		(int firstBlock, int lastBlock,
												const BitBoardN& rhs		)			const;
	/**
	* @brief TRUE if this bitstring has no bits in common with neither lhs NOR rhs bitstrings
	* @details Currently not available for sparse bitsets
	**/
	inline bool is_disjoint				(const BitBoardN& lhs, const  BitBoardN& rhs)	const;
		
/////////////////////
// I/O 
	/**
	* @brief streams bb and its popcount to the output stream o
	* @details format example [...000111 (3)]
	* @param o: output stream
	* @param show_pc: if true, shows popcount
	* @returns output stream
	**/
	std::ostream& print				( std::ostream& o = std::cout,
									  bool show_pc = true, bool endl = true	)		const override;
	/**
	* @brief converts bb and its popcount to a readable string
	* @details format example [...000111 (3)]
	* @returns string
	**/
	std::string to_string			();
	
///////////////////////
//Conversions / Casts

	/**
	* @brief Converts the bitstring to a std::vector of non-negative integers.
	*		 The size of the vector is the number of bits in the bitstring. 
	* @param lv: output vector 
	**/
	void to_vector					(vint& lv)							const;

	/**
	* @brief Casts the bitstring to a vector of non-negative integers
	* @details calls to_vector
	**/
	operator						vint()								const;
		
	/**
	* @brief Converts the bitstring to a stack object s 
	* @details no allocation! stack MUST BE of adequate size to hold all 1-bits
	* @param s: output stack 
	**/
	void to_stack					(com::stack_t<int>& s)				const;						
	
	/**
	* @brief Converts the bitstring to a C array
	* @param lv: output C-array as pointer
	* @param size: output size of the array
	* @param rev: if true, the array is filled in reverse order
	* @returns pointer to the array, size of the array
	**/
virtual	int* to_C_array				(int* lv, std::size_t& size, bool rev = false);

////////////////////////
//data members

protected:
	int nBB_;					//number of bitblocks 
	std::vector<BITBOARD> vBB_;	//vector of bitblocks		
	

	//deprecated
	//BITBOARD* vBB_;			//C-array of bitblocks - not using std::vector because of memory allignment, TODO-CHANGE TO VECTOR (04/02/2025)

}; //end BitBoardN class

////////////////////////////////////////////////////////////////////////////////////////
// 
// Inline function implementations - must be in header file

inline 
int BitBoardN::find_first_common_bit	(const BitBoardN& rhs) const {

	BITBOARD bb = 0;
	for(auto i = 0; i < nBB_; ++i){
		if( (bb = (vBB_[i] & rhs.vBB_[i])) ){
			return bblock::lsb64_intrinsic(bb) + WMUL(i);
		}
	}
	return EMPTY_ELEM;
}

inline int BitBoardN::msbn64() const{
///////////////////////
// Look up table implementation (best found so far)

	register union u {
		U16 c[4];
		BITBOARD b;
	} val;
	
	//reverse loop (most significant bit block early exit)
	for(auto i = nBB_ - 1; i >= 0; i--){
		val.b = vBB_[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]] + WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]] + WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]] + WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]] + WMUL(i));
		}
	}

	return EMPTY_ELEM;		//should not reach here
}

inline
int BitBoardN::next_bit(int bit) const{

	//bit = -1 is a special case of early exit
	//typically used in a loop, in the first bitscan call.
	//Determines the least significant bit in the bitsring
	if (bit == EMPTY_ELEM) {
		return lsbn64();
	}

	//compute bitlbock of the bit
	int bbl = WDIV(bit);

	//looks for the next bit in the current block
	int npos = bblock::lsb64_de_Bruijn(Tables::mask_left[bit - WMUL(bbl) /*WMOD(bit)*/] & vBB_[bbl]);
	if (npos >= 0) {
		//////////////////////////////
		return (npos + WMUL(bbl));
		////////////////////////////
	}

	//looks in remaining biblocks
	for (auto i = bbl + 1; i < nBB_; ++i) {
		if (vBB_[i]) {
			return( bblock::lsb64_de_Bruijn(vBB_[i]) + WMUL(i));
		}
	}
	
	//should not reach here
	return EMPTY_ELEM;
}

inline int BitBoardN::next_bit_if_del(int bit) const{
	
	//special case - first bitscan,
	//calls for the least significant bit in the bitstring
	if (bit == EMPTY_ELEM) {
		return lsbn64();
	}
			
	for (auto i = WDIV(bit); i < nBB_; ++i) {
		if (vBB_[i]) {
			return(bblock::lsb64_de_Bruijn(vBB_[i]) + WMUL(i));
		}
	}
		
	return EMPTY_ELEM;		//should not reach here
}

inline int BitBoardN::prev_bit(int bit) const{

	//special case - first bitscan,
	//calls for the most-significant bit in the bitstring
	if (bit == EMPTY_ELEM) {
		return msbn64();
	}
	
	//bitblock of input bit
	int bbh = WDIV(bit);
	
	//looks for the msb in the (trimmed) current block
	int npos = bblock::msb64_lup( Tables::mask_right[bit - WMUL(bbh) /* WMOD(bit) */] & vBB_[bbh]);
	if (npos != EMPTY_ELEM) {
		return ( npos + WMUL(bbh) );
	}

	//looks for the msb in the remaining blocks
	register union u {
		U16 c[4];
		BITBOARD b;
	}val;

	for(auto i = bbh - 1; i >= 0; --i){
		val.b = vBB_[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+ WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+ WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+ WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+ WMUL(i));
		}
	}

	return EMPTY_ELEM;		//should not reach here
}

inline bool BitBoardN::is_bit (int nbit/*0 based*/) const{
//////////////////////////////
// RETURNS: TRUE if the bit is 1 in the position nbit, FALSE if opposite case or ERROR

	return (vBB_[WDIV(nbit)] & Tables::mask[WMOD(nbit)]);

}
 
inline bool BitBoardN::is_empty() const
{
	for (auto i = 0; i < nBB_; ++i) {
		if (vBB_[i]) {
			return false;
		}
	}

	return true;	
}

inline bool BitBoardN::is_empty_block (int firstBlock, int lastBlock) const
{

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (lastBlock < nBB_) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? nBB_ - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		if (vBB_[i]) {
			return false;
		}
	}

	return true;	
}

inline 
bool BitBoardN::is_disjoint	(const BitBoardN& rhs) const
{
	for (auto i = 0; i < nBB_; ++i) {
		if (vBB_[i] & rhs.vBB_[i]) {
			return false;
		}
	}
	return true;
}

inline 
bool BitBoardN::is_disjoint	(const BitBoardN& lhs,  const BitBoardN& rhs)	const
{
	for (auto i = 0; i < nBB_; ++i) {
		if (vBB_[i] & lhs.vBB_[i] & rhs.vBB_[i]) {
			return false;
		}
	}
	return true;
}

inline 
bool BitBoardN::is_disjoint_block (int firstBlock, int lastBlock, const BitBoardN& rhs)	const{

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (lastBlock < nBB_) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? nBB_ - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		if (vBB_[i] & rhs.vBB_[i]) {
			return false;
		}
	}
	return true;
}

inline
BitBoardN& BitBoardN::set_bit (int high, const BitBoardN& bb_add){

	int bbh = WDIV(high);	

	for(auto i = 0; i <= bbh; ++i){
		vBB_[i] = bb_add.vBB_[i];
	}

	//trim last bit block up to, and exluding, high
	vBB_[bbh] &= bblock::MASK_1_RIGHT(high - WMUL(bbh));			//also vBB_[bbh] &= bblock::MASK_1(0, high - WMUL(bbh)); 
	
	return *this;
}

inline
int  BitBoardN::is_singleton (int firstBit, int lastBit) const{

	int nbbl = WDIV(firstBit);
	int nbbh = WDIV(lastBit);
	int pc = 0;
	
	//both ends
	if(nbbl == nbbh){
		if ( (pc = bblock::popc64( vBB_[nbbl] & bblock::MASK_1(firstBit - WMUL(nbbl), lastBit - WMUL(nbbh))) ) > 1) {
			return -1;
		}
	}
	else {

		//checks first block
		if( (pc = bblock::popc64( vBB_[nbbl] & bblock::MASK_1_LEFT(firstBit - WMUL(nbbl)  /* eq. to WMOD(nbbl) */ ))) > 1) {
			return -1;
		}

		//checks intermediate blocks
		for(auto i = nbbl + 1; i < nbbh; ++i){
			if (pc += bblock::popc64(vBB_[i]) > 1){
				return -1;
			}
		}

		//checks last block
		if ((pc += bblock::popc64(vBB_[nbbh] & bblock::MASK_1_RIGHT(lastBit - WMUL(nbbh)  /* eq. to WMOD(nbbl) */ ))) > 1) {
			return -1;
		}
	}

	//reasons on return value - 0 empty, 1 singleton (-1 early exit)
	if (pc == 0) { return 0; }
	return 1;						//MUST BE singleton
}

inline
int  BitBoardN::find_singleton (int firstBit, int lastBit, int& singleton) const{

	int nbbl = WDIV(firstBit);
	int	nbbh = WDIV(lastBit);
	int posl = firstBit - WMUL(nbbl);		//equiv. WMOD(low);
	int posh = lastBit - WMOD(nbbh);		//equiv. WMOD(high);
	int pc = 0;
	bool vertex_not_found = true;
	singleton = EMPTY_ELEM;
	
	//both ends
	if(nbbl == nbbh){
		BITBOARD bbl= vBB_[nbbl] & bblock::MASK_1(posl, posh);
		pc = bblock::popc64(bbl);
		if( (pc = bblock::popc64(bbl)) == 1){
			singleton = bblock::lsb64_intrinsic(bbl) + WMUL(nbbl);
			/////////
			return 1;
			////////
		}
	}
	else{

		//checks first block
		BITBOARD bbl = vBB_[nbbl] & bblock::MASK_1_LEFT(posl);

		if( (pc = bblock::popc64(bbl)) > 1){
			/////////
			return -1;
			/////////
		}else if(pc == 1){
			vertex_not_found = false;
			singleton = bblock::lsb64_intrinsic(bbl) + WMUL(nbbl);
		}

		//checks intermediate blocks
		for(auto i = nbbl + 1; i < nbbh; ++i){			
			if( (pc += bblock::popc64(vBB_[i])) > 1){
				/////////
				return -1;
				/////////
			}else if( vertex_not_found && (pc ==1 ) ){
				singleton = bblock::lsb64_intrinsic(vBB_[i]) + WMUL(i);
				vertex_not_found = false;
			}
		}

		//checks last block
		BITBOARD bbh = vBB_[nbbh] & bblock::MASK_1_RIGHT(posh);
			
		if ( (pc += bblock::popc64(bbh)) > 1) {
			/////////
			return -1;
			/////////
		}else if (vertex_not_found && (pc == 1)) {
			singleton = bblock::lsb64_intrinsic(bbh) + WMUL(nbbh);
		}
	}

	//reason with pc
	if (pc == 0) { return 0; }

	//must be singleton
	return 1;
}

template<bool EraseAll>
inline BitBoardN& BitBoardN::set_bit	(int bit ){
	
	if (EraseAll) {
		erase_bit();
	}

	vBB_[WDIV(bit)] |= Tables::mask[ WMOD(bit) ];
	return *this;
}	

template<bool EraseAll>
inline 
BitBoardN&  BitBoardN::set_bit (int firstBit, int lastBit){

	//general comment: low - WMUL(bbl) = WMOD(bbl) but supposed to be less expensive (CHECK 01/02/25)

	//////////////////////////////
	assert(firstBit <= lastBit && firstBit > 0);
	//////////////////////////////

	int bbl= WDIV(firstBit);
	int bbh= WDIV(lastBit);
	

	//clears all bits if required
	if (EraseAll) {
		erase_bit();
	}	

	if(bbl == bbh)
	{			
		vBB_[bbh] |= bblock::MASK_1(firstBit - WMUL(bbl), lastBit - WMUL(bbh));		
	}
	else
	{
		//set to one the intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			vBB_[i] = ONE;
		}
		
		//sets the first and last blocks
		vBB_[bbh] |= bblock::MASK_1_RIGHT	(lastBit - WMUL(bbh));
		vBB_[bbl] |= bblock::MASK_1_LEFT	(firstBit - WMUL(bbl));

	}

	return *this;
}

inline
BitBoardN& BitBoardN::set_bit (){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] = ONE;
	}
	return *this;
}

inline
BitBoardN&  BitBoardN::set_bit (const BitBoardN& bb_add){

	/////////////////////////////////
	assert(nBB_ <= bb_add.nBB_);
	/////////////////////////////////

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] |= bb_add.vBB_[i];
	}

	return *this;
}

inline
BitBoardN&  BitBoardN::set_block (int firstBlock, int lastBlock, const BitBoardN& bb_add){

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (LastBlock < nBB_) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? nBB_ - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		vBB_[i] |= bb_add.vBB_[i];
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit (){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] = ZERO;
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit(int nBit) {

	vBB_[WDIV(nBit)] &= ~Tables::mask[WMOD(nBit)]; 
	return *this;
}

inline 
BitBoardN&  BitBoardN::erase_bit (int firstBit, int lastBit){
	
	//general comment: low - WMUL(bbl) = WMOD(bbl) but supposed to be less expensive (CHECK 01/02/25)

	//////////////////////////////
	assert(firstBit <= lastBit && firstBit > 0);
	//////////////////////////////

	int bbl = WDIV(firstBit);
	int bbh = WDIV(lastBit);


	if (bbl == bbh)
	{
		vBB_[bbh] &= bblock::MASK_0(firstBit - WMUL(bbl), lastBit - WMUL(bbh));		
	}
	else
	{
		//set to one the intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			vBB_[i] = ZERO;
		}
				
		vBB_[bbh] &= bblock::MASK_0_RIGHT	(lastBit - WMUL(bbh));					//Tables::mask_left
		vBB_[bbl] &= bblock::MASK_0_LEFT	(firstBit - WMUL(bbl));					//Tables::mask_right	
	}

	return *this;
}

inline int BitBoardN::lsbn64() const{
/////////////////
// different implementations of lsbn depending on configuration

#ifdef DE_BRUIJN
	for(auto i = 0; i < nBB_; ++i){
		if(vBB_[i])
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((vBB_[i] & -vBB_[i]) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(i));	
#else
			return(Tables::indexDeBruijn64_SEP[ ( (vBB_[i]^ (vBB_[i]-1) ) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >>
												bblock::DEBRUIJN_MN_64_SHIFT	]	+	WMUL(i)							);
#endif
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<nBB_; i++){
		val.b=vBB_[i];
		if(val.b){
			if(val.c[0]) return (Tables::lsba[0][val.c[0]]+WMUL(i));
			if(val.c[1]) return (Tables::lsba[1][val.c[1]]+WMUL(i));
			if(val.c[2]) return (Tables::lsba[2][val.c[2]]+WMUL(i));
			if(val.c[3]) return (Tables::lsba[3][val.c[3]]+WMUL(i));
		}
	}

#endif

	return EMPTY_ELEM;	
}

inline
int BitBoardN::is_singleton() const {

	int pc = 0;
	for(auto i = 0; i < nBB_; ++i){
		if ((pc += bblock::popc64(vBB_[i])) > 1) {
			return -1;
		}
	}

	//reasons with pc: 1-singleton, 0-empty
	if (pc == 1) {	return 1;}

	//must be empty bitset
	return 0;		
}

inline 
int BitBoardN::is_singleton_block(int firstBlock, int lastBlock) const
{
	///////////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < nBB_));
	/////////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? nBB_ - 1 : lastBlock);

	int pc = 0;
	for (auto i = firstBlock; i < last_block; ++i) {
		if ((pc += bblock::popc64(vBB_[i])) > 1) {
			return -1;
		}
	}

	//reasons with pc: 1- singleton, 0-empty
	if (pc == 1) { return 1; }

	//must be empty bitset
	return 0;
}

int BitBoardN::popcn64() const{

	int pc = 0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	for(auto i = 0; i < nBB_; ++i){

		//loads union
		val.b = vBB_[i]; 

		//counts population
		pc += Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

	return pc;
}

inline int BitBoardN::popcn64(int firstBit, int lastBit) const
{

	/////////////////////////////////////////////
	assert( firstBit > 0 && firstBit <= lastBit);
	/////////////////////////////////////////////
		
	int pc = 0;
	int bbl = WDIV(firstBit);
	int bbh = WDIV(lastBit);
	

	if (bbl == bbh)
	{
		//same block
		pc = bblock::popc64(vBB_[bbl] & bblock::MASK_1(firstBit - WMUL(bbl), lastBit - WMUL(bbh)));		
					
	}
	else
	{
		//count the population of the intermediate blocks
		for (auto i = bbl + 1; i < bbh; ++i) {
			pc += bblock::popc64( vBB_[i]);
		}

		//count the population of the first and last blocks
		pc += bblock::popc64(vBB_[bbh] & bblock::MASK_1_RIGHT(lastBit - WMUL(bbh)));
		pc += bblock::popc64(vBB_[bbl] & bblock::MASK_1_LEFT(firstBit - WMUL(bbl)));
	
	}
		
	return pc;
}


inline
int BitBoardN::find_common_singleton (const BitBoardN& rhs, int& bit) const{

	int pc = 0;
	bool is_first_vertex = true;
	bit = EMPTY_ELEM;
	
	//main loop
	for(auto i = 0; i < nBB_; ++i){
		pc += bblock::popc64 (vBB_[i] & rhs.vBB_[i]);
		if(pc > 1){
			bit = EMPTY_ELEM;
			return -1;
		}else if(is_first_vertex && pc == 1 ) { //stores bit the first time pc == 1 
						
			bit = bblock::lsb64_intrinsic ( vBB_[i] & rhs.vBB_[i] )+ WMUL(i);
			is_first_vertex = false;
		}
	}
	
	//disjoint - pc = 0
	return pc;		
}


inline	
int	BitBoardN::find_common_singleton_block (int firstBlock, int lastBlock, const BitBoardN& rhs, int& bit) const{

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (lastBlock < nBB_) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block;
	(lastBlock == -1) ? last_block = nBB_ - 1 : last_block = lastBlock;

	int pc = 0;
	bit = EMPTY_ELEM;
	bool is_first_vertex = true;
	
	for(auto i= firstBlock; i <= last_block; ++i){
		pc += bblock::popc64(vBB_[i] & rhs.vBB_[i]);
		if(pc > 1){
			bit = EMPTY_ELEM;
			return -1;
		}else if(is_first_vertex && pc == 1 ){	//stores bit the first time pc == 1 
			
			bit = bblock::lsb64_intrinsic(vBB_[i] & rhs.vBB_[i] )+ WMUL(i);
			is_first_vertex = false;
		}
	}

	//pc = 0 (disjoint) /pc = 1 (intersection between *this and rhs a single bit)	
	return pc;		
}

inline 
int BitBoardN::find_diff_singleton(const BitBoardN& rhs, int& bit) const{
	
	int pc = 0;
	bit = EMPTY_ELEM;
	bool is_first_vertex = true;
	
	for(auto i = 0; i < nBB_; ++i){

		//popcount of set difference - removes bits of rhs from *this
		pc += bblock::popc64(vBB_[i] &~ rhs.vBB_[i]);

		if(pc > 1){
			bit = EMPTY_ELEM;
			return -1;
		}else if( pc == 1 && is_first_vertex){ //stores bit the first time pc == 1 
			
			bit = bblock::lsb64_intrinsic(vBB_[i] &~ rhs.vBB_[i] ) + WMUL(i);
			is_first_vertex = false;
		}
	}
	
	//pc = 0 (*this subset of rhs, empty setdiff) / pc = 1 (singleton setdiff)
	return pc;		
}

inline
int BitBoardN::find_diff_pair(const BitBoardN& rhs, int& bit1, int& bit2) const {

	int pc = 0;
	bool is_first_bit = true;
	bool is_second_bit = true;
	bit1 = EMPTY_ELEM;
	bit2 = EMPTY_ELEM;

	//main loop
	for (auto i = 0; i < nBB_; ++i) {

		//popcount of set difference - removes bits of rhs from *this
		BITBOARD bb = vBB_[i] & ~rhs.vBB_[i];
		pc += bblock::popc64(bb);

		if (pc > 2) {

			bit1 = EMPTY_ELEM;
			bit2 = EMPTY_ELEM;
			return EMPTY_ELEM;

		}
		else if (pc == 1 && is_first_bit) {  //stores bit the first time pc == 1 

			bit1 = bblock::lsb64_intrinsic(bb) + WMUL(i);
			is_first_bit = false;

		}
		else if (pc == 2 && is_second_bit) {  //stores the two bits the first time pc == 2 

			if (is_first_bit) {

				//determines the two bits in the same block
				bit1 = bblock::lsb64_intrinsic(bb) + WMUL(i);
				bit2 = bblock::msb64_intrinsic(bb) + WMUL(i);

			}
			else {

				//determines the second bit directly since the
				//two bits of the set difference are in different bitblocks
				bit2 = bblock::lsb64_intrinsic(bb) + WMUL(i);
			}

			is_second_bit = false;
		}
	}

	//pc=0, 1, 2 (size of the set difference)
	return pc;
}

inline
bool operator==	(const BitBoardN& lhs, const BitBoardN& rhs){
	
	for (auto i = 0; i < lhs.nBB_; ++i) {
		if (lhs.vBB_[i] != rhs.vBB_[i]) {
			return false;
		}
	}

	return true;
}

inline
bool operator!=	(const BitBoardN& lhs, const BitBoardN& rhs){
	 return ! operator==(lhs, rhs);
}

inline
BitBoardN& BitBoardN::erase_bit (const BitBoardN& bbn){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] &= ~bbn.vBB_[i];
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit (const BitBoardN& bb_lhs, const BitBoardN& bb_rhs ){

	for (auto i = 0; i < nBB_; i++) {
		vBB_[i] &= ~(bb_lhs.vBB_[i] | bb_rhs.vBB_[i]);
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_block (int FirstBlock, int LastBlock, const BitBoardN& bb_lhs, const BitBoardN& bb_rhs ){

	///////////////////////////////////////////////////////////////////////////////
	assert((FirstBlock >= 0) && (LastBlock < nBB_) && (FirstBlock <= LastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block;
	(LastBlock == -1) ? last_block = nBB_ - 1 : last_block = LastBlock;

	for (auto i = FirstBlock; i <= LastBlock; ++i) {
		vBB_[i] &= ~(bb_lhs.vBB_[i] | bb_rhs.vBB_[i]);
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_block(int FirstBlock, int LastBlock, const BitBoardN& bb_del){

	///////////////////////////////////////////////////////////////////////////////
	assert((FirstBlock >= 0) && (LastBlock < nBB_) && (FirstBlock <= LastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((LastBlock == -1) ? nBB_ - 1 : LastBlock);

	for (auto i = FirstBlock; i <= last_block; ++i) {
		vBB_[i] &= ~bb_del.vBB_[i];
	}

	return *this;
}

inline
int FIRST_SHARED (const BitBoardN& lhs, const BitBoardN& rhs){
/////////////////////
// RETURNS first bit common to lhs and rhs, EMPTY ELEM if sets are disjoint
	
	for(int i=0; i<lhs.nBB_; i++){
		BITBOARD bb=lhs.vBB_[i] & rhs.vBB_[i];
		if(bb){
			return bblock::lsb64_intrinsic(bb)+WMUL(i);
		}
	}
	
	return EMPTY_ELEM;		//disjoint
}

inline
int FIRST_SHARED(int first_block, const BitBoardN& lhs, const BitBoardN& rhs) {
	/////////////////////
	// RETURNS first bit common to lhs and rhs, EMPTY ELEM if sets are disjoint

	for (int i = first_block; i < lhs.nBB_; i++) {
		BITBOARD bb = lhs.vBB_[i] & rhs.vBB_[i];
		if (bb) {
			return bblock::lsb64_intrinsic(bb) + WMUL(i);
		}
	}

	return EMPTY_ELEM;		//disjoint
}

template<bool EraseAll>
inline
BitBoardN& BitBoardN::set_bit(const vint& lv) {

	//cleans the bitstring if required
	if (EraseAll) {
		erase_bit();
	}

	//copies elements up to the maximum capacity of the bitstring
	auto POPSIZE = WMUL(nBB_);
	for (auto i = 0; i < lv.size(); ++i) {

		/////////////////////
		assert(lv[i] >= 0);
		////////////////////

		if (lv[i] < POPSIZE /* 1-based*/) {
			set_bit(lv[i]);
		}
	}

	return *this;

}

template<bool Erase = false>
inline
BitBoardN& AND(int lastBit, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res) {

	//determine bitblock
	int nbb = WDIV(lastBit);

	for (auto i = 0; i <= nbb; ++i) {
		res.vBB_[i] = rhs.vBB_[i] & lhs.vBB_[i];
	}

	//trim last part of the bitblock - including lastBit
	res.vBB_[nbb] &= Tables::mask_right[lastBit - WMUL(nbb) /* WMOD(lastBit)*/];

	//delete the rest of bitstring if the operation is not lazy
	if (Erase) {
		for (int i = nbb + 1; i < lhs.nBB_; ++i) {
			res.vBB_[i] = ZERO;
		}
	}

	return res;
}

template<bool Erase = false>
BitBoardN& AND(int firstBit, int lastBit, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res)
{

	//////////////////////////////
	assert(firstBit <= lastBit && firstBit > 0);
	//////////////////////////////

	int bbl = WDIV(firstBit);
	int bbh = WDIV(lastBit);
	int bith = lastBit - WMUL(bbh);	
	int bitl = firstBit - WMUL(bbl);
	
	//////////////////////////////////
	//Blocks within the range
	 
	//special case - single block
	if (bbl == bbh)
	{
		if (Erase) {
			//overwrites
			res.vBB_[bbh] = lhs.vBB_[bbh] & rhs.vBB_[bbh] & bblock::MASK_1(bitl, bith);
		}
		else {
			//overwrites partially in the closed range
			bblock::copy(bitl, bith, lhs.vBB_[bbh] & rhs.vBB_[bbh], res.vBB_[bbh]);		
		}
	}
	else
	{
		//AND intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			//overwrites
			res.vBB_[i] = lhs.vBB_[i] & rhs.vBB_[i];
		}

		//updates the last block bbh in the range
		if (Erase) {
			//overwrites
			res.vBB_[bbh] = lhs.vBB_[bbh] & rhs.vBB_[bbh] & bblock::MASK_1_RIGHT(bith);
		}
		else {
			//overwrites bbh partially inside the range (including bith)
			bblock::copy_right(bith, lhs.vBB_[bbh] & rhs.vBB_[bbh], res.vBB_[bbh]);
		}

		//updates the first block bbl in the range
		if (Erase) {
			//overwrites
			res.vBB_[bbl] = lhs.vBB_[bbl] & rhs.vBB_[bbl] & bblock::MASK_1_LEFT(bitl);
		}
		else {
			//overwrites bbl partially inside the range (including bitl)
			bblock::copy_left(bitl, lhs.vBB_[bbl] & rhs.vBB_[bbl], res.vBB_[bbl]);
		}
	}

	//////////////////////////////////
	//Blocks outside the range

	//set to 0 all bits outside the bitblock range if required
	if (Erase) {
		for (int i = bbh + 1; i < res.nBB_; ++i) {
			res.vBB_[i] = ZERO;	
		}

		for (int i = 0; i < bbl; ++i) {
			res.vBB_[i] = ZERO;
		}
	}

	return res;
}

template<bool Erase = false>
BitBoardN& AND_block(int firstBlock, int lastBlock, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res) {

	//////////////////////////////////////////////////////////////////
	assert(	(firstBlock >= 0) && (LastBlock < lhs.nBB_) &&
			(firstBlock <= lastBlock) && (rhs.nBB_ == lhs.nBB_)		);
	//////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? lhs.nBB_ - 1 : lastBlock);

	//AND mask in the range
	for (auto i = firstBlock; i <= last_block; ++i) {
		res.vBB_[i] = rhs.vBB_[i] & lhs.vBB_[i];
	}
	
	//set bits to 0 outside the range if required
	if (Erase) {
		for (int i = lastBlock + 1; i < lhs.nBB_; ++i) {
			res.vBB_[i] = ZERO;
		}
		for (int i = 0; i < firstBlock; ++i) {
			res.vBB_[i] = ZERO;
		}
	}

	return res;
}



#endif	