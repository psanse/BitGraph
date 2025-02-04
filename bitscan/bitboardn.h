/**  
 * @file bitboardn.h file 
 * @brief header file of the BitBoardN class from the BITSCAN library.
 *		  Manages bitstrings of any size as an array of bitblocks (64-bit numbers)
 * @author pss
 * @created 2014
 * @last_update 01/02/2025
 * 
 * TODO - refactoring and testing (31/01/2025)
 *
 * Permission to use, modify and distribute this software is
 * granted provided that this copyright notice appears in all 
 * copies, in source code or in binaries. For precise terms 
 * see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any 
 * kind, express or implied, and with no claim as to its
 * suitability for any purpose.
 *
 **/

#ifndef __BITBOARDN__H_
#define __BITBOARDN__H_

#include "bbobject.h"
#include "bitboard.h"	
#include <vector>	
#include "utils/common.h"			//stack

#include <cassert>					//uncomment NDEBUG in config.h to enable run-time assertions

//alias
using vint = std::vector<int>;

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
// independent operators / services (no allocation or copies)

	friend bool operator ==			(const BitBoardN& lhs, const BitBoardN& rhs);
	friend bool operator !=			(const BitBoardN& lhs, const BitBoardN& rhs);

	friend BitBoardN&  AND			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);							
	friend BitBoardN&  AND			(int first_block, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  AND			(int first_block, int last_block, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  AND			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res, int last_vertex, bool lazy);			//up to and excluding last_vertex
	friend int*	       AND			(const BitBoardN& lhs, const BitBoardN& rhs, int last_vertex, int res[], int& size);				//returns the operation as a set of vertices	

	friend BitBoardN&  OR			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  OR			(int from, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);	
	friend BitBoardN&  OR			(int v, bool from /* to */, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res);

	friend BitBoardN&  ERASE		(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);										//removes rhs from lhs
	friend int FIRST_SHARED			(const BitBoardN& lhs, const BitBoardN& rhs);
	friend int FIRST_SHARED			(int first_block, const BitBoardN& lhs, const BitBoardN& rhs);	//first elem in common

////////////
//construction / destruction 

	 BitBoardN						(): m_nBB(EMPTY_ELEM), m_aBB(NULL)		{};	
	 BitBoardN						(const vint& v, int popsize);
explicit  BitBoardN					(int popsize /*1 based*/, bool reset=true);	
explicit  BitBoardN					(const vint& v);
	
	 	 
	 //TODO - move and copy semantics...

	 BitBoardN						(const BitBoardN& bbN);
	 BitBoardN& operator =			(const BitBoardN&);

virtual	~BitBoardN					();

////////////
//memory allocation 
	void init						(int popsize, bool reset = true);										
	void init						(int popsize, const vint& );
	
	//TODO- add reset(...) with the same semantics as init (04/02/2025)

/////////////////////
//setters and getters (will not allocate memory)
	
	int number_of_bitblocks()					const					{ return m_nBB; }
	
	/**
	* @brief alternative syntax for number_of_bitblocks
	**/
	int capacity()								const					{ return m_nBB; }

	BITBOARD* bitstring				()									{ return m_aBB; }
	const BITBOARD* bitstring		()			const					{ return m_aBB; }
	
const BITBOARD bitblock				(int block) const					{ return m_aBB[block]; }
	BITBOARD& bitblock				(int block)							{ return m_aBB[block]; }

//////////////////////////////
// Bitscanning

	//find least/most signinficant bit
inline virtual int msbn64			()	const;		//lookup
inline virtual int lsbn64			()	const; 		//de Bruijn	/ lookup								

	//for looping (does not use state info)	
inline int next_bit					(int nBit)	const;					//de Bruijn
inline int next_bit_if_del			(int nBit)	const;					//de Bruijn
inline int previous_bit				(int nbit)	const;					//lookup 
	
/////////////////
// Popcount

	/**
	* @brief returns the number of 1-bits in the bitstring
	**/
std::size_t	size					()						const		{(std::size_t)popcn64();}
virtual	inline int popcn64			()						const;		//lookup 
virtual	inline int popcn64			(int nBit/* 0 based*/)	const;

/////////////////////
//Setting / Erasing bits 
			
	/**
	* @brief sets bit in position idx to 1 in the bitstring
	* @param  idx: index of the bit to set (nBit >= 0)
	**/
		
	template<bool EraseAll = false>
	BitBoardN&  set_bit			(int nBit);

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
	*		 If LastBlock == -1, the range is [firstBlock, m_nBB]
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
	*		If lastBlock == -1, the range is [firstBlock, m_nBB]
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
	*		 If lastBlock == -1 the range is [firstBlock, m_nBB]
	*
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<m_nBB). 
	* @param rhs: bitstring 
	* @returns reference to the modified bitstring
	* @date: 04/02/2025 during a refactorization of BITSCAN
	**/
	BitBoardN&  AND_EQUAL_block		(int firstBlock, int lastBlock, const BitBoardN& rhs );								
	
	/**
	* @brief OR between rhs and caller bitstring in the closed range of bitblocks [firstBlock, lastBlock]
	*		 If lastBlock == -1 the range is [firstBlock, m_nBB]
	*
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<m_nBB)
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
	*		  If lastBlock == -1 the range is [firstBlock, m_nBB]
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
	*		  If lastBlock == -1 the range is [firstBlock, m_nBB]
	*
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<m_nBB).
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
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<m_nBB).
	* @returns 1 if singleton, 0 if empty, -1 if more than one bit in the specifed range
	* @details optimized for non-sparse bitsets - early exit
	**/
	inline int  is_singleton			(int firstBit, int lastBit)						const;					

	/**
	* @brief TRUE if caller bitstring has a single 1-bit in the closed range [firstBlock, lastBlock]
	*		  If lastBlock == -1 the range is [firstBlock, m_nBB]
	* 
	* @param firstBlock, lastBlock: range of blocks (0<=firstBlock<=lastBlock<m_nBB). 
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
	*		If lastBlock == -1, the range is [firstBlock, m_nBB]
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
	BITBOARD* m_aBB;				//C-array of bitblocks - not using std::vector because of memory allignment, TODO-CHANGE TO VECTOR (04/02/2025)
	int m_nBB;						//number of bitblocks 

}; //end BitBoardN class

////////////////////////////////////////////////////////////////////////////////////////
// 
// Inline function implementations - must be in header file

inline 
int BitBoardN::find_first_common_bit	(const BitBoardN& rhs) const {

	BITBOARD bb = 0;
	for(auto i = 0; i < m_nBB; ++i){
		if( (bb = (m_aBB[i] & rhs.m_aBB[i])) ){
			return bblock::lsb64_intrinsic(bb) + WMUL(i);
		}
	}
	return -1;
}

inline int BitBoardN::msbn64() const{
///////////////////////
// Look up table implementation (best found so far)

	register union u {
		U16 c[4];
		BITBOARD b;
	} val;
	
	//reverse loop (most significant bit block early exit)
	for(auto i = m_nBB - 1; i >= 0; i--){
		val.b = m_aBB[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]] + WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]] + WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]] + WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]] + WMUL(i));
		}
	}

	return EMPTY_ELEM;		//should not reach here
}


inline int BitBoardN::next_bit(int nBit/* 0 based*/) const{
////////////////////////////
//
// Returns next bit from nBit in the bitstring (to be used in a bitscan loop)
//
// NOTES: if nBit is FIRST_BITSCAN returns lsb

	int index, npos;

	if(nBit==EMPTY_ELEM)
				return lsbn64();
	else{
		index=WDIV(nBit);

		//looks in same BB as nBit
		npos=bblock::lsb64_de_Bruijn(Tables::mask_left[WMOD(nBit) /*-WORD_SIZE*index*/] & m_aBB[index] );
		if(npos>=0)
			return (WMUL(index)/*WORD_SIZE*index*/ + npos);

		//looks in remaining BBs
		for(int i=index+1; i<m_nBB; i++){
			if(m_aBB[i])
				return(bblock::lsb64_de_Bruijn(m_aBB[i])+WMUL(i)/*WORD_SIZE*i*/ );
		}
	}
	
return -1;	
}

