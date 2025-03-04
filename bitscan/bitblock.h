/** 
 * @file bitboard.h
 * @brief Header file for the class BitBoard of the BITSCAN 1.0 library
 * @details The BitBoard class manages bitsets of 64-bit length
 * @created ?
 * @last_update 30/01/2025 (refactored, added trimming functions, Doxygen tags)
 * @author pss
 * 
 **/

#ifndef __BITBLOCK_H__
#define __BITBLOCK_H__

#include <iostream>
#include "bbtypes.h"
#include "bbconfig.h"
#include "tables.h"

///////////////////////////
//Platform-dependent dependency settings for basic lsb, msb and popcount bitblcok operations

#ifdef __GNUC__

#ifdef __MINGW64__
//	#define __popcnt64 __builtin_popcountll					/* removed 6/11/2017 for mingw_64 */
	#include <intrin.h>										//windows specific
#else
	#define __popcnt64 __builtin_popcountll			
	#include <x86intrin.h>										//linux specific
	 static inline unsigned char _BitScanForward64(unsigned long* Index,  unsigned long long  Mask)
		{
			unsigned long long  Ret;
			__asm__
			(
				"bsfq %[Mask], %[Ret]"
				:[Ret] "=r" (Ret)
				:[Mask] "mr" (Mask)
			);
			*Index = (unsigned long)Ret;
			return Mask?1:0;
		}
		static inline unsigned char _BitScanReverse64(unsigned long* Index,  unsigned long long  Mask)
		{
			 unsigned long long  Ret;
			__asm__
			(
				"bsrq %[Mask], %[Ret]"
				:[Ret] "=r" (Ret)
				:[Mask] "mr" (Mask)
			);
			*Index = (unsigned long)Ret;
			return Mask?1:0;
		}
#endif


#else
	#include <intrin.h>										//windows specific
#endif


/////////////////////////////////
//
// namespace bblock
// 
// (manages bitset optimizations in 64-bitsets)
//
///////////////////////////////////

namespace bblock {

	//compile time globals for bitscanning operations in bitblocks
	constexpr unsigned long long DEBRUIJN_MN_64_ISOL = 0x07EDD5E59A4E28C2;
	constexpr unsigned long long DEBRUIJN_MN_64_SEP = 0x03f79d71b4cb0a89;
	constexpr unsigned long long DEBRUIJN_MN_64_SHIFT = 58;

	////////////////////
	// Boolean

	/**
	* @brief Determines if the bit [0...63] is set in the block bb
	* @returns TRUE if the bit is set, FALSE otherwise
	**/
	inline
	bool is_bit			(const BITBOARD bb, int bit)		 { return (bb & Tables::mask[bit]); }


/////////////////////
// BitScanning
	/**
	* @brief Index of the least significant bit of bb implemented
	*		 with perfect hashing according to modulo 
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	* 
	* @details T_64[67] returns the index of the single 1-bit
	*		   in the family of bitblocks 000...1...000
	* @returns index of the least significant bit or -1 if empty
	* @comment Modulus operation is not efficient.
	**/
	 int  lsb64_mod		(const BITBOARD bb);				
   
	/**
	* @brief Index of the least significant bit of bb implemented
	*		 with a 16-bit lookup table
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	**/
	 int  lsb64_lup		(const BITBOARD bb);

	/**
	* @brief Index of the least significant bit of bb implemented
	*		 with a 16-bit lookup table 
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	* @details Best implementation for 32 bits on x86 on average. Is is worse than LSB_32x only for sparse BB with
	*			1bits in the final BB
	**/
	 int  lsb64_lup_eff	(const BITBOARD bb);
	
	/**
	* @brief Index of the least significant bit of bb function implemented
	*		 as a population count operation 
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	* @date 2008
	**/
	 int  lsb64_pc			(const BITBOARD bb);

	/**
	* @brief Index of the least significant bit of bb function implemented
	*		 with a De Bruijn magic word for perfect hashing as 
	*		 a product and a shift operation.
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	* @date 2008
	* @details There are two implementations:
	*		    a) ISOLANI_LSB with hashing bb &(-bb)
	*			b) All 1-bits to LSB with hashing bb^(bb-1)
	* 
	*			Option b) would seem to exploit the CPU HW better on average
	*			and is defined as default.
	*			To change this option go to config.h file
	**/ 
 inline	 int lsb64_de_Bruijn (const BITBOARD bb);
	
	/**
	* @brief Index of the least significant bit of bb (default)
	*		(intrin.h WIN / x86intrin.h Linux lib)	
	* @param bb: input 64-bit bitblock
	* @details implemented by calling processor instructions
	* @returns index of the least significant bit or -1 if empty
	**/
 inline int lsb64_intrinsic	(const BITBOARD bb);


