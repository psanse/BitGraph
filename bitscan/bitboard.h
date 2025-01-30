/** 
 * @file bitboard.h
 * @brief Header file for the class BitBoard of the BITSCAN library
 * @details The BitBoard class manages bitsets of 64-bit length
 * @created ?
 * @last_update 29/01/2025
 * @author pss
 * 
 * TODO -  consider moving implementations to the corresponding source file (29/01/2025)
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
		
	static int  lsb64_mod		(const BITBOARD);				//modulus of magic number perfect hashing	
    static int  lsb64_lup		(const BITBOARD);				//lookup implementations
	static int  lsb64_lup_1		(const BITBOARD);
	static int	msb64_lup		(const BITBOARD);
	static int  lsb64_pc		(const BITBOARD);				//popcount based implementation (2008)
	
	//ref
	static int  lsb64_de_Bruijn	(const BITBOARD bb_dato);		//De Bruijn magic word (2008)
	static int  msb64_de_Bruijn	(const BITBOARD bb_dato);		//De Bruijn magic word 
    static int lsb64_intrinsic	(const BITBOARD bb_dato);		//intrinsics	
    static int msb64_intrinsic	(const BITBOARD bb_dato);		//intrinsics	
	
	/**
	* @brief Default least significant bit function which uses intrinsics
	*		 if POPCOUNT_64 switch is ON (see config.h)
	**/
	static int lsb64			(const BITBOARD bb_dato);		

/////////////////////
// Bit population
		
	static int popc64_lup_1		(const BITBOARD);	
	static int popc64_lup		(const BITBOARD);				//Lookup
	
	/**
	* @brief Default popcount which uses intrinsics if POPCOUNT_64 switch is ON (see config.h)
	**/
	static int popc64			(const BITBOARD);				

//////////////////////
//  Masks
// 
//1-bit mask in the CLOSED range
	static BITBOARD MASK_1(int low, int high) { return ~Tables::mask_right[low] & ~Tables::mask_left[high]; }

//0-bit mask in the CLOSED range
	static BITBOARD MASK_0(int low, int high) { return Tables::mask_right[low] | Tables::mask_left[high]; }

//TODO - add trimming masks (29/01/25) 

/////////////////////
// I/O
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


#endif