inline int BitBoardN::next_bit_if_del(int nBit/* 0 based*/) const{
////////////////////////////
// Returns next bit assuming, when used in a loop, that the last bit
// scanned is deleted prior to the call
		
	if(nBit==EMPTY_ELEM)
		return lsbn64();
	else{
		for(int i = WDIV(nBit); i<m_nBB; i++){
			if(m_aBB[i]){
				return(bblock::lsb64_de_Bruijn(m_aBB[i])+WMUL(i) );
			}
		}
	}
	
return -1;	
}

inline int BitBoardN::previous_bit(int nBit/* 0 bsed*/) const{
////////////////////////////
// Gets the previous bit to nBit. If nBits is -10 is a MSB
//
// COMMENTS
// 1-There is no control of EMPTY_ELEM

	if(nBit==EMPTY_ELEM) 
			return msbn64();
	

	int index=WDIV(nBit);
	int npos;

	union u {
		U16 c[4];
		BITBOARD b;
	};
	u val;
		
	
	//BitBoard pos
	npos=bblock::msb64_lup( Tables::mask_right[WMOD(nBit) /*nBit-WMUL(index)*/] & m_aBB[index] );
	if(npos!=EMPTY_ELEM)
			return (WMUL(index) + npos);

	for(int i=index-1; i>=0; i--){
		val.b=m_aBB[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+ WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+ WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+ WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+ WMUL(i));
		}
	}

