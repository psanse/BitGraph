/**  
 * @file config.h
 * @brief configuration parameters for the BITSCAN library
 * @author pss
 * @created ?
 * @last_update 31/01/2025
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

////////////////////
//DEBUG mode assertions

#define NDEBUG
//#undef NDEBUG												// uncomment for assertions in debug mode

/////////////
//Popcount

#define POPCOUNT_64											//uses HW assembler operation _popcn64 function (DEFAULT)
//#undef  POPCOUNT_64										//will use lookup table popcount function

/////////////
//bitscanning implementation choices for the non-intrinsic variants

#define DE_BRUIJN
#ifndef DE_BRUIJN
	#define LOOKUP
#endif

#ifdef DE_BRUIJN
	#define ISOLANI_LSB										//b&(-b) implementation
    #undef  ISOLANI_LSB										//b^(b-1) implementation (DEFAULT)
#endif

////////////////////
//Memory allignment	
		
#define _MEM_ALIGNMENT 				32						//change this for different allignments (set to 64 bits? - 01/02/2025)
#undef  _MEM_ALIGNMENT										//DEFAULT - currently MUST BE DISABLED , TODO CHECK (01/02/2025)
 
//////////////////////
// Configuration of precomputed tables

#define CACHED_INDEX_OPERATIONS								//uses extra storage space for fast bitscanning 
#undef  CACHED_INDEX_OPERATIONS								//comment for fast bitscanning	(DEFAULT-  uncommented)

#ifdef CACHED_INDEX_OPERATIONS
	constexpr int MAX_CACHED_INDEX = 15001;					//size of cached WMOD, WDIV, WMUL indexes (number of bits)
#endif

//DO NOT UNCOMMENT undef - bitscan operations which used extended lookups have been removed in this release
#define EXTENDED_LOOKUPS									//uses extra storage space for fast lookup 
#undef	EXTENDED_LOOKUPS									//comment for fast bitset operations	






