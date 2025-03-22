/*  
 * bbtypes.h file from the BITSCAN library, a C++ library for bit set
 * optimization. BITSCAN has been used to implement BBMC, a very
 * succesful bit-parallel algorithm for exact maximum clique. 
 * (see license file for references)
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
 */

#ifndef __BBTYPES_H__
#define __BBTYPES_H__
		
using U8		= unsigned char;
using U16		= unsigned short;
using U32		= unsigned long;
using BITBOARD	= unsigned long long;
using BYTE		= unsigned char;	
using BOOL		= int;								//Deprecated: use bool instead (CHECK 08/02/25)

#define ONE		0xFFFFFFFFFFFFFFFF					//1 bit mask	
#define EVEN    0x5555555555555555					//1 bits in even positions: 64 bits
#define EVEN_32 0x55555555							//1 bits in even positions: 32 bits
#define EVEN_8  0x55						    	//1 bits in even positions: 8 bits
#define ZERO	0x0000000000000000					//0 64 bit mask

constexpr int EMPTY_ELEM = -1;						//empty table element, bitboard or bitstring 	

//size of the register word - DO NOT CHANGE!
constexpr int WORD_SIZE = 64;
constexpr int WORD_SIZE_MINUS_ONE = WORD_SIZE - 1;
constexpr int MASK_LIM = WORD_SIZE + 1;				  //mask limit for bitscan operations of a single BITBOARD



#endif