return -1;		//should not reach here
}

inline bool BitBoardN::is_bit (int nbit/*0 based*/) const{
//////////////////////////////
// RETURNS: TRUE if the bit is 1 in the position nbit, FALSE if opposite case or ERROR

	return (m_aBB[WDIV(nbit)] & Tables::mask[WMOD(nbit)]);

}
 
inline bool BitBoardN::is_empty() const
{
	for (auto i = 0; i < m_nBB; ++i) {
		if (m_aBB[i]) {
			return false;
		}
	}

	return true;	
}

inline bool BitBoardN::is_empty_block (int firstBlock, int lastBlock) const
{

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (lastBlock < m_nBB) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? m_nBB - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		if (m_aBB[i]) {
			return false;
		}
	}

	return true;	
}

inline 
bool BitBoardN::is_disjoint	(const BitBoardN& rhs) const
{
	for (auto i = 0; i < m_nBB; ++i) {
		if (m_aBB[i] & rhs.m_aBB[i]) {
			return false;
		}
	}
	return true;
}

inline 
bool BitBoardN::is_disjoint	(const BitBoardN& lhs,  const BitBoardN& rhs)	const
{
	for (auto i = 0; i < m_nBB; ++i) {
		if (m_aBB[i] & lhs.m_aBB[i] & rhs.m_aBB[i]) {
			return false;
		}
	}
	return true;
}

inline 
bool BitBoardN::is_disjoint_block (int firstBlock, int lastBlock, const BitBoardN& rhs)	const{

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (lastBlock < m_nBB) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? m_nBB - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		if (m_aBB[i] & rhs.m_aBB[i]) {
			return false;
		}
	}
	return true;
}

inline
BitBoardN& BitBoardN::set_bit (int high, const BitBoardN& bb_add){

	int bbh = WDIV(high);	

	for(auto i = 0; i <= bbh; ++i){
		m_aBB[i] = bb_add.m_aBB[i];
	}

	//trim last bit block up to, and exluding, high
	m_aBB[bbh] &= bblock::MASK_1_RIGHT(high - WMUL(bbh));			//also m_aBB[bbh] &= bblock::MASK_1(0, high - WMUL(bbh)); 
	
	return *this;
}

inline
int  BitBoardN::is_singleton (int firstBit, int lastBit) const{

	int nbbl = WDIV(firstBit);
	int nbbh = WDIV(lastBit);
	int pc = 0;
	
	//both ends
	if(nbbl == nbbh){
		if ( (pc = bblock::popc64( m_aBB[nbbl] & bblock::MASK_1(firstBit - WMUL(nbbl), lastBit - WMUL(nbbh))) ) > 1) {
			return -1;
		}
	}
	else {

		//checks first block
		if( (pc = bblock::popc64( m_aBB[nbbl] & bblock::MASK_1_LEFT(firstBit - WMUL(nbbl)  /* eq. to WMOD(nbbl) */ ))) > 1) {
			return -1;
		}

		//checks intermediate blocks
		for(auto i = nbbl + 1; i < nbbh; ++i){
			if (pc += bblock::popc64(m_aBB[i]) > 1){
				return -1;
			}
		}

		//checks last block
		if ((pc += bblock::popc64(m_aBB[nbbh] & bblock::MASK_1_RIGHT(lastBit - WMUL(nbbh)  /* eq. to WMOD(nbbl) */ ))) > 1) {
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
		BITBOARD bbl= m_aBB[nbbl] & bblock::MASK_1(posl, posh);
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
		BITBOARD bbl = m_aBB[nbbl] & bblock::MASK_1_LEFT(posl);

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
			if( (pc += bblock::popc64(m_aBB[i])) > 1){
				/////////
				return -1;
				/////////
			}else if( vertex_not_found && (pc ==1 ) ){
				singleton = bblock::lsb64_intrinsic(m_aBB[i]) + WMUL(i);
				vertex_not_found = false;
			}
		}

		//checks last block
		BITBOARD bbh = m_aBB[nbbh] & bblock::MASK_1_RIGHT(posh);
			
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
inline BitBoardN& BitBoardN::set_bit	(int nbit ){
	
	if (EraseAll) {
		erase_bit();
	}

	m_aBB[WDIV(nbit)] |= Tables::mask[WMOD(nbit)];
	return *this;
}	

template<bool EraseAll>
inline 
BitBoardN&  BitBoardN::set_bit (int low, int high){

	//////////////////////////////
	assert(low < high || low > 0);
	//////////////////////////////

	int bbl= WDIV(low);
	int bbh= WDIV(high); 
	

	//clears all bits if required
	if (EraseAll) {
		erase_bit();
	}	

	if(bbl == bbh)
	{			
		m_aBB[bbh] |= bblock::MASK_1(low - WMUL(bbl), high - WMUL(bbh));			//low - WMUL(bbl) = WMOD(bbl) but less expensive (CHECK 01/02/25)
	}
	else
	{
		//set to one the intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			m_aBB[i] = ONE;
		}
		
		//sets the first and last blocks
		m_aBB[bbh] |= bblock::MASK_1_RIGHT	(high - WMUL(bbh));
		m_aBB[bbl] |= bblock::MASK_1_LEFT	(low - WMUL(bbl));

	}

	return *this;
}

inline
BitBoardN& BitBoardN::set_bit (){

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] = ONE;
	}
	return *this;
}

