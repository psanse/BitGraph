/** 
 * @file bitboard.h
 * @brief Header file for the class BitBoard of the BITSCAN 1.0 library
 * @details The BitBoard class manages bitsets of 64-bit length
 * @created ?
 * @last_update 29/01/2025
 * @author pss
 * 
 **/

#ifndef __BITBOARD_H__
#define __BITBOARD_H__

#include "tables.h"


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

#include <iostream>

//////////////////////////////////////////////////////////////////////////
//compile time globals
constexpr unsigned long long DEBRUIJN_MN_64_ISOL	 = 0x07EDD5E59A4E28C2;
constexpr unsigned long long DEBRUIJN_MN_64_SEP		 = 0x03f79d71b4cb0a89;
constexpr unsigned long long DEBRUIJN_MN_64_SHIFT	 = 58;


/////////////////////////////////
//
// BITBOARD CLASS
// 
// (manages bitset optimizations in 64-bitsets)
// 
// TODO - consider using a namespace instead of a class (29/01/2025)
// 
///////////////////////////////////

class BitBoard {

	//constructor / destructor
	BitBoard() = delete;
	
	
public:
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
	static int  lsb64_mod		(const BITBOARD bb);				
   
	/**
	* @brief Index of the least significant bit of bb implemented
	*		 with a 16-bit lookup table
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	**/
	static int  lsb64_lup		(const BITBOARD bb);

	/**
	* @brief Index of the least significant bit of bb implemented
	*		 with a 16-bit lookup table 
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	* @details Best implementation for 32 bits on x86 on average. Is is worse than LSB_32x only for sparse BB with
	*			1bits in the final BB
	**/
	static int  lsb64_lup_eff	(const BITBOARD bb);
	
	/**
	* @brief Index of the least significant bit of bb function implemented
	*		 as a population count operation 
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	* @date 2008
	**/
	static int  lsb64_pc		(const BITBOARD bb);

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
	static int  lsb64_de_Bruijn	(const BITBOARD bb);		
	
	/**
	* @brief Index of the least significant bit of bb (default)
	*		(intrin.h WIN / x86intrin.h Linux lib)	
	* @param bb: input 64-bit bitblock
	* @details implemented by calling processor instructions
	* @returns index of the least significant bit or -1 if empty
	**/
	static int lsb64_intrinsic	(const BITBOARD bb);


	/**
	* @brief Index of the least significant bit in bb
	*		 (RECOMMENDED to use - calls lsb64_intrinsic)
	* @param bb: input 64-bit bitblock
	* @returns index of the least significant bit or -1 if empty
	**/
	///////////////////////////////////////////////////////////////////////////////////
	static int lsb64			(const BITBOARD bb) { return 	lsb64_intrinsic(bb); }
	///////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief Index of the most significant bit in bb implemented
	*		 with a 16-bit lookup table
	* @param bb: input 64-bit bitblock
	* @returns Index of the most least significant bit or -1 if empty
	**/
	static int	msb64_lup		(const BITBOARD bb);
		
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
	static int  msb64_de_Bruijn	(const BITBOARD bb);		//De Bruijn magic word 
   
	/**
	* @brief Index of the most significant bit of bb which uses processor instructions
	*		(intrin.h WIN / x86intrin.h Linux lib)
	* @param bb: input 64-bit bitblock
	* @returns index of the most significant bit or -1 if empty
	**/
	/////////////////////////////////////////////////////////
    static int msb64_intrinsic	(const BITBOARD bb);
	/////////////////////////////////////////////////////////

	/**
	* @brief Index of themost significant bit of in bb
	* @param bb: input 64-bit bitblock
	* @returns index of the most significant bit or -1 if empty
	**/
	static int msb64			(const BITBOARD bb)		{ return msb64_intrinsic(bb); }

/////////////////////
// Bit population
	
	/**
	* @brief population count in bb implemented with lookup tables
	* @param bb: input 64-bit bitblock
	* @returns number of 1-bits in the bitblock 
	**/
	static int popc64_lup_1		(const BITBOARD bb);	
	static int popc64_lup		(const BITBOARD bb);				//Lookup
	
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
	static int popc64			(const BITBOARD bb);	
	////////////////////////////////////////////////

//////////////////////
//  Masks

//1-bit mask in the CLOSED range
	static BITBOARD MASK_1(int low, int high) { return ~Tables::mask_right[low] & ~Tables::mask_left[high]; }

//0-bit mask in the CLOSED range
	static BITBOARD MASK_0(int low, int high) { return Tables::mask_right[low] | Tables::mask_left[high]; }

//TODO - add trimming masks (29/01/25) 

/////////////////////
// I/O

	/**
	* @brief Prints the bitblock  and the popcount to the output stream
	**/
	static std::ostream& print		(const BITBOARD, std::ostream&  = std::cout) ;
};

//aliases

//////////////////////////////
// Necessary implementations in header file (inlining)


inline
int BitBoard::lsb64_intrinsic(const BITBOARD bb_dato) {
	unsigned long index;
	if (_BitScanForward64(&index, bb_dato))
		return(index);

	return EMPTY_ELEM;
}

inline
int BitBoard::msb64_intrinsic(const BITBOARD bb_dato) {
	unsigned long index;
	if (_BitScanReverse64(&index, bb_dato))
		return(index);

	return EMPTY_ELEM;
}

inline int BitBoard::popc64(const BITBOARD bb_dato) {

#ifdef POPCOUNT_64
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


inline
int BitBoard::lsb64_de_Bruijn(const BITBOARD bb_dato) {

#ifdef ISOLANI_LSB
		return (bb_dato == 0) ? EMPTY_ELEM : Tables::indexDeBruijn64_ISOL[((bb_dato & -bb_dato) * DEBRUIJN_MN_64_ISOL) >> DEBRUIJN_MN_64_SHIFT];
#else
		return (bb_dato == 0) ? EMPTY_ELEM : Tables::indexDeBruijn64_SEP[((bb_dato ^ (bb_dato - 1)) * DEBRUIJN_MN_64_SEP) >> DEBRUIJN_MN_64_SHIFT];
#endif

}

inline
int BitBoard::msb64_de_Bruijn(const BITBOARD bb_dato) {

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


#endif