	/**
	* @brief Index of the least significant bit in bb
	*		 (RECOMMENDED to use - calls lsb64_intrinsic)
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	**/
	inline
	int lsb					(const BITBOARD bb) { return 	lsb64_intrinsic(bb); }
	

	/**
	* @brief Index of the most significant bit in bb implemented
	*		 with a 16-bit lookup table
	* @param bb: input 64-bit bitblock
	* @returns Index of the most least significant bit or -1 if empty
	**/
	 int msb64_lup			(const BITBOARD bb);
		
	/**
	* @brief Index of the most significant bit of bb implemented
	*		 with a De Bruijn magic word for perfect hashing as 
	*		 a product and a shift operation.
	* 
	*		I. Does not require lookup tables
	*		II. It first creates 1-bits from the least significant to MSB
	* @param bb: input 64-bit bitblock
	* @details Nice to not require LUPs of 65535 entries
	*
	* TODO - efficiency tests 
	**/
	 int  msb64_de_Bruijn	(const BITBOARD bb);		//De Bruijn magic word 
   
	/**
	* @brief Index of the most significant bit of bb which uses processor instructions
	*		(intrin.h WIN / x86intrin.h Linux lib)
	* @param bb: input 64-bit bitblock
	* @returns index of the most significant bit or -1 if empty
	**/
	 inline  int msb64_intrinsic (const BITBOARD bb);
	

	/**
	* @brief Index of themost significant bit of in bb
	* @param bb: input 64-bit bitblock
	* @returns index of the most significant bit or -1 if empty
	**/
	 inline  int msb		(const BITBOARD bb)		{ return msb64_intrinsic(bb); }

/////////////////////
// Bit population
	
	/**
	* @brief population count in bb implemented with 16-bit lookup tables
	*		 (default lookup table implementation)
	* @param bb: input 64-bit bitblock
	* @returns number of 1-bits in the bitblock 
	**/	
	 int popc64_lup			(const BITBOARD bb);		

	/**
	* @brief population count in bb implemented with 16-bit lookup tables
	*	     but no intermediate storage
	* @param bb: input 64-bit bitblock
	* @returns number of 1-bits in the bitblock
	**/
	 int popc64_lup_1		(const BITBOARD bb);	
		
	/**
	* @brief Default population count in bb (RECOMMENDED) 
	* @param bb: input 64-bit bitblock
	* @details Calls assembler processor instructions if POPCOUNT_64 switch is ON (see config.h)
	*		   else uses a table lookup implementation.
	* 
	*			I. By default POPCOUNT_64 switch is ON
	* 
	* @return number of 1-bits in the bitblock 
	**/
	////////////////////////////////////////////////
	 inline	 int popc64		(const BITBOARD bb);
	////////////////////////////////////////////////

//////////////////////
//  Masks
	/**
	* @brief Sets to 1 the bit passed and zero the rest of bits 
	* @param bit: bit in the bitblock [0...63]
	* @returns 64-bit bitblock mask with only one bit set
	**/	  
	 inline
	BITBOARD MASK_BIT		(int bit)					 { return Tables::mask[bit]; }

	/**
	* @brief Sets to 1 the bits inside the closed range [low, high], sets to 0 the rest
	* @param low, high: positions in the bitblock [0...63]
	* @returns 64-bit bitblock mask
	**/
	 inline
	 BITBOARD MASK_1	(int low, int high)				{ return Tables::mask_mid[low][high]; };
	
	 /**
	 * @brief Sets to 1 all bits in the closed range [0, 63]
	 * @param idx: input reference bit position [0...63]
	 * @returns 64-bit bitblock mask
	 **/
	 inline
	 BITBOARD MASK_1_LOW	(int idx)					{ return ~Tables::mask_high[idx]; }
	
	 /**
	 * @brief Sets to 1 all bits in the closed range [idx, 63]
	 * @param idx: input reference bit position [0...63]
	 * @returns 64-bit bitblock mask
	 **/
	 inline
	 BITBOARD MASK_1_HIGH	(int idx)			{ return ~Tables::mask_low[idx]; }

	/**
	* @brief Sets to 0 the bits inside the closed range [low, high], sets to 1 the rest
	* @param low, high: positions in the bitblock [0...63]
	* @returns 64-bit bitblock mask
	**/	
	 inline
	 BITBOARD MASK_0		(int low, int high) {  return ~Tables::mask_mid[low][high];	 }
	
	 /**
	* @brief Sets to 0 all bits in the closed range [0, idx]
	* @param idx: input reference bit position [0...63]
	* @returns 64-bit bitblock mask
	**/	
	 inline
	 BITBOARD MASK_0_LOW	(int idx)			{ return Tables::mask_high[idx]; }
	 