inline
BitBoardN&  BitBoardN::set_bit (const BitBoardN& bb_add){

	/////////////////////////////////
	assert(m_nBB <= bb_add.m_nBB);
	/////////////////////////////////

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] |= bb_add.m_aBB[i];
	}

	return *this;
}

inline
BitBoardN&  BitBoardN::set_block (int firstBlock, int lastBlock, const BitBoardN& bb_add){

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (LastBlock < m_nBB) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? m_nBB - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		m_aBB[i] |= bb_add.m_aBB[i];
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit (){

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] = ZERO;
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit(int nBit) {

	m_aBB[WDIV(nBit)] &= ~Tables::mask[WMOD(nBit)]; 
	return *this;
}

inline 
BitBoardN&  BitBoardN::erase_bit (int low, int high){

	/////////////////////////////////////////////
	assert(low < high || low > 0 );
	/////////////////////////////////////////////

	int bbl = WDIV(low);
	int bbh = WDIV(high);


	if (bbl == bbh)
	{
		m_aBB[bbh] &= bblock::MASK_0(low - WMUL(bbl), high - WMUL(bbh));			//low - WMUL(bbl) = WMOD(bbl) but less expensive (CHECK 01/02/25)
	}
	else
	{
		//set to one the intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			m_aBB[i] = ZERO;
		}
				
		m_aBB[bbh] &= bblock::MASK_0_RIGHT	(high - WMUL(bbh));					//Tables::mask_left
		m_aBB[bbl] &= bblock::MASK_0_LEFT	(low - WMUL(bbl));					//Tables::mask_right	
	}

	return *this;
}

