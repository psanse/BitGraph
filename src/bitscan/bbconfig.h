/**  
 * @file config.h
 * @brief Global configuration parameters for the BITSCAN library.
 *
 * This header defines compile-time and configuration-level parameters
 * that control low-level behavior of the BITSCAN core (bitsets, bitblocks,
 * word size assumptions, and related utilities).
 *
 * The contents of this file are intended to be stable across releases and
 * are used throughout the library to ensure consistent low-level semantics.
 *
 * @author Pablo San Segundo
 * @created ?
 * @last_update 2025-12-19
 **/

#include "bbtypes.h"

/////////////
// Population-count (popcount) implementation selection
//
// Different processors provide different levels of hardware support
// for population-count operations. BITSCAN supports multiple popcount
// backends with varying performance and portability characteristics.
//
// POPCOUNT_INTRINSIC_64
//   Enables the use of the native 64-bit hardware popcount instruction
//   (e.g. __popcnt64 / _popcnt64), when available.
//   This is the most efficient implementation and is the DEFAULT.
//
//   Undefining this macro forces the use of alternative software-based
//   popcount implementations (e.g. lookup tables or portable fallbacks),
//   which may be slower but can be used on platforms without native
//   popcount support.
//
#define POPCOUNT_INTRINSIC_64        // DEFAULT: use hardware 64-bit popcount
// #undef  POPCOUNT_INTRINSIC_64     // use software-based popcount fallback


/////////////
// Bit-scanning implementation choices (legacy options)
//
// NOTE:
//   These configuration switches select legacy bit-scanning algorithms.
//   They are retained for historical reference and experimentation.
//
//   By default, the library uses the De Bruijn–based approach with the
//   b^(b-1) formulation for isolating the least-significant bit (LSB).
//
//   These options should not be modified unless legacy behavior is
//   explicitly required or for benchmarking purposes.
//

#define DE_BRUIJN						// enable De Bruijn–based bit scanning (legacy)

#ifndef DE_BRUIJN
  // Fallback legacy implementation based on lookup tables
	#define LOOKUP
#endif

#ifdef DE_BRUIJN

	// LSB isolation method selection (De Bruijn path)
	//
	//   ISOLANI_LSB defined   → use b & (-b)
	//   ISOLANI_LSB undefined → use b ^ (b - 1)   [DEFAULT]
	//
	// Both formulations are functionally equivalent; the default is chosen
	// for historical consistency with previous BITSCAN releases.
	//
	#define ISOLANI_LSB										 // use b & (-b)
    #undef  ISOLANI_LSB										 // DEFAULT: use b ^ (b - 1)

#endif

//////////////////////
// Configuration of precomputed index tables
//
// NOTE:
//   Support for precomputed index tables is DISABLED by default.
//
//   When enabled, CACHED_INDEX_OPERATIONS activates the use of auxiliary
//   lookup tables for bit-index–to–word-index computations (e.g. WDIV,
//   WMOD, WMUL). These tables trade additional memory consumption for
//   faster bit-scanning operations.
//
//   This feature is currently disabled and should not be enabled unless
//   the cached tables are fully implemented, validated, and benchmarked.
//
//   DEFAULT: disabled (do not change).
//

#define CACHED_INDEX_OPERATIONS								// (optional) enable cached index tables
#undef  CACHED_INDEX_OPERATIONS								// DEFAULT: cached index tables disabled

#ifdef CACHED_INDEX_OPERATIONS

	// Maximum number of bit indices covered by the cached tables.
	// This value defines the size of the precomputed lookup arrays
	constexpr int MAX_CACHED_INDEX = 15001;					
#endif

#ifdef  CACHED_INDEX_OPERATIONS 
	#define WDIV(i) (Tables::t_wdindex[(i)])
	#define WMOD(i) (Tables::t_wmodindex[(i)])
	#define WMUL(i) (Tables::t_wxindex[(i)])
#else 

	//mirror macro for constexpr bitgraph::WORD_SIZE (used by #if)
	#ifndef BITGRAPH_WORD_SIZE
		#define BITGRAPH_WORD_SIZE 64    
	#endif
	static_assert(BITGRAPH_WORD_SIZE == bitgraph::WORD_SIZE,
					"different values BITGRAPH_WORD_SIZE and bitgraph::WORD_SIZE");

	// Optimization for  WORD_SIZE = 64 
	#if (BITGRAPH_WORD_SIZE == 64)
		#define WDIV(i)        ((i) >> 6)          // (i / 64)
		#define WMOD(i)        ((i) & 63)          // (i % 64)
		#define WMUL(i)        ((i) << 6)          // (i * 64)
		#define WMOD_MUL(i)    ((i) & 63)          // originally a quick WMOD - legacy name, to remove (19/12/2025)
	#else
		#define WDIV(i)        ((i) / WORD_SIZE)
		#define WMOD(i)        ((i) % WORD_SIZE)
		#define WMUL(i)        ((i) * WORD_SIZE)
	
		#define WMOD_MUL(i)    ((i) - WMUL(WDIV(i)))
	#endif
#endif

//TODO: change WDIV / WDIV / WMUL  macro names for clarity to the following:
//#define WORD_INDEX(i)       ((i) >> 6)      // bit index → word index
//#define BIT_OFFSET(i)       ((i) & 63)      // bit index → bit offset
//#define BIT_INDEX(i)        ((i) << 6)      // word index → bit index

////////////////////
//MACROS for mapping bit indexes to bitblock indexes (0 or 1 based)
#define INDEX_0TO0(p)			(WDIV(p))									// p>0
#define INDEX_0TO1(p)			(WDIV(p)+1)									// p>0
#define INDEX_1TO1(p)			((((p)-1)/bitgraph::WORD_SIZE)+1)			// p>0
#define INDEX_1TO0(p)			((((p)-1)/bitgraph::WORD_SIZE))				// p>0

//TODO: change macro names for clarity to the following:
//#define BIT0_WORD0(p)   (WDIV(p))
//#define BIT0_WORD1(p)   (WDIV(p) + 1)
//#define BIT1_WORD1(p)   (((p - 1) / WORD_SIZE) + 1)
//#define BIT1_WORD0(p)   (((p - 1) / WORD_SIZE)


////////////////////
// Extended lookup support
//
// NOTE:
//   Extended lookup–based BitScan operations are NOT available in this release.
//   The macro EXTENDED_LOOKUPS is intentionally undefined by default.
//
//   Defining EXTENDED_LOOKUPS enables legacy code paths that rely on additional
//   auxiliary tables for faster bit scanning at the cost of extra memory.
//   These code paths are currently unsupported and must not be enabled.
//
//   DO NOT define EXTENDED_LOOKUPS unless full extended-lookup support is
//   restored and validated.
//
#define EXTENDED_LOOKUPS											
#undef	EXTENDED_LOOKUPS											