	 /**
	* @brief Sets to 0 all bits in the closed range [idx, 63]
	* @param idx: input reference bit position [0...63]
	* @returns 64-bit bitblock mask
	**/	
	 inline
	 BITBOARD MASK_0_HIGH	(int idx)			{ return Tables::mask_low[idx]; }
	
	/**
	* @brief sets to 0 the bits of the bitblock bb to the right of index (the index-bit is not trimmed)
	* @param bb: input 64-bit bitblock
	* @param idx: position in the bitblock [0...63]
	* @returns the trimmed bitblock
	* @date 30/01/2015 
	**/	
	 inline
	 BITBOARD trim_low	(BITBOARD bb, int idx) { return bb &~ Tables::mask_low[idx]; }

	/**
	* @brief sets to 0 the bits of the bitblock bb to the left side of index (the index-bit is not trimmed)
	* @param bb: input 64-bit bitblock
	* @param idx: position in the bitblock [0...63]
	* @returns the trimmed bitblock
	* @date 30/01/2015 
	**/	
	 inline
	 BITBOARD trim_high		(BITBOARD bb, int idx) { return bb &~ Tables::mask_high[idx]; }

	 /**
	 * @brief replaces bits in the closed range [firstBit, lastBit] of source bitblock (source)
	 *		  in destination bitblock (dest)
	 * @param firstBit, lastBit: closed range of bits [0...63]
	 * @param source, dest: input bitblocks
	 **/
	 void copy				(int firstBit, int lastBit, const BITBOARD& source,  BITBOARD& dest);
	
	 /**
	 * @brief replaces bits in the range [bit, 63] of source bitblock (source)
	 *		  in destination bitblock (dest)
	 * @param firstBit, lastBit: closed range of bits [0...63]
	 * @param source, dest: input bitblocks
	 **/
	 void copy_high			(int bit, const BITBOARD& source, BITBOARD& dest);
	
	 /**
	 * @brief replaces bits in the range [0, bit] of source bitblock (source)
	 *		  in destination bitblock (dest)
	 * @param firstBit, lastBit: closed range of bits [0...63]
	 * @param source, dest: input bitblocks
	 **/
	 void copy_low			(int bit, const BITBOARD& source, BITBOARD& dest);

/////////////////////
// I/O

	/**
	* @brief streams bb and its popcount to the output stream
	*		 (format ...000111 [3])
	**/
	 std::ostream& print	( BITBOARD bb, std::ostream&  = std::cout, bool endofl = true) ;

} //end namespace bblock



//////////////////////////////
// inline implementations in header file 

namespace bblock {

	int lsb64_intrinsic(const BITBOARD bb_dato) {
		unsigned long index;
		if (_BitScanForward64(&index, bb_dato))
			return(index);

		return EMPTY_ELEM;
	}

	
	int msb64_intrinsic(const BITBOARD bb_dato) {
		unsigned long index;
		if (_BitScanReverse64(&index, bb_dato))
			return(index);

		return EMPTY_ELEM;
	}

	
	int popc64(const BITBOARD bb_dato) {

#ifdef POPCOUNT_INTRINSIC_64
		return __popcnt64(bb_dato);
#else
		//lookup table popcount
		register union x {
			U16 c[4];
			BITBOARD b;
		} val;

		val.b = bb_dato; //Carga unisn

		return (Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]]); //Suma de poblaciones  
#endif

	}

	//alias for population count	
	inline
	int size(const BITBOARD bb_dato) { return popc64(bb_dato); }	
	
	
	int lsb64_de_Bruijn(const BITBOARD bb_dato) {

#ifdef ISOLANI_LSB
		return (bb_dato == 0) ? EMPTY_ELEM : Tables::indexDeBruijn64_ISOL[((bb_dato & -bb_dato) * DEBRUIJN_MN_64_ISOL) >> DEBRUIJN_MN_64_SHIFT];
#else
		return (bb_dato == 0) ? EMPTY_ELEM : Tables::indexDeBruijn64_SEP[((bb_dato ^ (bb_dato - 1)) * DEBRUIJN_MN_64_SEP) >> DEBRUIJN_MN_64_SHIFT];
#endif

	}

	inline
	int msb64_de_Bruijn(const BITBOARD bb_dato) {

		if (bb_dato == 0) return EMPTY_ELEM;

		//creates all 1s up to MSB position
		BITBOARD bb = bb_dato;
		bb |= bb >> 1;
		bb |= bb >> 2;
		bb |= bb >> 4;
		bb |= bb >> 8;
		bb |= bb >> 16;
		bb |= bb >> 32;

		//applys same computation as for LSB-de Bruijn
		return Tables::indexDeBruijn64_SEP[(bb * DEBRUIJN_MN_64_SEP) >> DEBRUIJN_MN_64_SHIFT];
	}
}//end namespace bblock


#endif