inline int BitBoardN::lsbn64() const{
/////////////////
// different implementations of lsbn depending on configuration

#ifdef DE_BRUIJN
	for(int i=0; i<m_nBB; i++){
		if(m_aBB[i])
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((m_aBB[i] & -m_aBB[i]) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(i));	
#else
			return(Tables::indexDeBruijn64_SEP[((m_aBB[i]^ (m_aBB[i]-1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >> bblock::DEBRUIJN_MN_64_SHIFT]+ WMUL(i));
#endif
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<m_nBB; i++){
		val.b=m_aBB[i];
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
	for(auto i = 0; i < m_nBB; ++i){
		if ((pc += bblock::popc64(m_aBB[i])) > 1) {
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
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < m_nBB));
	/////////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? m_nBB - 1 : lastBlock);

	int pc = 0;
	for (auto i = firstBlock; i < last_block; ++i) {
		if ((pc += bblock::popc64(m_aBB[i])) > 1) {
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

	for(auto i = 0; i < m_nBB; ++i){

		//loads union
		val.b = m_aBB[i]; 

		//counts population
		pc += Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

	return pc;
}

int BitBoardN::popcn64(int nBit) const{
/////////////////////////
// Population size from nBit(included) onwards

	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	int nBB=WDIV(nBit);

	for(int i=nBB+1; i<m_nBB; i++){
		val.b = m_aBB[i]; //Loads union
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

	//special case of nBit bit block
	val.b = m_aBB[nBB]&~Tables::mask_right[WMOD(nBit)];		//Loads union
	npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	
return npc;
}


inline
int BitBoardN::find_common_singleton (const BitBoardN& rhs, int& bit) const{

	int pc = 0;
	bool is_first_vertex = true;
	bit = EMPTY_ELEM;
	
	//main loop
	for(auto i = 0; i < m_nBB; ++i){
		pc += bblock::popc64 (m_aBB[i] & rhs.m_aBB[i]);
		if(pc > 1){
			bit = EMPTY_ELEM;
			return -1;
		}else if(is_first_vertex && pc == 1 ) { //stores bit the first time pc == 1 
						
			bit = bblock::lsb64_intrinsic ( m_aBB[i] & rhs.m_aBB[i] )+ WMUL(i);
			is_first_vertex = false;
		}
	}
	
	//disjoint - pc = 0
	return pc;		
}


inline	
int	BitBoardN::find_common_singleton_block (int firstBlock, int lastBlock, const BitBoardN& rhs, int& bit) const{

	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (lastBlock < m_nBB) && (firstBlock <= lastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block;
	(lastBlock == -1) ? last_block = m_nBB - 1 : last_block = lastBlock;

	int pc = 0;
	bit = EMPTY_ELEM;
	bool is_first_vertex = true;
	
	for(auto i= firstBlock; i <= last_block; ++i){
		pc += bblock::popc64(m_aBB[i] & rhs.m_aBB[i]);
		if(pc > 1){
			bit = EMPTY_ELEM;
			return -1;
		}else if(is_first_vertex && pc == 1 ){	//stores bit the first time pc == 1 
			
			bit = bblock::lsb64_intrinsic(m_aBB[i] & rhs.m_aBB[i] )+ WMUL(i);
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
	
	for(auto i = 0; i < m_nBB; ++i){

		//popcount of set difference - removes bits of rhs from *this
		pc += bblock::popc64(m_aBB[i] &~ rhs.m_aBB[i]);

		if(pc > 1){
			bit = EMPTY_ELEM;
			return -1;
		}else if( pc == 1 && is_first_vertex){ //stores bit the first time pc == 1 
			
			bit = bblock::lsb64_intrinsic(m_aBB[i] &~ rhs.m_aBB[i] ) + WMUL(i);
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
	for (auto i = 0; i < m_nBB; ++i) {

		//popcount of set difference - removes bits of rhs from *this
		BITBOARD bb = m_aBB[i] & ~rhs.m_aBB[i];
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
	
	for(int i=0; i<lhs.m_nBB; i++)
		if( lhs.m_aBB[i]!=rhs.m_aBB[i] ) return false;

return true;
}

inline
bool operator!=	(const BitBoardN& lhs, const BitBoardN& rhs){
	 return ! operator==(lhs, rhs);
}

inline
BitBoardN& BitBoardN::erase_bit (const BitBoardN& bbn){

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] &= ~bbn.m_aBB[i];
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit (const BitBoardN& bb_lhs, const BitBoardN& bb_rhs ){

	for (auto i = 0; i < m_nBB; i++) {
		m_aBB[i] &= ~(bb_lhs.m_aBB[i] | bb_rhs.m_aBB[i]);
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_block (int FirstBlock, int LastBlock, const BitBoardN& bb_lhs, const BitBoardN& bb_rhs ){

	///////////////////////////////////////////////////////////////////////////////
	assert((FirstBlock >= 0) && (LastBlock < m_nBB) && (FirstBlock <= LastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block;
	(LastBlock == -1) ? last_block = m_nBB - 1 : last_block = LastBlock;

	for (auto i = FirstBlock; i <= LastBlock; ++i) {
		m_aBB[i] &= ~(bb_lhs.m_aBB[i] | bb_rhs.m_aBB[i]);
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_block(int FirstBlock, int LastBlock, const BitBoardN& bb_del){

	///////////////////////////////////////////////////////////////////////////////
	assert((FirstBlock >= 0) && (LastBlock < m_nBB) && (FirstBlock <= LastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((LastBlock == -1) ? m_nBB - 1 : LastBlock);

	for (auto i = FirstBlock; i <= last_block; ++i) {
		m_aBB[i] &= ~bb_del.m_aBB[i];
	}

	return *this;
}


inline
int FIRST_SHARED (const BitBoardN& lhs, const BitBoardN& rhs){
/////////////////////
// RETURNS first bit common to lhs and rhs, EMPTY ELEM if sets are disjoint
	
	for(int i=0; i<lhs.m_nBB; i++){
		BITBOARD bb=lhs.m_aBB[i] & rhs.m_aBB[i];
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

	for (int i = first_block; i < lhs.m_nBB; i++) {
		BITBOARD bb = lhs.m_aBB[i] & rhs.m_aBB[i];
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
	auto POPSIZE = WMUL(m_nBB);
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



#endif