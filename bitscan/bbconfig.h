/**  
 * @file config.h
 * @brief configuration parameters for the BITSCAN 1.0 library
 * @author pss
 * @created ?
 * @last_update 09/02/2025
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

// uncomment for assertions in debug mode
//#undef NDEBUG												



/////////////
//Popcount
//(switch in case processors do not support operations for population count)

#define POPCOUNT_INTRINSIC_64								//uses HW assembler operation _popcn64 function (most efficient - DEFAULT)
//#undef  POPCOUNT_INTRINSIC_64								//will use other population-count functions (lookup table...)


/////////////
//Bit-scanning implementation choices 

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
		
//#define _MEM_ALIGNMENT 				32					//change this for different allignments (set to 64 bits? - 01/02/2025)
//#undef  _MEM_ALIGNMENT									//DEFAULT - currently MUST BE DISABLED , TODO CHECK (01/02/2025)
 


//////////////////////
// Configuration of precomputed tables

#define CACHED_INDEX_OPERATIONS								//uses extra storage space for fast bitscanning 
#undef  CACHED_INDEX_OPERATIONS								//comment for fast bitscanning	(DEFAULT-  uncommented)

#ifdef CACHED_INDEX_OPERATIONS
	constexpr int MAX_CACHED_INDEX = 15001;					//size of cached WMOD, WDIV, WMUL indexes (number of bits)
#endif

#ifdef  CACHED_INDEX_OPERATIONS 
#define WDIV(i) (Tables::t_wdindex[(i)])
#define WMOD(i) (Tables::t_wmodindex[(i)])
#define WMUL(i) (Tables::t_wxindex[(i)])
#else 
#define WDIV(i) ((i)/WORD_SIZE)
#define WMOD(i) ((i)%WORD_SIZE)
#define WMUL(i) ((i)*WORD_SIZE)
#define WMOD_MUL(i) ((i)-WMUL(WDIV(i)))						  //WMOD without modulo operation
#endif 

////////////////////
//MACROS for mapping bit index to bitblock index (0 or 1 based)
#define INDEX_0TO0(p)			(WDIV(p))					//p>0
#define INDEX_0TO1(p)			(WDIV(p)+1)					//p>0
#define INDEX_1TO1(p)			((((p)-1)/WORD_SIZE)+1)		//p>0
#define INDEX_1TO0(p)			((((p)-1)/WORD_SIZE))		//p>0

//Bitscan operations which used extended lookups
//have been removed in this release
#define EXTENDED_LOOKUPS									//uses extra storage space for fast lookup 
#undef	EXTENDED_LOOKUPS									//comment for fast bitset operations - DEFAULT, DO NOT CHANGE!	






