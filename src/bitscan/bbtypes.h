/** 
 * bbtypes.h file from the BITSCAN part of the BITGRAPH library
 *
 * Copyright (C)
 * Author: Pablo San Segundo
 * Intelligent Control Research Group (CSIC-UPM) 
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

#ifndef __BBTYPES_H__
#define __BBTYPES_H__

#include <cstdint>
#include <vector>
#include <set>

namespace bitgraph {

	using U8 = std::uint8_t;
	using U16 = std::uint16_t;
	using U32 = std::uint32_t;
	using U64 = std::uint64_t;

	using Ul = unsigned long;							// for intrinsics
	
	using BITBOARD = U64;								// alias for semantics
	using BYTE = U8;

	//C++14 style to avoid ODR issues
	enum : BITBOARD {
		ONE = 0xFFFFFFFFFFFFFFFFULL,					// 1 bit mask
		EVEN = 0x5555555555555555ULL,					// 1 bits in even positions: 64 bits
		ZERO = 0ULL	
	};

	enum : U32 {
		EVEN_32 = 0x55555555U
	};

	enum : U8 {
		EVEN_8 = 0x55U
	};

	//legacy code
	//constexpr BITBOARD ONE = 0xFFFFFFFFFFFFFFFF;		// 1 bit mask	
	//constexpr BITBOARD EVEN = 0x5555555555555555;		// 1 bits in even positions: 64 bits
	//constexpr U32 EVEN_32 = 0x55555555;				// 1 bits in even positions: 32 bits
	//constexpr U8 EVEN_8 = 0x55;						// 1 bits in even positions: 8 bits
	//constexpr BITBOARD ZERO = 0ull;					// 0 64 bit mask


	//C++14 style to avoid ODR issues
	enum : int {
		EMPTY_ELEM = -1,								// empty table element,... 	
		WORD_SIZE = 64,									// size of the register word - DO NOT CHANGE!
		WORD_SIZE_MINUS_ONE = WORD_SIZE - 1,
		MASK_LIM = WORD_SIZE + 1						// mask limit for bitscan operations on a single BITBOARD
	};

	//legacy code
	//constexpr int EMPTY_ELEM = -1;						
	
	////size of the register word - DO NOT CHANGE!
	//constexpr int WORD_SIZE = 64;
	//constexpr int WORD_SIZE_MINUS_ONE = WORD_SIZE - 1;
	//constexpr int MASK_LIM = WORD_SIZE + 1;	
	
	using bitpos_list = std::vector<int>;
	using bitpos_set = std::set<int>;

} // end namespace bitgraph

#endif // __BBTYPES_H__